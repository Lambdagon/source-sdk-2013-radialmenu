//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "cs_playeranimstate.h"
#include "base_playeranimstate.h"
#include "tier0/vprof.h"
#include "animation.h"
#include "weapon_csbase.h"
#include "studio.h"
#include "apparent_velocity_helper.h"
#include "utldict.h"
#include "weapon_basecsgrenade.h"
#include "datacache/imdlcache.h"

#ifdef CLIENT_DLL
	#include "c_cs_player.h"
	#include "bone_setup.h"
	#include "interpolatedvar.h"
	#include "c_cs_hostage.h"
#else
	#include "cs_player.h"
	#include "cs_simple_hostage.h"
	#include "cs_gamestats.h"
#endif

#define ANIM_TOPSPEED_WALK			100
#define ANIM_TOPSPEED_RUN			250
#define ANIM_TOPSPEED_RUN_CROUCH	85

#define DEFAULT_IDLE_NAME "idle_upper_"
#define DEFAULT_CROUCH_IDLE_NAME "crouch_idle_upper_"
#define DEFAULT_CROUCH_WALK_NAME "crouch_walk_upper_"
#define DEFAULT_WALK_NAME "walk_upper_"
#define DEFAULT_RUN_NAME "run_upper_"

#define DEFAULT_FIRE_IDLE_NAME "idle_shoot_"
#define DEFAULT_FIRE_CROUCH_NAME "crouch_idle_shoot_"
#define DEFAULT_FIRE_CROUCH_WALK_NAME "crouch_walk_shoot_"
#define DEFAULT_FIRE_WALK_NAME "walk_shoot_"
#define DEFAULT_FIRE_RUN_NAME "run_shoot_"


#define FIRESEQUENCE_LAYER		(AIMSEQUENCE_LAYER+NUM_AIMSEQUENCE_LAYERS+1)
#define RELOADSEQUENCE_LAYER	(FIRESEQUENCE_LAYER + 1)
#define GRENADESEQUENCE_LAYER	(RELOADSEQUENCE_LAYER + 1)
#define NUM_LAYERS_WANTED		(GRENADESEQUENCE_LAYER + 1)



// ------------------------------------------------------------------------------------------------ //
// CCSPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //

class CCSPlayerAnimState : public CBasePlayerAnimState, public ICSPlayerAnimState
{
public:
	DECLARE_CLASS( CCSPlayerAnimState, CBasePlayerAnimState );
	friend ICSPlayerAnimState* CreatePlayerAnimState( CBaseAnimatingOverlay *pEntity, ICSPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences );

	CCSPlayerAnimState();

	virtual void Update( float eyeYaw, float eyePitch );
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData );
	virtual bool IsThrowingGrenade();
	virtual int CalcAimLayerSequence( float *flCycle, float *flAimSequenceWeight, bool bForceIdle );
	virtual void ClearAnimationState();
	virtual bool CanThePlayerMove();
	virtual float GetCurrentMaxGroundSpeed();
	virtual Activity CalcMainActivity();
	virtual void DebugShowAnimState( int iStartLine );
	virtual void ComputeSequences( CStudioHdr *pStudioHdr );
	virtual void ClearAnimationLayers();
	virtual int SelectWeightedSequence( Activity activity );

	void InitCS( CBaseAnimatingOverlay *pPlayer, ICSPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences );
	
protected:

	int CalcFireLayerSequence(PlayerAnimEvent_t event);
	void ComputeFireSequence( CStudioHdr *pStudioHdr );

	void ComputeReloadSequence( CStudioHdr *pStudioHdr );
	int CalcReloadLayerSequence( PlayerAnimEvent_t event );

	bool IsOuterGrenadePrimed();
	void ComputeGrenadeSequence( CStudioHdr *pStudioHdr );
	int CalcGrenadePrimeSequence();
	int CalcGrenadeThrowSequence();
	int GetOuterGrenadeThrowCounter();

	const char* GetWeaponSuffix();
	bool HandleJumping();

	void UpdateLayerSequenceGeneric( CStudioHdr *pStudioHdr, int iLayer, bool &bEnabled, float &flCurCycle, int &iSequence, bool bWaitAtEnd );

	virtual int CalcSequenceIndex( const char *pBaseName, ... );

	void UpdateAimSequenceLayers(
		float flCycle,
		int iFirstLayer,
		bool bForceIdle,
		CSequenceTransitioner* pTransitioner,
		float flWeightScale
	);
private:

	// Current state variables.
	bool m_bJumping;			// Set on a jump event.
	float m_flJumpStartTime;
	bool m_bFirstJumpFrame;
	bool m_bTankDeathRestarted;
	float m_flTankDeathPrevCycle;
	bool m_bTankDeathPrevCycleValid;
	bool m_bWasPounceVictim;
	bool m_bWasPounceAttacker;
	bool m_bWasIncapacitated;
	bool m_bWasBeingRevived;
	bool m_bIncapDyingFinished;

	// Aim sequence plays reload while this is on.
	bool m_bReloading;
	float m_flReloadCycle;
	int m_iReloadSequence;
	float m_flReloadHoldEndTime;	// Intermediate shotgun reloads get held a fraction of a second

	// This is set to true if ANY animation is being played in the fire layer.
	bool m_bFiring;						// If this is on, then it'll continue the fire animation in the fire layer
										// until it completes.
	int m_iFireSequence;				// (For any sequences in the fire layer, including grenade throw).
	float m_flFireCycle;
	PlayerAnimEvent_t m_delayedFire;	// if we fire while reloading, delay the fire by one frame so we can cancel the reload first

	// These control grenade animations.
	bool m_bThrowingGrenade;
	bool m_bPrimingGrenade;
	float m_flGrenadeCycle;
	int m_iGrenadeSequence;
	int m_iLastThrowGrenadeCounter;	// used to detect when the guy threw the grenade.

	CCSPlayer *m_pPlayer;

	ICSPlayerAnimStateHelpers *m_pHelpers;

	void CheckCachedSequenceValidity( void );

	int m_sequenceCache[ ACT_CROUCHIDLE+1 ];	// Cache the first N sequences, since we don't have weights.
	int m_cachedModelIndex;						// Model index for which the sequence cache is valid.

	CUtlDict<int,int> m_namedSequence;			// Dictionary of sequences computed with CalcSequenceIndex.  This is because LookupSequence is a performance hit - CS:S player models have 750+ sequences!

	int m_nLadderClimbDir;
};


ICSPlayerAnimState* CreatePlayerAnimState( CBaseAnimatingOverlay *pEntity, ICSPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences )
{
	CCSPlayerAnimState *pRet = new CCSPlayerAnimState;
	pRet->InitCS( pEntity, pHelpers, legAnimType, bUseAimSequences );
	return pRet;
}




