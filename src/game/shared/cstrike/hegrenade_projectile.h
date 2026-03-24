//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HEGRENADE_PROJECTILE_H
#define HEGRENADE_PROJECTILE_H
#ifdef _WIN32
#pragma once
#endif

#include "basecsgrenade_projectile.h"

class CHEGrenadeProjectile : public CBaseCSGrenadeProjectile
{
public:
	DECLARE_CLASS( CHEGrenadeProjectile, CBaseCSGrenadeProjectile );


// Overrides.
public:
	virtual void Spawn();
	virtual void Precache();
	virtual void BounceSound( void );
	virtual void Detonate();

#ifndef CLIENT_DLL
	void BeepThink();
#endif

// Grenade stuff.
public:

	static CHEGrenadeProjectile* Create( 
		const Vector &position, 
		const QAngle &angles, 
		const Vector &velocity, 
		const AngularImpulse &angVelocity, 
		CBaseCombatCharacter *pOwner, 
		float timer );

private:
#ifndef CLIENT_DLL
	bool m_bDisabled = false;
	float m_flBeepInterval = 0.5f;
	float m_flNextBeepTime = 0.0f;
#endif
};


#endif // HEGRENADE_PROJECTILE_H
