#include "cbase.h"
#include "ai_baseactor.h"
#include "npcevent.h"
#include "cs_shareddefs.h"
#include "cs_gamerules.h"
#ifdef USE_NAV_MESH
#include "nav_mesh.h"
#include "nav_area.h"
#include "nav_pathfind.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	sk_infected_swipe_damage( "sk_infected_swipe_damage", "2" );

#define INFECTED_SKIN_COUNT 32
#define	INFECTED_MELEE1_RANGE		100.0f
#define INFECTED_ALLMODELS 1
ConVar	sk_infected_health( "z_health","50");

static ConVar z_throttle_hit_interval_easy( "z_throttle_hit_interval_easy", "0.5", FCVAR_GAMEDLL | FCVAR_CHEAT, "Minimum time between damaging a Survivor from a mob", true, 0.0f, true, 10.0f );
static ConVar z_throttle_hit_interval_normal( "z_throttle_hit_interval_normal", "0.33", FCVAR_GAMEDLL | FCVAR_CHEAT, "Minimum time between damaging a Survivor from a mob", true, 0.0f, true, 10.0f );
static ConVar z_throttle_hit_interval_hard( "z_throttle_hit_interval_hard", "0.5", FCVAR_GAMEDLL | FCVAR_CHEAT, "Minimum time between damaging a Survivor from a mob", true, 0.0f, true, 10.0f );
static ConVar z_throttle_hit_interval_expert( "z_throttle_hit_interval_expert", "1", FCVAR_GAMEDLL | FCVAR_CHEAT, "Minimum time between damaging a Survivor from a mob", true, 0.0f, true, 10.0f );
static ConVar z_common_separation_radius( "z_common_separation_radius", "40", FCVAR_GAMEDLL, "How close common infected can get before they push each other apart.", true, 1.0f, true, 128.0f );
static ConVar z_common_separation_force( "z_common_separation_force", "35", FCVAR_GAMEDLL, "Horizontal push impulse applied when common infected crowd each other.", true, 0.0f, true, 500.0f );

//-----------------------------------------------------------------------------
// Custom schedules/tasks
//-----------------------------------------------------------------------------
enum
{
	SCHED_TERROR_SHOVE = LAST_SHARED_SCHEDULE,
	SCHED_TERROR_WANDER_ANGRILY,
	LAST_INFECTED_SCHEDULE,
};

enum
{
	TASK_TERROR_SHOVE = LAST_SHARED_TASK,
	LAST_INFECTED_TASK,
};

struct CommonModel_t
{
	const char* model;
	int weight;
};

CommonModel_t g_CommonModels_Default[] =
{
	{ "models/infected/common_male_tshirt_cargos.mdl", 30 },
	{ "models/infected/common_male_tankTop_jeans.mdl", 20 },
	{ "models/infected/common_male_dressShirt_jeans.mdl", 15 },
	{ "models/infected/common_female_tankTop_jeans.mdl", 15 },
	{ "models/infected/common_female_tshirt_skirt.mdl", 20 },
};

CommonModel_t g_CommonModels_L4D1[] =
{
	{ "models/infected/common_male01.mdl", 15 },
	{ "models/infected/common_male02.mdl", 15 },
	{ "models/infected/common_female01.mdl", 15 },
	{ "models/infected/common_police_male01.mdl", 15 },
	{ "models/infected/common_military_male01.mdl", 10 },
	{ "models/infected/common_worker_male01.mdl", 10 },
	{ "models/infected/common_male_suit.mdl", 10 },
	{ "models/infected/common_female01_suit.mdl", 10 },
};
extern ConVar survivor_set;

const char* PickCommonModel()
{
	CommonModel_t* table = nullptr;
	int count = 0;

	if (survivor_set.GetInt() == 1)
	{
		table = g_CommonModels_L4D1;
		count = ARRAYSIZE(g_CommonModels_L4D1);
	}
	else
	{
		table = g_CommonModels_Default;
		count = ARRAYSIZE(g_CommonModels_Default);
	}

	int totalWeight = 0;

	for (int i = 0; i < count; i++)
	{
		totalWeight += table[i].weight;
	}

	int r = RandomInt(0, totalWeight - 1);

	int cumulative = 0;

	for (int i = 0; i < count; i++)
	{
		cumulative += table[i].weight;

		if (r < cumulative)
		{
			return table[i].model;
		}
	}

	return table[0].model; // fallback
}
//-----------------------------------------------------------------------------
// Custom Activities
//-----------------------------------------------------------------------------
Activity ACT_TERROR_IDLE_NEUTRAL;
Activity ACT_TERROR_IDLE_ALERT;
Activity ACT_TERROR_WALK_NEUTRAL;
Activity ACT_TERROR_WALK_INTENSE;
Activity ACT_TERROR_RUN_INTENSE;
Activity ACT_TERROR_JUMP;
Activity ACT_TERROR_JUMP_OVER_GAP;
Activity ACT_TERROR_JUMP_LANDING;
Activity ACT_TERROR_SHOVED_BACKWARD_INTO_WALL;
Activity ACT_TERROR_SHOVED_FORWARD_INTO_WALL;
Activity ACT_TERROR_SHOVED_LEFTWARD_INTO_WALL;
Activity ACT_TERROR_SHOVED_RIGHTWARD_INTO_WALL;

//-----------------------------------------------------------------------------
// Animation events
//-----------------------------------------------------------------------------

static const char *s_szInfectedInsideWorldThinkContext = "InfectedInsideWorldThink";
static const char *s_szInfectedSeparationThinkContext = "InfectedSeparationThink";

static bool IsPipeBombClassname( const char *pszClassname )
{
	if ( !pszClassname )
		return false;

	return ( V_stristr( pszClassname, "pipebomb" ) != NULL ) || ( V_stristr( pszClassname, "pipe_bomb" ) != NULL );
}

static bool IsPipeBombEntity( CBaseEntity *pEntity )
{
	return pEntity && IsPipeBombClassname( pEntity->GetClassname() );
}

#ifdef USE_NAV_MESH
static bool TeleportToRandomNavAreaNear( CBaseEntity *pEnt, const Vector &anchor, float radius )
{
	if ( !pEnt )
		return false;

	if ( !TheNavMesh || !TheNavMesh->IsLoaded() || TheNavMesh->IsGenerating() )
		return false;

	CNavArea *startArea = TheNavMesh->GetNearestNavArea( anchor );
	if ( !startArea )
		return false;

	class CollectAreasFunctor
	{
	public:
		CollectAreasFunctor( const Vector &anchor, float radius )
			: m_anchor( anchor ), m_radiusSqr( radius * radius )
		{
		}

		bool operator()( CNavArea *area )
		{
			if ( !area )
				return true;

			if ( ( area->GetCenter() - m_anchor ).LengthSqr() > m_radiusSqr )
				return true;

			m_areas.AddToTail( area );
			return true;
		}

		CUtlVector< CNavArea * > m_areas;
		Vector m_anchor;
		float m_radiusSqr;
	};

	CollectAreasFunctor collector( anchor, radius );
	SearchSurroundingAreas( startArea, anchor, collector, radius );
	if ( collector.m_areas.Count() <= 0 )
		return false;

	const int which = RandomInt( 0, collector.m_areas.Count() - 1 );
	CNavArea *area = collector.m_areas[ which ];
	if ( !area )
		return false;

	Vector pos = area->GetRandomPoint();

	// Step slightly into the area to avoid edge overlaps.
	Vector toCenter = area->GetCenter() - pos;
	toCenter.z = 0.0f;
	if ( toCenter.NormalizeInPlace() > 0.0f )
	{
		const float stepInDist = 5.0f;
		pos += stepInDist * toCenter;
	}

	pos.z = area->GetZ( pos );

	QAngle ang = pEnt->GetAbsAngles();
	Vector vel( vec3_origin );
	pEnt->Teleport( &pos, &ang, &vel );
	return true;
}
#endif // USE_NAV_MESH