//----------------------------------------------------------------------------------------------
/**
 * Hostage animation mechanism
 */
class CCSHostageAnimState : public CCSPlayerAnimState
{
public:
	DECLARE_CLASS( CCSHostageAnimState, CCSPlayerAnimState );

	CCSHostageAnimState();

	virtual Activity CalcMainActivity();

	// No need to cache sequences, and we *do* have multiple sequences per activity
	virtual int SelectWeightedSequence( Activity activity ) { return GetOuter()->SelectWeightedSequence( activity ); }
};


//----------------------------------------------------------------------------------------------
ICSPlayerAnimState* CreateHostageAnimState( CBaseAnimatingOverlay *pEntity, ICSPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences )
{
	CCSHostageAnimState *anim = new CCSHostageAnimState;
	anim->InitCS( pEntity, pHelpers, legAnimType, bUseAimSequences );
	return anim;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
CCSHostageAnimState::CCSHostageAnimState()
{
}


//----------------------------------------------------------------------------------------------
/**
 * Set hostage animation state
 */
Activity CCSHostageAnimState::CalcMainActivity()
{
	float flOuterSpeed = GetOuterXYSpeed();

	if ( HandleJumping() )
	{
		return ACT_JUMP;
	}
	else
	{
		Assert( dynamic_cast<CHostage*>( m_pOuter ) );
		CHostage *me = (CHostage*)m_pOuter;

		// if we have no leader, hang out
		Activity idealActivity = me->GetLeader() ? ACT_IDLE : ACT_BUSY_QUEUE;

		if ( m_pOuter->GetFlags() & FL_DUCKING )
		{
			if ( flOuterSpeed > MOVING_MINIMUM_SPEED )
				idealActivity = ACT_RUN_CROUCH;
			else
				idealActivity = ACT_COVER_LOW;
		}
		else
		{
			if (flOuterSpeed > MOVING_MINIMUM_SPEED)
			{
				if (GetOuter()->GetTeamNumber() == TEAM_SURVIVOR && GetOuter()->GetHealth() < 40) {
					if (flOuterSpeed > ARBITRARY_RUN_SPEED)
						idealActivity = ACT_RUN_HURT;
					else
						idealActivity = ACT_WALK_HURT;
				}
				else {
					if (flOuterSpeed > ARBITRARY_RUN_SPEED)
						idealActivity = ACT_RUN;
					else
						idealActivity = ACT_WALK;
				}
			}
		}

		return idealActivity;
	}
}


// ------------------------------------------------------------------------------------------------ //
// CCSPlayerAnimState implementation.
// ------------------------------------------------------------------------------------------------ //

CCSPlayerAnimState::CCSPlayerAnimState()
{
	m_pOuter = NULL;

	m_bJumping = false;
	m_flJumpStartTime = 0.0f;
	m_bFirstJumpFrame = false;
	m_bTankDeathRestarted = false;
	m_flTankDeathPrevCycle = 0.0f;
	m_bTankDeathPrevCycleValid = false;
	m_bWasPounceVictim = false;
	m_bWasPounceAttacker = false;
	m_bWasIncapacitated = false;
	m_bWasBeingRevived = false;
	m_bIncapDyingFinished = false;

	m_bReloading = false;
	m_flReloadCycle = 0.0f;
	m_iReloadSequence = -1;
	m_flReloadHoldEndTime = 0.0f;

	m_bFiring = false;
	m_iFireSequence = -1;
	m_flFireCycle = 0.0f;
	m_delayedFire = PLAYERANIMEVENT_COUNT;

	m_bThrowingGrenade = false;
	m_bPrimingGrenade = false;
	m_flGrenadeCycle = 0.0f;
	m_iGrenadeSequence = -1;
	m_iLastThrowGrenadeCounter = 0;
	m_cachedModelIndex = -1;

	m_pPlayer = NULL;

	m_pHelpers = NULL;

	m_nLadderClimbDir = 1;
}

void CCSPlayerAnimState::Update( float eyeYaw, float eyePitch )
{
	const bool bIsPounceVictim = ( m_pPlayer && m_pPlayer->m_pounceAttacker.Get() != NULL );
	const bool bIsPounceAttacker = ( m_pPlayer && m_pPlayer->m_pounceVictim.Get() != NULL );
	const bool bIsIncapacitated = ( m_pPlayer && m_pPlayer->GetTeamNumber() == TEAM_SURVIVOR && m_pPlayer->m_bIncapacitated );
	const bool bIsBeingRevived = ( m_pPlayer && m_pPlayer->GetTeamNumber() == TEAM_SURVIVOR && m_pPlayer->m_bBeingRevived );

	if ( m_pOuter && ( ( bIsPounceVictim && !m_bWasPounceVictim ) || ( bIsPounceAttacker && !m_bWasPounceAttacker ) ) )
	{
		// Ensure pounce sequences always start at cycle 0.
		RestartMainSequence();
	}

	if ( m_pOuter && bIsIncapacitated && !m_bWasIncapacitated )
	{
		// Ensure the incapacitation "down" animation starts at cycle 0.
		m_bIncapDyingFinished = false;
		RestartMainSequence();
	}

	if ( m_pOuter && bIsBeingRevived && !m_bWasBeingRevived )
	{
		// Ensure the revive/get-up animation starts at cycle 0.
		RestartMainSequence();
	}
	else if ( m_pOuter && !bIsBeingRevived && m_bWasBeingRevived && bIsIncapacitated )
	{
		// If revive is interrupted, replay the down animation.
		m_bIncapDyingFinished = false;
		RestartMainSequence();
	}

	BaseClass::Update( eyeYaw, eyePitch );

	if ( !m_pOuter || !m_pPlayer )
		return;

	m_bWasPounceVictim = bIsPounceVictim;
	m_bWasPounceAttacker = bIsPounceAttacker;
	m_bWasIncapacitated = bIsIncapacitated;
	m_bWasBeingRevived = bIsBeingRevived;

	// Once ACT_DIESIMPLE completes, transition into the appropriate incapacitated idle.
	if ( bIsIncapacitated && !m_bIncapDyingFinished )
	{
		const Activity curAct = (Activity)m_pOuter->GetSequenceActivity( m_pOuter->GetSequence() );
		if ( curAct == ACT_DIESIMPLE && m_pOuter->IsSequenceFinished() )
		{
			m_bIncapDyingFinished = true;
			RestartMainSequence();
		}
	}

#ifndef CLIENT_DLL
	const bool bTankStagedDeath =
		( m_pPlayer->IsAlive() &&
		m_pPlayer->GetTeamNumber() == TEAM_INFECTED &&
		m_pPlayer->GetZombieClass() == 8 &&
		m_pOuter->GetMoveType() == MOVETYPE_NONE &&
		m_pOuter->GetSolid() == SOLID_NONE &&
		m_pOuter->GetHealth() == 1 );

	if ( !bTankStagedDeath )
	{
		m_bTankDeathPrevCycleValid = false;
	}
	else
	{
		const float flToCycle = m_pOuter->GetCycle();
		if ( !m_bTankDeathPrevCycleValid )
		{
			m_flTankDeathPrevCycle = flToCycle;
			m_bTankDeathPrevCycleValid = true;
		}
		else if ( flToCycle != m_flTankDeathPrevCycle )
		{
			const float flFromCycle = m_flTankDeathPrevCycle;

			Vector deltaPosLocal( vec3_origin );
			QAngle deltaAngLocal( 0.0f, 0.0f, 0.0f );

			if ( flToCycle > flFromCycle )
			{
				m_pOuter->GetSequenceMovement( m_pOuter->GetSequence(), flFromCycle, flToCycle, deltaPosLocal, deltaAngLocal );
			}
			else
			{
				Vector dp1, dp2;
				QAngle da1, da2;
				m_pOuter->GetSequenceMovement( m_pOuter->GetSequence(), flFromCycle, 1.0f, dp1, da1 );
				m_pOuter->GetSequenceMovement( m_pOuter->GetSequence(), 0.0f, flToCycle, dp2, da2 );
				deltaPosLocal = dp1 + dp2;
				deltaAngLocal = da1 + da2;
			}

			Vector deltaPosWorld = deltaPosLocal;
			VectorYawRotate( deltaPosWorld, m_pOuter->GetAbsAngles().y, deltaPosWorld );

			const Vector start = m_pOuter->GetAbsOrigin();
			const Vector end = start + deltaPosWorld;

			trace_t tr;
			CTraceFilterSimple filter( m_pOuter, COLLISION_GROUP_NONE );
			UTIL_TraceHull( start, end, m_pOuter->WorldAlignMins(), m_pOuter->WorldAlignMaxs(), MASK_SOLID, &filter, &tr );

			QAngle ang = m_pOuter->GetAbsAngles();
			ang.y += deltaAngLocal.y;

			Vector vel( vec3_origin );
			m_pOuter->Teleport( &tr.endpos, &ang, &vel );

			m_flTankDeathPrevCycle = flToCycle;
		}
	}
#endif

	if ( m_pOuter->GetMoveType() != MOVETYPE_LADDER )
		return;

	// Playback rate should freeze while stationary on ladders.
	Vector vel;
	GetOuterAbsVelocity( vel );
	const bool bMovingOnLadder = ( fabs( vel.z ) > MOVING_MINIMUM_SPEED || vel.Length2D() > MOVING_MINIMUM_SPEED );
	GetOuter()->SetPlaybackRate( bMovingOnLadder ? 1.0f : 0.0f );

	// Worldmodel should face the ladder while climbing.
	Vector ladderNormal = m_pPlayer->GetLadderNormal();
	ladderNormal.z = 0.0f;
	if ( ladderNormal.LengthSqr() > 1e-6f )
	{
		Vector ladderForward = -ladderNormal;
		ladderForward.NormalizeInPlace();

		const float ladderYaw = AngleNormalize( RAD2DEG( atan2( ladderForward.y, ladderForward.x ) ) );

		m_angRender[YAW] = ladderYaw;
		m_angRender[PITCH] = m_angRender[ROLL] = 0.0f;

		m_flGoalFeetYaw = ladderYaw;
		m_flCurrentFeetYaw = ladderYaw;
		m_bCurrentFeetYawInitialized = true;

		// Keep torso aligned with feet while on ladder.
		SetOuterBodyYaw( 0.0f );
	}
}


void CCSPlayerAnimState::InitCS( CBaseAnimatingOverlay *pEntity, ICSPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences )
{
	CModAnimConfig config;
	config.m_flMaxBodyYawDegrees = 90;
	config.m_LegAnimType = legAnimType;
	config.m_bUseAimSequences = bUseAimSequences;

	m_pPlayer = ToCSPlayer( pEntity );

	m_pHelpers = pHelpers;

	BaseClass::Init( pEntity, config );
}


//--------------------------------------------------------------------------------------------------------------
void CCSPlayerAnimState::CheckCachedSequenceValidity( void )
{
	if ( m_cachedModelIndex != GetOuter()->GetModelIndex() )
	{
		m_namedSequence.RemoveAll();

		m_cachedModelIndex = GetOuter()->GetModelIndex();
		for ( int i=0; i<=ACT_CROUCHIDLE; ++i )
		{
			m_sequenceCache[i] = -1;
		}

		// precache the sequences we'll be using for movement
		if ( m_cachedModelIndex > 0 )
		{
			m_sequenceCache[ACT_HOP - 1] = GetOuter()->SelectWeightedSequence( ACT_HOP );
			m_sequenceCache[ACT_IDLE - 1] = GetOuter()->SelectWeightedSequence( ACT_IDLE );
			m_sequenceCache[ACT_RUN_CROUCH - 1] = GetOuter()->SelectWeightedSequence( ACT_RUN_CROUCH );
			m_sequenceCache[ACT_CROUCHIDLE - 1] = GetOuter()->SelectWeightedSequence( ACT_CROUCHIDLE );
			m_sequenceCache[ACT_RUN - 1] = GetOuter()->SelectWeightedSequence( ACT_RUN );
			m_sequenceCache[ACT_WALK - 1] = GetOuter()->SelectWeightedSequence( ACT_WALK );
			m_sequenceCache[ACT_IDLE - 1] = GetOuter()->SelectWeightedSequence( ACT_IDLE );
		}
	}
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Cache the sequence numbers for the first ACT_HOP activities, since the CS player doesn't have multiple
 * sequences per activity.
 */
int CCSPlayerAnimState::SelectWeightedSequence( Activity activity )
{
	VPROF( "CCSPlayerAnimState::ComputeMainSequence" );

	if ( activity > ACT_CROUCHIDLE || activity < 1 )
	{
		return GetOuter()->SelectWeightedSequence( activity );
	}

	CheckCachedSequenceValidity();

	int sequence = m_sequenceCache[ activity - 1 ];
	if ( sequence < 0 )
	{
		// just in case, look up the sequence if we didn't precache it above
		sequence = m_sequenceCache[ activity - 1 ] = GetOuter()->SelectWeightedSequence( activity );
	}

#if defined(CLIENT_DLL) && defined(_DEBUG)
	int realSequence = GetOuter()->SelectWeightedSequence( activity );
	Assert( realSequence == sequence );
#endif

	return sequence;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Try to look up named sequences in a CUtlDict cache before falling back to the normal LookupSequence.  It's
 * best to avoid the normal LookupSequence when your models have 750+ sequences...
 */
int CCSPlayerAnimState::CalcSequenceIndex( const char *pBaseName, ... )
{
	VPROF( "CCSPlayerAnimState::CalcSequenceIndex" );

	CheckCachedSequenceValidity();

	char szFullName[512];
	va_list marker;
	va_start( marker, pBaseName );
	Q_vsnprintf( szFullName, sizeof( szFullName ), pBaseName, marker );
	va_end( marker );

	int iSequence = m_namedSequence.Find( szFullName );
	if ( iSequence == m_namedSequence.InvalidIndex() )
	{
		iSequence = GetOuter()->LookupSequence( szFullName );
		m_namedSequence.Insert( szFullName, iSequence );
	}
	else
	{
		iSequence = m_namedSequence[iSequence];
	}

#if defined(CLIENT_DLL) && defined(_DEBUG)
	int realSequence = GetOuter()->LookupSequence( szFullName );
	Assert( realSequence == iSequence );
#endif
	
	// Show warnings if we can't find anything here.
	if ( iSequence == -1 )
	{
		static CUtlDict<int,int> dict;
		if ( dict.Find( szFullName ) == -1 )
		{
			dict.Insert( szFullName, 0 );
			Warning( "CalcSequenceIndex: can't find '%s'.\n", szFullName );
		}

		iSequence = 0;
	}

	return iSequence;
}


void CCSPlayerAnimState::ClearAnimationState()
{
	m_bJumping = false;
	m_bFiring = false;
	m_bReloading = false;
	m_flReloadHoldEndTime = 0.0f;
	m_bThrowingGrenade = m_bPrimingGrenade = false;
	m_iLastThrowGrenadeCounter = GetOuterGrenadeThrowCounter();
	m_bTankDeathRestarted = false;
	m_flTankDeathPrevCycle = 0.0f;
	m_bTankDeathPrevCycleValid = false;
	m_bWasIncapacitated = false;
	m_bWasBeingRevived = false;
	m_bIncapDyingFinished = false;
	
	BaseClass::ClearAnimationState();
}


void CCSPlayerAnimState::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	Assert( event != PLAYERANIMEVENT_THROW_GRENADE );

	MDLCACHE_CRITICAL_SECTION();
	switch ( event )
	{
	case PLAYERANIMEVENT_PRIMARYATTACK:
		m_flFireCycle = 0;
		m_iFireSequence = GetOuter()->SelectWeightedSequence(ACT_SECONDARYATTACK);
		m_bFiring = m_iFireSequence != -1;
		// If we are interrupting a (shotgun) reload, cancel the reload, and fire next frame.
		if (m_bFiring && m_bReloading)
		{
			m_bReloading = false;
			m_iReloadSequence = -1;

			m_delayedFire = event;
			m_bFiring = false;
			m_iFireSequence = -1;

			CAnimationLayer* pLayer = m_pOuter->GetAnimOverlay(RELOADSEQUENCE_LAYER);
			if (pLayer)
			{
				pLayer->m_flWeight = 0.0f;
				pLayer->m_nOrder = 15;
			}
		}
		break;
	case PLAYERANIMEVENT_FIRE_GUN_PRIMARY:
	case PLAYERANIMEVENT_FIRE_GUN_SECONDARY:
		if (GetOuter()->GetTeamNumber() == 3) {
			m_flFireCycle = 0;
			m_iFireSequence = GetOuter()->SelectWeightedSequence(ACT_TERROR_ATTACK);
			m_bFiring = m_iFireSequence != -1;
			// If we are interrupting a (shotgun) reload, cancel the reload, and fire next frame.
			if (m_bFiring && m_bReloading)
			{
				m_bReloading = false;
				m_iReloadSequence = -1;

				m_delayedFire = event;
				m_bFiring = false;
				m_iFireSequence = -1;

				CAnimationLayer* pLayer = m_pOuter->GetAnimOverlay(RELOADSEQUENCE_LAYER);
				if (pLayer)
				{
					pLayer->m_flWeight = 0.0f;
					pLayer->m_nOrder = 15;
				}
			}
			return;
		}

		// Regardless of what we're doing in the fire layer, restart it.
		m_flFireCycle = 0;
		m_iFireSequence = CalcFireLayerSequence( event );
		m_bFiring = m_iFireSequence != -1;

		// If we are interrupting a (shotgun) reload, cancel the reload, and fire next frame.
		if ( m_bFiring && m_bReloading )
		{
			m_bReloading = false;
			m_iReloadSequence = -1;

			m_delayedFire = event;
			m_bFiring = false;
			m_iFireSequence = -1;

			CAnimationLayer *pLayer = m_pOuter->GetAnimOverlay( RELOADSEQUENCE_LAYER );
			if ( pLayer )
			{
				pLayer->m_flWeight = 0.0f;
				pLayer->m_nOrder = 15;
			}
		}

#ifdef CLIENT_DLL
		if ( m_bFiring && !m_bReloading )
		{
			if ( m_pPlayer )
			{
				m_pPlayer->ProcessMuzzleFlashEvent();
			}
		}
#endif
		break;

	case PLAYERANIMEVENT_JUMP:
		// Play the jump animation.
		m_bJumping = true;
		m_bFirstJumpFrame = true;
		m_flJumpStartTime = gpGlobals->curtime;
		break;

	case PLAYERANIMEVENT_RELOAD:
		{
			// ignore normal reload events for shotguns - they get sent to trigger sounds etc only
			CWeaponCSBase *pWeapon = m_pHelpers->CSAnim_GetActiveWeapon();
			if ( pWeapon && pWeapon->GetCSWpnData().m_WeaponType != WEAPONTYPE_SHOTGUN )
			{
				m_iReloadSequence = CalcReloadLayerSequence( event );
				if ( m_iReloadSequence != -1 )
				{
					m_bReloading = true;
					m_flReloadCycle = 0;
				}
				else
				{
					m_bReloading = false;
				}
			}
		}
		break;

	case PLAYERANIMEVENT_RELOAD_START:
	case PLAYERANIMEVENT_RELOAD_LOOP:
		// Set the hold time for _start and _loop anims, then fall through to the _end case
		m_flReloadHoldEndTime = gpGlobals->curtime + 0.75f;

	case PLAYERANIMEVENT_RELOAD_END:
		{
			// ignore shotgun reload events for non-shotguns
			CWeaponCSBase *pWeapon = m_pHelpers->CSAnim_GetActiveWeapon();
			if ( pWeapon && pWeapon->GetCSWpnData().m_WeaponType != WEAPONTYPE_SHOTGUN )
			{
				m_flReloadHoldEndTime = 0.0f;  // clear this out in case we set it in _START or _LOOP above
			}
			else
			{
				m_iReloadSequence = CalcReloadLayerSequence( event );
				if ( m_iReloadSequence != -1 )
				{
					m_bReloading = true;
					m_flReloadCycle = 0;
				}
				else
				{
					m_bReloading = false;
				}
			}
		}
		break;

	case PLAYERANIMEVENT_CLEAR_FIRING:
		{
			m_iFireSequence = -1;
		}
		break;

	default:
		Assert( !"CCSPlayerAnimState::DoAnimationEvent" );
	}
}


float g_flThrowGrenadeFraction = 0.25;
bool CCSPlayerAnimState::IsThrowingGrenade()
{
	if ( m_bThrowingGrenade )
	{
		// An animation event would be more appropriate here.
		return m_flGrenadeCycle < g_flThrowGrenadeFraction;
	}
	else
	{
		bool bThrowPending = (m_iLastThrowGrenadeCounter != GetOuterGrenadeThrowCounter());
		return bThrowPending || IsOuterGrenadePrimed();
	}
}


int CCSPlayerAnimState::CalcReloadLayerSequence( PlayerAnimEvent_t event )
{
	if ( m_delayedFire != PLAYERANIMEVENT_COUNT )
		return -1;

	const char *weaponSuffix = GetWeaponSuffix();
	if ( !weaponSuffix )
		return -1;

	CWeaponCSBase *pWeapon = m_pHelpers->CSAnim_GetActiveWeapon();
	if ( !pWeapon )
		return -1;

	const char *prefix = "";
	switch ( GetCurrentMainSequenceActivity() )
	{
		case ACT_PLAYER_RUN_FIRE:
		case ACT_RUN:
			prefix = "run";
			break;

		case ACT_PLAYER_WALK_FIRE:
		case ACT_WALK:
			prefix = "walk";
			break;

		case ACT_PLAYER_CROUCH_FIRE:
		case ACT_CROUCHIDLE:
			prefix = "crouch_idle";
			break;

		case ACT_PLAYER_CROUCH_WALK_FIRE:
		case ACT_RUN_CROUCH:
			prefix = "crouch_walk";
			break;

		default:
		case ACT_PLAYER_IDLE_FIRE:
			prefix = "idle";
			break;
	}

	const char *reloadSuffix = "";
	switch ( event )
	{
	case PLAYERANIMEVENT_RELOAD_START:
		reloadSuffix = "_start";
		break;

	case PLAYERANIMEVENT_RELOAD_LOOP:
		reloadSuffix = "_loop";
		break;

	case PLAYERANIMEVENT_RELOAD_END:
		reloadSuffix = "_end";
		break;
	}

	// First, look for <prefix>_reload_<weapon name><_start|_loop|_end>.
	char szName[512];
	Q_snprintf( szName, sizeof( szName ), "%s_reload_%s%s", prefix, weaponSuffix, reloadSuffix );
	int iReloadSequence = m_pOuter->LookupSequence( szName );
	if ( iReloadSequence != -1 )
		return iReloadSequence;

	// Next, look for reload_<weapon name><_start|_loop|_end>.
	Q_snprintf( szName, sizeof( szName ), "reload_%s%s", weaponSuffix, reloadSuffix );
	iReloadSequence = m_pOuter->LookupSequence( szName );
	if ( iReloadSequence != -1 )
		return iReloadSequence;

	// Ok, look for generic categories.. pistol, shotgun, rifle, etc.
	if ( pWeapon->GetCSWpnData().m_WeaponType == WEAPONTYPE_PISTOL )
	{
		Q_snprintf( szName, sizeof( szName ), "reload_pistol" );
		iReloadSequence = m_pOuter->LookupSequence( szName );
		if ( iReloadSequence != -1 )
			return iReloadSequence;
	}
			
	// Fall back to reload_m4.
	iReloadSequence = CalcSequenceIndex( "reload_m4" );
	if ( iReloadSequence > 0 )
		return iReloadSequence;

	return -1;
}

	void CCSPlayerAnimState::UpdateLayerSequenceGeneric( CStudioHdr *pStudioHdr, int iLayer, bool &bEnabled, float &flCurCycle, int &iSequence, bool bWaitAtEnd )
{
	if ( !bEnabled || iSequence < 0 )
		return;

		// Increment the fire sequence's cycle.
		flCurCycle += m_pOuter->GetSequenceCycleRate( pStudioHdr, iSequence ) * gpGlobals->frametime;
		if ( flCurCycle > 1 )
		{
			if ( bWaitAtEnd )
			{
				flCurCycle = 1;
			}
			else
			{
				// Not firing anymore.
				bEnabled = false;
				iSequence = 0;
				return;
			}
		}

	// Now dump the state into its animation layer.
	CAnimationLayer *pLayer = m_pOuter->GetAnimOverlay( iLayer );

	pLayer->m_flCycle = flCurCycle;
	pLayer->m_nSequence = iSequence;

	pLayer->m_flPlaybackRate = 1.0f;

	if (m_pOuter && m_pOuter->GetMoveType() == MOVETYPE_LADDER) {
		pLayer->m_flWeight = 0.0f;
	}
	else {
		pLayer->m_flWeight = 1.0f;
	}

	pLayer->m_nOrder = iLayer;
#ifndef CLIENT_DLL
	pLayer->m_fFlags |= ANIM_LAYER_ACTIVE; 
#endif
}

bool CCSPlayerAnimState::IsOuterGrenadePrimed()
{
	CBaseCombatCharacter *pChar = m_pOuter->MyCombatCharacterPointer();
	if ( pChar )
	{
		CBaseCSGrenade *pGren = dynamic_cast<CBaseCSGrenade*>( pChar->GetActiveWeapon() );
		return pGren && pGren->IsPinPulled();
	}
	else
	{
		return NULL;
	}
}


void CCSPlayerAnimState::ComputeGrenadeSequence( CStudioHdr *pStudioHdr )
{
	VPROF( "CCSPlayerAnimState::ComputeGrenadeSequence" );

	if ( m_bThrowingGrenade )
	{
		UpdateLayerSequenceGeneric( pStudioHdr, GRENADESEQUENCE_LAYER, m_bThrowingGrenade, m_flGrenadeCycle, m_iGrenadeSequence, false );
	}
	else
	{
		if ( m_pPlayer )
		{
			CBaseCombatWeapon *pWeapon = m_pPlayer->GetActiveWeapon();
			CBaseCSGrenade *pGren = dynamic_cast<CBaseCSGrenade*>( pWeapon );
			if ( !pGren )
			{
				// The player no longer has a grenade equipped. Bail.
				m_iLastThrowGrenadeCounter = GetOuterGrenadeThrowCounter();
				return;
			}
		}

		// Priming the grenade isn't an event.. we just watch the player for it.
		// Also play the prime animation first if he wants to throw the grenade.
		bool bThrowPending = (m_iLastThrowGrenadeCounter != GetOuterGrenadeThrowCounter());
		if ( IsOuterGrenadePrimed() || bThrowPending )
		{
			if ( !m_bPrimingGrenade )
			{
				// If this guy just popped into our PVS, and he's got his grenade primed, then
				// let's assume that it's all the way primed rather than playing the prime
				// animation from the start.
				if ( TimeSinceLastAnimationStateClear() < 0.4f )
				{
					m_flGrenadeCycle = 1;
				}
				else
				{
					m_flGrenadeCycle = 0;
				}
					
				m_iGrenadeSequence = CalcGrenadePrimeSequence();
				m_bPrimingGrenade = true;
			}

			UpdateLayerSequenceGeneric( pStudioHdr, GRENADESEQUENCE_LAYER, m_bPrimingGrenade, m_flGrenadeCycle, m_iGrenadeSequence, true );
			
			// If we're waiting to throw and we're done playing the prime animation...
			if ( bThrowPending && m_flGrenadeCycle == 1 )
			{
				m_iLastThrowGrenadeCounter = GetOuterGrenadeThrowCounter();

				// Now play the throw animation.
				m_iGrenadeSequence = CalcGrenadeThrowSequence();
				if ( m_iGrenadeSequence != -1 )
				{
					// Configure to start playing 
					m_bThrowingGrenade = true;
					m_bPrimingGrenade = false;
					m_flGrenadeCycle = 0;
				}
			}
		}
		else
		{
			m_bPrimingGrenade = false;
		}
	}
}


int CCSPlayerAnimState::CalcGrenadePrimeSequence()
{
	return CalcSequenceIndex( "idle_shoot_gren1" );
}


int CCSPlayerAnimState::CalcGrenadeThrowSequence()
{
	return CalcSequenceIndex( "idle_shoot_gren2" );
}


int CCSPlayerAnimState::GetOuterGrenadeThrowCounter()
{
	if ( m_pPlayer )
		return m_pPlayer->m_iThrowGrenadeCounter;
	else
		return 0;
}


void CCSPlayerAnimState::ComputeReloadSequence( CStudioHdr *pStudioHdr )
{
	VPROF( "CCSPlayerAnimState::ComputeReloadSequence" );
	bool hold = m_flReloadHoldEndTime > gpGlobals->curtime;
	UpdateLayerSequenceGeneric( pStudioHdr, RELOADSEQUENCE_LAYER, m_bReloading, m_flReloadCycle, m_iReloadSequence, hold );
	if ( !m_bReloading )
	{
		m_flReloadHoldEndTime = 0.0f;
	}
}

void CCSPlayerAnimState::UpdateAimSequenceLayers(
	float flCycle,
	int iFirstLayer,
	bool bForceIdle,
	CSequenceTransitioner* pTransitioner,
	float flWeightScale
)
{
	if (m_pPlayer->m_pounceAttacker) {
		// Don't update aim sequences if we're being pounced, since we'll be playing the pounce animation.
		return;
	}

	BaseClass::UpdateAimSequenceLayers(flCycle, iFirstLayer, bForceIdle, pTransitioner, flWeightScale);
}

int CCSPlayerAnimState::CalcAimLayerSequence( float *flCycle, float *flAimSequenceWeight, bool bForceIdle )
{
	VPROF( "CCSPlayerAnimState::CalcAimLayerSequence" );

	const char *pSuffix = GetWeaponSuffix();
	if ( !pSuffix )
		return 0;

	if ( bForceIdle )
	{
		switch ( GetCurrentMainSequenceActivity() )
		{
			case ACT_CROUCHIDLE:
			case ACT_RUN_CROUCH:
				return CalcSequenceIndex( "%s%s", DEFAULT_CROUCH_IDLE_NAME, pSuffix );

			default:
				return CalcSequenceIndex( "%s%s", DEFAULT_IDLE_NAME, pSuffix );
		}
	}
	else
	{
		switch ( GetCurrentMainSequenceActivity() )
		{
			case ACT_RUN:
			case ACT_RUN_HURT:
				return CalcSequenceIndex( "%s%s", DEFAULT_RUN_NAME, pSuffix );

			case ACT_WALK:
			case ACT_WALK_HURT:
			case ACT_RUNTOIDLE:
			case ACT_IDLETORUN:
				return CalcSequenceIndex( "%s%s", DEFAULT_WALK_NAME, pSuffix );

			case ACT_CROUCHIDLE:
				return CalcSequenceIndex( "%s%s", DEFAULT_CROUCH_IDLE_NAME, pSuffix );

			case ACT_RUN_CROUCH:
				return CalcSequenceIndex( "%s%s", DEFAULT_CROUCH_WALK_NAME, pSuffix );

			case ACT_IDLE:
			default:
				return CalcSequenceIndex( "%s%s", DEFAULT_IDLE_NAME, pSuffix );
		}
	}
}


const char* CCSPlayerAnimState::GetWeaponSuffix()
{
	VPROF( "CCSPlayerAnimState::GetWeaponSuffix" );

	// Figure out the weapon suffix.
	CWeaponCSBase *pWeapon = m_pHelpers->CSAnim_GetActiveWeapon();
	if ( !pWeapon )
		return 0;

	const char *pSuffix = pWeapon->GetCSWpnData().m_szAnimExtension;

#ifdef CS_SHIELD_ENABLED
	if ( m_pOuter->HasShield() == true )
	{
		if ( m_pOuter->IsShieldDrawn() == true )
			pSuffix = "shield";
		else 
			pSuffix = "shield_undeployed";
	}
#endif

	return pSuffix;
}


int CCSPlayerAnimState::CalcFireLayerSequence(PlayerAnimEvent_t event)
{
	// Figure out the weapon suffix.
	CWeaponCSBase *pWeapon = m_pHelpers->CSAnim_GetActiveWeapon();
	if ( !pWeapon )
		return -1;

	const char *pSuffix = GetWeaponSuffix();
	if ( !pSuffix )
		return -1;

	char tempsuffix[32];
	if ( pWeapon->GetWeaponID() == WEAPON_ELITE )
	{
		bool bPrimary = (event == PLAYERANIMEVENT_FIRE_GUN_PRIMARY);
		Q_snprintf( tempsuffix, sizeof(tempsuffix), "%s_%c", pSuffix, bPrimary?'r':'l' );
		pSuffix = tempsuffix;
	}

	// Grenades handle their fire events separately
	if ( event == PLAYERANIMEVENT_THROW_GRENADE ||
		pWeapon->GetWeaponID() == WEAPON_HEGRENADE ||
		pWeapon->GetWeaponID() == WEAPON_SMOKEGRENADE ||
		pWeapon->GetWeaponID() == WEAPON_FLASHBANG )
	{
		return -1;
	}

	switch ( GetCurrentMainSequenceActivity() )
	{
		case ACT_PLAYER_RUN_FIRE:
		case ACT_RUN:
		case ACT_RUN_HURT:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_RUN_NAME, pSuffix );

		case ACT_PLAYER_WALK_FIRE:
		case ACT_WALK:
		case ACT_WALK_HURT:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_WALK_NAME, pSuffix );

		case ACT_PLAYER_CROUCH_FIRE:
		case ACT_CROUCHIDLE:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_CROUCH_NAME, pSuffix );

		case ACT_PLAYER_CROUCH_WALK_FIRE:
		case ACT_RUN_CROUCH:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_CROUCH_WALK_NAME, pSuffix );

		default:
		case ACT_PLAYER_IDLE_FIRE:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_IDLE_NAME, pSuffix );
	}
}


