//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Left 4 Dead-style script-backed weapon pickup spawners.
//
//=============================================================================//

#include "cbase.h"
#include "cs_player.h"
#include "weapon_scripted_spawn.h"
#include "weapon_csbase.h"
#include "util.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

namespace
{
	struct WeaponSpawnAlias_t
	{
		const char *pszEntityClass;
		const char *pszRequestedWeaponClass;
		const char *pszGiveWeaponClass;
		bool bAmmoOnly;
	};

	static const WeaponSpawnAlias_t s_WeaponSpawnAliases[] =
	{
		{ "weapon_adrenaline_spawn",					"weapon_adrenaline",					NULL,					false },
		{ "weapon_ammo_spawn",							NULL,									NULL,					true  },
		{ "weapon_autoshotgun_spawn",					"weapon_xm1014",						"weapon_xm1014",		false },
		{ "weapon_chainsaw_spawn",						"weapon_chainsaw",						"weapon_knife",			false },
		{ "weapon_defibrillator_spawn",					"weapon_defibrillator",					NULL,					false },
		{ "weapon_first_aid_kit",						"weapon_c4",					NULL,					false },
		{ "weapon_first_aid_kit_spawn",					"weapon_c4",					NULL,					false },
		{ "weapon_gascan_spawn",						"weapon_gascan",						NULL,					false },
		{ "weapon_grenade_launcher",					"weapon_grenade_launcher",				NULL,					false },
		{ "weapon_grenade_launcher_spawn",				"weapon_grenade_launcher",				NULL,					false },
		{ "weapon_hunting_rifle_spawn",					"weapon_g3sg1",							"weapon_g3sg1",			false },
		{ "weapon_item_spawn",							NULL,									NULL,					false },
		{ "weapon_melee_spawn",							"weapon_knife",							"weapon_knife",			false },
		{ "weapon_molotov_spawn",						"weapon_molotov",						"weapon_molotov",		false },
		{ "weapon_pain_pills_spawn",					"weapon_pain_pills",					NULL,					false },
		{ "weapon_pipe_bomb_spawn",						"weapon_pipe_bomb",						"weapon_hegrenade",		false },
		{ "weapon_pistol_magnum_spawn",					"weapon_deagle",						"weapon_deagle",		false },
		{ "weapon_pistol_spawn",						"weapon_glock",							"weapon_glock",			false },
		{ "weapon_pumpshotgun_spawn",					"weapon_m3",							"weapon_m3",			false },
		{ "weapon_rifle_ak47_spawn",					"weapon_ak47",							"weapon_ak47",			false },
		{ "weapon_rifle_desert_spawn",					"weapon_aug",							"weapon_aug",			false },
		{ "weapon_rifle_m60_spawn",						"weapon_m249",							"weapon_m249",			false },
		{ "weapon_rifle_sg552_spawn",					"weapon_sg552",							"weapon_sg552",			false },
		{ "weapon_rifle_spawn",							"weapon_m4a1",							"weapon_m4a1",			false },
		{ "weapon_scavenge_item_spawn",					"weapon_gascan",						NULL,					false },
		{ "weapon_shotgun_chrome_spawn",				"weapon_m3",							"weapon_m3",			false },
		{ "weapon_shotgun_spas_spawn",					"weapon_xm1014",						"weapon_xm1014",		false },
		{ "weapon_smg_mp5_spawn",						"weapon_mp5navy",						"weapon_mp5navy",		false },
		{ "weapon_smg_silenced_spawn",					"weapon_mac10",							"weapon_mac10",			false },
		{ "weapon_smg_spawn",							"weapon_p90",							"weapon_p90",			false },
		{ "weapon_sniper_awp_spawn",					"weapon_awp",							"weapon_awp",			false },
		{ "weapon_sniper_military_spawn",				"weapon_sg550",							"weapon_sg550",			false },
		{ "weapon_sniper_scout_spawn",					"weapon_scout",							"weapon_scout",			false },
		{ "weapon_spawn",								NULL,									NULL,					false },
		{ "weapon_upgradepack_explosive_spawn",			"weapon_upgradepack_explosive",			NULL,					false },
		{ "weapon_upgradepack_incendiary_spawn",		"weapon_upgradepack_incendiary",		NULL,					false },
		{ "weapon_vomitjar_spawn",						"weapon_vomitjar",						"weapon_smokegrenade",	false },
	};

