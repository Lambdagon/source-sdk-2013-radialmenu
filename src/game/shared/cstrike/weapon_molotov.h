//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef WEAPON_MOLOTOV_CS_H
#define WEAPON_MOLOTOV_CS_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_hegrenade.h"

#if defined( CLIENT_DLL )

	#define CMolotovGrenade C_MolotovGrenade

#endif

//-----------------------------------------------------------------------------
// Molotov grenade weapon (spawns grenade_molotov entity on throw)
//-----------------------------------------------------------------------------
class CMolotovGrenade : public CHEGrenade
{
public:
	DECLARE_CLASS( CMolotovGrenade, CHEGrenade );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CMolotovGrenade() {}

#ifndef CLIENT_DLL
	DECLARE_DATADESC();

	virtual void Precache( void );
	virtual void EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer );
#endif

private:
	CMolotovGrenade( const CMolotovGrenade & ) {}
};

#endif // WEAPON_MOLOTOV_CS_H