bool CCSPlayerAnimState::CanThePlayerMove()
{
	return m_pHelpers->CSAnim_CanMove();
}


float CCSPlayerAnimState::GetCurrentMaxGroundSpeed()
{
	Activity currentActivity = 	m_pOuter->GetSequenceActivity( m_pOuter->GetSequence() );
	if ( currentActivity == ACT_WALK || currentActivity == ACT_IDLE )
		return ANIM_TOPSPEED_WALK;
	else if ( currentActivity == ACT_RUN )
	{
		if ( m_pPlayer )
		{
			CBaseCombatWeapon *activeWeapon = m_pPlayer->GetActiveWeapon();
			if ( activeWeapon )
			{
				CWeaponCSBase *csWeapon = dynamic_cast< CWeaponCSBase * >( activeWeapon );
				if ( csWeapon )
				{
					return csWeapon->GetMaxSpeed();
				}
			}
		}
		return ANIM_TOPSPEED_RUN;
	}
	else if ( currentActivity == ACT_RUN_CROUCH )
		return ANIM_TOPSPEED_RUN_CROUCH;
	else
		return 0;
}


bool CCSPlayerAnimState::HandleJumping()
{
	if ( m_bJumping )
	{
		if ( m_bFirstJumpFrame )
		{

#if !defined(CLIENT_DLL)
            //=============================================================================
            // HPE_BEGIN:
            // [dwenger] Needed for fun-fact implementation
            //=============================================================================

			CCS_GameStats.IncrementStat(m_pPlayer, CSSTAT_TOTAL_JUMPS, 1);

            //=============================================================================
            // HPE_END
            //=============================================================================
#endif

			m_bFirstJumpFrame = false;
			RestartMainSequence();	// Reset the animation.
		}

		// Don't check if he's on the ground for a sec.. sometimes the client still has the
		// on-ground flag set right when the message comes in.
		if ( gpGlobals->curtime - m_flJumpStartTime > 0.2f )
		{
			if ( m_pOuter->GetFlags() & FL_ONGROUND )
			{
				m_bJumping = false;
				RestartMainSequence();	// Reset the animation.
			}
		}
	}

	// Are we still jumping? If so, keep playing the jump animation.
	return m_bJumping;
}


