//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hegrenade_projectile.h"
#include "soundent.h"
#include "cs_player.h"
#include "KeyValues.h"
#include "weapon_csbase.h"

#define GRENADE_MODEL "models/w_models/weapons/w_eq_pipebomb.mdl"

static ConVar PipeBombBeepIntervalDelta( "PipeBombBeepIntervalDelta", "0.05", FCVAR_REPLICATED | FCVAR_NOTIFY, "How much the pipebomb beep interval shrinks each beep." );
static ConVar PipeBombBeepMinInterval( "PipeBombBeepMinInterval", "0.1", FCVAR_REPLICATED | FCVAR_NOTIFY, "Minimum pipebomb beep interval." );
static const char *s_szPipeBombBeepThinkContext = "PipeBombBeepThink";

LINK_ENTITY_TO_CLASS( hegrenade_projectile, CHEGrenadeProjectile );
PRECACHE_WEAPON_REGISTER( hegrenade_projectile );

CHEGrenadeProjectile* CHEGrenadeProjectile::Create( 
	const Vector &position, 
	const QAngle &angles, 
	const Vector &velocity, 
	const AngularImpulse &angVelocity, 
	CBaseCombatCharacter *pOwner, 
	float timer )
{
	CHEGrenadeProjectile *pGrenade = (CHEGrenadeProjectile*)CBaseEntity::Create( "hegrenade_projectile", position, angles, pOwner );
	
	pGrenade->SetDetonateTimerLength( timer );
	pGrenade->SetAbsVelocity( velocity );
	pGrenade->SetupInitialTransmittedGrenadeVelocity( velocity );
	pGrenade->SetThrower( pOwner ); 

	pGrenade->SetGravity( BaseClass::GetGrenadeGravity() );
	pGrenade->SetFriction( BaseClass::GetGrenadeFriction() );
	pGrenade->SetElasticity( BaseClass::GetGrenadeElasticity() );

	pGrenade->m_flDamage = 100;
	pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;
	pGrenade->ChangeTeam( pOwner->GetTeamNumber() );
	pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );	

	// make NPCs afaid of it while in the air
	pGrenade->SetThink( &CHEGrenadeProjectile::DangerSoundThink );
	pGrenade->SetNextThink( gpGlobals->curtime );

	pGrenade->m_pWeaponInfo = GetWeaponInfo( WEAPON_HEGRENADE );

	pGrenade->m_bDisabled = false;
	pGrenade->m_flBeepInterval = 0.6f;
	pGrenade->m_flNextBeepTime = gpGlobals->curtime + pGrenade->m_flBeepInterval;
	pGrenade->SetContextThink( &CHEGrenadeProjectile::BeepThink, pGrenade->m_flNextBeepTime, s_szPipeBombBeepThinkContext );
	pGrenade->AddFlag(FL_OBJECT);
	return pGrenade;
}

void CHEGrenadeProjectile::BeepThink()
{
	// If the pipe bomb is disabled, stop thinking
	if ( m_bDisabled )
	{
		SetContextThink( NULL, TICK_NEVER_THINK, s_szPipeBombBeepThinkContext );
		return;
	}

	// Play the beep sound
	EmitSound( "PipeBomb.TimerBeep", m_flNextBeepTime );

	// Reduce the beep interval so the beeps get faster
	float newInterval = m_flBeepInterval - PipeBombBeepIntervalDelta.GetFloat();

	// Clamp to minimum interval
	float minInterval = PipeBombBeepMinInterval.GetFloat();
	if ( newInterval < minInterval )
		newInterval = minInterval;

	m_flBeepInterval = newInterval;

	// Schedule next beep
	m_flNextBeepTime += newInterval;

	// Schedule next think
	SetContextThink( &CHEGrenadeProjectile::BeepThink, m_flNextBeepTime, s_szPipeBombBeepThinkContext );
}

void CHEGrenadeProjectile::Spawn()
{
	SetModel( GRENADE_MODEL );
	BaseClass::Spawn();
}

void CHEGrenadeProjectile::Precache()
{
	PrecacheModel( GRENADE_MODEL );

	PrecacheScriptSound( "HEGrenade.Bounce" );
	PrecacheScriptSound( "PipeBomb.TimerBeep" );

	BaseClass::Precache();
}

void CHEGrenadeProjectile::BounceSound( void )
{
	EmitSound( "HEGrenade.Bounce" );
}

void CHEGrenadeProjectile::Detonate()
{
	BaseClass::Detonate();

	// tell the bots an HE grenade has exploded
	CCSPlayer *player = ToCSPlayer(GetThrower());
	if ( player )
	{
		IGameEvent * event = gameeventmanager->CreateEvent( "hegrenade_detonate" );
		if ( event )
		{
			event->SetInt( "userid", player->GetUserID() );
			event->SetFloat( "x", GetAbsOrigin().x );
			event->SetFloat( "y", GetAbsOrigin().y );
			event->SetFloat( "z", GetAbsOrigin().z );
			gameeventmanager->FireEvent( event );
		}
	}
}