	static const WeaponSpawnAlias_t *FindWeaponSpawnAlias( const char *pszName )
	{
		if ( !pszName || !pszName[0] )
			return NULL;

		for ( int i = 0; i < ARRAYSIZE( s_WeaponSpawnAliases ); ++i )
		{
			if ( !Q_stricmp( s_WeaponSpawnAliases[i].pszEntityClass, pszName ) )
				return &s_WeaponSpawnAliases[i];
		}

		return NULL;
	}

	static bool IsNumericString( const char *pszValue )
	{
		if ( !pszValue || !pszValue[0] )
			return false;

		for ( const char *p = pszValue; *p; ++p )
		{
			if ( *p < '0' || *p > '9' )
				return false;
		}

		return true;
	}

	static const CCSWeaponInfo *GetScriptWeaponInfo( const char *pszWeaponClass )
	{
		if ( !pszWeaponClass || !pszWeaponClass[0] )
			return NULL;

		WEAPON_FILE_INFO_HANDLE hInfo = LookupWeaponInfoSlot( pszWeaponClass );
		if ( hInfo == GetInvalidWeaponInfoHandle() )
		{
			if ( !ReadWeaponDataFromFileForSlot( filesystem, pszWeaponClass, &hInfo, g_pGameRules ? g_pGameRules->GetEncryptionKey() : NULL ) )
				return NULL;
		}

		return static_cast< CCSWeaponInfo * >( GetFileWeaponInfoFromHandle( hInfo ) );
	}
}

class CWeaponScriptedSpawn : public CBaseAnimating
{
public:
	DECLARE_CLASS( CWeaponScriptedSpawn, CBaseAnimating );
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );
	bool KeyValue( const char *szKeyName, const char *szValue );
	int ObjectCaps( void ) { return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS; }
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	bool WouldGiveUsefulWeaponToPlayer( CCSPlayer *pPlayer ) const;

private:
	const WeaponSpawnAlias_t *GetAlias() const;
	const char *GetRequestedWeaponClass() const;
	const char *GetGiveWeaponClass() const;
	const char *ResolveGiveWeaponClassForPlayer( CCSPlayer *pPlayer ) const;
	const CCSWeaponInfo *GetRequestedWeaponInfo() const;
	const CCSWeaponInfo *GetGiveWeaponInfo() const;
	const char *ResolveWorldModel() const;
	bool WouldGiveWeaponClassToPlayer( CCSPlayer *pPlayer, const char *pszGiveClass ) const;
	bool GiveWeaponToPlayer( CCSPlayer *pPlayer, const char *pszGiveClass );

	string_t m_iszWeaponClassOverride;
};

LINK_ENTITY_TO_CLASS( weapon_adrenaline_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_ammo_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_autoshotgun_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_chainsaw_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_defibrillator_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_first_aid_kit, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_first_aid_kit_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_gascan_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_grenade_launcher, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_grenade_launcher_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_hunting_rifle_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_item_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_melee_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_molotov_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_pain_pills_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_pipe_bomb_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_pistol_magnum_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_pistol_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_pumpshotgun_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_rifle_ak47_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_rifle_desert_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_rifle_m60_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_rifle_sg552_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_rifle_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_scavenge_item_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_shotgun_chrome_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_shotgun_spas_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_smg_mp5_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_smg_silenced_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_smg_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_sniper_awp_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_sniper_military_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_sniper_scout_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_upgradepack_explosive_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_upgradepack_incendiary_spawn, CWeaponScriptedSpawn );
LINK_ENTITY_TO_CLASS( weapon_vomitjar_spawn, CWeaponScriptedSpawn );

BEGIN_DATADESC( CWeaponScriptedSpawn )

	DEFINE_FIELD( m_iszWeaponClassOverride, FIELD_STRING ),

END_DATADESC()

bool CWeaponScriptedSpawn::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( !Q_stricmp( szKeyName, "weaponclass" ) ||
		 !Q_stricmp( szKeyName, "weaponname" ) ||
		 !Q_stricmp( szKeyName, "weapon_name" ) ||
		 !Q_stricmp( szKeyName, "item" ) )
	{
		m_iszWeaponClassOverride = AllocPooledString( szValue );
		return true;
	}

	if ( !Q_stricmp( szKeyName, "weapon_selection" ) )
	{
		if ( !IsNumericString( szValue ) )
		{
			m_iszWeaponClassOverride = AllocPooledString( szValue );
		}
		return true;
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}