class CNPC_Infected : public CAI_BaseActor
{
	DECLARE_CLASS( CNPC_Infected, CAI_BaseActor );

public:
	CNPC_Infected();
	~CNPC_Infected();

	//---------------------------------

	void			Precache();
	void			Spawn();
	void			Activate();

	void			SetZombieModel();

	Class_T			Classify();
	
	void			SetupGlobalModelData();
	Activity		NPC_TranslateActivity( Activity baseAct );
	virtual	bool	OverrideMoveFacing( const AILocalMoveGoal_t &move, float flInterval );
	virtual	bool	ShouldCollide( int collisionGroup, int contentsMask ) const OVERRIDE;

	virtual const char *GetEyeAttachmentName(){ return "forward"; }

	virtual float	GetIdealSpeed( ) const;
	virtual float	GetSequenceGroundSpeed( CStudioHdr *pStudioHdr, int iSequence );

	virtual void		DeathSound( const CTakeDamageInfo &info );
	virtual void		AlertSound( void );
	virtual void		IdleSound( void );
	virtual void		PainSound( const CTakeDamageInfo &info );
	//virtual void		FearSound( void );

	int OnTakeDamage_Alive( const CTakeDamageInfo &info );
	virtual void	MeleeAttack( float distance, float damage, QAngle &viewPunch, Vector &shove );
	virtual void	HandleAnimEvent( animevent_t *pEvent );

	virtual Disposition_t IRelationType( CBaseEntity *pTarget ) OVERRIDE;
	virtual int IRelationPriority( CBaseEntity *pTarget ) OVERRIDE;

	void			StartTask( const Task_t *pTask );
	void			RunTask( const Task_t *pTask );

	void			RunAttackTask( int task );
	virtual int		SelectSchedule( void ) OVERRIDE;
	virtual int		SelectFailSchedule( int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode ) OVERRIDE;

	void			InsideWorldFixupThink();
	void			CommonSeparationThink();
	bool		ShouldPlayIdleSound(void);
	virtual	bool		AllowedToIgnite(void) { return true; }

public:
	static int gm_nMoveXPoseParam;
	static int gm_nMoveYPoseParam;
	static int gm_nLeanYawPoseParam;
	static int gm_nLeanPitchPoseParam;
	float	m_flIdleDelay;

private:
	DEFINE_CUSTOM_AI;

	DECLARE_DATADESC();

	int m_iAttackLayer;
	bool m_bContinuousAttacking;
	Vector m_lastNonSolidSpot;

	Vector m_lastStuckCheckSpot;
	float m_flStuckStartTime;
	float m_flLastStuckTeleportTime;

	bool m_bPendingShove;
	bool m_bInShove;
	bool m_bShoveHitWall;
	Activity m_ShoveActivity;
	Vector m_vecShoveDir;
	float m_flNextShoveTime;
};

LINK_ENTITY_TO_CLASS( infected, CNPC_Infected );


BEGIN_DATADESC( CNPC_Infected )
	DEFINE_FIELD(m_flIdleDelay, FIELD_TIME),
END_DATADESC()

int CNPC_Infected::gm_nMoveXPoseParam = -1;
int CNPC_Infected::gm_nMoveYPoseParam = -1;
int CNPC_Infected::gm_nLeanYawPoseParam = -1;
int CNPC_Infected::gm_nLeanPitchPoseParam = -1;

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_Infected::ShouldPlayIdleSound(void)
{
	//Gagged monsters don't talk
	if (m_spawnflags & SF_NPC_GAG)
		return false;

	//Don't cut off another sound or play again too soon
	if (m_flIdleDelay > gpGlobals->curtime)
		return false;

	return true;
}


CNPC_Infected::CNPC_Infected()
{
	m_iAttackLayer = -1;
	m_bContinuousAttacking = false;
	m_lastNonSolidSpot = vec3_origin;
	m_lastStuckCheckSpot = vec3_origin;
	m_flStuckStartTime = 0.0f;
	m_flLastStuckTeleportTime = 0.0f;

	m_bPendingShove = false;
	m_bInShove = false;
	m_bShoveHitWall = false;
	m_ShoveActivity = ACT_INVALID;
	m_vecShoveDir = vec3_origin;
	m_flNextShoveTime = 0.0f;
}

CNPC_Infected::~CNPC_Infected()
{

}

void CNPC_Infected::Precache()
{
	for (int i = 0; i < ARRAYSIZE(g_CommonModels_Default); i++)
	{
		PrecacheModel(g_CommonModels_Default[i].model);
	}

	for (int i = 0; i < ARRAYSIZE(g_CommonModels_L4D1); i++)
	{
		PrecacheModel(g_CommonModels_L4D1[i].model);
	}

    PrecacheScriptSound("Zombie.Sleeping");
    PrecacheScriptSound("Zombie.Wander");
    PrecacheScriptSound("Zombie.BecomeAlert");
    PrecacheScriptSound("Zombie.Alert");
    PrecacheScriptSound("Zombie.BecomeEnraged");
    PrecacheScriptSound("Zombie.Rage");
    PrecacheScriptSound("Zombie.RageAtVictim");
    PrecacheScriptSound("Zombie.Shoved");
    PrecacheScriptSound("Zombie.Shot");
    PrecacheScriptSound("Zombie.Die");
    PrecacheScriptSound("Zombie.IgniteScream");
    PrecacheScriptSound("Zombie.HeadlessCough");
    PrecacheScriptSound("Zombie.AttackMiss");
    PrecacheScriptSound("Zombie.BulletImpact");
    PrecacheScriptSound("Zombie.ClawScrape");
    PrecacheScriptSound("Zombie.Punch");
    PrecacheScriptSound("MegaMobIncoming");

	BaseClass::Precache();
}

