//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"
#include "in_buttons.h"

#if defined( CLIENT_DLL )

	#define CWeaponScout C_WeaponScout
	#include "c_cs_player.h"

#else

	#include "cs_player.h"
	#include "KeyValues.h"

#endif

const int cScoutMidZoomFOV = 40;
const int cScoutMaxZoomFOV = 15;


class CWeaponScout : public CWeaponCSBaseGun
{
public:
	DECLARE_CLASS( CWeaponScout, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponScout();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void ItemPostFrame();

 	virtual float GetInaccuracy() const;
	virtual float GetMaxSpeed() const;
	virtual bool Reload();
	virtual bool Deploy();

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_SCOUT; }


private:
	
	CWeaponScout( const CWeaponScout & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponScout, DT_WeaponScout )

BEGIN_NETWORK_TABLE( CWeaponScout, DT_WeaponScout )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponScout )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_scout, CWeaponScout );
PRECACHE_WEAPON_REGISTER( weapon_scout );



CWeaponScout::CWeaponScout()
{
}

void CWeaponScout::SecondaryAttack()
{
	BaseClass::SecondaryAttack();
}

void CWeaponScout::ItemPostFrame()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( pPlayer && ( pPlayer->m_afButtonPressed & IN_ATTACK3 ) && m_flNextAttack3 <= gpGlobals->curtime )
	{
		const float kZoomTime = 0.10f;

		if ( pPlayer->GetFOV() == pPlayer->GetDefaultFOV() )
		{
			pPlayer->SetFOV( pPlayer, cScoutMidZoomFOV, kZoomTime );
			pPlayer->m_iLastZoom = cScoutMidZoomFOV;
			m_weaponMode = Secondary_Mode;
			m_fAccuracyPenalty += GetCSWpnData().m_fInaccuracyAltSwitch;
		}
		else if ( pPlayer->GetFOV() == cScoutMidZoomFOV )
		{
			pPlayer->SetFOV( pPlayer, cScoutMaxZoomFOV, kZoomTime );
			pPlayer->m_iLastZoom = cScoutMaxZoomFOV;
			m_weaponMode = Secondary_Mode;
		}
		else
		{
			pPlayer->SetFOV( pPlayer, pPlayer->GetDefaultFOV(), kZoomTime );
			pPlayer->m_iLastZoom = 0;
			m_weaponMode = Primary_Mode;
		}

#ifndef CLIENT_DLL
		if ( GetPlayerOwner() )
		{
			GetPlayerOwner()->EmitSound( "Default.Zoom" );
		}

		IGameEvent *event = gameeventmanager->CreateEvent( "weapon_zoom" );
		if ( event )
		{
			event->SetInt( "userid", pPlayer->GetUserID() );
			gameeventmanager->FireEvent( event );
		}
#endif

		m_flNextAttack3 = gpGlobals->curtime + 0.3f;
		m_zoomFullyActiveTime = gpGlobals->curtime + 0.15f;
	}

	BaseClass::ItemPostFrame();
}

float CWeaponScout::GetInaccuracy() const
{
	if ( weapon_accuracy_model.GetInt() == 1 )
	{
		CCSPlayer *pPlayer = GetPlayerOwner();
		if (pPlayer == NULL)
			return 0.0f;
	
		float fSpread = 0.0f;
	
		if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
			fSpread = 0.2f;
		else if (pPlayer->GetAbsVelocity().Length2D() > 170)
			fSpread = 0.075f;
		else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
			fSpread = 0.0f;
		else
			fSpread = 0.007f;
	
		// If we are not zoomed in, or we have very recently zoomed and are still transitioning, the bullet diverts more.
		if (pPlayer->GetFOV() == pPlayer->GetDefaultFOV() || (gpGlobals->curtime < m_zoomFullyActiveTime))
		{
			fSpread += 0.025;
		}
	
		return fSpread;
	}
	else
		return BaseClass::GetInaccuracy();
}

void CWeaponScout::PrimaryAttack( void )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if (pPlayer == NULL)
		return;

	if ( !CSBaseGunFire( GetCSWpnData().m_flCycleTime, m_weaponMode ) )
		return;

	if ( m_weaponMode == Secondary_Mode )
	{	
		float	midFOVdistance = fabs( pPlayer->GetFOV() - (float)cScoutMidZoomFOV );
		float	farFOVdistance = fabs( pPlayer->GetFOV() - (float)cScoutMaxZoomFOV );

		if ( midFOVdistance < farFOVdistance )
		{
			pPlayer->m_iLastZoom = cScoutMidZoomFOV;
		}
		else
		{
			pPlayer->m_iLastZoom = cScoutMaxZoomFOV;
		}
		
// 		#ifndef CLIENT_DLL
			pPlayer->m_bResumeZoom = true;
			pPlayer->SetFOV( pPlayer, pPlayer->GetDefaultFOV(), 0.05f );
			m_weaponMode = Primary_Mode;
// 		#endif
	}

	QAngle angle = pPlayer->GetPunchAngle();
	angle.x -= 2;
	pPlayer->SetPunchAngle( angle );
}


float CWeaponScout::GetMaxSpeed() const
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if (pPlayer == NULL)
	{
		Assert(pPlayer != NULL);
		return BaseClass::GetMaxSpeed();
	}

	if ( pPlayer->GetFOV() == pPlayer->GetDefaultFOV() )
		return BaseClass::GetMaxSpeed();
	else
		return 220;	// zoomed in.
}


bool CWeaponScout::Reload()
{
	m_weaponMode = Primary_Mode;
	return BaseClass::Reload();

}

bool CWeaponScout::Deploy()
{
	m_weaponMode = Primary_Mode;
	return BaseClass::Deploy();
}