const WeaponSpawnAlias_t *CWeaponScriptedSpawn::GetAlias() const
{
	return FindWeaponSpawnAlias(const_cast<CWeaponScriptedSpawn*>(this)->GetClassname());
}

const char *CWeaponScriptedSpawn::GetRequestedWeaponClass() const
{
	if ( m_iszWeaponClassOverride != NULL_STRING )
		return STRING( m_iszWeaponClassOverride );

	const WeaponSpawnAlias_t *pAlias = GetAlias();
	return pAlias ? pAlias->pszRequestedWeaponClass : NULL;
}

const char *CWeaponScriptedSpawn::GetGiveWeaponClass() const
{
	const WeaponSpawnAlias_t *pAlias = GetAlias();
	if ( pAlias && pAlias->pszGiveWeaponClass )
		return pAlias->pszGiveWeaponClass;

	const char *pszRequestedClass = GetRequestedWeaponClass();
	if ( FindWeaponSpawnAlias( pszRequestedClass ) != NULL )
		return NULL;

	return pszRequestedClass;
}

const char *CWeaponScriptedSpawn::ResolveGiveWeaponClassForPlayer( CCSPlayer *pPlayer ) const
{
	const char *pszGiveClass = GetGiveWeaponClass();
	if ( !pszGiveClass || !pszGiveClass[0] || !pPlayer )
		return pszGiveClass;

	if ( !Q_stricmp( const_cast< CWeaponScriptedSpawn * >( this )->GetClassname(), "weapon_pistol_spawn" ) && pPlayer->Weapon_GetSlot( WEAPON_SLOT_PISTOL ) != NULL )
		return "weapon_elite";

	return pszGiveClass;
}

const CCSWeaponInfo *CWeaponScriptedSpawn::GetRequestedWeaponInfo() const
{
	return GetScriptWeaponInfo( GetRequestedWeaponClass() );
}

const CCSWeaponInfo *CWeaponScriptedSpawn::GetGiveWeaponInfo() const
{
	return GetScriptWeaponInfo( GetGiveWeaponClass() );
}

const char *CWeaponScriptedSpawn::ResolveWorldModel() const
{
	if ( GetModelName() != NULL_STRING )
		return STRING( GetModelName() );

	const CCSWeaponInfo *pRequestedInfo = GetRequestedWeaponInfo();
	if ( pRequestedInfo && pRequestedInfo->szWorldModel[0] )
		return pRequestedInfo->szWorldModel;

	const CCSWeaponInfo *pGiveInfo = GetGiveWeaponInfo();
	if ( pGiveInfo && pGiveInfo->szWorldModel[0] )
		return pGiveInfo->szWorldModel;

	return NULL;
}

void CWeaponScriptedSpawn::Precache( void )
{
	const char *pszGiveClass = GetGiveWeaponClass();
	if ( pszGiveClass && pszGiveClass[0] )
	{
		UTIL_PrecacheOther( pszGiveClass );
	}

	if ( !Q_stricmp( GetClassname(), "weapon_pistol_spawn" ) )
	{
		UTIL_PrecacheOther( "weapon_elite" );
	}

	const char *pszWorldModel = ResolveWorldModel();
	if ( pszWorldModel && pszWorldModel[0] )
	{
		PrecacheModel( pszWorldModel );
	}
}

void CWeaponScriptedSpawn::Spawn( void )
{
	Precache();

	SetMoveType( MOVETYPE_NONE );
	SetSolid( SOLID_NONE );
	SetCollisionGroup( COLLISION_GROUP_DEBRIS_TRIGGER );

	const char *pszWorldModel = ResolveWorldModel();
	if ( pszWorldModel && pszWorldModel[0] )
	{
		SetModel( pszWorldModel );
		RemoveEffects( EF_NODRAW );
	}
	else
	{
		AddEffects( EF_NODRAW );
	}
}

