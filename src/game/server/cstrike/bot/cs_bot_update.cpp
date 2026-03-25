//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

// Author: Michael S. Booth (mike@turtlerockstudios.com), 2003

#include "cbase.h"
#include "cs_gamerules.h"
#include "cs_bot.h"
#include "weapon_knife.h"
#include "fmtstr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CCSPlayer *FindClosestAliveSurvivor( CCSBot *me, float *outDistSqr = NULL )
{
	if ( outDistSqr )
		*outDistSqr = 1e30f;

	if ( !me )
		return NULL;

	CCSPlayer *closest = NULL;
	float bestDistSqr = 1e30f;
	const Vector myOrigin = me->GetAbsOrigin();

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( player->GetTeamNumber() != TEAM_SURVIVOR )
			continue;

		if ( !player->IsAlive() )
			continue;

		const float distSqr = ( player->GetAbsOrigin() - myOrigin ).LengthSqr();
		if ( distSqr < bestDistSqr )
		{
			bestDistSqr = distSqr;
			closest = player;
		}
	}

	if ( outDistSqr )
		*outDistSqr = bestDistSqr;

	return closest;
}

static CCSPlayer *FindClosestPounceRescueAttacker( CCSBot *me, CCSPlayer **outVictim = NULL )
{
	if ( outVictim )
		*outVictim = NULL;

	if ( !me )
		return NULL;

	if ( me->GetTeamNumber() != TEAM_SURVIVOR )
		return NULL;

	// Can't rescue others if we're currently pinned.
	if ( me->HasPounceAttacker() )
		return NULL;

	CCSPlayer *bestAttacker = NULL;
	CCSPlayer *bestVictim = NULL;
	float bestDistSqr = 1e30f;
	const Vector myOrigin = me->GetAbsOrigin();

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *victim = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !victim )
			continue;

		if ( victim == me )
			continue;

		if ( victim->GetTeamNumber() != TEAM_SURVIVOR )
			continue;

		if ( !victim->IsAlive() )
			continue;

		if ( !victim->HasPounceAttacker() )
			continue;

		CCSPlayer *attacker = victim->GetPounceAttacker();
		if ( !attacker || !attacker->IsAlive() )
			continue;

		if ( attacker->GetTeamNumber() != TEAM_INFECTED )
			continue;

		// Only treat hunter pounces as rescue events.
		if ( attacker->GetZombieClass() != 3 )
			continue;

		const float distSqr = ( victim->GetAbsOrigin() - myOrigin ).LengthSqr();
		if ( distSqr < bestDistSqr )
		{
			bestDistSqr = distSqr;
			bestAttacker = attacker;
			bestVictim = victim;
		}
	}

	if ( outVictim )
		*outVictim = bestVictim;

	return bestAttacker;
}

static CCSPlayer *FindClosestIncapReviveTarget( CCSBot *me )
{
	if ( !me )
		return NULL;

	if ( me->GetTeamNumber() != TEAM_SURVIVOR )
		return NULL;

	// Can't revive others if we're pinned or incapacitated ourselves.
	if ( me->HasPounceAttacker() || me->HasPounceVictim() || me->IsIncapacitated() )
		return NULL;

	CCSPlayer *best = NULL;
	float bestDistSqr = 1e30f;
	const Vector myOrigin = me->GetAbsOrigin();

	const float maxDist = 2500.0f;
	const float maxDistSqr = maxDist * maxDist;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player || player == me )
			continue;

		if ( player->GetTeamNumber() != TEAM_SURVIVOR )
			continue;

		if ( !player->IsAlive() )
			continue;

		if ( !player->IsIncapacitated() )
			continue;

		const float distSqr = ( player->GetAbsOrigin() - myOrigin ).LengthSqr();
		if ( distSqr > maxDistSqr )
			continue;

		if ( distSqr < bestDistSqr )
		{
			bestDistSqr = distSqr;
			best = player;
		}
	}

	return best;
}

static CBaseEntity *FindClosestVisibleCommonInfected( CCSBot *me, float maxRange )
{
	if ( !me )
		return NULL;

	const float maxRangeSqr = maxRange * maxRange;
	const Vector myOrigin = me->GetAbsOrigin();

	CBaseEntity *closest = NULL;
	float bestDistSqr = 1e30f;

	CBaseEntity *ent = NULL;
	while ( ( ent = gEntList.FindEntityByClassname( ent, "infected" ) ) != NULL )
	{
		CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( ent );
		if ( !combat || !combat->IsAlive() )
			continue;

		const Vector target = ent->WorldSpaceCenter();
		const float distSqr = ( target - myOrigin ).LengthSqr();
		if ( distSqr > maxRangeSqr )
			continue;

		if ( distSqr >= bestDistSqr )
			continue;

		if ( !me->IsVisible( target ) )
			continue;

		bestDistSqr = distSqr;
		closest = ent;
	}

	return closest;
}

