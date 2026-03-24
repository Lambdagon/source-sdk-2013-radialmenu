//========= Copyright (c) 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=====================================================================================//

#include "cbase.h"

#if defined ( GAME_DLL )
	#include "terror/inferno.h"
	#define INFERNOCLASS		CInferno
	#define FIRECRACKERBLASTCLASS	CFireCrackerBlast
#endif

#if defined( CLIENT_DLL )
	#include "cstrike/clientinferno.h"
	#define INFERNOCLASS		C_Inferno
	#define FIRECRACKERBLASTCLASS	C_FireCrackerBlast
#endif

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

//---------------------------------------------------------
const char *INFERNOCLASS::GetParticleEffectName()
{
	switch ( GetInfernoType() )
	{
	case INFERNO_TYPE_SPITTER_ACID:
		return "spitter_areaofdenial";
	default:
		return "molotov_groundfire";
	}
}
#if defined( GAME_DLL )
const char *INFERNOCLASS::GetImpactParticleEffectName()
{
	switch ( GetInfernoType() )
	{
	case INFERNO_TYPE_SPITTER_ACID:
		return "spitter_areaofdenial";
	default:
		return "molotov_explosion";
	}
}
#endif


//---------------------------------------------------------
const char *FIRECRACKERBLASTCLASS::GetParticleEffectName()
{
	return "firework_crate_ground_effect";
}
#if defined( GAME_DLL )
const char *FIRECRACKERBLASTCLASS::GetImpactParticleEffectName()
{
	return "firework_crate_explosion_01";
}
#endif