bool CWeaponScriptedSpawn::GiveWeaponToPlayer( CCSPlayer *pPlayer, const char *pszGiveClass )
{
	if ( !pPlayer || !pszGiveClass || !pszGiveClass[0] )
		return false;

	if ( WeaponIdFromString( pszGiveClass ) == WEAPON_NONE )
		return false;

	const CCSWeaponInfo *pInfo = GetScriptWeaponInfo( pszGiveClass );
	if ( !pInfo )
		return false;

	if ( !WouldGiveWeaponClassToPlayer( pPlayer, pszGiveClass ) )
		return false;

	if ( pInfo->iSlot >= 0 )
	{
		CBaseCombatWeapon *pExistingWeapon = pPlayer->Weapon_GetSlot( pInfo->iSlot );
		if ( pExistingWeapon )
		{
			pExistingWeapon->DestroyItem();
		}
	}

	CBaseEntity *pGivenEntity = pPlayer->GiveNamedItem( pszGiveClass );
	CBaseCombatWeapon *pWeapon = dynamic_cast< CBaseCombatWeapon * >( pGivenEntity );
	if ( !pWeapon || pWeapon->GetOwner() != pPlayer )
	{
		if ( pGivenEntity && !pGivenEntity->IsMarkedForDeletion() )
		{
			UTIL_Remove( pGivenEntity );
		}
		return false;
	}

	return true;
}

bool CWeaponScriptedSpawn::WouldGiveWeaponClassToPlayer( CCSPlayer *pPlayer, const char *pszGiveClass ) const
{
	if ( !pPlayer || !pszGiveClass || !pszGiveClass[0] )
		return false;

	const CCSWeaponInfo *pInfo = GetScriptWeaponInfo( pszGiveClass );
	if ( !pInfo )
		return false;

	if ( pInfo->m_WeaponType == WEAPONTYPE_GRENADE )
		return pPlayer->Weapon_OwnsThisType( pszGiveClass ) == NULL;

	if ( pInfo->iSlot < 0 )
		return true;

	CBaseCombatWeapon *pExistingWeapon = pPlayer->Weapon_GetSlot( pInfo->iSlot );
	if ( !pExistingWeapon )
		return true;

	if ( !Q_stricmp( pExistingWeapon->GetClassname(), pszGiveClass ) )
		return false;

	if ( Q_stricmp( const_cast< CWeaponScriptedSpawn * >( this )->GetClassname(), "weapon_spawn" ) != 0 )
		return true;

	CWeaponCSBase *pExistingCSWeapon = dynamic_cast< CWeaponCSBase * >( pExistingWeapon );
	if ( !pExistingCSWeapon )
		return true;

	return pInfo->GetWeaponPrice() > pExistingCSWeapon->GetCSWpnData().GetWeaponPrice();
}

bool CWeaponScriptedSpawn::WouldGiveUsefulWeaponToPlayer( CCSPlayer *pPlayer ) const
{
	return WouldGiveWeaponClassToPlayer( pPlayer, ResolveGiveWeaponClassForPlayer( pPlayer ) );
}

void CWeaponScriptedSpawn::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CCSPlayer *pPlayer = ToCSPlayer( pActivator );
	if ( !pPlayer || !pPlayer->IsAlive() || pPlayer->GetTeamNumber() != TEAM_SURVIVOR )
		return;

	const WeaponSpawnAlias_t *pAlias = GetAlias();
	if ( pAlias && pAlias->bAmmoOnly )
	{
		pPlayer->StockPlayerAmmo();
		return;
	}

	const char *pszGiveClass = ResolveGiveWeaponClassForPlayer( pPlayer );
	if ( GiveWeaponToPlayer( pPlayer, pszGiveClass ) )
	{
		return;
	}

	Warning( "weapon spawn '%s' could not give '%s'\n", GetClassname(), pszGiveClass ? pszGiveClass : "<null>" );
}

bool Terror_IsScriptedWeaponPickupEntity( CBaseEntity *pEntity )
{
	return dynamic_cast< CWeaponScriptedSpawn * >( pEntity ) != NULL;
}

bool Terror_ScriptedWeaponPickupIsUsefulForPlayer( CBaseEntity *pEntity, CCSPlayer *pPlayer )
{
	CWeaponScriptedSpawn *pSpawn = dynamic_cast< CWeaponScriptedSpawn * >( pEntity );
	return pSpawn ? pSpawn->WouldGiveUsefulWeaponToPlayer( pPlayer ) : false;
}
