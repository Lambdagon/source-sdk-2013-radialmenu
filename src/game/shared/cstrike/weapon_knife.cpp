//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_knife.h"
#include "cs_gamerules.h"
#include "IEffects.h"
#include "particle_parse.h"
#include "in_buttons.h"
#include "fx_cs_shared.h"

#if defined( CLIENT_DLL )
	#include "c_cs_player.h"
#else
	#include "cs_player.h"
	#include "ilagcompensationmanager.h"
	#include "cs_gamestats.h"
	#include "ndebugoverlay.h"
	#include "basegrenade_shared.h"
	#include "terror/inferno.h"
	#include "mp_shareddefs.h"
#endif
#include "cs_playeranimstate.h"

#define	KNIFE_BODYHIT_VOLUME 128
#define	KNIFE_WALLHIT_VOLUME 512


Vector head_hull_mins( -16, -16, -18 );
Vector head_hull_maxs( 16, 16, 18 );

#ifndef CLIENT_DLL
ConVar z_vomit_boxsize( "z_vomit_boxsize", "1.0", FCVAR_GAMEDLL | FCVAR_CHEAT, "Size of vomit damage entities." );
ConVar z_vomit_drag( "z_vomit_drag", "0.89", FCVAR_GAMEDLL | FCVAR_CHEAT, "Air drag of vomit damage entities." );
ConVar z_vomit_float( "z_vomit_float", "-130.0", FCVAR_GAMEDLL | FCVAR_CHEAT, "Upward float velocity of vomit damage entities." );
ConVar z_vomit_fatigue( "z_vomit_fatigue", "3000", FCVAR_GAMEDLL | FCVAR_CHEAT, "Stamina impact of puking. High number will pin in place for a long time, lower will just slow." );
ConVar z_vomit_hit_pitch_max( "z_vomit_hit_pitch_max", "15", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_vomit_hit_pitch_min( "z_vomit_hit_pitch_min", "-15", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_vomit_hit_yaw_max( "z_vomit_hit_yaw_max", "10", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_vomit_hit_yaw_min( "z_vomit_hit_yaw_min", "-10", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_vomit_lifetime( "z_vomit_lifetime", "0.5", FCVAR_GAMEDLL | FCVAR_CHEAT, "Time to live of vomit damage entities." );
ConVar z_vomit_maxdamagedist( "z_vomit_maxdamagedist", "350.0", FCVAR_GAMEDLL | FCVAR_CHEAT, "Maximum damage distance for vomit." );
ConVar z_vomit_vecrand( "z_vomit_vecrand", "0.05", FCVAR_GAMEDLL | FCVAR_CHEAT, "Random vector added to initial velocity of vomit damage entities." );
ConVar z_vomit_velocity( "z_vomit_velocity", "1700.0", FCVAR_GAMEDLL | FCVAR_CHEAT, "Initial velocity of vomit damage entities." );
ConVar z_vomit_velocityfadeend( "z_vomit_velocityfadeend", ".5", FCVAR_GAMEDLL | FCVAR_CHEAT, "Time at which attacker's velocity contribution finishes fading." );
ConVar z_vomit_velocityfadestart( "z_vomit_velocityfadestart", ".3", FCVAR_GAMEDLL | FCVAR_CHEAT, "Time at which attacker's velocity contribution starts to fade." );

ConVar z_spit_velocity( "z_spit_velocity", "900.0", FCVAR_GAMEDLL | FCVAR_CHEAT, "Initial velocity of spitter spit projectile." );
ConVar z_spit_gravity( "z_spit_gravity", "0.8", FCVAR_GAMEDLL | FCVAR_CHEAT, "Gravity multiplier for spitter spit projectile." );
ConVar z_spit_lifetime( "z_spit_lifetime", "5.0", FCVAR_GAMEDLL | FCVAR_CHEAT, "Time to live of spitter spit projectile." );
#endif

#ifdef CLIENT_DLL
ConVar z_vomit_debug( "z_vomit_debug", "0", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "Visualize the vomit damage." );
ConVar z_vomit_duration( "z_vomit_duration", "1.5", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How long a puker continuously pukes for." );
ConVar z_vomit_fade_duration( "z_vomit_fade_duration", "5", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How long the fade takes" );
ConVar z_vomit_fade_start( "z_vomit_fade_start", "5", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "When the vomit starts to fade away" );
ConVar z_vomit_interval( "z_vomit_interval", "30", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How often a puker can puke." );
ConVar z_vomit_range( "z_vomit_range", "300", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_vomit_slide_mult( "z_vomit_slide_mult", "0.5", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "Multiplier for second texture slide rate" );
ConVar z_vomit_slide_rate( "z_vomit_slide_rate", "0.1", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "Percentage of screen height per second" );
ConVar z_vomit_target_dot( "z_vomit_target_dot", "0.6", FCVAR_CLIENTDLL | FCVAR_CHEAT, "" );
ConVar z_vomit_target_range( "z_vomit_target_range", "280", FCVAR_CLIENTDLL | FCVAR_CHEAT, "" );
ConVar z_spit_interval( "z_spit_interval", "30", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How often a spitter can spit." );
ConVar z_spit_target_range( "z_spit_target_range", "500", FCVAR_CLIENTDLL | FCVAR_CHEAT, "Maximum range for spitter spit." );
#else
ConVar z_vomit_debug( "z_vomit_debug", "0", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "Visualize the vomit damage." );
ConVar z_vomit_duration( "z_vomit_duration", "1.5", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How long a puker continuously pukes for." );
ConVar z_vomit_fade_duration( "z_vomit_fade_duration", "5", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How long the fade takes" );
ConVar z_vomit_fade_start( "z_vomit_fade_start", "5", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "When the vomit starts to fade away" );
ConVar z_vomit_interval( "z_vomit_interval", "30", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How often a puker can puke." );
ConVar z_vomit_range( "z_vomit_range", "300", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_vomit_slide_mult( "z_vomit_slide_mult", "0.5", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "Multiplier for second texture slide rate" );
ConVar z_vomit_slide_rate( "z_vomit_slide_rate", "0.1", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "Percentage of screen height per second" );
ConVar z_vomit_target_dot( "z_vomit_target_dot", "0.6", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_vomit_target_range( "z_vomit_target_range", "280", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_spit_interval( "z_spit_interval", "30", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "How often a spitter can spit." );
ConVar z_spit_target_range( "z_spit_target_range", "500", FCVAR_GAMEDLL | FCVAR_CHEAT, "Maximum range for spitter spit." );
#endif

// ----------------------------------------------------------------------------- //
// Hunter / Lunge convars
// ----------------------------------------------------------------------------- //

#ifdef CLIENT_DLL
ConVar z_hunter_ground_normal( "z_hunter_ground_normal", "0.2", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_hunter_lunge_distance( "z_hunter_lunge_distance", "750", FCVAR_CLIENTDLL, "Distance at which bot hunters will try to lunge" );
ConVar z_hunter_lunge_pitch( "z_hunter_lunge_pitch", "25", FCVAR_CLIENTDLL, "Extra pitch bot hunters will lunge with at their max range (goes to 0 when bots are lunging up close)" );
ConVar z_hunter_lunge_stagger_time( "z_hunter_lunge_stagger_time", "1", FCVAR_CLIENTDLL | FCVAR_CHEAT, "" );
ConVar z_hunter_max_pounce_bonus_damage( "z_hunter_max_pounce_bonus_damage", "24", FCVAR_CLIENTDLL | FCVAR_CHEAT, "" );
ConVar z_lunge_cooldown( "z_lunge_cooldown", "0.1", FCVAR_CLIENTDLL | FCVAR_REPLICATED, "Cooldown after lunge where zombies can't attack" );
ConVar z_lunge_interval( "z_lunge_interval", "0.1", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_lunge_power( "z_lunge_power", "600", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_lunge_reflect( "z_lunge_reflect", "0", FCVAR_CLIENTDLL | FCVAR_REPLICATED, "Reflects wall-kick lunges" );
ConVar z_lunge_up( "z_lunge_up", "200", FCVAR_CLIENTDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
#else
ConVar z_hunter_ground_normal( "z_hunter_ground_normal", "0.2", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_hunter_lunge_distance( "z_hunter_lunge_distance", "750", FCVAR_GAMEDLL, "Distance at which bot hunters will try to lunge" );
ConVar z_hunter_lunge_pitch( "z_hunter_lunge_pitch", "25", FCVAR_GAMEDLL, "Extra pitch bot hunters will lunge with at their max range (goes to 0 when bots are lunging up close)" );
ConVar z_hunter_lunge_stagger_time( "z_hunter_lunge_stagger_time", "1", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_hunter_max_pounce_bonus_damage( "z_hunter_max_pounce_bonus_damage", "24", FCVAR_GAMEDLL | FCVAR_CHEAT, "" );
ConVar z_lunge_cooldown( "z_lunge_cooldown", "0.1", FCVAR_GAMEDLL | FCVAR_REPLICATED, "Cooldown after lunge where zombies can't attack" );
ConVar z_lunge_interval( "z_lunge_interval", "0.1", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_lunge_power( "z_lunge_power", "600", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar z_lunge_reflect( "z_lunge_reflect", "0", FCVAR_GAMEDLL | FCVAR_REPLICATED, "Reflects wall-kick lunges" );
ConVar z_lunge_up( "z_lunge_up", "200", FCVAR_GAMEDLL | FCVAR_CHEAT | FCVAR_REPLICATED, "" );
#endif

#ifndef CLIENT_DLL
	//-----------------------------------------------------------------------------
	// Purpose: Only send to local player if this weapon is the active weapon
	// Input  : *pStruct - 
	//			*pVarData - 
	//			*pRecipients - 
	//			objectID - 
	// Output : void*
	//-----------------------------------------------------------------------------
	void* SendProxy_SendActiveLocalKnifeDataTable( const SendProp *pProp, const void *pStruct, const void *pVarData, CSendProxyRecipients *pRecipients, int objectID )
	{
		// Get the weapon entity
		CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)pVarData;
		if ( pWeapon )
		{
			// Only send this chunk of data to the player carrying this weapon
			CBasePlayer *pPlayer = ToBasePlayer( pWeapon->GetOwner() );
			if ( pPlayer /*&& pPlayer->GetActiveWeapon() == pWeapon*/ )
			{
				pRecipients->SetOnly( pPlayer->GetClientIndex() );
				return (void*)pVarData;
			}
		}
		
		return NULL;
	}
	REGISTER_SEND_PROXY_NON_MODIFIED_POINTER( SendProxy_SendActiveLocalKnifeDataTable );
#endif

// ----------------------------------------------------------------------------- //
// CKnife tables.
// ----------------------------------------------------------------------------- //
	
IMPLEMENT_NETWORKCLASS_ALIASED( Knife, DT_WeaponKnife )


BEGIN_NETWORK_TABLE_NOBASE( CKnife, DT_LocalActiveWeaponKnifeData )
	#if !defined( CLIENT_DLL )
		SendPropTime( SENDINFO( m_flSmackTime ) ),
	#else
		RecvPropTime( RECVINFO( m_flSmackTime ) ),
	#endif
END_NETWORK_TABLE()


BEGIN_NETWORK_TABLE( CKnife, DT_WeaponKnife )
	#if !defined( CLIENT_DLL )
		SendPropDataTable("LocalActiveWeaponKnifeData", 0, &REFERENCE_SEND_TABLE(DT_LocalActiveWeaponKnifeData), SendProxy_SendActiveLocalKnifeDataTable ),
	#else
		RecvPropDataTable("LocalActiveWeaponKnifeData", 0, 0, &REFERENCE_RECV_TABLE(DT_LocalActiveWeaponKnifeData)),
	#endif
END_NETWORK_TABLE()


#if defined CLIENT_DLL
BEGIN_PREDICTION_DATA( CKnife )
	DEFINE_PRED_FIELD( m_flSmackTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif


LINK_ENTITY_TO_CLASS(weapon_knife, CKnife);
PRECACHE_WEAPON_REGISTER(weapon_knife);
CREATE_SIMPLE_WEAPON_TABLE(HunterClaw, weapon_hunter_claw);
CREATE_SIMPLE_WEAPON_TABLE(TankClaw, weapon_tank_claw);
CREATE_SIMPLE_WEAPON_TABLE(BoomerClaw, weapon_boomer_claw);
CREATE_SIMPLE_WEAPON_TABLE(SmokerClaw, weapon_smoker_claw);
CREATE_SIMPLE_WEAPON_TABLE(ChargerClaw, weapon_charger_claw);
CREATE_SIMPLE_WEAPON_TABLE(JockeyClaw, weapon_jockey_claw);
CREATE_SIMPLE_WEAPON_TABLE(SpitterClaw, weapon_spitter_claw);

#ifndef CLIENT_DLL

	BEGIN_DATADESC( CKnife )
		DEFINE_THINKFUNC( Smack )
	END_DATADESC()

	class CVomitDamage : public CBaseEntity
	{
	public:
		DECLARE_CLASS( CVomitDamage, CBaseEntity );
		DECLARE_DATADESC();

		static CVomitDamage *Create( CCSPlayer *owner, const Vector &startPos, const QAngle &angles );

		void Spawn() OVERRIDE;
		void VomitThink();

	private:
		EHANDLE m_hVomitOwner;
		Vector m_vecBaseVelocity;
		Vector m_vecOwnerVelocity;
		float m_flSpawnTime;
		float m_flDieTime;
	};

	LINK_ENTITY_TO_CLASS( vomit_damage, CVomitDamage );

	BEGIN_DATADESC( CVomitDamage )
		DEFINE_FIELD( m_hVomitOwner, FIELD_EHANDLE ),
		DEFINE_FIELD( m_vecBaseVelocity, FIELD_VECTOR ),
		DEFINE_FIELD( m_vecOwnerVelocity, FIELD_VECTOR ),
		DEFINE_FIELD( m_flSpawnTime, FIELD_TIME ),
		DEFINE_FIELD( m_flDieTime, FIELD_TIME ),
		DEFINE_THINKFUNC( VomitThink ),
	END_DATADESC()

	CVomitDamage *CVomitDamage::Create( CCSPlayer *owner, const Vector &startPos, const QAngle &angles )
	{
		if ( !owner )
			return NULL;

		CVomitDamage *vomit = dynamic_cast< CVomitDamage * >( CreateEntityByName( "vomit_damage" ) );
		if ( !vomit )
			return NULL;

		vomit->m_hVomitOwner = owner;
		vomit->SetAbsOrigin( startPos );
		vomit->SetAbsAngles( angles );

		DispatchSpawn( vomit );
		vomit->Activate();

		return vomit;
	}

	void CVomitDamage::Spawn()
	{
		BaseClass::Spawn();

		SetMoveType( MOVETYPE_NONE );
		SetSolid( SOLID_BBOX );
		SetCollisionGroup( COLLISION_GROUP_PROJECTILE );
		AddSolidFlags( FSOLID_NOT_STANDABLE );

		// Server-only damage helper entity; don't network it.
		AddEFlags( EFL_DONTBLOCKLOS );
		SetTransmitState( FL_EDICT_DONTSEND );

		const float boxScale = MAX( 0.05f, z_vomit_boxsize.GetFloat() );
		const float halfSize = 4.0f * boxScale;
		UTIL_SetSize( this, -Vector( halfSize, halfSize, halfSize ), Vector( halfSize, halfSize, halfSize ) );

		CCSPlayer *owner = ToCSPlayer( m_hVomitOwner.Get() );
		Vector ownerVel = owner ? owner->GetAbsVelocity() : vec3_origin;
		m_vecOwnerVelocity = ownerVel;

		Vector forward;
		AngleVectors( GetAbsAngles(), &forward );

		const float speed = z_vomit_velocity.GetFloat();
		m_vecBaseVelocity = forward * speed;

		const float randAmt = MAX( 0.0f, z_vomit_vecrand.GetFloat() );
		if ( randAmt > 0.0f )
		{
			m_vecBaseVelocity.x += random->RandomFloat( -1.0f, 1.0f ) * randAmt * speed;
			m_vecBaseVelocity.y += random->RandomFloat( -1.0f, 1.0f ) * randAmt * speed;
			m_vecBaseVelocity.z += random->RandomFloat( -1.0f, 1.0f ) * randAmt * speed;
		}

		m_flSpawnTime = gpGlobals->curtime;
		m_flDieTime = gpGlobals->curtime + MAX( 0.01f, z_vomit_lifetime.GetFloat() );

		SetThink( &CVomitDamage::VomitThink );
		SetNextThink( gpGlobals->curtime );
	}

	void CVomitDamage::VomitThink()
	{
		if ( gpGlobals->curtime >= m_flDieTime )
		{
			UTIL_Remove( this );
			return;
		}

		const float dt = 0.02f;

		// Apply drag to our base velocity and a constant vertical "float".
		const float drag = clamp( z_vomit_drag.GetFloat(), 0.0f, 1.0f );
		m_vecBaseVelocity *= drag;
		m_vecBaseVelocity.z += z_vomit_float.GetFloat() * dt;

		// Fade out owner's velocity contribution.
		const float elapsed = gpGlobals->curtime - m_flSpawnTime;
		const float fadeStart = MAX( 0.0f, z_vomit_velocityfadestart.GetFloat() );
		const float fadeEnd = MAX( fadeStart, z_vomit_velocityfadeend.GetFloat() );

		float ownerFactor = 1.0f;
		if ( elapsed >= fadeEnd )
			ownerFactor = 0.0f;
		else if ( elapsed > fadeStart && fadeEnd > fadeStart )
			ownerFactor = RemapValClamped( elapsed, fadeStart, fadeEnd, 1.0f, 0.0f );

		const Vector vel = m_vecBaseVelocity + ( m_vecOwnerVelocity * ownerFactor );

		const Vector curPos = GetAbsOrigin();
		const Vector nextPos = curPos + vel * dt;

		const Vector localMins = CollisionProp()->OBBMins();
		const Vector localMaxs = CollisionProp()->OBBMaxs();

		trace_t tr;
		CCSPlayer *owner = ToCSPlayer( m_hVomitOwner.Get() );
		UTIL_TraceHull( curPos, nextPos, localMins, localMaxs, MASK_SHOT_HULL, owner, COLLISION_GROUP_PROJECTILE, &tr );

		if ( z_vomit_debug.GetBool() )
		{
			NDebugOverlay::Box( curPos, localMins, localMaxs, 0, 255, 0, 32, dt );
		}

		if ( tr.startsolid || tr.allsolid || tr.fraction < 1.0f )
		{
			CBaseEntity *hit = tr.m_pEnt;
			CCSPlayer *victim = ToCSPlayer( hit );
			if ( victim && victim->IsAlive() && victim->GetTeamNumber() == TEAM_SURVIVOR && owner )
			{
				const float maxDist = MAX( 0.0f, MIN( z_vomit_maxdamagedist.GetFloat(), z_vomit_range.GetFloat() ) );
				if ( ( victim->GetAbsOrigin() - owner->GetAbsOrigin() ).IsLengthLessThan( maxDist ) )
				{
					CSGameRules()->OnSurvivorVomited( victim, owner );
				}
			}

			UTIL_Remove( this );
			return;
		}

		SetAbsOrigin( nextPos );
		SetNextThink( gpGlobals->curtime + dt );
	}

	//-----------------------------------------------------------------------------
	// Spitter spit projectile - spawns an Inferno of type INFERNO_TYPE_SPITTER_ACID.
	//-----------------------------------------------------------------------------
	class CSpitterSpitProjectile : public CBaseGrenade
	{
	public:
		DECLARE_CLASS( CSpitterSpitProjectile, CBaseGrenade );
		DECLARE_DATADESC();

		static CSpitterSpitProjectile *Create( CCSPlayer *owner, const Vector &startPos, const QAngle &angles, const Vector &velocity );

		void Spawn() OVERRIDE;
		void Precache() OVERRIDE;

		void SpitTouch( CBaseEntity *pOther );
		void SpitThink();

	private:
		float m_flDieTime;
	};

	LINK_ENTITY_TO_CLASS( spitter_spit, CSpitterSpitProjectile );

	BEGIN_DATADESC( CSpitterSpitProjectile )
		DEFINE_FIELD( m_flDieTime, FIELD_TIME ),
		DEFINE_ENTITYFUNC( SpitTouch ),
		DEFINE_THINKFUNC( SpitThink ),
	END_DATADESC()

	CSpitterSpitProjectile *CSpitterSpitProjectile::Create( CCSPlayer *owner, const Vector &startPos, const QAngle &angles, const Vector &velocity )
	{
		if ( !owner )
			return NULL;

		CSpitterSpitProjectile *spit = dynamic_cast< CSpitterSpitProjectile * >( CreateEntityByName( "spitter_spit" ) );
		if ( !spit )
			return NULL;

		spit->SetAbsOrigin( startPos );
		spit->SetAbsAngles( angles );
		spit->SetOwnerEntity( owner );
		spit->SetThrower( owner );

		DispatchSpawn( spit );
		spit->SetAbsVelocity( velocity );
		spit->Activate();

		return spit;
	}

	void CSpitterSpitProjectile::Precache()
	{
		BaseClass::Precache();
		PrecacheModel( "models/infected/limbs/exploded_boomer_steak1.mdl" );
		PrecacheParticleSystem("spitter_projectile");
	}

	void CSpitterSpitProjectile::Spawn()
	{
		Precache();

		SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
		SetSolid( SOLID_BBOX );
		SetCollisionGroup( COLLISION_GROUP_PROJECTILE );
		AddSolidFlags( FSOLID_NOT_STANDABLE );

		SetModel( "models/infected/limbs/exploded_boomer_steak1.mdl" );
		UTIL_SetSize( this, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ) );

		SetGravity( clamp( z_spit_gravity.GetFloat(), 0.05f, 5.0f ) );
		SetFriction( 0.8f );

		SetTouch( &CSpitterSpitProjectile::SpitTouch );
		SetThink( &CSpitterSpitProjectile::SpitThink );

		m_flDieTime = gpGlobals->curtime + MAX( 0.1f, z_spit_lifetime.GetFloat() );
		SetNextThink( gpGlobals->curtime + 0.05f );
		DispatchParticleEffect("spitter_projectile", PATTACH_ABSORIGIN_FOLLOW, this, 0);
	}

	void CSpitterSpitProjectile::SpitThink()
	{
		if ( gpGlobals->curtime >= m_flDieTime )
		{
			UTIL_Remove( this );
			return;
		}

		SetNextThink( gpGlobals->curtime + 0.05f );
	}

	void CSpitterSpitProjectile::SpitTouch( CBaseEntity *pOther )
	{
		if ( !pOther )
			return;

		// Ignore triggers/volumes.
		if ( pOther->IsSolidFlagSet( FSOLID_VOLUME_CONTENTS | FSOLID_TRIGGER ) )
		{
			if ( ( pOther->m_takedamage == DAMAGE_NO ) || ( pOther->m_takedamage == DAMAGE_EVENTS_ONLY ) )
				return;
		}

		// Don't immediately collide with the thrower.
		if ( pOther == GetThrower() )
			return;

		const trace_t *pTrace = &CBaseEntity::GetTouchTrace();
		Vector impactPos = pTrace ? pTrace->endpos : GetAbsOrigin();
		Vector impactNormal = pTrace ? pTrace->plane.normal : Vector( 0, 0, 1 );

		// Try to find the ground below the impact point so we always create a usable puddle.
		trace_t ground;
		UTIL_TraceLine( impactPos + Vector( 0, 0, 8.0f ), impactPos - Vector( 0, 0, 256.0f ), MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &ground );
		if ( ground.fraction < 1.0f )
		{
			impactPos = ground.endpos;
			impactNormal = ground.plane.normal;
		}

		const int contents = UTIL_PointContents( impactPos );
		if ( contents & MASK_WATER )
		{
			UTIL_Remove( this );
			return;
		}

		CInferno *inferno = (CInferno*)CBaseEntity::Create( "inferno", impactPos, QAngle( 0, 0, 0 ), GetThrower() );
		if ( inferno )
		{
			inferno->SetOwnerEntity( GetThrower() );
			inferno->SetMaxFlames( 8 );
			inferno->SetInfernoType( INFERNO_TYPE_SPITTER_ACID );
			inferno->StartBurning( impactPos, impactNormal, GetAbsVelocity() );
		}

		UTIL_Remove( this );
	}

	static void SpitterRandomSurvivorWarnIncoming( CCSPlayer *pSpitter )
	{
		if ( !pSpitter || !pSpitter->IsAlive() )
			return;

		CUtlVector< CCSPlayer* > candidates;
		const float maxDist = 1500.0f;
		const float maxDistSqr = maxDist * maxDist;
		const Vector spitPos = pSpitter->GetAbsOrigin();

		for ( int i = 1; i <= gpGlobals->maxClients; ++i )
		{
			CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
			if ( !pPlayer || !pPlayer->IsAlive() )
				continue;
			if ( pPlayer->GetTeamNumber() != TEAM_SURVIVOR )
				continue;

			if ( ( pPlayer->GetAbsOrigin() - spitPos ).LengthSqr() <= maxDistSqr )
			{
				candidates.AddToTail( pPlayer );
			}
		}

		if ( candidates.Count() <= 0 )
			return;

		CCSPlayer *speaker = candidates[ random->RandomInt( 0, candidates.Count() - 1 ) ];
		if ( speaker )
		{
			speaker->SpeakConceptIfAllowed( GetMPConceptIndexFromString( "WarnSpitterIncoming" ) );
		}
	}

#endif

// ----------------------------------------------------------------------------- //
// CKnife implementation.
// ----------------------------------------------------------------------------- //

CKnife::CKnife()
{
}

CBoomerClaw::CBoomerClaw()
{
#ifndef CLIENT_DLL
	m_flNextVomitAllowedTime = 0.0f;
	m_flVomitEndTime = 0.0f;
	m_flNextVomitBlobTime = 0.0f;
#endif
}

CSmokerClaw::CSmokerClaw()
{
}

CHunterClaw::CHunterClaw()
{
#ifndef CLIENT_DLL
	m_hBotLungeTarget = NULL;
	m_flBotLungeTargetSetTime = 0.0f;
	m_flNextLungeAllowedTime = 0.0f;
	m_flLungeStartTime = 0.0f;
	m_flPounceStartZ = 0.0f;
	m_flNextWallKickTime = 0.0f;
	m_bIsLunging = false;
	m_bIsPouncing = false;
	m_bDidPounceHit = false;
	m_bPendingLandingDelay = false;
#endif
}

#ifndef CLIENT_DLL
void CHunterClaw::SetBotLungeTarget( CBaseEntity *target )
{
	m_hBotLungeTarget = target;
	m_flBotLungeTargetSetTime = gpGlobals ? gpGlobals->curtime : 0.0f;
}
#endif

CTankClaw::CTankClaw()
{
}

CChargerClaw::CChargerClaw()
{
}

CJockeyClaw::CJockeyClaw()
{
}

CSpitterClaw::CSpitterClaw()
{
#ifndef CLIENT_DLL
	m_flNextSpitAllowedTime = 0.0f;
#endif
}

bool CKnife::HasPrimaryAmmo()
{
	return true;
}


bool CKnife::CanBeSelected()
{
	return true;
}

void CKnife::Precache()
{
	BaseClass::Precache();

	PrecacheScriptSound("Weapon_Knife.Deploy");
	PrecacheScriptSound("Weapon_Knife.Slash");
	PrecacheScriptSound("Weapon_Knife.Stab");
	PrecacheScriptSound("Weapon_Knife.Hit");
	PrecacheScriptSound("Claw.Hit");
	PrecacheScriptSound("Claw.HitFlesh");
	PrecacheScriptSound("HulkZombie.Punch");
	PrecacheParticleSystem( "boomer_vomit" );

#ifndef CLIENT_DLL
	const char *classname = GetClassname();
	if ( classname && V_stristr( classname, "_claw" ) )
	{
		PrecacheModel( "models/empty.mdl" );
	}
#endif
}

void CKnife::Spawn()
{
	Precache();

	m_iClip1 = -1;
	BaseClass::Spawn();

#ifndef CLIENT_DLL
	const char *classname = GetClassname();
	if ( classname && V_stristr( classname, "_claw" ) )
	{
		SetModel( "models/empty.mdl" );
		AddEffects( EF_NODRAW );
	}
#endif
}


bool CKnife::Deploy()
{
	// not this time, srry - siobhan
	//CPASAttenuationFilter filter( this );
	//filter.UsePredictionRules();
	//EmitSound( filter, entindex(), "Weapon_Knife.Deploy" );

	return BaseClass::Deploy();
}

void CKnife::Holster( int skiplocal )
{
	if ( GetPlayerOwner() )
	{
		GetPlayerOwner()->m_flNextAttack = gpGlobals->curtime + 0.5;
	}
}

void CKnife::WeaponAnimation ( int iAnimation )
{
	/*
	int flag;
	#if defined( CLIENT_WEAPONS )
		flag = FEV_NOTHOST;
	#else
		flag = 0;
	#endif

	PLAYBACK_EVENT_FULL( flag, pPlayer->edict(), m_usKnife,
		0.0, (float *)&g_vecZero, (float *)&g_vecZero, 
		0.0,
		0.0,
		iAnimation, 2, 3, 4 );
	*/
}

void FindHullIntersection( const Vector &vecSrc, trace_t &tr, const Vector &mins, const Vector &maxs, CBaseEntity *pEntity )
{
	int			i, j, k;
	float		distance;
	Vector minmaxs[2] = {mins, maxs};
	trace_t tmpTrace;
	Vector		vecHullEnd = tr.endpos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, MASK_SOLID, pEntity, COLLISION_GROUP_NONE, &tmpTrace );
	if ( tmpTrace.fraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID, pEntity, COLLISION_GROUP_NONE, &tmpTrace );
				if ( tmpTrace.fraction < 1.0 )
				{
					float thisDistance = (tmpTrace.endpos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}


void CKnife::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( pPlayer )
	{
#if !defined (CLIENT_DLL)
		// Move other players back to history positions based on local player's lag
		lagcompensation->StartLagCompensation( pPlayer, pPlayer->GetCurrentCommand() );
#endif
		SwingOrStab( false );
#if !defined (CLIENT_DLL)
		lagcompensation->FinishLagCompensation( pPlayer );
#endif
	}
}

#ifndef CLIENT_DLL
bool CBoomerClaw::CanStartVomit() const
{
	if ( !gpGlobals )
		return false;

	CCSPlayer *owner = GetPlayerOwner();
	if ( !owner || !owner->IsAlive() )
		return false;

	if ( owner->GetZombieClass() != 2 )
		return false;

	if ( IsVomiting() )
		return false;

	if ( CSGameRules() && CSGameRules()->IsFreezePeriod() )
		return false;

	return gpGlobals->curtime >= m_flNextVomitAllowedTime;
}
#endif

void CBoomerClaw::PrimaryAttack()
{
	const float duration = MAX(0.05f, z_vomit_duration.GetFloat());
	const float interval = MAX(0.05f, z_vomit_interval.GetFloat());
	CCSPlayer* owner = GetPlayerOwner();
	if (!owner || !owner->IsAlive())
		return;
	SendWeaponAnim(ACT_VM_VOMIT_LAYER);
	DispatchParticleEffect("boomer_vomit", PATTACH_POINT_FOLLOW, owner, 1);
	FX_PlantBomb(owner->entindex(), owner->Weapon_ShootPosition(), PLANTBOMB_PLANT);
#ifndef CLIENT_DLL

	// Only boomers can puke.
	if ( owner->GetZombieClass() != 2 )
	{
		BaseClass::PrimaryAttack();
		return;
	}

	if ( !CanStartVomit() )
		return;

	owner->Vocalize("Vomit.Use", 3.0f, 0.0f);
	m_flVomitEndTime = gpGlobals->curtime + duration;
	m_flNextVomitAllowedTime = gpGlobals->curtime + interval;
	m_flNextVomitBlobTime = gpGlobals->curtime;

	// Apply stamina so the boomer is slowed/pinned while puking.
	owner->m_flStamina = MAX( owner->m_flStamina, z_vomit_fatigue.GetFloat() );

#endif
	// Don't allow immediate weapon spam.
	m_flNextPrimaryAttack = gpGlobals->curtime + interval;
	m_flNextSecondaryAttack = gpGlobals->curtime + duration;
}

#ifndef CLIENT_DLL
bool CSpitterClaw::CanStartSpit() const
{
	if ( !gpGlobals )
		return false;

	CCSPlayer *owner = GetPlayerOwner();
	if ( !owner || !owner->IsAlive() )
		return false;

	if ( owner->GetZombieClass() != 4 )
		return false;

	if ( CSGameRules() && CSGameRules()->IsFreezePeriod() )
		return false;

	return gpGlobals->curtime >= m_flNextSpitAllowedTime;
}
#endif

void CSpitterClaw::PrimaryAttack()
{
	const float interval = MAX( 0.05f, z_spit_interval.GetFloat() );
	CCSPlayer *owner = GetPlayerOwner();
	if ( !owner || !owner->IsAlive() )
		return;

	SendWeaponAnim( ACT_VM_VOMIT_LAYER );
	FX_PlantBomb(owner->entindex(), owner->Weapon_ShootPosition(), PLANTBOMB_PLANT);

#ifndef CLIENT_DLL
	// Only spitters can spit.
	if ( owner->GetZombieClass() != 4 )
	{
		BaseClass::PrimaryAttack();
		return;
	}

	if ( !CanStartSpit() )
		return;

	owner->Vocalize( "SpitterZombie.Spit", 3.0f, 0.0f );
	owner->m_flStamina = 0.0f;

	SpitterRandomSurvivorWarnIncoming( owner );

	// Fire the spit projectile.
	QAngle ang = owner->EyeAngles();
	ang.x = clamp( ang.x, -60.0f, 60.0f );

	Vector forward;
	AngleVectors( ang, &forward );
	forward.NormalizeInPlace();

	const Vector startPos = owner->EyePosition() + forward * 24.0f;
	const Vector velocity = forward * z_spit_velocity.GetFloat();

	CSpitterSpitProjectile::Create( owner, startPos, ang, velocity );

	m_flNextSpitAllowedTime = gpGlobals->curtime + interval;
#endif

	// Prevent melee from shortening the cooldown by keeping primary locked to the full interval.
	m_flNextPrimaryAttack = gpGlobals->curtime + interval;
	// Short post-spit delay before the spitter can melee again.
	m_flNextSecondaryAttack = gpGlobals->curtime + 0.75f;
}

void CKnife::SecondaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( pPlayer && !pPlayer->m_bIsDefusing && !CSGameRules()->IsFreezePeriod() )
	{
#if !defined (CLIENT_DLL)
		// Move other players back to history positions based on local player's lag
		lagcompensation->StartLagCompensation( pPlayer, pPlayer->GetCurrentCommand() );
#endif
		SwingOrStab( true );
#if !defined (CLIENT_DLL)
		lagcompensation->FinishLagCompensation( pPlayer );
#endif
	}
}

void CBoomerClaw::ItemPostFrame( void )
{
#ifndef CLIENT_DLL
	if ( m_flVomitEndTime > 0.0f )
	{
		if ( !gpGlobals || gpGlobals->curtime >= m_flVomitEndTime )
		{
			m_flVomitEndTime = 0.0f;
		}
		else
		{
			CCSPlayer *owner = GetPlayerOwner();
			if ( owner )
			{
				const float spawnInterval = 0.03f;
				while ( gpGlobals->curtime >= m_flNextVomitBlobTime && m_flNextVomitBlobTime < m_flVomitEndTime )
				{
					m_flNextVomitBlobTime += spawnInterval;

					QAngle ang = owner->EyeAngles();
					ang.x += random->RandomFloat( z_vomit_hit_pitch_min.GetFloat(), z_vomit_hit_pitch_max.GetFloat() );
					ang.y += random->RandomFloat( z_vomit_hit_yaw_min.GetFloat(), z_vomit_hit_yaw_max.GetFloat() );

					Vector forward;
					AngleVectors( ang, &forward );

					const Vector startPos = owner->EyePosition() + forward * 24.0f;
					CVomitDamage::Create( owner, startPos, ang );
				}
			}
		}
	}
#endif

	BaseClass::ItemPostFrame();
}

void CHunterClaw::PrimaryAttack()
{
#ifdef CLIENT_DLL
	// Hunter primary attack is an ability (lunge/pounce) driven by the server.
	return;
#else
	CCSPlayer *owner = GetPlayerOwner();
	if ( !owner || !owner->IsAlive() )
		return;

	// Only hunters can lunge/pounce.
	if ( owner->GetZombieClass() != 3 )
	{
		BaseClass::PrimaryAttack();
		return;
	}

	if ( CSGameRules() && CSGameRules()->IsFreezePeriod() )
		return;

	if ( m_flNextPrimaryAttack >= gpGlobals->curtime )
		return;

	if (!(owner->GetFlags() & FL_ONGROUND)) {

		const float dt = clamp(gpGlobals->frametime, 0.01f, 0.05f);

		const Vector curPos = owner->GetAbsOrigin();
		const Vector nextPos = curPos + owner->GetAbsVelocity() * dt;

		trace_t tr;
		const Vector mins = owner->CollisionProp()->OBBMins() * 2;
		const Vector maxs = owner->CollisionProp()->OBBMaxs() * 2;
		UTIL_TraceHull(curPos, nextPos, mins, maxs, MASK_SOLID, owner, COLLISION_GROUP_PLAYER, &tr);

		if (tr.fraction < 1.0f)
		{
			{
				// Wall kick while lunging/pouncing (including world collisions).
				const float groundNormal = clamp(z_hunter_ground_normal.GetFloat(), 0.0f, 1.0f);
				if ((owner->m_afButtonPressed & IN_ATTACK))
				{
					Vector n = tr.plane.normal;
					n.NormalizeInPlace();

					Vector forward;
					AngleVectors(owner->EyeAngles(), &forward);
					forward.NormalizeInPlace();

					Vector vel = forward * z_lunge_power.GetFloat();
					vel.z += z_lunge_up.GetFloat();

					owner->SetGroundEntity(NULL);
					owner->SetAbsVelocity(vel);
					SendWeaponAnim(ACT_VM_LUNGE_POUNCE_LAYER);
					const bool isPounce = owner->GetFlags() & FL_DUCKING || ((owner->m_nButtons & IN_DUCK) != 0);
					m_bIsLunging = true;
					m_bIsPouncing = isPounce;
					m_bDidPounceHit = false;
					m_bPendingLandingDelay = true;
					m_flLungeStartTime = gpGlobals->curtime;


					if (isPounce)
					{
						m_flPounceStartZ = owner->GetAbsOrigin().z;
						m_bIsPouncing = true;

						if (gpGlobals->curtime > owner->m_nextVocalizeTime)
							owner->Vocalize("HunterZombie.Pounce", 1.5f, 1.5f);
					}
					else
					{
						if (gpGlobals->curtime > owner->m_nextVocalizeTime)
							owner->Vocalize("HunterZombie.Lunge", 1.5f, 1.5f);
					}
					m_flNextPrimaryAttack = gpGlobals->curtime + 0.2f;
				}
			}
		}
		return;
	}
	const bool isPounce = owner->GetFlags() & FL_DUCKING || ((owner->m_nButtons & IN_DUCK) != 0);

	Vector forward;
	AngleVectors( owner->EyeAngles(), &forward );
	forward.NormalizeInPlace();

	Vector vel = forward * z_lunge_power.GetFloat();
	vel.z += z_lunge_up.GetFloat();
	SendWeaponAnim(ACT_VM_LUNGE_LAYER);
	owner->SetGroundEntity(NULL);
	owner->SetAbsVelocity( vel );
	m_flNextPrimaryAttack = gpGlobals->curtime + MAX(0.01f, z_lunge_interval.GetFloat());
	m_bIsLunging = true;
	m_bIsPouncing = isPounce;
	m_bDidPounceHit = false;
	m_bPendingLandingDelay = true;
	m_flLungeStartTime = gpGlobals->curtime;



	if (isPounce)
	{
		m_flPounceStartZ = owner->GetAbsOrigin().z;
		m_bIsPouncing = true;

		if (gpGlobals->curtime > owner->m_nextVocalizeTime)
			owner->Vocalize("HunterZombie.Pounce", 1.5f, 1.5f);
	}
	else
	{
		if (gpGlobals->curtime > owner->m_nextVocalizeTime)
			owner->Vocalize("HunterZombie.Lunge", 1.5f, 1.5f);
	}

	// Cooldowns are applied on landing (touching ground) so hunters can't chain-lunge as they land.
#endif
}

void CHunterClaw::ItemPostFrame()
{
#ifndef CLIENT_DLL
	auto ApplyLandingDelay = [&]( CCSPlayer *pOwner )
	{
		if ( !pOwner )
			return;

		const float interval = MAX( 0.01f, z_lunge_interval.GetFloat() );
		m_flNextLungeAllowedTime = gpGlobals->curtime + interval;

		const float cooldown = MAX( 0.0f, z_lunge_cooldown.GetFloat() );
		m_flNextPrimaryAttack = gpGlobals->curtime + cooldown;
		m_flNextSecondaryAttack = gpGlobals->curtime + cooldown;
		pOwner->m_flNextAttack = gpGlobals->curtime + cooldown;

		m_bPendingLandingDelay = false;
	};

	if ( m_bIsLunging || m_bIsPouncing )
	{
		CCSPlayer *owner = GetPlayerOwner();
		if ( !owner || !owner->IsAlive() || owner->GetZombieClass() != 3 )
		{
			m_bIsLunging = false;
			m_bIsPouncing = false;
			m_bDidPounceHit = false;
			m_bPendingLandingDelay = false;
		}
		else
		{
			// If a lunge ended in midair (impact/cancel), still apply the landing delay once we touch ground.
			if ( m_bPendingLandingDelay && owner->GetGroundEntity() != NULL && gpGlobals->curtime > ( m_flLungeStartTime + 0.1f ) )
			{
				ApplyLandingDelay( owner );
			}

			// Stop lunging once we've landed (after a short grace period).
			if ( owner->GetGroundEntity() != NULL && gpGlobals->curtime > ( m_flLungeStartTime + 0.1f ) )
			{
				m_flNextPrimaryAttack = gpGlobals->curtime + 0.2f;
				if ( m_bIsPouncing )
				{
					owner->EmitSound( "HunterZombie.Pounce.Miss" );
					m_bIsLunging = false;
					m_bIsPouncing = false;

					return;
				}

				owner->Vocalize( "HunterZombie.LungeLand" );

				m_bIsLunging = false;
				m_bIsPouncing = false;
			}
			else
			{
				const float dt = clamp( gpGlobals->frametime, 0.01f, 0.05f );

				Vector vel = owner->GetAbsVelocity();

				// Bot-only assist: steer bot pounces towards their intended target and treat very-close passes as hits.
				if ( m_bIsPouncing && !m_bDidPounceHit && owner->IsBot() )
				{
					const float targetWindow = 0.35f;
					if ( m_hBotLungeTarget.Get() && ( gpGlobals->curtime - m_flBotLungeTargetSetTime ) > targetWindow )
					{
						m_hBotLungeTarget = NULL;
					}

					CCSPlayer *target = m_hBotLungeTarget.Get() ? ToCSPlayer( m_hBotLungeTarget.Get() ) : NULL;
					if ( target && target->IsAlive() && target->GetTeamNumber() == TEAM_SURVIVOR && !target->HasPounceAttacker() )
					{
						const Vector ownerCenter = owner->WorldSpaceCenter();
						const Vector targetCenter = target->WorldSpaceCenter();
						Vector toTarget = targetCenter - ownerCenter;
						const float distSqr = toTarget.LengthSqr();

						if ( distSqr > 1.0f )
						{
							Vector desiredDir = toTarget;
							desiredDir.NormalizeInPlace();

							const float speed = vel.Length();
							Vector desiredVel = desiredDir * speed;

							// Preserve vertical arc, only correct horizontally.
							desiredVel.z = vel.z;

							const float steerStrength = 0.85f;
							vel += ( desiredVel - vel ) * steerStrength;
							owner->SetAbsVelocity( vel );
						}

						const float assistRange = 90.0f;
						if ( distSqr <= ( assistRange * assistRange ) )
						{
							trace_t los;
							UTIL_TraceLine( ownerCenter, targetCenter, MASK_SOLID_BRUSHONLY, owner, COLLISION_GROUP_NONE, &los );
							if ( los.fraction == 1.0f )
							{
								const float height = MAX( 0.0f, m_flPounceStartZ - target->GetAbsOrigin().z );
								const int maxBonus = MAX( 0, z_hunter_max_pounce_bonus_damage.GetInt() );

								int bonus = (int)floor( height / 100.0f );
								bonus = clamp( bonus, 0, maxBonus );

								const float damage = 1.0f + (float)bonus;
								CTakeDamageInfo info( owner, owner, damage, DMG_SLASH | DMG_NEVERGIB );
								target->TakeDamage( info );

								owner->Vocalize( "HunterZombie.Pounce.Hit", 3.0f, 0.0f );
								m_bDidPounceHit = true;

								owner->StartPounce( target );
								m_hBotLungeTarget = NULL;

								owner->SetGroundEntity( NULL );
								owner->SetAbsVelocity( vec3_origin );
								m_bIsLunging = false;
								m_bIsPouncing = false;
							}
						}
					}
					else
					{
						m_hBotLungeTarget = NULL;
					}
				}

				const Vector curPos = owner->GetAbsOrigin();
				const Vector nextPos = curPos + vel * dt;

				trace_t tr;
				const Vector mins = owner->CollisionProp()->OBBMins();
				const Vector maxs = owner->CollisionProp()->OBBMaxs();
				UTIL_TraceHull( curPos, nextPos, mins, maxs, MASK_SOLID, owner, COLLISION_GROUP_PLAYER, &tr );

				if ( tr.startsolid || tr.allsolid )
				{
					// If we ended up in something, cancel.
					m_bIsLunging = false;
					m_bIsPouncing = false;
				}
				else if ( tr.fraction < 1.0f )
				{
					CCSPlayer *victim = tr.m_pEnt ? ToCSPlayer( tr.m_pEnt ) : NULL;
					if ( victim && victim->IsAlive() && victim->GetTeamNumber() == TEAM_SURVIVOR )
					{
						if ( m_bIsPouncing && !m_bDidPounceHit )
						{
							const float height = MAX( 0.0f, m_flPounceStartZ - victim->GetAbsOrigin().z );
							const int maxBonus = MAX( 0, z_hunter_max_pounce_bonus_damage.GetInt() );

							// Roughly 100 units per bonus damage, capped by z_hunter_max_pounce_bonus_damage.
							int bonus = (int)floor( height / 100.0f );
							bonus = clamp( bonus, 0, maxBonus );

							const float damage = 1.0f + (float)bonus;
							CTakeDamageInfo info( owner, owner, damage, DMG_SLASH | DMG_NEVERGIB );
							victim->TakeDamage( info );

							owner->Vocalize( "HunterZombie.Pounce.Hit", 3.0f, 0.0f );
							m_bDidPounceHit = true;

#ifndef CLIENT_DLL
							owner->StartPounce( victim );
#endif
						}
						else if ( !m_bIsPouncing )
						{
							// Lunge impact: shove the survivor away (no pounce damage).
							Vector shoveDir = victim->WorldSpaceCenter() - owner->WorldSpaceCenter();
							shoveDir.z = 0.0f;

							if ( shoveDir.NormalizeInPlace() < 0.01f )
							{
								AngleVectors( owner->EyeAngles(), &shoveDir );
								shoveDir.z = 0.0f;
								shoveDir.NormalizeInPlace();
							}

							SendWeaponAnim(ACT_VM_LUNGE_PUSH_LAYER);
							Vector shoveImpulse = shoveDir * 400.0f;
							shoveImpulse.z += 200.0f;
							victim->SetGroundEntity(NULL);
							victim->ApplyAbsVelocityImpulse( shoveImpulse );
						}

						// Stop forward momentum on impact so follow-up melee is reliable.
						owner->SetGroundEntity(NULL);
						owner->SetAbsVelocity( vec3_origin );
						m_bIsLunging = false;
						m_bIsPouncing = false;
					}
					else
					{
						// moved to PrimaryAttack()
					}
				}
			}
		}
	}
	else
	{
		// If a lunge/pounce ended outside the lunge state, apply the landing delay once we touch ground.
		if ( m_bPendingLandingDelay )
		{
			CCSPlayer *owner = GetPlayerOwner();
			if ( !owner || !owner->IsAlive() || owner->GetZombieClass() != 3 )
			{
				m_bPendingLandingDelay = false;
			}
			else if ( owner->GetGroundEntity() != NULL && gpGlobals->curtime > ( m_flLungeStartTime + 0.1f ) )
			{
				ApplyLandingDelay( owner );
			}
		}
	}
#endif

	BaseClass::ItemPostFrame();
}

#include "effect_dispatch_data.h"

static bool IsTankClawWeapon( const CKnife *weapon )
{
	return weapon && FClassnameIs((CBaseEntity*)weapon, "weapon_tank_claw" );
}

static bool IsClawWeapon( const CKnife *weapon )
{
	return weapon && !FClassnameIs( (CBaseEntity *)weapon, "weapon_knife" );
}

static float GetClawRange( const CKnife *weapon, CCSPlayer *owner)
{
	if ( !weapon || !owner )
		return 70.0f;

	const float kClawRange = 80.0f;
	const float kClawRangeLookingDown = 70.0f;
	const float kTankClawRange = 120.0f;
	const float kLookingDownPitch = 35.0f;

	if ( IsTankClawWeapon( weapon ) )
		return kTankClawRange;

	return kClawRange;
}

static float GetClawYaw( const CKnife *weapon )
{
	// Degrees of yaw cone (half-angle) for claw hit detection.
	return IsTankClawWeapon( weapon ) ? 80.0f : 60.0f;
}

void CKnife::Smack( void )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	const bool isTankClaw = IsTankClawWeapon( this );
	const bool isClawWeapon = IsClawWeapon( this );

	// For claws, choose the trace entity at Smack() time (not at SwingOrStab() time),
	// so delayed smacks (ex: tank) hit what the player is actually aiming at.
	if ( isClawWeapon )
	{
		const float range = GetClawRange( this, pPlayer );

		Vector forward;
		AngleVectors( pPlayer->EyeAngles(), &forward );

		const Vector src = pPlayer->Weapon_ShootPosition();
		Vector end = src + forward * range;

		trace_t tr;
		UTIL_TraceLine( src, end, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );
		if ( tr.fraction >= 1.0f )
		{
			UTIL_TraceHull( src, end, head_hull_mins, head_hull_maxs, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );
			if ( tr.fraction < 1.0f )
			{
				CBaseEntity *pHit = tr.m_pEnt;
				if ( !pHit || pHit->IsBSPModel() )
					FindHullIntersection( src, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, pPlayer );
			}
		}

		m_trHit = tr;
		m_pTraceHitEnt = tr.m_pEnt;
		m_trHit.m_pEnt = m_pTraceHitEnt;
	}
	else
	{
		// Regular knife uses the stored trace entity to match original timing.
		m_trHit.m_pEnt = m_pTraceHitEnt;
	}

	const bool hasTraceHit = ( m_trHit.m_pEnt != NULL ) && !( m_trHit.surface.flags & SURF_SKY ) && ( m_trHit.fraction < 1.0f );

	CEffectData data;
	data.m_vOrigin = m_trHit.endpos;
	data.m_vStart = m_trHit.startpos;
	data.m_nSurfaceProp = m_trHit.surface.surfaceProps;
	data.m_nDamageType = DMG_SLASH;
	data.m_nHitBox = m_trHit.hitbox;
#ifdef CLIENT_DLL
	if ( m_trHit.m_pEnt )
		data.m_hEntity = m_trHit.m_pEnt->GetRefEHandle();
#else
	if ( m_trHit.m_pEnt )
		data.m_nEntIndex = m_trHit.m_pEnt->entindex();
#endif

	CPASFilter effectFilter( data.m_vOrigin );
	
#ifndef CLIENT_DLL
	effectFilter.RemoveRecipient( GetPlayerOwner() );
#endif

	data.m_vAngles = GetPlayerOwner()->GetAbsAngles();
	data.m_fFlags = 0x1;	//IMPACT_NODECAL;
	if ( hasTraceHit )
	{
		te->DispatchEffect( effectFilter, 0.0, data.m_vOrigin, "KnifeSlash", data );
	}
#ifndef CLIENT_DLL
	// Damage / interaction (server-only).
	if ( !isClawWeapon )
	{
		if ( !hasTraceHit )
			return;

		CBaseEntity *pEntity = m_trHit.m_pEnt;
		if (!pEntity) {

			if (isTankClaw)
			{
				pEntity->EmitSound("HulkZombie.Punch");
			}
			else
			{
				EmitSound("Claw.Hit");
			}
			return;
		}

		CPASAttenuationFilter hitFilter( this );
		hitFilter.UsePredictionRules();
		hitFilter.AddAllPlayers();
		if ( isTankClaw )
		{
			pEntity->EmitSound( "HulkZombie.Punch" );
		}
		else if ( pEntity->IsPlayer() || pEntity->IsNPC() )
		{
			pEntity->EmitSound( "Claw.HitFlesh" );
		}
		else
		{
			EmitSound( "Claw.Hit" );
		}

		Vector vForward;
		AngleVectors( pPlayer->EyeAngles(), &vForward );

		float flDamage = 0.0f;
		if ( isTankClaw )
		{
			flDamage = 24.0f;
		}
		else if ( pPlayer->GetZombieClass() == 1 || pPlayer->GetZombieClass() == 2 || pPlayer->GetZombieClass() == 5 )
		{
			flDamage = 4.0f;
		}
		else if ( pPlayer->GetZombieClass() == 6 )
		{
			flDamage = 10.0f;
			pEntity->EmitSound( "ChargerZombie.Smash" );
		}
		else if ( pPlayer->GetZombieClass() == 3 )
		{
			flDamage = 6.0f;
		}
		else
		{
			flDamage = 4.0f;
		}

		pPlayer->SetAnimation( PLAYER_ATTACK1 );
		ClearMultiDamage();

		CTakeDamageInfo info( pPlayer, pPlayer, flDamage, DMG_BULLET | DMG_NEVERGIB );
		CalculateMeleeDamageForce( &info, vForward, m_trHit.endpos, flDamage );
		pEntity->DispatchTraceAttack( info, vForward, &m_trHit );
		ApplyMultiDamage();

		if ( isTankClaw && pEntity->IsPlayer() )
		{
			CCSPlayer *pVictim = ToCSPlayer( pEntity );
			if ( pVictim && pVictim->IsAlive() && pVictim->GetTeamNumber() == TEAM_SURVIVOR )
			{
				pVictim->ViewPunch( QAngle( 20, 0, -20 ) );

				Vector forward, up;
				AngleVectors( pPlayer->GetLocalAngles(), &forward, NULL, &up );
				pVictim->SetGroundEntity( NULL );
				pVictim->ApplyAbsVelocityImpulse( forward * 1100.0f + up * 450.0f );
			}
		}

		CCS_GameStats.Event_KnifeUse( pPlayer, false, flDamage );
		return;
	}

	// Claws: hit multiple entities in a POV cone.
	const float range = GetClawRange( this, pPlayer );
	const float yaw = GetClawYaw( this );
	const float cosYaw = cosf( DEG2RAD( yaw ) );

	const Vector src = pPlayer->Weapon_ShootPosition();

	Vector forward3d;
	AngleVectors( pPlayer->EyeAngles(), &forward3d );

	Vector forward2d = forward3d;
	forward2d.z = 0.0f;
	if ( forward2d.NormalizeInPlace() < 0.001f )
		return;

	float flDamage = 0.0f;
	if ( isTankClaw )
	{
		flDamage = 24.0f;
	}
	else if ( pPlayer->GetZombieClass() == 1 || pPlayer->GetZombieClass() == 2 || pPlayer->GetZombieClass() == 5 )
	{
		flDamage = 4.0f;
	}
	else if ( pPlayer->GetZombieClass() == 6 )
	{
		flDamage = 10.0f;
	}
	else if ( pPlayer->GetZombieClass() == 3 )
	{
		flDamage = 6.0f;
	}
	else
	{
		flDamage = 4.0f;
	}

	CBaseEntity *pList[128];
	const int count = UTIL_EntitiesInSphere( pList, ARRAYSIZE( pList ), src, range + 32.0f, 0 );

	bool playedHitSound = false;
	int hitCount = 0;
	const int maxHits = 12;

	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	for ( int i = 0; i < count && hitCount < maxHits; ++i )
	{
		CBaseEntity *pEntity = pList[ i ];
		if ( !pEntity || pEntity == pPlayer )
			continue;

		if ( pEntity->m_takedamage == DAMAGE_NO )
			continue;

		if ( pEntity->IsPlayer() && pEntity->GetTeamNumber() == pPlayer->GetTeamNumber() )
			continue;

		CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( pEntity );
		if ( combat && !combat->IsAlive() )
			continue;

		const Vector targetPos = pEntity->WorldSpaceCenter();
		Vector toTarget = targetPos - src;
		if ( toTarget.LengthSqr() > Square( range ) )
			continue;

		Vector toTarget2d = toTarget;
		toTarget2d.z = 0.0f;
		if ( toTarget2d.NormalizeInPlace() < 0.001f )
			continue;

		const float dot2d = DotProduct( forward2d, toTarget2d );
		if ( dot2d < cosYaw )
			continue;

		trace_t tr;
		UTIL_TraceLine( src, targetPos, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );
		if ( tr.m_pEnt != pEntity )
			continue;

		if ( !playedHitSound )
		{
			CPASAttenuationFilter hitFilter( this );
			hitFilter.UsePredictionRules();
			hitFilter.AddAllPlayers();
			if ( isTankClaw )
			{
				EmitSound( hitFilter, entindex(), "HulkZombie.Punch" );
			}
			else if ( pEntity->IsPlayer() || pEntity->IsNPC() )
			{
				EmitSound( hitFilter, entindex(), "Claw.HitFlesh" );
			}
			else
			{
				EmitSound( hitFilter, entindex(), "Claw.Hit" );
			}

			playedHitSound = true;
		}

		if ( !isTankClaw && pPlayer->GetZombieClass() == 6 )
		{
			pEntity->EmitSound( "ChargerZombie.Smash" );
		}

		Vector attackDir = ( targetPos - src );
		attackDir.NormalizeInPlace();

		ClearMultiDamage();
		CTakeDamageInfo info( pPlayer, pPlayer, flDamage, DMG_BULLET | DMG_NEVERGIB );
		CalculateMeleeDamageForce( &info, attackDir, tr.endpos, flDamage );
		pEntity->DispatchTraceAttack( info, attackDir, &tr );
		ApplyMultiDamage();

		if ( isTankClaw && pEntity->IsPlayer() )
		{
			CCSPlayer *pVictim = ToCSPlayer( pEntity );
			if ( pVictim && pVictim->IsAlive() && pVictim->GetTeamNumber() == TEAM_SURVIVOR )
			{
				pVictim->ViewPunch( QAngle( 20, 0, -20 ) );

				Vector forward, up;
				AngleVectors( pPlayer->GetLocalAngles(), &forward, NULL, &up );
				pVictim->SetGroundEntity( NULL );
				pVictim->ApplyAbsVelocityImpulse( forward * 400.0f + up * 250.0f );
			}
		}

		++hitCount;
	}

	if ( hitCount > 0 )
	{
		CCS_GameStats.Event_KnifeUse( pPlayer, false, flDamage );
	}

#endif

}

void CKnife::WeaponIdle()
{
	if (m_flTimeWeaponIdle > gpGlobals->curtime)
		return;

	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( pPlayer->IsShieldDrawn() )
		 return;

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration( SelectWeightedSequence(ACT_VM_IDLE) ) );

	// only idle if the slid isn't back
	SendWeaponAnim( ACT_VM_IDLE );
}

//=============================================================================
// HPE_BEGIN:
// [tj] Hacky cheat code to control knife damage
//=============================================================================
#ifndef CLIENT_DLL
	ConVar KnifeDamageScale("knife_damage_scale", "100", FCVAR_DEVELOPMENTONLY);
#endif

//=============================================================================
// HPE_END
//=============================================================================


bool CKnife::SwingOrStab( bool bStab )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return false;

	const bool isClawWeapon = IsClawWeapon( this );

	float fRange = bStab ? 32.0f : 48.0f;
	if ( isClawWeapon )
	{
		fRange = GetClawRange( this, pPlayer );
	}
	
	Vector vForward; AngleVectors( pPlayer->EyeAngles(), &vForward );
	Vector vecSrc	= pPlayer->Weapon_ShootPosition();
	Vector vecEnd	= vecSrc + vForward * fRange;

	trace_t tr;
	UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );
#ifndef CLIENT_DLL
	//check for hitting glass - TODO - fix this hackiness, doesn't always line up with what FindHullIntersection returns
	CTakeDamageInfo glassDamage( pPlayer, pPlayer, 42.0f, DMG_BULLET | DMG_NEVERGIB );
	TraceAttackToTriggers( glassDamage, tr.startpos, tr.endpos, vForward );

	if ( tr.fraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, head_hull_mins, head_hull_maxs, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );
		if ( tr.fraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = tr.m_pEnt;
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, pPlayer );
			vecEnd = tr.endpos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif
	bool bDidHit = tr.fraction < 1.0f;

	const bool isTankClaw = IsTankClawWeapon( this );
	if (isTankClaw)	{
		// Tank claw hit duration / time between swings.
		const float nextAttack = gpGlobals->curtime + 2.0f;
		m_flNextPrimaryAttack = MAX( m_flNextPrimaryAttack, nextAttack );
		m_flNextSecondaryAttack = MAX( m_flNextSecondaryAttack, nextAttack );
		SetWeaponIdleTime( gpGlobals->curtime + 1.6f );
		pPlayer->EmitSound("HulkZombie.Attack");
	}
	else
	{
		const float nextAttack = gpGlobals->curtime + 1.0f;
		m_flNextPrimaryAttack = MAX( m_flNextPrimaryAttack, nextAttack );
		m_flNextSecondaryAttack = MAX( m_flNextSecondaryAttack, nextAttack );
		SetWeaponIdleTime(gpGlobals->curtime + 2);
	}
	
	if ( !isTankClaw )
	{
		// play wiff or swish sound
		CPASAttenuationFilter filter( this );
		filter.UsePredictionRules();
		EmitSound( filter, entindex(), "Claw.Swing" );
	}

	pPlayer->DoAnimationEvent(PLAYERANIMEVENT_FIRE_GUN_PRIMARY); 
	SendWeaponAnim(ACT_VM_SHOOT_LAYER);
	if ( isClawWeapon )
	{
		// Claws always run Smack() (multi-hit and/or delayed timing).
		// The trace entity is chosen inside Smack() at the moment it runs.
		m_bStab = bStab;
		m_flSmackTime = gpGlobals->curtime + ( isTankClaw ? 0.6f : 0.0f );
	}
	else if ( bDidHit )
	{
		// delay the decal a bit
		m_trHit = tr;
		
		// Store the ent in an EHANDLE, just in case it goes away by the time we get into our think function.
		m_pTraceHitEnt = tr.m_pEnt; 

		m_bStab = bStab;	//store this so we know what hit sound to play
		m_flSmackTime = gpGlobals->curtime + 0.0f;
	}

	return bDidHit;
}

void CKnife::ItemPostFrame( void )
{
	if( m_flSmackTime > 0 && gpGlobals->curtime > m_flSmackTime )
	{
		Smack();
		m_flSmackTime = -1;
	}

	BaseClass::ItemPostFrame();
}

bool CKnife::CanDrop()
{
	return false;
}