void CNPC_Infected::Spawn()
{
	Precache();

	SetZombieModel();
	BaseClass::Spawn();

	SetHullType( HULL_HUMAN );
	SetHullSizeNormal();
	SetDefaultEyeOffset();
	
	SetNavType( NAV_GROUND );
	m_NPCState = NPC_STATE_NONE;
	
	m_iHealth = m_iMaxHealth = sk_infected_health.GetInt();

	m_flFieldOfView		= 0.2;

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetMoveType( MOVETYPE_STEP );

	SetupGlobalModelData();
	
	CapabilitiesAdd( bits_CAP_INNATE_MELEE_ATTACK1 );
	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_OPEN_DOORS | bits_CAP_MOVE_CLIMB | bits_CAP_MOVE_JUMP | bits_CAP_MOVE_CRAWL | bits_CAP_SQUAD);
	CapabilitiesAdd(bits_CAP_TURN_HEAD | bits_CAP_ANIMATEDFACE);

	NPCInit();

	m_lastNonSolidSpot = GetAbsOrigin();
	SetContextThink( &CNPC_Infected::InsideWorldFixupThink, gpGlobals->curtime + 0.5f, s_szInfectedInsideWorldThinkContext );
	SetContextThink( &CNPC_Infected::CommonSeparationThink, gpGlobals->curtime + 0.05f, s_szInfectedSeparationThinkContext );
}

void CNPC_Infected::Activate()
{
	BaseClass::Activate();

	SetupGlobalModelData();
}

bool CNPC_Infected::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	// Common infected shouldn't physically collide with special infected (CT players).
	// Player movement traces include a team contents bit (see CCSGameMovement::PlayerSolidMask).
	// Survivors use CONTENTS_TEAM1, infected use CONTENTS_TEAM2.
	if ( collisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT )
	{
		if ( ( contentsMask & CONTENTS_TEAM1 ) == 0 )
			return false;
	}

	return BaseClass::ShouldCollide( collisionGroup, contentsMask );
}

void CNPC_Infected::InsideWorldFixupThink()
{
#ifdef USE_NAV_MESH
	const Vector origin = GetAbsOrigin();

	// If a survivor is marked as "IT" (boomer vomit), focus them.
	if ( CSGameRules() )
	{
		CCSPlayer *it = CSGameRules()->GetItTarget();
		if ( it && it->IsAlive() && it->GetTeamNumber() == TEAM_SURVIVOR && !IsPipeBombEntity( GetEnemy() ) )
		{
			SetEnemy( it );
		}
	}

	// If we're stuck (not moving) while we have a target, relocate to a random nav area near them.
	{
		CBaseEntity *anchorEnt = GetEnemy();
		if ( !anchorEnt && CSGameRules() )
		{
			CCSPlayer *it = CSGameRules()->GetItTarget();
			if ( it && it->IsAlive() && it->GetTeamNumber() == TEAM_SURVIVOR )
			{
				anchorEnt = it;
			}
		}

		if ( anchorEnt && anchorEnt->IsAlive() && GetEnemy() == NULL)
		{
			const float stuckMoveThreshold = 15.0f;
			const float stuckMoveThresholdSqr = stuckMoveThreshold * stuckMoveThreshold;

			Vector delta = origin - m_lastStuckCheckSpot;
			delta.z = 0.0f;

			if ( m_lastStuckCheckSpot != vec3_origin && delta.LengthSqr() <= stuckMoveThresholdSqr )
			{
				if ( m_flStuckStartTime <= 0.0f )
				{
					m_flStuckStartTime = gpGlobals->curtime;
				}
				else
				{
					const float stuckSeconds = 1.5f;
					const float teleportCooldown = 2.0f;
					if ( ( gpGlobals->curtime - m_flStuckStartTime ) >= stuckSeconds &&
						 ( gpGlobals->curtime - m_flLastStuckTeleportTime ) >= teleportCooldown )
					{
						const float recoverRadius = 500.0f;
						if ( TeleportToRandomNavAreaNear( this, anchorEnt->GetAbsOrigin(), recoverRadius ) )
						{
							m_lastNonSolidSpot = GetAbsOrigin();
							m_lastStuckCheckSpot = m_lastNonSolidSpot;
							m_flStuckStartTime = 0.0f;
							m_flLastStuckTeleportTime = gpGlobals->curtime;
						}
					}
				}
			}
			else
			{
				m_flStuckStartTime = 0.0f;
			}
		}
		else
		{
			m_flStuckStartTime = 0.0f;
		}
	}

	// Track the last known non-solid spot so we can teleport back onto the nav mesh if we ever end up embedded.
	if ( ( UTIL_PointContents( origin ) & CONTENTS_SOLID ) == 0 )
	{
		m_lastStuckCheckSpot = GetAbsOrigin();
		m_lastNonSolidSpot = origin;
		SetContextThink( &CNPC_Infected::InsideWorldFixupThink, gpGlobals->curtime + 0.5f, s_szInfectedInsideWorldThinkContext );
		return;
	}

	if ( TheNavMesh && TheNavMesh->IsLoaded() && !TheNavMesh->IsGenerating() )
	{
		const Vector referencePos = ( m_lastNonSolidSpot != vec3_origin ) ? m_lastNonSolidSpot : origin;
		CNavArea *area = TheNavMesh->GetNearestNavArea( referencePos );
		if ( area )
		{
			Vector pos;
			area->GetClosestPointOnArea( referencePos, &pos );

			// Step slightly into the area to avoid edge overlaps.
			Vector toCenter = area->GetCenter() - pos;
			toCenter.z = 0.0f;
			if ( toCenter.NormalizeInPlace() > 0.0f )
			{
				const float stepInDist = 5.0f;
				pos += stepInDist * toCenter;
			}

			pos.z = area->GetZ( pos );

			QAngle ang = GetAbsAngles();
			Vector vel( vec3_origin );
			Teleport( &pos, &ang, &vel );

			m_lastNonSolidSpot = pos;
		}
	}
#endif

	SetContextThink( &CNPC_Infected::InsideWorldFixupThink, gpGlobals->curtime + 0.5f, s_szInfectedInsideWorldThinkContext );
}

void CNPC_Infected::CommonSeparationThink()
{
	if ( m_lifeState == LIFE_DEAD )
		return;

	if ( GetFlags() & FL_ONGROUND )
	{
		const float flRadius = MAX( 1.0f, z_common_separation_radius.GetFloat() );
		Vector avoid = vec3_origin;
		float flAvoidWeight = 0.0f;

		for ( CEntitySphereQuery sphere( GetAbsOrigin(), flRadius ); CBaseEntity *pEntity = sphere.GetCurrentEntity(); sphere.NextEntity() )
		{
			if ( !pEntity || pEntity == this )
				continue;

			if ( V_stricmp( pEntity->GetClassname(), "infected" ) != 0 )
				continue;

			CNPC_Infected *pOther = dynamic_cast< CNPC_Infected * >( pEntity );
			if ( !pOther || pOther->m_lifeState == LIFE_DEAD )
				continue;

			Vector toOther = pOther->GetAbsOrigin() - GetAbsOrigin();
			toOther.z = 0.0f;

			float flDist = toOther.NormalizeInPlace();
			if ( flDist >= flRadius )
				continue;

			if ( flDist <= 0.001f )
			{
				const float flYaw = random->RandomFloat( 0.0f, 360.0f );
				float s, c;
				SinCos( DEG2RAD( flYaw ), &s, &c );
				toOther.Init( c, s, 0.0f );
				flDist = 0.0f;
			}

			const float flWeight = ( flRadius - flDist ) / flRadius;
			avoid -= toOther * flWeight;
			flAvoidWeight += flWeight;
		}

		if ( flAvoidWeight > 0.0f )
		{
			avoid /= flAvoidWeight;
			avoid.z = 0.0f;
			const float flLength = avoid.NormalizeInPlace();
			if ( flLength > 0.0f )
			{
				ApplyAbsVelocityImpulse( avoid * z_common_separation_force.GetFloat() );
			}
		}
	}

	SetContextThink( &CNPC_Infected::CommonSeparationThink, gpGlobals->curtime + 0.05f, s_szInfectedSeparationThinkContext );
}