static bool IsFriendlyPlayerNearPosition( CCSBot *me, const Vector &pos, float radius )
{
	if ( !me )
		return false;

	const float radiusSqr = radius * radius;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player || player == me )
			continue;

		if ( !player->IsAlive() )
			continue;

		if ( player->GetTeamNumber() != me->GetTeamNumber() )
			continue;

		if ( ( player->GetAbsOrigin() - pos ).LengthSqr() <= radiusSqr )
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------------
float CCSBot::GetMoveSpeed( void )
{
	return 250.0f;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Lightweight maintenance, invoked frequently
 */
void CCSBot::Upkeep( void )
{
	VPROF_BUDGET( "CCSBot::Upkeep", VPROF_BUDGETGROUP_NPCS );

	if (TheNavMesh->IsGenerating() || !IsAlive())
		return;
	
	// If bot_flipout is on, then generate some random commands.
	if ( cv_bot_flipout.GetBool() )
	{
		int val = RandomInt( 0, 2 );
		if ( val == 0 )
			MoveForward();
		else if ( val == 1 )
			MoveBackward();
		
		val = RandomInt( 0, 2 );
		if ( val == 0 )
			StrafeLeft();
		else if ( val == 1 )
			StrafeRight();

		if ( RandomInt( 0, 5 ) == 0 )
			Jump( true );
		
		val = RandomInt( 0, 2 );
		if ( val == 0 )
			Crouch();
		else ( val == 1 );
			StandUp();
	
		return;
	}
	
	// BOTPORT: Remove this nasty hack
	m_eyePosition = EyePosition();

	Vector myOrigin = GetCentroid( this );

	// Survivor bots: if we're trying to revive an incapacitated teammate, keep "use" held and keep looking at them.
	if ( GetTeamNumber() == TEAM_SURVIVOR && !IsAttacking() && !HasPounceAttacker() && !HasPounceVictim() && !IsIncapacitated() )
	{
		CCSPlayer *target = m_incapReviveTarget.Get();
		if ( target )
		{
			if ( !target->IsAlive() || target->GetTeamNumber() != TEAM_SURVIVOR || !target->IsIncapacitated() )
			{
				m_incapReviveTarget = NULL;
			}
			else if ( !IsUsingLadder() )
			{
				const Vector revivePos = target->EyePosition();
				const Vector to = revivePos - EyePosition();

				const float watchRange = 250.0f;
				if ( to.IsLengthLessThan( watchRange ) )
				{
					// Override any lingering "aim at last enemy" behavior while reviving.
					SetLookAt( "Revive", revivePos, PRIORITY_UNINTERRUPTABLE, 0.25f );

					const float useRange = PLAYER_USE_RADIUS - 10.0f; // shave off a fudge factor
					if ( to.IsLengthLessThan( useRange ) )
					{
						UseEnvironment();
						ResetStuckMonitor();
						ClearMovement();
					}
				}
			}
		}
	}

	// aiming must be smooth - update often
	if (IsAimingAtEnemy())
	{
		UpdateAimOffset();

		// aim at enemy, if he's still alive
		if (m_enemy != NULL && m_enemy->IsAlive())
		{
			Vector enemyOrigin = GetCentroid( m_enemy );

			if (m_isEnemyVisible)
			{
				//
				// Enemy is visible - determine which part of him to shoot at
				//
				const float sharpshooter = 0.8f;
				VisiblePartType aimAtPart;

				if (IsUsingMachinegun())
				{
					// spray the big machinegun at the enemy's gut
					aimAtPart = GUT;
				}
				else if (IsUsing( WEAPON_AWP ) || IsUsingShotgun())
				{
					// these weapons are best aimed at the chest
					aimAtPart = GUT;
				}
				else if (GetProfile()->GetSkill() > 0.5f && IsActiveWeaponRecoilHigh() )
				{
					// sprayin' and prayin' - aim at the gut since we're not going to be accurate
					aimAtPart = GUT;
				}
				else if (GetProfile()->GetSkill() < sharpshooter)
				{
					// low skill bots don't go for headshots
					aimAtPart = GUT;
				}
				else
				{
					// high skill - aim for the head
					aimAtPart = HEAD;
				}

				if (IsEnemyPartVisible( aimAtPart ))
				{
					m_aimSpot = GetPartPosition( GetBotEnemy(), aimAtPart );
				}
				else
				{
					// desired part is blocked - aim at whatever part is visible 
					if (IsEnemyPartVisible( GUT ))
					{
						m_aimSpot = GetPartPosition( GetBotEnemy(), GUT );
					}
					else if (IsEnemyPartVisible( HEAD ))
					{
						m_aimSpot = GetPartPosition( GetBotEnemy(), HEAD );
					}
					else if (IsEnemyPartVisible( LEFT_SIDE ))
					{
						m_aimSpot = GetPartPosition( GetBotEnemy(), LEFT_SIDE );
					}
					else if (IsEnemyPartVisible( RIGHT_SIDE ))
					{
						m_aimSpot = GetPartPosition( GetBotEnemy(), RIGHT_SIDE );
					}
					else // FEET
					{
						m_aimSpot = GetPartPosition( GetBotEnemy(), FEET );
					}
				}

				// high skill bots lead the target a little to compensate for update tick latency
				/*
				if (false && GetProfile()->GetSkill() > 0.5f)
				{
					const float k = 1.0f;
					m_aimSpot += k * g_flBotCommandInterval * (m_enemy->GetAbsVelocity() - GetAbsVelocity());
				}
				*/

			}
			else
			{
				// aim where we last saw enemy - but bend the ray so we dont point directly into walls
				// if we put this back, make sure you only bend the ray ONCE and keep the bent spot - dont continually recompute
				//BendLineOfSight( m_eyePosition, m_lastEnemyPosition, &m_aimSpot );
				m_aimSpot = m_lastEnemyPosition;
			}

			// add in aim error
			m_aimSpot.x += m_aimOffset.x;
			m_aimSpot.y += m_aimOffset.y;
			m_aimSpot.z += m_aimOffset.z;

			Vector to = m_aimSpot - EyePositionConst();

			QAngle idealAngle;
			VectorAngles( to, idealAngle );

			// adjust aim angle for recoil, based on bot skill
			const QAngle &punchAngles = GetPunchAngle();
			idealAngle -= punchAngles * GetProfile()->GetSkill();

			SetLookAngles( idealAngle.y, idealAngle.x );
		}
	}
	else
	{
		if (m_lookAtSpotClearIfClose)
		{
			// dont look at spots just in front of our face - it causes erratic view rotation
			const float tooCloseRange = 100.0f;
			if ((m_lookAtSpot - myOrigin).IsLengthLessThan( tooCloseRange ))
				m_lookAtSpotState = NOT_LOOKING_AT_SPOT;
		}

		switch( m_lookAtSpotState )
		{
			case NOT_LOOKING_AT_SPOT:
			{
				// look ahead
				SetLookAngles( m_lookAheadAngle, 0.0f );
				break;
			}

			case LOOK_TOWARDS_SPOT:
			{
				UpdateLookAt();
				if (IsLookingAtPosition( m_lookAtSpot, m_lookAtSpotAngleTolerance ))
				{
					m_lookAtSpotState = LOOK_AT_SPOT;
					m_lookAtSpotTimestamp = gpGlobals->curtime;
				}
				break;
			}

			case LOOK_AT_SPOT:
			{
				UpdateLookAt();

				if (m_lookAtSpotDuration >= 0.0f && gpGlobals->curtime - m_lookAtSpotTimestamp > m_lookAtSpotDuration)
				{
					m_lookAtSpotState = NOT_LOOKING_AT_SPOT;
					m_lookAtSpotDuration = 0.0f;
				}
				break;
			}
		}

		// have view "drift" very slowly, so view looks "alive"
		if (!IsUsingSniperRifle())
		{
			float driftAmplitude = 2.0f;
			if (IsBlind())
			{
				driftAmplitude = 5.0f;
			}

			m_lookYaw += driftAmplitude * BotCOS( 33.0f * gpGlobals->curtime );
			m_lookPitch += driftAmplitude * BotSIN( 13.0f * gpGlobals->curtime );
		}
	}
	
	// Survivor bots: shoot/shove common infected responsively (needs to run here since command flags reset every tick).
	if ( GetTeamNumber() == TEAM_SURVIVOR &&
		!IsSpecialInfected() &&
		!m_isOpeningDoor &&
		!IsAttacking() &&
		!IsAimingAtEnemy() &&
		!IsThrowingGrenade() &&
		gpGlobals->curtime < m_commonInfectedEngageUntil &&
		m_commonInfectedTarget != NULL )
	{
		CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( m_commonInfectedTarget.Get() );
		if ( combat && combat->IsAlive() )
		{
			const Vector targetPos = combat->WorldSpaceCenter();
			const float dist = ( targetPos - GetAbsOrigin() ).Length();

			// Aim towards the infected.
			Vector to = targetPos - EyePosition();
			QAngle idealAngle;
			VectorAngles( to, idealAngle );
			SetLookAngles( idealAngle.y, idealAngle.x );

			// Keep moving while fighting common infected so we don't get stuck toe-to-toe.
			StandUp();
			Run();
			if ( dist < 220.0f )
			{
				MoveAwayFromPosition( targetPos );
			}

			const float meleeRange = 90.0f;
			if ( gpGlobals->curtime < m_commonInfectedMeleeUntil || dist <= meleeRange )
			{
				m_commonInfectedMeleeUntil = gpGlobals->curtime + 0.6f;
				EquipKnife();
				PrimaryAttack();
			}
			else
			{
				m_commonInfectedMeleeUntil = 0.0f;
				if ( IsUsingKnife() )
				{
					EquipBestWeapon( true );
				}
				PrimaryAttack();
			}
		}
	}
	
	// view angles can change quickly
	UpdateLookAngles();
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Heavyweight processing, invoked less often
 */
void CCSBot::Update( void )
{
	VPROF_BUDGET( "CCSBot::Update", VPROF_BUDGETGROUP_NPCS );

	// If bot_flipout is on, then we only do stuff in Upkeep().
	if ( cv_bot_flipout.GetBool() )
		return;

	Vector myOrigin = GetCentroid( this );

	// if we are spectating, get into the game
	if (GetTeamNumber() == 0)
	{
		HandleCommand_JoinTeam( m_desiredTeam );
		int desiredClass = GetProfile()->GetSkin();
		if ( m_desiredTeam == TEAM_CT && desiredClass )
		{
			desiredClass = FIRST_CT_CLASS + desiredClass - 1;
		}
		else if ( m_desiredTeam == TEAM_TERRORIST && desiredClass )
		{
			desiredClass = FIRST_T_CLASS + desiredClass - 1;
		}
		HandleCommand_JoinClass( desiredClass );
		return;
	}


	// update our radio chatter
	// need to allow bots to finish their chatter even if they are dead
	GetChatter()->Update();
	
	// check if we are dead
	if (!IsAlive())
	{
		// remember that we died
		m_diedLastRound = true;

		BotDeathThink();
		return;
	}

	// the bot is alive and in the game at this point
	m_hasJoined = true;

	//
	// Debug beam rendering
	//

	if (cv_bot_debug.GetBool() && IsLocalPlayerWatchingMe())
	{
		DebugDisplay();
	}

	if (cv_bot_stop.GetBool())
		return;

	// Clear per-update squad ignore by default (will be re-enabled for urgent actions below).
	SetIgnoreSurvivorSquad( false );

	// Only survivors can revive/incap-rescue; clear any stale target if we changed teams.
	if ( GetTeamNumber() != TEAM_SURVIVOR )
	{
		m_incapReviveTarget = NULL;
	}

	// check if we are stuck
	StuckCheck();

	// Check for physics props and other breakables in our way that we can break
	BreakablesCheck();

	// Check for useable doors in our way that we need to open
	DoorCheck();

	// update travel distance to all players (this is an optimization)
	UpdateTravelDistanceToAllPlayers();

	// if our current 'noise' was heard a long time ago, forget it
	const float rememberNoiseDuration = 20.0f;
	if (m_noiseTimestamp > 0.0f && gpGlobals->curtime - m_noiseTimestamp > rememberNoiseDuration)
	{
		ForgetNoise();
	}

	// where are we
	if (!m_currentArea || !m_currentArea->Contains( myOrigin ))
	{
		m_currentArea = (CCSNavArea *)TheNavMesh->GetNavArea( myOrigin );
	}

	// track the last known area we were in
	if (m_currentArea && m_currentArea != m_lastKnownArea)
	{
		m_lastKnownArea = m_currentArea;

		OnEnteredNavArea( m_currentArea );
	}

	// keep track of how long we have been motionless
	const float stillSpeed = 10.0f;
	if (GetAbsVelocity().IsLengthLessThan( stillSpeed ))
	{
		m_stillTimer.Start();
	}
	else
	{
		m_stillTimer.Invalidate();
	}

	// if we're blind, retreat!
	if (IsBlind())
	{
		if (m_blindFire)
		{
			PrimaryAttack();
		}
	}

	UpdatePanicLookAround();

	//
	// Enemy acquisition and attack initiation
	//

	// take a snapshot and update our reaction time queue
	UpdateReactionQueue();

	// "threat" may be the same as our current enemy
	CCSPlayer *threat = GetRecognizedEnemy();

	// If a survivor is marked as "IT" (boomer vomit), special infected should focus them.
	if ( IsSpecialInfected() && CSGameRules() )
	{
		CCSPlayer *it = CSGameRules()->GetItTarget();
		if ( it )
		{
			threat = it;
		}
	}

	bool isRescuingPounce = false;
	bool isRevivingIncap = false;
	CCSPlayer *pounceVictim = NULL;
	if ( GetTeamNumber() == TEAM_SURVIVOR )
	{
		// Validate (or clear) current pounce rescue victim.
		{
			CCSPlayer *victim = m_pounceRescueVictim.Get();
			if ( victim )
			{
				if ( !victim->IsAlive() || victim->GetTeamNumber() != TEAM_SURVIVOR || !victim->HasPounceAttacker() )
				{
					m_pounceRescueVictim = NULL;
				}
			}
		}

		// Validate (or clear) current revive target.
		{
			CCSPlayer *target = m_incapReviveTarget.Get();
			if ( target )
			{
				if ( !target->IsAlive() || target->GetTeamNumber() != TEAM_SURVIVOR || !target->IsIncapacitated() )
				{
					m_incapReviveTarget = NULL;
				}
			}
		}

		CCSPlayer *pounceAttacker = FindClosestPounceRescueAttacker( this, &pounceVictim );
		if ( pounceAttacker )
		{
			threat = pounceAttacker;
			isRescuingPounce = true;
			m_pounceRescueVictim = pounceVictim;
			// Don't try to revive while we're actively rescuing a pounced teammate.
			m_incapReviveTarget = NULL;
		}
		else
		{
			m_pounceRescueVictim = NULL;
			pounceVictim = NULL;

			// If we see any enemy, don't commit to a revive.
			if ( threat )
			{
				m_incapReviveTarget = NULL;
			}
			else if ( !m_incapReviveTarget.Get() )
			{
				CCSPlayer *reviveTarget = FindClosestIncapReviveTarget( this );
				if ( reviveTarget )
				{
					m_incapReviveTarget = reviveTarget;
				}
			}
		}

		isRevivingIncap = ( m_incapReviveTarget.Get() != NULL );
	}

	// Survivor squad system: ignore forced squad follow while rescuing a pounce or reviving an incapacitated survivor.
	SetIgnoreSurvivorSquad( isRescuingPounce || isRevivingIncap );

	if (threat)
	{
		Vector threatOrigin = GetCentroid( threat );

		// adjust our personal "safe" time
		AdjustSafeTime();

		BecomeAlert();

		const float selfDefenseRange = 500.0f; // 750.0f;
		const float farAwayRange = 2000.0f;

		// Pounce rescue: move to the pinned survivor first (so we actually close distance),
		// then kill the pouncer once we're in position.
		if ( isRescuingPounce && pounceVictim )
		{
			// Don't risk grenades on a pinned teammate, and cancel reloads so we can intervene immediately.
			if ( IsUsingGrenade() || IsReloading() )
			{
				EquipBestWeapon( true );
			}

			// Make sure we keep tracking the pouncer as our enemy.
			SetBotEnemy( threat );

			const Vector victimOrigin = GetCentroid( pounceVictim );
			const float distToVictimSqr = ( victimOrigin - myOrigin ).LengthSqr();

			const float rescueEngageDist = 550.0f;
			const bool shouldApproachVictim = ( distToVictimSqr > ( rescueEngageDist * rescueEngageDist ) );

			if ( shouldApproachVictim )
			{
				// Ensure we can actually move (Attack state may otherwise stall at range).
				if ( IsAttacking() )
				{
					StopAttacking();
				}

				SetGoalEntity( pounceVictim );
				MoveTo( victimOrigin, FASTEST_ROUTE );

				// Opportunistic shots while we close distance.
				if ( IsVisible( threat, false, &m_visibleEnemyParts ) )
				{
					m_isEnemyVisible = true;
					m_lastSawEnemyTimestamp = gpGlobals->curtime;
					m_lastEnemyPosition = threatOrigin;
					AimAtEnemy();
					FireWeaponAtEnemy();
				}
				else
				{
					m_isEnemyVisible = false;
				}
			}
			else
			{
				Attack( threat );
			}

			TheCSBots()->SetLastSeenEnemyTimestamp();
		}
		else
		{
			//
			// Decide if we should attack
			//
			bool doAttack = false;
			switch( GetDisposition() )
			{
				case IGNORE_ENEMIES:
				{
					// never attack
					doAttack = false;
					break;
				}

				case SELF_DEFENSE:
				{
					// attack if fired on
					doAttack = (IsPlayerLookingAtMe( threat, 0.99f ) && DidPlayerJustFireWeapon( threat ));

					// attack if enemy very close
					if (!doAttack)
					{
						doAttack = (myOrigin - threatOrigin).IsLengthLessThan( selfDefenseRange );
					}

					break;
				}

				case ENGAGE_AND_INVESTIGATE:
				case OPPORTUNITY_FIRE:
				{
					if ((myOrigin - threatOrigin).IsLengthGreaterThan( farAwayRange ))
					{
						// enemy is very far away - wait to take our shot until he is closer
						// unless we are a sniper or he is shooting at us
						if (IsSniper())
						{
							// snipers love far away targets
							doAttack = true;
						}
						else
						{
							// attack if fired on
							doAttack = (IsPlayerLookingAtMe( threat, 0.99f ) && DidPlayerJustFireWeapon( threat ));					
						}
					}
					else
					{
						// normal combat range
						doAttack = true;
					}

					break;
				}
			}

			// if we aren't attacking but we are being attacked, retaliate
			if (!doAttack && !IsAttacking() && GetDisposition() != IGNORE_ENEMIES)
			{
				const float recentAttackDuration = 1.0f;
				if (GetTimeSinceAttacked() < recentAttackDuration)
				{
					// we may not be attacking our attacker, but at least we're not just taking it
					// (since m_attacker isn't reaction-time delayed, we can't directly use it)
					doAttack = true;
					PrintIfWatched( "Ouch! Retaliating!\n" );
				}
			}

			// Special infected should aggressively hunt survivors, but some classes (like boomers)
			// should avoid committing to the Attack state from long range.
			if ( IsSpecialInfected() )
			{
				const int zombieClass = GetZombieClass();

				if ( IsAttacking() )
				{
					doAttack = true;
				}
				else if ( zombieClass == 2 )
				{
					ConVarRef z_vomit_target_range( "z_vomit_target_range" );
					const float vomitRange = z_vomit_target_range.IsValid() ? z_vomit_target_range.GetFloat() : 280.0f;
					const float boomerVomitCommitRange = MAX( 350.0f, vomitRange + 100.0f );

					if ( ( myOrigin - threatOrigin ).IsLengthLessThan( boomerVomitCommitRange ) )
					{
						doAttack = true;
					}
					else
					{
						doAttack = false;
					}
				}
				else if ( zombieClass == 3 )
				{
					// Hunters should disengage when hurt, then try again with a pounce when safe.
					ConVarRef z_hunter_lunge_stagger_time( "z_hunter_lunge_stagger_time" );
					const float hurtWindow = z_hunter_lunge_stagger_time.IsValid() ? z_hunter_lunge_stagger_time.GetFloat() : 1.0f;

					if ( GetTimeSinceAttacked() < hurtWindow )
					{
						doAttack = false;
					}
					else
					{
						doAttack = true;
					}
				}
				else
				{
					doAttack = true;
				}
			}

			if (doAttack)
			{
				if (!IsAttacking() || threat != GetBotEnemy())
				{
					Attack( threat );
				}
			}
			else
			{
				// dont attack, but keep track of nearby enemies
				SetBotEnemy( threat );
				m_isEnemyVisible = true;
			}

			TheCSBots()->SetLastSeenEnemyTimestamp();
		}
	}

	//
	// Validate existing enemy, if any
	//
	if (m_enemy != NULL)
	{
		CCSPlayer *enemyPlayer = ToCSPlayer( m_enemy );
		if ( GetTeamNumber() == TEAM_SURVIVOR &&
			 enemyPlayer &&
			 enemyPlayer->GetTeamNumber() == TEAM_INFECTED &&
			 enemyPlayer->GetZombieClass() > 0 &&
			 enemyPlayer->IsGhost() )
		{
			m_enemy = NULL;
			m_isEnemyVisible = false;
		}
		else if (IsAwareOfEnemyDeath())
		{
			// we have noticed that our enemy has died
			m_enemy = NULL;
			m_isEnemyVisible = false;
		}
		else
		{
			// check LOS to current enemy (chest & head), in case he's dead (GetNearestEnemy() only returns live players)
			// note we're not checking FOV - once we've acquired an enemy (which does check FOV), assume we know roughly where he is
			if (IsVisible( m_enemy, false, &m_visibleEnemyParts ))
			{
				m_isEnemyVisible = true;
				m_lastSawEnemyTimestamp = gpGlobals->curtime;
				m_lastEnemyPosition = GetCentroid( m_enemy );
			}
			else
			{
				m_isEnemyVisible = false;
			}
				
			// check if enemy died
			if (m_enemy->IsAlive())
			{
				m_enemyDeathTimestamp = 0.0f;
				m_isLastEnemyDead = false;
			}
			else if (m_enemyDeathTimestamp == 0.0f)
			{
				// note time of death (to allow bots to overshoot for a time)
				m_enemyDeathTimestamp = gpGlobals->curtime;
				m_isLastEnemyDead = true;
			}
		}
	}
	else
	{
		m_isEnemyVisible = false;
	}


	// if we have seen an enemy recently, keep an eye on him if we can
	if (!IsBlind() && !IsLookingAtSpot(PRIORITY_UNINTERRUPTABLE) )
	{
		const float seenRecentTime = 3.0f;
		if (m_enemy != NULL && GetTimeSinceLastSawEnemy() < seenRecentTime)
		{
			AimAtEnemy();
		}
		else
		{
			StopAiming();
		}
	}
	else if( IsAimingAtEnemy() )
	{
		StopAiming();
	}

	//
	// Hack to fire while retreating
	/// @todo Encapsulate aiming and firing on enemies separately from current task
	//
	if (GetDisposition() == IGNORE_ENEMIES)
	{
		FireWeaponAtEnemy();
	}

	// toss grenades
	LookForGrenadeTargets();

	// process grenade throw state machine
	UpdateGrenadeThrow();

	// avoid enemy grenades
	AvoidEnemyGrenades();


	// check if our weapon is totally out of ammo
	// or if we no longer feel "safe", equip our weapon
	if (!IsSafe() && !IsUsingGrenade() && IsActiveWeaponOutOfAmmo())
	{
		EquipBestWeapon();
	}

	/// @todo This doesn't work if we are restricted to just knives and sniper rifles because we cant use the rifle at close range
	if (!IsSafe() && !IsUsingGrenade() && IsUsingKnife() && !IsEscapingFromBomb())
	{
		EquipBestWeapon();
	}

	// if we haven't seen an enemy in awhile, and we switched to our pistol during combat, 
	// switch back to our primary weapon (if it still has ammo left)
	const float safeRearmTime = 5.0f;
	if (!IsReloading() && IsUsingPistol() && !IsPrimaryWeaponEmpty() && GetTimeSinceLastSawEnemy() > safeRearmTime)
	{
		EquipBestWeapon();
	}

	// reload our weapon if we must
	ReloadCheck();

	// equip silencer
	SilencerCheck();

	// listen to the radio
	RespondToRadioCommands();

	// make way
	const float avoidTime = 0.33f;
	if (gpGlobals->curtime - m_avoidTimestamp < avoidTime && m_avoid != NULL)
	{
		StrafeAwayFromPosition( GetCentroid( m_avoid ) );
	}
	else
	{
		m_avoid = NULL;
	}

	// if we're using a sniper rifle and are no longer attacking, stop looking thru scope
	if (!IsAtHidingSpot() && !IsAttacking() && IsUsingSniperRifle() && IsUsingScope())
	{
		Attack3();
	}

	if (!IsBlind())
	{
		// check encounter spots
		UpdatePeripheralVision();

		// watch for snipers
		if (CanSeeSniper() && !HasSeenSniperRecently())
		{
			GetChatter()->SpottedSniper();

			const float sniperRecentInterval = 20.0f;
			m_sawEnemySniperTimer.Start( sniperRecentInterval );
		}

		//
		// Update gamestate
		//
		if (m_bomber != NULL)
			GetChatter()->SpottedBomber( GetBomber() );

		if (CanSeeLooseBomb())
			GetChatter()->SpottedLooseBomb( TheCSBots()->GetLooseBomb() );
	}

	//
	// Scenario interrupts
	//
	switch (TheCSBots()->GetScenario())
	{
		case CCSBotManager::SCENARIO_DEFUSE_BOMB:
		{
			// flee if the bomb is ready to blow and we aren't defusing it or attacking and we know where the bomb is
			// (aggressive players wait until its almost too late)
			float gonnaBlowTime = 8.0f - (2.0f * GetProfile()->GetAggression());

			// if we have a defuse kit, can wait longer
			if (m_bHasDefuser)
				gonnaBlowTime *= 0.66f;

			if (!IsEscapingFromBomb() &&								// we aren't already escaping the bomb
				TheCSBots()->IsBombPlanted() &&							// is the bomb planted
				GetGameState()->IsPlantedBombLocationKnown() &&			// we know where the bomb is
				TheCSBots()->GetBombTimeLeft() < gonnaBlowTime &&		// is the bomb about to explode
				!IsDefusingBomb() &&									// we aren't defusing the bomb
				!IsAttacking())											// we aren't in the midst of a firefight
			{
				EscapeFromBomb();
				break;
			}

			break;
		}

		case CCSBotManager::SCENARIO_RESCUE_HOSTAGES:
		{
			if (GetTeamNumber() == TEAM_CT)
			{
				UpdateHostageEscortCount();
			}
			else
			{
				// Terrorists have imperfect information on status of hostages
				unsigned char status = GetGameState()->ValidateHostagePositions();

				if (status & CSGameState::HOSTAGES_ALL_GONE)
				{
					GetChatter()->HostagesTaken();
					Idle();
				}
				else if (status & CSGameState::HOSTAGE_GONE)
				{
					GetGameState()->HostageWasTaken();
					Idle();
				}
			}
			break;
		}
	}


	//
	// Follow nearby humans if our co-op is high and we have nothing else to do
	// If we were just following someone, don't auto-follow again for a short while to 
	// give us a chance to do something else.
	//
	const float earliestAutoFollowTime = 5.0f;
	const float minAutoFollowTeamwork = 0.4f;
	if (cv_bot_auto_follow.GetBool() &&
		TheCSBots()->GetElapsedRoundTime() > earliestAutoFollowTime &&
		GetProfile()->GetTeamwork() > minAutoFollowTeamwork && 
		CanAutoFollow() &&
		!IsBusy() && 
		!IsFollowing() && 
		!IsBlind() && 
		!GetGameState()->IsAtPlantedBombsite())
	{

		// chance of following is proportional to teamwork attribute
		if (GetProfile()->GetTeamwork() > RandomFloat( 0.0f, 1.0f ))
		{
			CCSPlayer *leader = GetClosestVisibleHumanFriend();
			if (leader && leader->IsAutoFollowAllowed())
			{
				// count how many bots are already following this player
				const float maxFollowCount = 2;
				if (GetBotFollowCount( leader ) < maxFollowCount)
				{
					const float autoFollowRange = 300.0f;
					Vector leaderOrigin = GetCentroid( leader );
					if ((leaderOrigin - myOrigin).IsLengthLessThan( autoFollowRange ))
					{
						CNavArea *leaderArea = TheNavMesh->GetNavArea( leaderOrigin );
						if (leaderArea)
						{
							PathCost cost( this, FASTEST_ROUTE );
							float travelRange = NavAreaTravelDistance( GetLastKnownArea(), leaderArea, cost );
							if (travelRange >= 0.0f && travelRange < autoFollowRange)
							{
								// follow this human
								Follow( leader );
								PrintIfWatched( "Auto-Following %s\n", leader->GetPlayerName() );

								if (CSGameRules()->IsCareer())
								{
									GetChatter()->Say( "FollowingCommander", 10.0f );
								}
								else
								{
									GetChatter()->Say( "FollowingSir", 10.0f );
								}
							}
						}
					}
				}
			}
		}
		else
		{
			// we decided not to follow, don't re-check for a duration
			m_allowAutoFollowTime = gpGlobals->curtime + 15.0f + (1.0f - GetProfile()->GetTeamwork()) * 30.0f;
		}
	}

	if (IsFollowing())
	{
		// if we are following someone, make sure they are still alive
		CBaseEntity *leader = m_leader;
		if (leader == NULL || !leader->IsAlive())
		{
			StopFollowing();
		}

		// decide whether to continue following them
		const float highTeamwork = 0.85f;
		if (GetProfile()->GetTeamwork() < highTeamwork)
		{
			float minFollowDuration = 15.0f;
			if (GetFollowDuration() > minFollowDuration + 40.0f * GetProfile()->GetTeamwork())
			{
				// we are bored of following our leader
				StopFollowing();
				PrintIfWatched( "Stopping following - bored\n" );
			}
		}
	}

	// Survivor bots should fight common infected when they see them.
	if ( GetTeamNumber() == TEAM_SURVIVOR && !IsSpecialInfected() && !m_isOpeningDoor )
	{
		// Acquire a visible common infected target to shoot/shove (separate from player enemies).
		if ( !IsAttacking() && !IsAimingAtEnemy() )
		{
			const float scanInterval = 0.35f;
			if ( gpGlobals->curtime >= m_commonInfectedNextScanTimestamp )
			{
				m_commonInfectedNextScanTimestamp = gpGlobals->curtime + scanInterval;

				CBaseEntity *target = NULL;

				// Prefer the common infected that is actively hurting us (if it's still alive/visible).
				const float attackerMemory = 1.25f;
				if ( m_commonInfectedAttacker && ( gpGlobals->curtime - m_commonInfectedAttackedTimestamp ) < attackerMemory )
				{
					CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( m_commonInfectedAttacker.Get() );
					if ( combat && combat->IsAlive() && IsVisible( combat->WorldSpaceCenter() ) )
					{
						target = combat;
					}
				}

				// Otherwise, shoot the closest visible one in range.
				if ( !target )
				{
					const float maxCommonRange = 750.0f;
					target = FindClosestVisibleCommonInfected( this, maxCommonRange );
				}

				m_commonInfectedTarget = target;
				if ( target )
				{
					// Keep engaging for a short time even if LOS flickers between updates.
					m_commonInfectedEngageUntil = gpGlobals->curtime + 0.5f;
					SetLookAt( "Common Infected", target->WorldSpaceCenter(), PRIORITY_HIGH, 0.2f, true, 15.0f, true );
				}
				else
				{
					m_commonInfectedEngageUntil = 0.0f;
					m_commonInfectedMeleeUntil = 0.0f;
				}
			}
		}

		// Survivor bots should also use grenades (when they have them) against packs of common infected.
		const float commonGrenadeCooldown = 12.0f;
		if ( gpGlobals->curtime >= ( m_commonInfectedGrenadeTimestamp + commonGrenadeCooldown ) && HasGrenade() && !IsThrowingGrenade() )
		{
			const float minThrowRange = 250.0f;
			const float maxThrowRange = 900.0f;
			const float teammateSafetyRadius = 250.0f;

			Vector grenadeTarget( vec3_origin );
			bool shouldThrow = false;

			// Prefer throwing at the common infected that is currently hurting us.
			if ( m_commonInfectedAttacker )
			{
				CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( m_commonInfectedAttacker.Get() );
				if ( combat && combat->IsAlive() )
				{
					const Vector target = combat->WorldSpaceCenter();
					const float distToTarget = ( target - GetAbsOrigin() ).Length();
					if ( distToTarget >= minThrowRange && distToTarget <= maxThrowRange && IsVisible( target ) && !IsFriendlyPlayerNearPosition( this, target, teammateSafetyRadius ) )
					{
						grenadeTarget = target;
						shouldThrow = true;
					}
				}
			}

			// Otherwise, throw at the center of a nearby visible pack.
			if ( !shouldThrow )
			{
				const float packRange = 650.0f;
				const float packRangeSqr = packRange * packRange;

				int visibleCount = 0;
				Vector sum( vec3_origin );

				CBaseEntity *ent = NULL;
				while ( ( ent = gEntList.FindEntityByClassname( ent, "infected" ) ) != NULL )
				{
					CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( ent );
					if ( !combat || !combat->IsAlive() )
						continue;

					const Vector target = ent->WorldSpaceCenter();
					if ( ( target - GetAbsOrigin() ).LengthSqr() > packRangeSqr )
						continue;

					if ( !IsVisible( target ) )
						continue;

					sum += target;
					++visibleCount;

					if ( visibleCount >= 8 )
						break;
				}

				const int minPackSizeToGrenade = 4;
				if ( visibleCount >= minPackSizeToGrenade )
				{
					const Vector target = sum / (float)visibleCount;
					const float distToTarget = ( target - GetAbsOrigin() ).Length();
					if ( distToTarget >= minThrowRange && distToTarget <= maxThrowRange && !IsFriendlyPlayerNearPosition( this, target, teammateSafetyRadius ) )
					{
						grenadeTarget = target;
						shouldThrow = true;
					}
				}
			}

			if ( shouldThrow )
			{
				if ( !IsUsingGrenade() )
				{
					EquipGrenade();
				}

				if ( IsUsingGrenade() )
				{
					ThrowGrenade( grenadeTarget );
					m_commonInfectedGrenadeTimestamp = gpGlobals->curtime;
				}
			}
		}

		// If we're being damaged by a common infected, prioritize killing that one immediately.
		// This prevents survivor bots from getting chewed down while they ignore the attacker.
		const float commonAttackerFocusTime = 2.0f;
		if ( m_commonInfectedAttacker && ( gpGlobals->curtime - m_commonInfectedAttackedTimestamp ) <= commonAttackerFocusTime )
		{
			CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( m_commonInfectedAttacker.Get() );
			if ( combat && combat->IsAlive() )
			{
				EquipBestWeapon();

				const Vector target = combat->WorldSpaceCenter();
				SetLookAt( "Common Infected (attacker)", target, PRIORITY_HIGH, 0.2f, true, 10.0f, true );

				// If a common infected is right on us, shove it back (melee bash) instead of trying to shoot through it.
				{
					CWeaponCSBase* active = GetActiveCSWeapon();
					const float shoveRange = 85.0f;
					if (active && gpGlobals->curtime >= active->m_flNextSecondaryAttack && (target - GetAbsOrigin()).Length2DSqr() <= Square(shoveRange * 0.75f))
					{
						SecondaryAttack();
					}
				}

				if ( GetActiveWeapon() )
				{
					const bool canShoot = IsLookingAtPosition( target, 10.0f ) && !IsFriendInLineOfFire();
					CWeaponCSBase* active = GetActiveCSWeapon();
					const float shoveRange = 85.0f;
					if (active && gpGlobals->curtime >= active->m_flNextSecondaryAttack && canShoot )
					{
						PrimaryAttack();
					}
				}
			}
			else
			{
				m_commonInfectedAttacker = NULL;
				m_commonInfectedAttackedTimestamp = 0.0f;
			}
		}

		const bool hasPlayerThreat = ( threat != NULL ) || ( m_enemy != NULL );
		if ( !hasPlayerThreat )
		{
			const float commonInfectedEngageRange = 1400.0f;
			CBaseEntity *common = FindClosestVisibleCommonInfected( this, commonInfectedEngageRange );
			if ( common )
			{
				EquipBestWeapon();
				SetLookAt( "Common Infected", common->WorldSpaceCenter(), PRIORITY_HIGH, 0.2f, true, 10.0f, true );
			}
		}

	}

	// Special infected behavior: always hunt survivors and keep following their position while searching.
	// Boomers prefer to hide/ambush when survivors are nearby, then commit once close enough to vomit.
	if ( IsSpecialInfected() && !m_isOpeningDoor && !m_isAttacking )
	{
		if ( IsFollowing() )
		{
			StopFollowing();
		}

		SetTask( SEEK_AND_DESTROY );
		SetRogue( true );
		SetDisposition( ENGAGE_AND_INVESTIGATE );

		EquipKnife();

		float targetDistSqr = 1e30f;
		CCSPlayer *targetSurvivor = threat;
		if ( !targetSurvivor )
		{
			targetSurvivor = FindClosestAliveSurvivor( this, &targetDistSqr );
		}
		else
		{
			targetDistSqr = ( targetSurvivor->GetAbsOrigin() - GetAbsOrigin() ).LengthSqr();
		}

		if ( targetSurvivor )
		{
			const Vector targetPos = targetSurvivor->GetAbsOrigin();

			SetLookAt( "Hunt Survivor", targetSurvivor->WorldSpaceCenter(), PRIORITY_HIGH, 0.2f, true, 10.0f, true );

			const float repathInterval = 1.0f;
			const float repathTolerance = 150.0f;
			bool shouldRepathNow = ( !IsMovingTo() && !IsHunting() && !IsHiding() ) || ( gpGlobals->curtime - m_stateTimestamp ) > repathInterval;

			// If the survivor moved far from our current goal, update our path to keep "following" them.
			if ( !shouldRepathNow )
			{
				if ( HasPath() )
				{
					if ( ( GetPathEndpoint() - targetPos ).IsLengthGreaterThan( repathTolerance ) )
					{
						shouldRepathNow = true;
					}
				}
				else
				{
					shouldRepathNow = true;
				}
			}

			const int zombieClass = GetZombieClass();

			// Hunter behavior: if hurt recently, disengage by lunging away and hiding.
			if ( zombieClass == 3 )
			{
				ConVarRef z_hunter_lunge_stagger_time( "z_hunter_lunge_stagger_time" );
				const float hurtWindow = z_hunter_lunge_stagger_time.IsValid() ? z_hunter_lunge_stagger_time.GetFloat() : 1.0f;

				if ( GetTimeSinceAttacked() < hurtWindow )
				{
					const bool seesSurvivor = IsVisible( targetSurvivor, CHECK_FOV );
					if ( seesSurvivor )
					{
						const float hunterHideRange = 1200.0f;
						const float hunterHideDuration = 999999.0f;

						if ( !IsHiding() )
						{
							// Lunge away (not a pounce) and immediately try to take cover.
							Vector away = GetAbsOrigin() - targetPos;
							away.z = 0.0f;
							if ( away.NormalizeInPlace() > 0.0f )
							{
								QAngle awayAngles;
								VectorAngles( away, awayAngles );
								awayAngles.x = 0.0f;
								SnapEyeAngles( awayAngles );
							}

							StandUp();
							Run();

							CBaseCombatWeapon *weapon = GetActiveWeapon();
							if ( weapon && weapon->m_flNextPrimaryAttack <= gpGlobals->curtime )
							{
								CHunterClaw *claw = dynamic_cast< CHunterClaw * >( weapon );
								if ( claw )
								{
									// This is a disengage lunge, not an aimed pounce.
									claw->SetBotLungeTarget( NULL );
								}
								PrimaryAttack();
							}

							if ( !TryToHide( NULL, hunterHideDuration, hunterHideRange, true, USE_NEAREST ) )
							{
								DestroyPath();
								MoveAwayFromPosition( targetPos );
							}
						}
					}
				}
				else if ( shouldRepathNow )
				{
					MoveTo( targetPos, FASTEST_ROUTE );
				}
			}
			// Boomer behavior.
			else if ( zombieClass == 2 )
			{
				ConVarRef z_vomit_target_range( "z_vomit_target_range" );
				const float vomitRange = z_vomit_target_range.IsValid() ? z_vomit_target_range.GetFloat() : 280.0f;

				const float boomerVomitCommitRange = MAX( 350.0f, vomitRange + 100.0f );
				const float boomerHideRange = MAX( 1200.0f, vomitRange + 400.0f );

				const float boomerCommitDistSqr = boomerVomitCommitRange * boomerVomitCommitRange;

				// Once we start hiding, don't immediately resume "following" just because LOS broke.
				// Stay hidden until survivors get close enough that we should commit to vomiting.
				if ( IsHiding() && targetDistSqr > boomerCommitDistSqr )
				{
					// Keep hiding.
				}
				else
				{
					const bool seesSurvivor = IsVisible( targetSurvivor, CHECK_FOV );
					if ( seesSurvivor && targetDistSqr > boomerCommitDistSqr )
					{
						if ( !IsHiding() )
						{
							// Keep hiding once we decide to hide, until survivors get close enough that
							// we should commit to vomiting.
							const float boomerHideDuration = 999999.0f;
							if ( !TryToHide( NULL, boomerHideDuration, boomerHideRange, true, USE_NEAREST ) )
							{
								// No good cover nearby - at least stop pushing in and try to break LOS.
								DestroyPath();
								Run();
								MoveAwayFromPosition( targetPos );
							}
						}
					}
					else if ( shouldRepathNow )
					{
						MoveTo( targetPos, FASTEST_ROUTE );
					}
				}
			}
			// Spitter behavior: hide when seen, commit to spit when close.
			else if ( zombieClass == 4 )
			{
				ConVarRef z_spit_target_range( "z_spit_target_range" );
				const float spitRange = z_spit_target_range.IsValid() ? z_spit_target_range.GetFloat() : 500.0f;

				const float spitterCommitDistSqr = spitRange * spitRange;
				const float spitterHideRange = MAX( 1200.0f, spitRange + 400.0f );

				if ( IsHiding() && targetDistSqr > spitterCommitDistSqr )
				{
					// Keep hiding until survivors get close enough to commit.
				}
				else
				{
					const bool seesSurvivor = IsVisible( targetSurvivor, CHECK_FOV );
					if ( seesSurvivor && targetDistSqr > spitterCommitDistSqr )
					{
						if ( !IsHiding() )
						{
							const float spitterHideDuration = 999999.0f;
							if ( !TryToHide( NULL, spitterHideDuration, spitterHideRange, true, USE_NEAREST ) )
							{
								DestroyPath();
								Run();
								MoveAwayFromPosition( targetPos );
							}
						}
					}
					else if ( shouldRepathNow )
					{
						MoveTo( targetPos, FASTEST_ROUTE );
					}
				}
			}
			else if ( shouldRepathNow )
			{
				MoveTo( targetPos, FASTEST_ROUTE );
			}
		}
	}

	// Survivor bots: if we have no immediate combat threat, prioritize reviving our incapacitated teammate.
	// Do this late, so scenario/auto-follow logic doesn't override it.
	if ( isRevivingIncap && !m_isAttacking && !m_isOpeningDoor && !IsBlind() )
	{
		CCSPlayer *target = m_incapReviveTarget.Get();
		if ( target && target->IsAlive() && target->IsIncapacitated() )
		{
			SetGoalEntity( target );
			MoveTo( GetCentroid( target ), FASTEST_ROUTE );
		}
		else
		{
			m_incapReviveTarget = NULL;
			SetGoalEntity( NULL );
			SetIgnoreSurvivorSquad( isRescuingPounce );
			isRevivingIncap = false;
		}
	}

	//
	// Execute state machine
	//
	if (m_isOpeningDoor)
	{
		// opening doors takes precedence over attacking because DoorCheck() will stop opening doors if
		// we don't have a knife out.
		m_openDoorState.OnUpdate( this );

		if (m_openDoorState.IsDone())
		{
			// open door behavior is finished, return to previous behavior context
			m_openDoorState.OnExit( this );
			m_isOpeningDoor = false;
		}
	}
	else if (m_isAttacking)
	{
		m_attackState.OnUpdate( this );
	}
	else
	{
		m_state->OnUpdate( this );
	}

	// do wait behavior
	if (!IsAttacking() && IsWaiting())
	{
		ResetStuckMonitor();
		ClearMovement();
	}

	// don't move while reloading unless we see an enemy
	if (IsReloading() && !m_isEnemyVisible)
	{
		ResetStuckMonitor();
		ClearMovement();
	}

	// if we get too far ahead of the hostages we are escorting, wait for them
	if (!IsAttacking() && m_inhibitWaitingForHostageTimer.IsElapsed())
	{
		const float waitForHostageRange = 500.0f;
		if ((GetTask() == COLLECT_HOSTAGES || GetTask() == RESCUE_HOSTAGES) && GetRangeToFarthestEscortedHostage() > waitForHostageRange)
		{
			if (!m_isWaitingForHostage)
			{
				// just started waiting
				m_isWaitingForHostage = true;
				m_waitForHostageTimer.Start( 10.0f );
			}
			else
			{
				// we've been waiting
				if (m_waitForHostageTimer.IsElapsed())
				{
					// give up waiting for awhile
					m_isWaitingForHostage = false;
					m_inhibitWaitingForHostageTimer.Start( 3.0f );
				}
				else
				{
					// keep waiting
					ResetStuckMonitor();
					ClearMovement();
				}
			}
		}
	}

	// remember our prior safe time status
	m_wasSafe = IsSafe();
}


//--------------------------------------------------------------------------------------------------------------
class DrawTravelTime 
{
public:
	DrawTravelTime( const CCSBot *me )
	{
		m_me = me;
	}

	bool operator() ( CBasePlayer *player )
	{
		if (player->IsAlive() && !m_me->InSameTeam( player ))
		{
			CFmtStr msg;
			player->EntityText(	0,
								msg.sprintf( "%3.0f", m_me->GetTravelDistanceToPlayer( (CCSPlayer *)player ) ),
								0.1f );


			if (m_me->DidPlayerJustFireWeapon( ToCSPlayer( player ) ))
			{
				player->EntityText( 1, "BANG!", 0.1f );
			}
		}

		return true;
	}

	const CCSBot *m_me;
};


//--------------------------------------------------------------------------------------------------------------
/**
 * Render bot debug info
 */
void CCSBot::DebugDisplay( void ) const
{
	const float duration = 0.15f;
	CFmtStr msg;
	
	NDebugOverlay::ScreenText( 0.5f, 0.34f, msg.sprintf( "Skill: %d%%", (int)(100.0f * GetProfile()->GetSkill()) ), 255, 255, 255, 150, duration );

	if ( m_pathLadder )
	{
		NDebugOverlay::ScreenText( 0.5f, 0.36f, msg.sprintf( "Ladder: %d", m_pathLadder->GetID() ), 255, 255, 255, 150, duration );
	}

	// show safe time
	float safeTime = GetSafeTimeRemaining();
	if (safeTime > 0.0f)
	{
		NDebugOverlay::ScreenText( 0.5f, 0.38f, msg.sprintf( "SafeTime: %3.2f", safeTime ), 255, 255, 255, 150, duration );
	}

	// show if blind
	if (IsBlind())
	{
		NDebugOverlay::ScreenText( 0.5f, 0.38f, msg.sprintf( "<<< BLIND >>>" ), 255, 255, 255, 255, duration );
	}

	// show if alert
	if (IsAlert())
	{
		NDebugOverlay::ScreenText( 0.5f, 0.38f, msg.sprintf( "ALERT" ), 255, 0, 0, 255, duration );
	}

	// show if panicked
	if (IsPanicking())
	{
		NDebugOverlay::ScreenText( 0.5f, 0.36f, msg.sprintf( "PANIC" ), 255, 255, 0, 255, duration );
	}

	// show behavior variables
	if (m_isAttacking)
	{
		NDebugOverlay::ScreenText( 0.5f, 0.4f, msg.sprintf( "ATTACKING: %s", GetBotEnemy()->GetPlayerName() ), 255, 0, 0, 255, duration );
	}
	else
	{
		NDebugOverlay::ScreenText( 0.5f, 0.4f, msg.sprintf( "State: %s", m_state->GetName() ), 255, 255, 0, 255, duration );
		NDebugOverlay::ScreenText( 0.5f, 0.42f, msg.sprintf( "Task: %s", GetTaskName() ), 0, 255, 0, 255, duration );
		NDebugOverlay::ScreenText( 0.5f, 0.44f, msg.sprintf( "Disposition: %s", GetDispositionName() ), 100, 100, 255, 255, duration );
		NDebugOverlay::ScreenText( 0.5f, 0.46f, msg.sprintf( "Morale: %s", GetMoraleName() ), 0, 200, 200, 255, duration );
	}

	// show look at status
	if (m_lookAtSpotState != NOT_LOOKING_AT_SPOT)
	{
		const char *string = msg.sprintf( "LookAt: %s (%s)", m_lookAtDesc, (m_lookAtSpotState == LOOK_TOWARDS_SPOT) ? "LOOK_TOWARDS_SPOT" : "LOOK_AT_SPOT" );

		NDebugOverlay::ScreenText( 0.5f, 0.60f, string, 255, 255, 0, 150, duration );
	}

	NDebugOverlay::ScreenText( 0.5f, 0.62f, msg.sprintf( "Steady view = %s", HasViewBeenSteady( 0.2f ) ? "YES" : "NO" ), 255, 255, 0, 150, duration );


	// show friend/foes I know of
	NDebugOverlay::ScreenText( 0.5f, 0.64f, msg.sprintf( "Nearby friends = %d", m_nearbyFriendCount ), 100, 255, 100, 150, duration );
	NDebugOverlay::ScreenText( 0.5f, 0.66f, msg.sprintf( "Nearby enemies = %d", m_nearbyEnemyCount ), 255, 100, 100, 150, duration );

	if ( m_lastNavArea )
	{
		NDebugOverlay::ScreenText( 0.5f, 0.68f, msg.sprintf( "Nav Area: %d (%s)", m_lastNavArea->GetID(), m_szLastPlaceName.Get() ), 255, 255, 255, 150, duration );
		/*
		if ( cv_bot_traceview.GetBool() )
		{
			if ( m_currentArea )
			{
				NDebugOverlay::Line( GetAbsOrigin(), m_currentArea->GetCenter(), 0, 255, 0, true, 0.1f );
			}
			else if ( m_lastKnownArea )
			{
				NDebugOverlay::Line( GetAbsOrigin(), m_lastKnownArea->GetCenter(), 255, 0, 0, true, 0.1f );
			}
			else if ( m_lastNavArea )
			{
				NDebugOverlay::Line( GetAbsOrigin(), m_lastNavArea->GetCenter(), 0, 0, 255, true, 0.1f );
			}
		}
		*/
	}

	// show debug message history
	float y = 0.8f;
	const float lineHeight = 0.02f;
	const float fadeAge = 7.0f;
	const float maxAge = 10.0f;
	for( int i=0; i<TheBots->GetDebugMessageCount(); ++i )
	{
		const CBotManager::DebugMessage *msg = TheBots->GetDebugMessage( i );

		if (msg->m_age.GetElapsedTime() < maxAge)
		{
			int alpha = 255;

			if (msg->m_age.GetElapsedTime() > fadeAge)
			{
				alpha *= (1.0f - (msg->m_age.GetElapsedTime() - fadeAge) / (maxAge - fadeAge));
			}

			NDebugOverlay::ScreenText( 0.5f, y, msg->m_string, 255, 255, 255, alpha, duration );
			y += lineHeight;
		}
	}

	// show noises
	const Vector *noisePos = GetNoisePosition();
	if (noisePos)
	{
		const float size = 25.0f;
		NDebugOverlay::VertArrow( *noisePos + Vector( 0, 0, size ), *noisePos, size/4.0f, 255, 255, 0, 0, true, duration );
	}

	// show aim spot
	if (IsAimingAtEnemy())
	{
		NDebugOverlay::Cross3D( m_aimSpot, 5.0f, 255, 0, 0, true, duration );
	}



	if (IsHiding())
	{
		// show approach points
		DrawApproachPoints();
	}
	else
	{
		// show encounter spot data
		if (false && m_spotEncounter)
		{
			NDebugOverlay::Line( m_spotEncounter->path.from, m_spotEncounter->path.to, 0, 150, 150, true, 0.1f );

			Vector dir = m_spotEncounter->path.to - m_spotEncounter->path.from;
			float length = dir.NormalizeInPlace();

			const SpotOrder *order;
			Vector along;

			FOR_EACH_VEC( m_spotEncounter->spots, it )
			{
				order = &m_spotEncounter->spots[ it ];

				// ignore spots the enemy could not have possibly reached yet
				if (order->spot->GetArea())
				{
					if (TheCSBots()->GetElapsedRoundTime() < order->spot->GetArea()->GetEarliestOccupyTime( OtherTeam( GetTeamNumber() ) ))
					{
						continue;
					}
				}

				along = m_spotEncounter->path.from + order->t * length * dir;

				NDebugOverlay::Line( along, order->spot->GetPosition(), 0, 255, 255, true, 0.1f );
			}
		}
	}

	// show aim targets
	if (false)
	{
		CStudioHdr *pStudioHdr = const_cast< CCSBot *>( this )->GetModelPtr();
		if ( !pStudioHdr )
			return;

		mstudiohitboxset_t *set = pStudioHdr->pHitboxSet( const_cast< CCSBot *>( this )->GetHitboxSet() );
		if ( !set )
			return;

		Vector position, forward, right, up;
		QAngle angles;
		char buffer[16];

		for ( int i = 0; i < set->numhitboxes; i++ )
		{
			mstudiobbox_t *pbox = set->pHitbox( i );

			const_cast< CCSBot *>( this )->GetBonePosition( pbox->bone, position, angles );
		
			AngleVectors( angles, &forward, &right, &up );

			NDebugOverlay::BoxAngles( position, pbox->bbmin, pbox->bbmax, angles, 255, 0, 0, 0, 0.1f );

			const float size = 5.0f;
			NDebugOverlay::Line( position, position + size * forward, 255, 255, 0, true, 0.1f );
			NDebugOverlay::Line( position, position + size * right, 255, 0, 0, true, 0.1f );
			NDebugOverlay::Line( position, position + size * up, 0, 255, 0, true, 0.1f );

			Q_snprintf( buffer, 16, "%d", i );
			if (i == 12)
			{
				// in local bone space
				const float headForwardOffset = 4.0f;
				const float headRightOffset = 2.0f;
				position += headForwardOffset * forward + headRightOffset * right;
			}
			NDebugOverlay::Text( position, buffer, true, 0.1f );
		}
	}


	/*
	const QAngle &angles = const_cast< CCSBot *>( this )->GetPunchAngle();
	NDebugOverlay::ScreenText( 0.3f, 0.66f, msg.sprintf( "Punch angle pitch = %3.2f", angles.x ), 255, 255, 0, 150, duration );
	*/

	DrawTravelTime drawTravelTime( this );
	ForEachPlayer( drawTravelTime );

/*
	// show line of fire
	if ((cv_bot_traceview.GetInt() == 100 && IsLocalPlayerWatchingMe()) || cv_bot_traceview.GetInt() == 101)
	{
		if (!IsFriendInLineOfFire())
		{
			Vector vecAiming = GetViewVector();
			Vector vecSrc	 = EyePositionConst();

			if (GetTeamNumber() == TEAM_TERRORIST)
				UTIL_DrawBeamPoints( vecSrc, vecSrc + 2000.0f * vecAiming, 1, 255, 0, 0 );
			else
				UTIL_DrawBeamPoints( vecSrc, vecSrc + 2000.0f * vecAiming, 1, 0, 50, 255 );
		}
	}

	// show path navigation data
	if (cv_bot_traceview.GetInt() == 1 && IsLocalPlayerWatchingMe())
	{
		Vector from = EyePositionConst();

		const float size = 50.0f;
		//Vector arrow( size * cos( m_forwardAngle * M_PI/180.0f ), size * sin( m_forwardAngle * M_PI/180.0f ), 0.0f );
		Vector arrow( size * (float)cos( m_lookAheadAngle * M_PI/180.0f ), 
					  size * (float)sin( m_lookAheadAngle * M_PI/180.0f ), 
					  0.0f );
		
		UTIL_DrawBeamPoints( from, from + arrow, 1, 0, 255, 255 );
	}

	if (cv_bot_show_nav.GetBool() && m_lastKnownArea)
	{
		m_lastKnownArea->DrawConnectedAreas();
	}
	*/


	if (IsAttacking())
	{
		const float crossSize = 2.0f;
		CCSPlayer *enemy = GetBotEnemy();

		if (enemy)
		{
			NDebugOverlay::Cross3D( GetPartPosition( enemy, GUT ), crossSize, 0, 255, 0, true, 0.1f );
			NDebugOverlay::Cross3D( GetPartPosition( enemy, HEAD ), crossSize, 0, 255, 0, true, 0.1f );
			NDebugOverlay::Cross3D( GetPartPosition( enemy, FEET ), crossSize, 0, 255, 0, true, 0.1f );
			NDebugOverlay::Cross3D( GetPartPosition( enemy, LEFT_SIDE ), crossSize, 0, 255, 0, true, 0.1f );
			NDebugOverlay::Cross3D( GetPartPosition( enemy, RIGHT_SIDE ), crossSize, 0, 255, 0, true, 0.1f );
		}
	}
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Periodically compute shortest path distance to each player.
 * NOTE: Travel distance is NOT symmetric between players A and B.  Each much be computed separately.
 */
void CCSBot::UpdateTravelDistanceToAllPlayers( void )
{
	const unsigned char numPhases = 3;

	if (m_updateTravelDistanceTimer.IsElapsed())
	{
		ShortestPathCost pathCost;

		for( int i=1; i<=gpGlobals->maxClients; ++i )
		{
			CCSPlayer *player = static_cast< CCSPlayer * >( UTIL_PlayerByIndex( i ) );

			if (player == NULL)
				continue;

			if (FNullEnt( player->edict() ))
				continue;

			if (!player->IsPlayer())
				continue;
			
			if (!player->IsAlive())
				continue;

			// skip friends for efficiency
			if (player->InSameTeam( this ))
				continue;

			int which = player->entindex() % MAX_PLAYERS;

			// if player is very far away, update every third time (on phase 0)
			const float veryFarAway = 4000.0f;
			if (m_playerTravelDistance[ which ] < 0.0f || m_playerTravelDistance[ which ] > veryFarAway)
			{
				if (m_travelDistancePhase != 0)
					continue;
			}
			else
			{
				// if player is far away, update two out of three times (on phases 1 and 2)
				const float farAway = 2000.0f;
				if (m_playerTravelDistance[ which ] > farAway && m_travelDistancePhase == 0)
					continue;
			}

			// if player is fairly close, update often
			m_playerTravelDistance[ which ] = NavAreaTravelDistance( EyePosition(), player->EyePosition(), pathCost );
		}

		// throttle the computation frequency
		const float checkInterval = 1.0f;
		m_updateTravelDistanceTimer.Start( checkInterval );

		// round-robin the phases
		++m_travelDistancePhase;
		if (m_travelDistancePhase >= numPhases)
		{
			m_travelDistancePhase = 0;
		}
	}
}