Activity CCSPlayerAnimState::CalcMainActivity()
{
	float flOuterSpeed = GetOuterXYSpeed();

	// Tank (hulk) staged death: keep playing the death activity while still "alive" so the model draws.
	const bool bTankStagedDeath =
		( m_pPlayer &&
		m_pPlayer->IsAlive() &&
		m_pPlayer->GetTeamNumber() == TEAM_INFECTED &&
		m_pPlayer->GetZombieClass() == 8 &&
		m_pOuter->GetMoveType() == MOVETYPE_NONE &&
		m_pOuter->GetSolid() == SOLID_NONE &&
		m_pOuter->GetHealth() == 1 );

	if ( !bTankStagedDeath )
	{
		m_bTankDeathRestarted = false;
	}
	else
	{
		if ( !m_bTankDeathRestarted )
		{
			RestartMainSequence();
			m_bTankDeathRestarted = true;
		}

		const Activity curAct = (Activity)m_pOuter->GetSequenceActivity( m_pOuter->GetSequence() );
		if ( curAct == ACT_TERROR_DIE_FROM_STAND || curAct == ACT_TERROR_DIE_WHILE_RUNNING )
			return curAct;

		return ( flOuterSpeed > MOVING_MINIMUM_SPEED ) ? ACT_TERROR_DIE_WHILE_RUNNING : ACT_TERROR_DIE_FROM_STAND;
	}

	// Hunter pounce: victims play a pinned idle, attackers play the ripping melee.
	if ( m_pPlayer )
	{
		// Survivor incapacitation: play ACT_DIESIMPLE first, then an incap idle based on pistol type.
		if ( m_pPlayer->GetTeamNumber() == TEAM_SURVIVOR && m_pPlayer->m_bIncapacitated )
		{
			if ( m_pPlayer->m_bBeingRevived )
				return ACT_TERROR_INCAP_TO_STAND;

			if ( !m_bIncapDyingFinished )
				return ACT_DIESIMPLE;

			CWeaponCSBase *weapon = dynamic_cast< CWeaponCSBase * >( m_pPlayer->GetActiveWeapon() );
			if ( weapon && weapon->GetWeaponID() == WEAPON_ELITE )
				return ACT_IDLE_INCAP_ELITES;

			return ACT_IDLE_INCAP_PISTOL;
		}

		if ( m_pPlayer->m_pounceAttacker.Get() )
			return ACT_IDLE_POUNCED;

		if (m_pPlayer->m_pounceVictim.Get()) {
			return ACT_TERROR_HUNTER_POUNCE_MELEE;
		}
	}

	// Charger: victims play carried/slam/pounded animations; chargers play stagger/slam/pound animations.
	if ( m_pPlayer )
	{
		if ( m_pPlayer->m_chargerAttacker.Get() )
		{
			switch ( m_pPlayer->m_nChargerVictimAction )
			{
			case CHARGER_VICTIM_SLAMMED_GROUND:
				return ACT_TERROR_SLAMMED_GROUND;
			case CHARGER_VICTIM_POUNDED_DOWN:
				return ACT_TERROR_CHARGER_POUNDED_DOWN;
			case CHARGER_VICTIM_CARRIED:
			default:
				return ACT_TERROR_CARRIED;
			}
		}

		if ( m_pPlayer->GetTeamNumber() == TEAM_INFECTED && m_pPlayer->GetZombieClass() == 6 )
		{
			if ( m_pPlayer->m_nChargerAction == CHARGER_ACTION_STAGGER )
			{
				switch ( m_pPlayer->m_nChargerStaggerDir )
				{
				case CHARGER_STAGGER_DIR_LEFT:
					return ACT_TERROR_SHOVED_LEFTWARD_INTO_WALL;
				case CHARGER_STAGGER_DIR_RIGHT:
					return ACT_TERROR_SHOVED_RIGHTWARD_INTO_WALL;
				case CHARGER_STAGGER_DIR_BACK:
				default:
					return ACT_TERROR_SHOVED_BACKWARD_INTO_WALL;
				}
			}

			if ( m_pPlayer->m_nChargerAction == CHARGER_ACTION_SLAM )
			{
				return ACT_TERROR_SLAM_GROUND;
			}

			if ( m_pPlayer->m_nChargerAction == CHARGER_ACTION_POUND )
			{
				CCSPlayer *victim = m_pPlayer->m_chargerVictim.Get();
				if ( victim )
				{
					const int survivorClass = victim->GetSurvivorClass();
					if ( survivorClass == 1 )
						return ACT_TERROR_CHARGER_POUND_DOWN_COACH;
					if ( survivorClass == 3 )
						return ACT_TERROR_CHARGER_POUND_DOWN_PRODUCER;
				}
				return ACT_TERROR_CHARGER_POUND_DOWN;
			}
		}

		if ( m_pPlayer->GetTeamNumber() == TEAM_INFECTED && m_pPlayer->GetZombieClass() == 8 )
		{
			if ( m_pPlayer->m_nTankAction == TANK_ACTION_ROCK_THROW )
			{
				return ACT_TANK_OVERHEAD_THROW;
			}
		}
	}

	// Ladder climbing uses explicit climb activities.
	if ( m_pOuter && m_pOuter->GetMoveType() == MOVETYPE_LADDER )
	{
		Vector vel;
		GetOuterAbsVelocity( vel );
		if ( vel.z > MOVING_MINIMUM_SPEED )
		{
			m_nLadderClimbDir = 1;
		}
		else if ( vel.z < -MOVING_MINIMUM_SPEED )
		{
			m_nLadderClimbDir = -1;
		}

		return ( m_nLadderClimbDir >= 0 ) ? ACT_CLIMB_UP : ACT_CLIMB_DOWN;
	}

	if ( HandleJumping() )
	{
		return ACT_JUMP;
	}
	else
	{
		Activity idealActivity = ACT_IDLE;

		if ( m_pOuter->GetFlags() & FL_ANIMDUCKING )
		{
			if ( flOuterSpeed > MOVING_MINIMUM_SPEED )
				idealActivity = ACT_RUN_CROUCH;
			else
				idealActivity = ACT_CROUCHIDLE;
		}
		else
		{
			if ( flOuterSpeed > MOVING_MINIMUM_SPEED )
			{
				if (GetOuter()->GetTeamNumber() == TEAM_SURVIVOR && GetOuter()->GetHealth() < 40) {
					if (flOuterSpeed > ARBITRARY_RUN_SPEED)
						idealActivity = ACT_RUN_HURT;
					else
						idealActivity = ACT_WALK_HURT;
				}
				else {
					if (flOuterSpeed > ARBITRARY_RUN_SPEED)
						idealActivity = ACT_RUN;
					else
						idealActivity = ACT_WALK;
				}
			}
			else
			{
				if (GetOuter()->GetTeamNumber() == TEAM_SURVIVOR && GetOuter()->GetHealth() < 40) {
					idealActivity = ACT_IDLE_HURT;
				}
				else {
					idealActivity = ACT_IDLE;
				}
			}
		}

		return idealActivity;
	}
}