Disposition_t CNPC_Infected::IRelationType( CBaseEntity *pTarget )
{
	if ( IsPipeBombEntity( pTarget ) )
		return D_HT;

	return BaseClass::IRelationType( pTarget );
}

int CNPC_Infected::IRelationPriority( CBaseEntity *pTarget )
{
	if ( IsPipeBombEntity( pTarget ) )
		return INT_MAX;

	return BaseClass::IRelationPriority( pTarget );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Class_T CNPC_Infected::Classify()
{
	return CLASS_ZOMBIE;
}

//---------------------------------------------------------
//---------------------------------------------------------
void CNPC_Infected::SetZombieModel( void )
{
	SetModel(PickCommonModel());
	m_nSkin = random->RandomInt(0, INFECTED_SKIN_COUNT - 1);


	SetRenderColor(random->RandomInt(180, 255),
		random->RandomInt(180, 255),
		random->RandomInt(180, 255));
}


void CNPC_Infected::SetupGlobalModelData()
{
	if ( gm_nMoveXPoseParam != -1 )
		return;

	gm_nMoveXPoseParam = LookupPoseParameter( "move_x" );
	gm_nMoveYPoseParam = LookupPoseParameter( "move_y" );
	gm_nLeanYawPoseParam = LookupPoseParameter( "lean_yaw" );
	gm_nLeanPitchPoseParam = LookupPoseParameter( "lean_pitch" );
}

float CNPC_Infected::GetIdealSpeed( ) const
{
	// Ensure navigator will move
	// TODO: Could limit it to move sequences only.
	float speed = BaseClass::GetIdealSpeed();
	if( speed <= 0 ) speed = 1.0f;
	return speed;
}

float CNPC_Infected::GetSequenceGroundSpeed( CStudioHdr *pStudioHdr, int iSequence )
{
	// Ensure navigator will move
	// TODO: Could limit it to move sequences only.
	float speed = BaseClass::GetSequenceGroundSpeed( pStudioHdr, iSequence );
	if( speed <= 0 /*&& GetSequenceActivity( iSequence) == ACT_TERROR_RUN_INTENSE*/ ) speed = 1.0f;
	return speed;
}

Activity CNPC_Infected::NPC_TranslateActivity( Activity baseAct )
{
	if (baseAct == ACT_IDLE)
	{
		return ACT_TERROR_IDLE_NEUTRAL;
	}
	else if( baseAct == ACT_WALK )
	{
		// Allow an "angry wander" walk style (HL2 zombie-like frustration wander).
		if ( IsCurSchedule( SCHED_TERROR_WANDER_ANGRILY ) && HaveSequenceForActivity( ACT_TERROR_WALK_INTENSE ) )
			return ACT_TERROR_WALK_INTENSE;

		return ACT_TERROR_WALK_NEUTRAL;
	}
	else if( baseAct == ACT_RUN )
	{
		return ACT_TERROR_RUN_INTENSE;
	}
	else if ( ( baseAct == ACT_IDLE_ANGRY ) )
	{
		return ACT_TERROR_IDLE_ALERT;
	}
	else if ( ( baseAct == ACT_MELEE_ATTACK1 ) )
	{
		// When in HL2 zombie melee range, switch to a continuous attack sequence; otherwise keep moving and use gestures.
		const float hl2ZombieMeleeRange = 55.0f; // ZOMBIE_MELEE_REACH from HL2 base zombie

		float dist = 1e30f;
		if ( GetEnemy() )
		{
			dist = ( GetEnemy()->GetAbsOrigin() - GetAbsOrigin() ).Length2D();
		}

		if ( dist <= hl2ZombieMeleeRange )
			return ACT_TERROR_ATTACK_CONTINUOUSLY;

		return ACT_TERROR_RUN_INTENSE;
	}

	return baseAct;
}

bool CNPC_Infected::OverrideMoveFacing( const AILocalMoveGoal_t &move, float flInterval )
{
	// required movement direction
	float flMoveYaw = UTIL_VecToYaw( move.dir );

	// FIXME: move this up to navigator so that path goals can ignore these overrides.
	Vector dir;
	float flInfluence = GetFacingDirection( dir );
	dir = move.facing * (1 - flInfluence) + dir * flInfluence;
	VectorNormalize( dir );

	// ideal facing direction
	float idealYaw = UTIL_AngleMod( UTIL_VecToYaw( dir ) );
		
	// FIXME: facing has important max velocity issues
	GetMotor()->SetIdealYawAndUpdate( idealYaw );	

	// find movement direction to compensate for not being turned far enough
	float flDiff = UTIL_AngleDiff( flMoveYaw, GetLocalAngles().y );

	// Setup the 9-way blend parameters based on our speed and direction.
	Vector2D vCurMovePose( 0, 0 );

	vCurMovePose.x = cos( DEG2RAD( flDiff ) ) * 1.0f; //flPlaybackRate;
	vCurMovePose.y = -sin( DEG2RAD( flDiff ) ) * 1.0f; //flPlaybackRate;

	SetPoseParameter( gm_nMoveXPoseParam, vCurMovePose.x );
	SetPoseParameter( gm_nMoveYPoseParam, vCurMovePose.y );

	// ==== Update Lean pose parameters
	if ( gm_nLeanYawPoseParam >= 0 )
	{
		float targetLean = GetPoseParameter( gm_nMoveYPoseParam ) * 30.0f;
		float curLean = GetPoseParameter( gm_nLeanYawPoseParam );
		if( curLean < targetLean )
			curLean += MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		else
			curLean -= MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		SetPoseParameter( gm_nLeanYawPoseParam, curLean );
	}

	if( gm_nLeanPitchPoseParam >= 0 )
	{
		float targetLean = GetPoseParameter( gm_nMoveXPoseParam ) * -30.0f;
		float curLean = GetPoseParameter( gm_nLeanPitchPoseParam );
		if( curLean < targetLean )
			curLean += MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		else
			curLean -= MIN(fabs(targetLean-curLean), GetAnimTimeInterval()*15.0f);
		SetPoseParameter( gm_nLeanPitchPoseParam, curLean );
	}

	return true;
}

//------------------------------------------------------------------------------
// Mob hit throttling (per Survivor)
//------------------------------------------------------------------------------
static float GetMobHitThrottleInterval()
{
	ConVarRef bot_difficulty( "bot_difficulty" );
	const int difficulty = clamp( bot_difficulty.IsValid() ? bot_difficulty.GetInt() : 1, 0, 3 );

	switch ( difficulty )
	{
		case 0: return z_throttle_hit_interval_easy.GetFloat();
		case 2: return z_throttle_hit_interval_hard.GetFloat();
		case 3: return z_throttle_hit_interval_expert.GetFloat();
		case 1:
		default: return z_throttle_hit_interval_normal.GetFloat();
	}
}

static bool IsSurvivorPlayer( CBasePlayer *player )
{
	return player && player->IsAlive() && player->GetTeamNumber() == TEAM_SURVIVOR;
}

static float s_flNextMobHitAllowed[MAX_PLAYERS + 1] = { 0 };
static float s_flLastMobHitCurtime = 0.0f;

static void MobHitThrottleMaybeReset()
{
	if ( gpGlobals && gpGlobals->curtime < s_flLastMobHitCurtime )
	{
		Q_memset( s_flNextMobHitAllowed, 0, sizeof( s_flNextMobHitAllowed ) );
	}

	s_flLastMobHitCurtime = gpGlobals ? gpGlobals->curtime : 0.0f;
}

static bool MobHitThrottleCanDamage( CBasePlayer *victim )
{
	if ( !victim || !gpGlobals )
		return true;

	const float interval = GetMobHitThrottleInterval();
	if ( interval <= 0.0f )
		return true;

	const int idx = victim->entindex();
	if ( idx <= 0 || idx > MAX_PLAYERS )
		return true;

	return gpGlobals->curtime >= s_flNextMobHitAllowed[idx];
}

static void MobHitThrottleRecordDamage( CBasePlayer *victim )
{
	if ( !victim || !gpGlobals )
		return;

	const float interval = GetMobHitThrottleInterval();
	if ( interval <= 0.0f )
		return;

	const int idx = victim->entindex();
	if ( idx <= 0 || idx > MAX_PLAYERS )
		return;

	s_flNextMobHitAllowed[idx] = gpGlobals->curtime + interval;
}

class CTraceFilterInfectedMeleeScan : public ITraceFilter
{
public:
	explicit CTraceFilterInfectedMeleeScan( CBaseCombatCharacter *attacker, int collisionGroup )
		: m_attacker( attacker ), m_collisionGroup( collisionGroup ), m_hit( NULL )
	{
	}

	bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask ) OVERRIDE
	{
		if ( !StandardFilterRules( pHandleEntity, contentsMask ) )
			return false;

		if ( !PassServerEntityFilter( pHandleEntity, m_attacker ) )
			return false;

		CBaseEntity *entity = EntityFromEntityHandle( pHandleEntity );
		if ( !entity )
			return false;

		if ( !entity->ShouldCollide( m_collisionGroup, contentsMask ) )
			return false;

		if ( g_pGameRules && !g_pGameRules->ShouldCollide( m_collisionGroup, entity->GetCollisionGroup() ) )
			return false;

		if ( entity->m_takedamage == DAMAGE_NO )
			return false;

		CBaseCombatCharacter *attackerBCC = m_attacker ? m_attacker->MyCombatCharacterPointer() : NULL;
		CBaseCombatCharacter *victimBCC = entity->MyCombatCharacterPointer();

		if ( attackerBCC && victimBCC )
		{
			if ( attackerBCC->IRelationType( entity ) != D_HT )
				return false;
		}

		m_hit = entity;
		return true;
	}

	TraceType_t GetTraceType() const OVERRIDE { return TRACE_ENTITIES_ONLY; }

public:
	CBaseCombatCharacter *m_attacker;
	int m_collisionGroup;
	CBaseEntity *m_hit;
};

