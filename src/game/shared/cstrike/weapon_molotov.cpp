//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "gamerules.h"
#include "weapon_molotov.h"

#ifdef CLIENT_DLL

#else

#include "cs_player.h"
#include "items.h"
#include "basegrenade_shared.h"
#include "vstdlib/random.h"

#endif

IMPLEMENT_NETWORKCLASS_ALIASED( MolotovGrenade, DT_MolotovGrenade )

BEGIN_NETWORK_TABLE( CMolotovGrenade, DT_MolotovGrenade )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CMolotovGrenade )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_molotov, CMolotovGrenade );
PRECACHE_WEAPON_REGISTER( weapon_molotov );

#ifndef CLIENT_DLL

BEGIN_DATADESC( CMolotovGrenade )
END_DATADESC()

void CMolotovGrenade::Precache( void )
{
	BaseClass::Precache();
	UTIL_PrecacheOther( "grenade_molotov" );
}

void CMolotovGrenade::EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer )
{
#ifdef GAME_DLL
	NOTE_UNUSED( angImpulse );

	CCSPlayer *pCSPlayer = ToCSPlayer( pPlayer );
	if ( pCSPlayer )
	{
		pCSPlayer->StartNewBulletGroup();
	}
	pCSPlayer->EmitSound("Molotov.Throw");
#endif

	CBaseEntity *pEnt = CreateEntityByName( "grenade_molotov" );
	if ( !pEnt )
		return;

	pEnt->SetAbsOrigin( vecSrc );
	pEnt->SetAbsAngles( vecAngles );
	DispatchSpawn( pEnt );
	pEnt->Activate();

	CBaseGrenade *pGrenade = dynamic_cast< CBaseGrenade * >( pEnt );
	if ( pGrenade )
	{
		pGrenade->SetThrower( pPlayer );
		pGrenade->SetOwnerEntity( pPlayer );
	}
	else
	{
		pEnt->SetOwnerEntity( pPlayer );
	}

	pEnt->SetAbsVelocity( vecVel );

	// Tumble through the air (grenade_molotov doesn't expose a Create(...) that takes AngularImpulse).
	QAngle angVel(
		random->RandomFloat( -100, -500 ),
		random->RandomFloat( -100, -500 ),
		random->RandomFloat( -100, -500 ) );
	pEnt->SetLocalAngularVelocity( angVel );
}

#endif