void CCSPlayerAnimState::DebugShowAnimState( int iStartLine )
{
	engine->Con_NPrintf( iStartLine++, "fire  : %s, cycle: %.2f\n", m_bFiring ? GetSequenceName( m_pOuter->GetModelPtr(), m_iFireSequence ) : "[not firing]", m_flFireCycle );
	engine->Con_NPrintf( iStartLine++, "reload: %s, cycle: %.2f\n", m_bReloading ? GetSequenceName( m_pOuter->GetModelPtr(), m_iReloadSequence ) : "[not reloading]", m_flReloadCycle );
	BaseClass::DebugShowAnimState( iStartLine );
}


void CCSPlayerAnimState::ComputeSequences( CStudioHdr *pStudioHdr )
{
	BaseClass::ComputeSequences( pStudioHdr );

	VPROF( "CCSPlayerAnimState::ComputeSequences" );

	ComputeFireSequence( pStudioHdr );
	ComputeReloadSequence( pStudioHdr );
	ComputeGrenadeSequence( pStudioHdr );
}


void CCSPlayerAnimState::ClearAnimationLayers()
{
	if ( !m_pOuter )
		return;

	m_pOuter->SetNumAnimOverlays( NUM_LAYERS_WANTED );
	for ( int i=0; i < m_pOuter->GetNumAnimOverlays(); i++ )
	{
		// Client obeys Order of CBaseAnimatingOverlay::MAX_OVERLAYS (15), but server trusts only the ANIM_LAYER_ACTIVE flag.
		m_pOuter->GetAnimOverlay( i )->SetOrder( CBaseAnimatingOverlay::MAX_OVERLAYS );
#ifndef CLIENT_DLL
		m_pOuter->GetAnimOverlay( i )->m_fFlags = 0;
#endif
	}
}


void CCSPlayerAnimState::ComputeFireSequence( CStudioHdr *pStudioHdr )
{
	VPROF( "CCSPlayerAnimState::ComputeFireSequence" );

	if ( m_delayedFire != PLAYERANIMEVENT_COUNT )
	{
		DoAnimationEvent( m_delayedFire, 0 );
		m_delayedFire = PLAYERANIMEVENT_COUNT;
	}

	UpdateLayerSequenceGeneric( pStudioHdr, FIRESEQUENCE_LAYER, m_bFiring, m_flFireCycle, m_iFireSequence, false );
}