static CBaseEntity *InfectedScanHullAttack( CBaseCombatCharacter *attacker, float distance, const Vector &mins, const Vector &maxs )
{
	if ( !attacker )
		return NULL;

	Vector forward;
	AngleVectors( attacker->GetAbsAngles(), &forward );

	Vector vStart = attacker->GetAbsOrigin();

	float flVerticalOffset = attacker->WorldAlignSize().z * 0.5f;
	if ( flVerticalOffset < maxs.z )
	{
		flVerticalOffset = maxs.z + 1.0f;
	}

	vStart.z += flVerticalOffset;
	Vector vEnd = vStart + ( forward * distance );

	CTraceFilterInfectedMeleeScan traceFilter( attacker, COLLISION_GROUP_PROJECTILE );
	Ray_t ray;
	ray.Init( vStart, vEnd, mins, maxs );

	trace_t tr;
	enginetrace->TraceRay( ray, MASK_SHOT_HULL, &traceFilter, &tr );

	CBaseEntity *entity = traceFilter.m_hit;

	if ( entity == NULL )
	{
		Vector vecTopCenter;
		Vector vecMins, vecMaxs;

		vecTopCenter = attacker->GetAbsOrigin();
		attacker->CollisionProp()->WorldSpaceAABB( &vecMins, &vecMaxs );
		vecTopCenter.z = vecMaxs.z + 1.0f;

		ray.Init( vecTopCenter, vEnd, mins, maxs );
		enginetrace->TraceRay( ray, MASK_SHOT_HULL, &traceFilter, &tr );

		entity = traceFilter.m_hit;
	}

	if ( entity && !entity->CanBeHitByMeleeAttack( attacker ) )
	{
		entity = NULL;
	}

	return entity;
}

void CNPC_Infected::MeleeAttack( float distance, float damage, QAngle &viewPunch, Vector &shove )
{
	Vector vecForceDir;

	// Always hurt bullseyes for now
	if ( ( GetEnemy() != NULL ) && ( GetEnemy()->Classify() == CLASS_BULLSEYE ) )
	{
		CTakeDamageInfo info( this, this, damage, DMG_SLASH );
		GetEnemy()->TakeDamage( info );
		return;
	}

	// If we'd hit a Survivor but they're throttled, skip the damaging trace entirely.
	CBaseEntity *pWouldHit = InfectedScanHullAttack( this, distance, -Vector( 16, 16, 32 ), Vector( 16, 16, 32 ) );
	CBasePlayer *wouldHitPlayer = ToBasePlayer( pWouldHit );
	if ( !MobHitThrottleCanDamage( wouldHitPlayer ) )
	{
		return;
	}

	CBaseEntity *pHurt = CheckTraceHullAttack( distance, -Vector(16,16,32), Vector(16,16,32), damage, DMG_SLASH, 5.0f );

	if ( pHurt )
	{
		vecForceDir = ( pHurt->WorldSpaceCenter() - WorldSpaceCenter() );

		//FIXME: Until the interaction is setup, kill combine soldiers in one hit -- jdw
		if ( FClassnameIs( pHurt, "npc_combine_s" ) )
		{
			CTakeDamageInfo	dmgInfo( this, this, pHurt->m_iHealth+25, DMG_SLASH );
			CalculateMeleeDamageForce( &dmgInfo, vecForceDir, pHurt->GetAbsOrigin() );
			pHurt->TakeDamage( dmgInfo );
			return;
		}

		CBasePlayer *pPlayer = ToBasePlayer( pHurt );

		if ( pPlayer != NULL )
		{
			if ( !( pPlayer->GetFlags() & FL_GODMODE ) && pPlayer->GetMoveType() != MOVETYPE_NOCLIP )
			{
				MobHitThrottleRecordDamage( pPlayer );
				pPlayer->SetPunchAngle( RandomAngle(-7,7) );
				pPlayer->EmitSound( "Player.HitInternal" );
			}
		}

		// Play a random attack hit sound
		EmitSound( "Zombie.Punch" );
	}
	else
	{
		EmitSound( "Zombie.AttackMiss" );
	}
}

#define ZOMBIE_SCORCH_RATE		8
#define ZOMBIE_MIN_RENDERCOLOR	50

int CNPC_Infected::OnTakeDamage_Alive(const CTakeDamageInfo& inputInfo)
{
	CTakeDamageInfo info = inputInfo;

	int tookDamage = BaseClass::OnTakeDamage_Alive(info);

	const int damageType = info.GetDamageType();
	if ( IsAlive() && ( damageType & ( DMG_CLUB | DMG_BLAST ) ) && gpGlobals->curtime >= m_flNextShoveTime )
	{
		if ( !m_bInShove )
		{
			EmitSound("Weapon.HitInfected");
			EmitSound("Zombie.Shoved");
			Vector toAttacker = vec3_origin;
			if ( damageType & DMG_BLAST )
			{
				// Prefer the explosion origin over the attacker position.
				if ( info.GetDamagePosition() != vec3_origin )
				{
					toAttacker = info.GetDamagePosition() - WorldSpaceCenter();
				}
				else if ( info.GetInflictor() )
				{
					toAttacker = info.GetInflictor()->WorldSpaceCenter() - WorldSpaceCenter();
				}
				else if ( info.GetAttacker() )
				{
					toAttacker = info.GetAttacker()->WorldSpaceCenter() - WorldSpaceCenter();
				}
				else if ( info.GetDamageForce() != vec3_origin )
				{
					// Damage force usually points from attacker -> victim.
					toAttacker = -info.GetDamageForce();
				}
			}
			else
			{
				if ( info.GetAttacker() )
				{
					toAttacker = info.GetAttacker()->WorldSpaceCenter() - WorldSpaceCenter();
				}
				else if ( info.GetDamagePosition() != vec3_origin )
				{
					toAttacker = info.GetDamagePosition() - WorldSpaceCenter();
				}
				else if ( info.GetDamageForce() != vec3_origin )
				{
					// Damage force usually points from attacker -> victim.
					toAttacker = -info.GetDamageForce();
				}
			}

			toAttacker.z = 0.0f;
			if ( toAttacker.NormalizeInPlace() > 0.0f )
			{
				Vector forward, right;
				GetVectors( &forward, &right, NULL );
				forward.z = 0.0f;
				right.z = 0.0f;
				forward.NormalizeInPlace();
				right.NormalizeInPlace();

				const float forwardDot = DotProduct( forward, toAttacker );
				const float rightDot = DotProduct( right, toAttacker );

				if ( fabsf( forwardDot ) >= fabsf( rightDot ) )
				{
					// Attacker in front -> shoved backward. Attacker behind -> shoved forward.
					if ( forwardDot >= 0.0f )
					{
						m_ShoveActivity = ACT_TERROR_SHOVED_BACKWARD;
						m_vecShoveDir = -forward;
					}
					else
					{
						m_ShoveActivity = ACT_TERROR_SHOVED_FORWARD;
						m_vecShoveDir = forward;
					}
				}
				else
				{
					// Attacker on right -> shoved left. Attacker on left -> shoved right.
					if ( rightDot >= 0.0f )
					{
						m_ShoveActivity = ACT_TERROR_SHOVED_LEFTWARD;
						m_vecShoveDir = -right;
					}
					else
					{
						m_ShoveActivity = ACT_TERROR_SHOVED_RIGHTWARD;
						m_vecShoveDir = right;
					}
				}

				m_bPendingShove = true;
				m_flNextShoveTime = gpGlobals->curtime + 0.05f;

				CBasePlayer *pAttackerPlayer = ToBasePlayer( info.GetAttacker() );
				const bool shoveFromSurvivorClub = ( ( damageType & DMG_CLUB ) != 0 ) && pAttackerPlayer && pAttackerPlayer->GetTeamNumber() == TEAM_SURVIVOR;
				const bool shoveFromBlast = ( damageType & DMG_BLAST ) != 0;
				if ( ( shoveFromSurvivorClub || shoveFromBlast ) && m_vecShoveDir != vec3_origin )
				{
					const float strength = shoveFromBlast ? clamp( info.GetDamage() * 18.0f, 250.0f, 900.0f ) : 250.0f;
					const float up = shoveFromBlast ? clamp( info.GetDamage() * 6.0f, 50.0f, 250.0f ) : 50.0f;
					Vector shoveVel = m_vecShoveDir * strength;
					shoveVel.z = up;
					ApplyAbsVelocityImpulse( shoveVel );
				}

				// Interrupt whatever we were doing; shove schedule will be selected next think.
				GetNavigator()->StopMoving();
				ClearSchedule( "Shoved" );
			}
		}
	}

	if (inputInfo.GetDamageType() & DMG_BURN)
	{
		Ignite(90.0f, true, 0.0F, false);
		Scorch(ZOMBIE_SCORCH_RATE, ZOMBIE_MIN_RENDERCOLOR);
	}
	PainSound(info);
	return tookDamage;
}
void CNPC_Infected::HandleAnimEvent( animevent_t *pEvent )
{
	switch (pEvent->event)
	{
		case AE_ATTACK_HIT:
			MeleeAttack(INFECTED_MELEE1_RANGE, sk_infected_swipe_damage.GetFloat(), QAngle(20.0f, 0.0f, -12.0f), Vector(-250.0f, 1.0f, 1.0f));
			break;
	}
	BaseClass::HandleAnimEvent( pEvent );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Infected::PainSound( const CTakeDamageInfo &info )
{
	// We're constantly taking damage when we are on fire. Don't make all those noises!
	if ( IsOnFire() )
	{
		return;
	}

	EmitSound( "Zombie.Shot" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Infected::DeathSound( const CTakeDamageInfo &info ) 
{
	EmitSound( "Zombie.Die" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Infected::AlertSound( void )
{
	EmitSound( "Zombie.BecomeEnraged" );
}

//-----------------------------------------------------------------------------
// Purpose: Play a random idle sound.
//-----------------------------------------------------------------------------
void CNPC_Infected::IdleSound( void )
{
	if (GetEnemy() != NULL)
	{
		EmitSound("Zombie.RageAtVictim");
		m_flIdleDelay = gpGlobals->curtime + GetSoundDuration("Zombie.RageAtVictim",STRING( GetModelName() )) + random->RandomFloat(1.0f, 2.0f);
		return;
	}
	EmitSound("Zombie.Wander");
	m_flIdleDelay = gpGlobals->curtime + GetSoundDuration("Zombie.Wander", STRING(GetModelName())) + random->RandomFloat(1.0f, 2.0f);
}

void CNPC_Infected::StartTask( const Task_t *pTask )
{
	switch( pTask->iTask )
	{
	case TASK_TERROR_SHOVE:
	{
		m_bPendingShove = false;
		m_bInShove = true;
		m_bShoveHitWall = false;

		// Clear any previous attack gesture/continuous mode.
		m_bContinuousAttacking = false;
		if ( m_iAttackLayer != -1 )
		{
			FastRemoveLayer( m_iAttackLayer );
			m_iAttackLayer = -1;
		}

		const int seq = ( m_ShoveActivity != ACT_INVALID ) ? SelectWeightedSequence( m_ShoveActivity ) : ACTIVITY_NOT_AVAILABLE;
		if ( seq > ACTIVITY_NOT_AVAILABLE )
		{
			ResetSequence( seq );
			ResetSequenceInfo();
			SetActivity( m_ShoveActivity );
		}
		else
		{
			m_bInShove = false;
			TaskComplete();
		}

		break;
	}
	case TASK_MELEE_ATTACK1:
	{
		SetLastAttackTime( gpGlobals->curtime );

		const float hl2ZombieMeleeRange = 55.0f; // ZOMBIE_MELEE_REACH from HL2 base zombie
		const float gestureRange = INFECTED_MELEE1_RANGE; // close enough that we're about to go continuous

		float dist = 1e30f;
		if ( GetEnemy() )
		{
			dist = ( GetEnemy()->GetAbsOrigin() - GetAbsOrigin() ).Length2DSqr();
		}

		// Clear any previous attack gesture.
		if ( m_iAttackLayer != -1 )
		{
			FastRemoveLayer( m_iAttackLayer );
			m_iAttackLayer = -1;
		}

		// If we're in melee range, switch to the continuous attack sequence and don't use gestures.
		if ( dist <= Square(55.f * 0.75f))
		{
			m_bContinuousAttacking = true;

			const int seq = SelectWeightedSequence( ACT_TERROR_ATTACK_CONTINUOUSLY );
			if ( seq > ACTIVITY_NOT_AVAILABLE )
			{
				ResetSequence( seq );
				ResetSequenceInfo();
			}
			SetActivity( ACT_TERROR_ATTACK_CONTINUOUSLY );
		}
		else
		{
			m_bContinuousAttacking = false;

			AddGesture( ACT_TERROR_ATTACK );
		}
		break;
	}
	default:
		BaseClass::StartTask( pTask );
		break;
	}
}

void CNPC_Infected::RunTask( const Task_t *pTask )
{
	switch( pTask->iTask )
	{
	case TASK_WAIT:
	case TASK_WAIT_RANDOM:
	case TASK_WAIT_FACE_ENEMY:
	case TASK_WAIT_FACE_ENEMY_RANDOM:
	case TASK_WAIT_INDEFINITE:
	case TASK_WAIT_PVS:
	{
		// Apply root-motion (sequence movement) while idle/waiting so idle anims can shuffle.
		if ( ( m_NPCState == NPC_STATE_IDLE || m_NPCState == NPC_STATE_ALERT ) && GetEnemy() == NULL && !IsInChoreo() )
		{
			AutoMovement();
		}

		BaseClass::RunTask( pTask );
		break;
	}
	case TASK_TERROR_SHOVE:
	{
		if ( !m_bShoveHitWall )
		{
			Vector forward, right;
			GetVectors( &forward, &right, NULL );
			forward.z = 0.0f;
			right.z = 0.0f;
			forward.NormalizeInPlace();
			right.NormalizeInPlace();

			Vector shoveDir = m_vecShoveDir;
			if ( shoveDir == vec3_origin )
			{
				if ( m_ShoveActivity == ACT_TERROR_SHOVED_BACKWARD )
					shoveDir = -forward;
				else if ( m_ShoveActivity == ACT_TERROR_SHOVED_FORWARD )
					shoveDir = forward;
				else if ( m_ShoveActivity == ACT_TERROR_SHOVED_LEFTWARD )
					shoveDir = -right;
				else if ( m_ShoveActivity == ACT_TERROR_SHOVED_RIGHTWARD )
					shoveDir = right;
			}

			if ( shoveDir != vec3_origin )
			{
				trace_t tr;
				const float testDist = 16.0f;
				UTIL_TraceHull( GetAbsOrigin(), GetAbsOrigin() + shoveDir * testDist, GetHullMins(), GetHullMaxs(), MASK_NPCSOLID_BRUSHONLY, this, COLLISION_GROUP_NPC, &tr );
				if ( tr.fraction < 1.0f && tr.DidHitWorld() )
				{
					Activity intoWallActivity = ACT_INVALID;
					if ( m_ShoveActivity == ACT_TERROR_SHOVED_BACKWARD )
						intoWallActivity = ACT_TERROR_SHOVED_BACKWARD_INTO_WALL;
					else if ( m_ShoveActivity == ACT_TERROR_SHOVED_FORWARD )
						intoWallActivity = ACT_TERROR_SHOVED_FORWARD_INTO_WALL;
					else if ( m_ShoveActivity == ACT_TERROR_SHOVED_LEFTWARD )
						intoWallActivity = ACT_TERROR_SHOVED_LEFTWARD_INTO_WALL;
					else if ( m_ShoveActivity == ACT_TERROR_SHOVED_RIGHTWARD )
						intoWallActivity = ACT_TERROR_SHOVED_RIGHTWARD_INTO_WALL;

					const Activity fallbackIntoWallActivity = ACT_TERROR_SHOVED_BACKWARD_INTO_WALL;
					const Activity candidates[2] = { intoWallActivity, fallbackIntoWallActivity };

					for ( int i = 0; i < ARRAYSIZE( candidates ); ++i )
					{
						const Activity candidate = candidates[i];
						if ( candidate == ACT_INVALID )
							continue;

						const int seq = SelectWeightedSequence( candidate );
						if ( seq > ACTIVITY_NOT_AVAILABLE )
						{
							m_bShoveHitWall = true;
							m_ShoveActivity = candidate;

							ResetSequence( seq );
							ResetSequenceInfo();
							SetActivity( candidate );
							SetCycle( 0.0f );
							break;
						}
					}
				}
			}
		}

		// Apply animation-driven movement during shove sequences (root motion).
		AutoMovement();

		if ( IsSequenceFinished() )
		{
			m_bInShove = false;
			TaskComplete();
		}
		break;
	}
	case TASK_MELEE_ATTACK1:
	{
		RunAttackTask( pTask->iTask );
		break;
	}
	default:
		BaseClass::RunTask( pTask );
		break;
	}
}

void CNPC_Infected::RunAttackTask( int task )
{
	AutoMovement( );

	Vector vecEnemyLKP = GetEnemyLKP();

	// If our enemy was killed, but I'm not done animating, the last known position comes
	// back as the origin and makes the me face the world origin if my attack schedule
	// doesn't break when my enemy dies. (sjb)
	if( vecEnemyLKP != vec3_origin )
	{
		if ( ( task == TASK_RANGE_ATTACK1 || task == TASK_RELOAD ) && 
			 ( CapabilitiesGet() & bits_CAP_AIM_GUN ) && 
			 FInAimCone( vecEnemyLKP ) )
		{
			// Arms will aim, so leave body yaw as is
			GetMotor()->SetIdealYawAndUpdate( GetMotor()->GetIdealYaw(), AI_KEEP_YAW_SPEED );
		}
		else
		{
			GetMotor()->SetIdealYawToTargetAndUpdate( vecEnemyLKP, AI_KEEP_YAW_SPEED );
		}
	}

	const float hl2ZombieMeleeRange = 55.0f; // ZOMBIE_MELEE_REACH from HL2 base zombie
	const float hl2ZombieMeleeStopRange = 75.0f; // hysteresis to prevent range jitter

	float dist = 1e30f;
	if ( GetEnemy() )
	{
		dist = ( GetEnemy()->GetAbsOrigin() - GetAbsOrigin() ).Length2D();
	}

	// Enter/exit continuous attack mode based on range.
	if ( dist <= hl2ZombieMeleeRange )
	{
		if ( !m_bContinuousAttacking )
		{
			m_bContinuousAttacking = true;

			if ( m_iAttackLayer != -1 )
			{
				FastRemoveLayer( m_iAttackLayer );
				m_iAttackLayer = -1;
			}

			const int seq = SelectWeightedSequence( ACT_TERROR_ATTACK_CONTINUOUSLY );
			if ( seq > ACTIVITY_NOT_AVAILABLE )
			{
				ResetSequence( seq );
				ResetSequenceInfo();
			}
			SetActivity( ACT_TERROR_ATTACK_CONTINUOUSLY );
		}
	}
	else if ( m_bContinuousAttacking && dist > hl2ZombieMeleeStopRange )
	{
		m_bContinuousAttacking = false;
		TaskComplete();
		return;
	}

	// While in continuous mode, loop the continuous attack sequence and never use gestures.
	if ( m_bContinuousAttacking )
	{
		if ( IsSequenceFinished() )
		{
			const int seq = SelectWeightedSequence( ACT_TERROR_ATTACK_CONTINUOUSLY );
			if ( seq > ACTIVITY_NOT_AVAILABLE )
			{
				ResetSequence( seq );
				ResetSequenceInfo();
			}
		}

		return;
	}

	// Not continuous: complete when our single-swing gesture finishes (or if we don't have one).
	if ( m_iAttackLayer == -1 )
	{
		TaskComplete();
		return;
	}

	CAnimationLayer *pPlayer = GetAnimOverlay( m_iAttackLayer );
	if ( !pPlayer || pPlayer->m_bSequenceFinished )
	{
		if ( task == TASK_RELOAD && GetShotRegulator() )
		{
			GetShotRegulator()->Reset( false );
		}

		TaskComplete();
	}
}

//-------------------------------------------------------------------------------------------------
//
// Schedules
//
//-------------------------------------------------------------------------------------------------
AI_BEGIN_CUSTOM_NPC( npc_infected, CNPC_Infected )
	DECLARE_ACTIVITY( ACT_TERROR_IDLE_NEUTRAL )
	DECLARE_ACTIVITY( ACT_TERROR_IDLE_ALERT )
	DECLARE_ACTIVITY( ACT_TERROR_WALK_NEUTRAL )
	DECLARE_ACTIVITY( ACT_TERROR_WALK_INTENSE )
	DECLARE_ACTIVITY( ACT_TERROR_RUN_INTENSE )
	DECLARE_ACTIVITY( ACT_TERROR_JUMP )
	DECLARE_ACTIVITY( ACT_TERROR_JUMP_OVER_GAP )
	DECLARE_ACTIVITY( ACT_TERROR_JUMP_LANDING )
	DECLARE_ACTIVITY( ACT_TERROR_SHOVED_BACKWARD_INTO_WALL )
	DECLARE_ACTIVITY( ACT_TERROR_SHOVED_FORWARD_INTO_WALL )
	DECLARE_ACTIVITY( ACT_TERROR_SHOVED_LEFTWARD_INTO_WALL )
	DECLARE_ACTIVITY( ACT_TERROR_SHOVED_RIGHTWARD_INTO_WALL )

	DECLARE_TASK( TASK_TERROR_SHOVE )

	DEFINE_SCHEDULE
	(
		SCHED_TERROR_SHOVE,

		"	Tasks"
		"		TASK_STOP_MOVING				0"
		"		TASK_TERROR_SHOVE				0"
		"	"
		"	Interrupts"
	)

	DEFINE_SCHEDULE
	(
		SCHED_TERROR_WANDER_ANGRILY,

		"	Tasks"
		"		TASK_WANDER						480240" // 48 units to 240 units.
		"		TASK_WALK_PATH					0"
		"		TASK_WAIT_FOR_MOVEMENT			4"
		"	"
		"	Interrupts"
		"		COND_ENEMY_DEAD"
		"		COND_NEW_ENEMY"
	)

AI_END_CUSTOM_NPC()

int CNPC_Infected::SelectSchedule( void )
{
	if ( m_bPendingShove && m_ShoveActivity != ACT_INVALID )
		return SCHED_TERROR_SHOVE;

	// Common infected should never idle-stand; keep them roaming when they have no enemy.
	// Use PATROL_WALK to avoid TASK_WAIT_PVS in SCHED_IDLE_WA	NDER (keeps them wandering even offscreen).
	if ( ( m_NPCState == NPC_STATE_IDLE || m_NPCState == NPC_STATE_ALERT ) && GetEnemy() == NULL && !IsInChoreo() )
		return SCHED_IDLE_WANDER;

	return BaseClass::SelectSchedule();
}

int CNPC_Infected::SelectFailSchedule( int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode )
{
	// Mirror HL2 zombie behavior: if we failed to chase or take cover, wander angrily instead of stalling.
	if ( failedSchedule != SCHED_TERROR_WANDER_ANGRILY &&
		 ( failedSchedule == SCHED_TAKE_COVER_FROM_ENEMY || failedSchedule == SCHED_CHASE_ENEMY_FAILED ) )
	{
		return SCHED_TERROR_WANDER_ANGRILY;
	}

	return BaseClass::SelectFailSchedule( failedSchedule, failedTask, taskFailCode );
}
