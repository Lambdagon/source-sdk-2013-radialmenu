//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "c_cs_player.h"
#include "c_user_message_register.h"
#include "view.h"
#include "iclientvehicle.h"
#include "ivieweffects.h"
#include "input.h"
#include "cam_thirdperson.h"
#include "IEffects.h"
#include "fx.h"
#include "c_basetempentity.h"
#include "hud_macros.h"	//HOOK_COMMAND
#include "engine/ivdebugoverlay.h"
#include "smoke_fog_overlay.h"
#include "bone_setup.h"
#include "in_buttons.h"
#include "r_efx.h"
#include "dlight.h"
#include "shake.h"
#include "cl_animevent.h"
#include "c_physicsprop.h"
#include "props_shared.h"
#include "obstacle_pushaway.h"
#include "death_pose.h"
#include "eventlist.h"
#include "choreoscene.h"
#include "choreoevent.h"

#include "effect_dispatch_data.h"	//for water ripple / splash effect
#include "c_te_effect_dispatch.h"	//ditto
#include "c_te_legacytempents.h"
#include "cs_gamerules.h"
#include "fx_cs_blood.h"
#include "c_cs_playerresource.h"
#include "c_team.h"

#include "history_resource.h"
#include "ragdoll_shared.h"
#include "collisionutils.h"

// NVNT - haptics system for spectating
#include "haptics/haptic_utils.h"

#include "steam/steam_api.h"

#include "cs_blackmarket.h"				// for vest/helmet prices

#if defined( CCSPlayer )
	#undef CCSPlayer
#endif

#include "materialsystem/imesh.h"		//for materials->FindMaterial
#include "iviewrender.h"				//for view->

#include "iviewrender_beams.h"			// flashlight beam
#include "engine/IEngineSound.h"

static const float CS_DEPLOY_ANIM_DELAY = 0.02f;
static const float CS_STAGGER_TAUNTCAM_DIST = 120.0f;
static const float CS_STAGGER_TAUNTCAM_DIST_UP = 0.0f;
static const float CS_STAGGER_TAUNTCAM_SPEED = 300.0f;

//=============================================================================
// HPE_BEGIN:
// [menglish] Adding and externing variables needed for the freezecam
//=============================================================================

static Vector WALL_MIN(-WALL_OFFSET,-WALL_OFFSET,-WALL_OFFSET);
static Vector WALL_MAX(WALL_OFFSET,WALL_OFFSET,WALL_OFFSET);

extern ConVar	spec_freeze_time;
extern ConVar	spec_freeze_traveltime;
extern ConVar	spec_freeze_distance_min;
extern ConVar	spec_freeze_distance_max;

//=============================================================================
// HPE_END
//=============================================================================

ConVar cl_left_hand_ik( "cl_left_hand_ik", "0", 0, "Attach player's left hand to rifle with IK." );

ConVar cl_ragdoll_physics_enable( "cl_ragdoll_physics_enable", "1", 0, "Enable/disable ragdoll physics." );
extern ConVar g_ragdoll_fadespeed;
extern ConVar g_ragdoll_lvfadespeed;

ConVar cl_minmodels( "cl_minmodels", "0", 0, "Uses one player model for each team." );
ConVar cl_min_ct( "cl_min_ct", "1", 0, "Controls which CT model is used when cl_minmodels is set.", true, 1, true, 4 );
ConVar cl_min_t( "cl_min_t", "1", 0, "Controls which Terrorist model is used when cl_minmodels is set.", true, 1, true, 4 );
const float CycleLatchTolerance = 0.15;	// amount we can diverge from the server's cycle before we're corrected

extern ConVar mp_playerid_delay;
extern ConVar mp_playerid_hold;
extern ConVar sv_allowminmodels;

class CAddonInfo
{
public:
	const char *m_pAttachmentName;
	const char *m_pWeaponClassName;	// The addon uses the w_ model from this weapon.
	const char *m_pModelName;		//If this is present, will use this model instead of looking up the weapon
	const char *m_pHolsterName;
};



// These must follow the ADDON_ ordering.
CAddonInfo g_AddonInfo[] =
{
	{ "grenade0",	"weapon_flashbang",		0, 0 },
	{ "grenade1",	"weapon_flashbang",		0, 0 },
	{ "grenade2",	"weapon_hegrenade",		0, 0 },
	{ "grenade3",	"weapon_smokegrenade",	0, 0 },
	{ "c4",			"weapon_c4",			0, 0 },
	{ "defusekit",	0,						"models/weapons/w_defuser.mdl", 0 },
	{ "primary",	0,						0, 0 },	// Primary addon model is looked up based on m_iPrimaryAddon
	{ "pistol",		0,						0, 0 },	// Pistol addon model is looked up based on m_iSecondaryAddon
	{ "eholster",	0,						"models/weapons/w_eq_eholster_elite.mdl", "models/weapons/w_eq_eholster.mdl" },
};

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS( C_TEPlayerAnimEvent, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

	virtual void PostDataUpdate( DataUpdateType_t updateType )
	{
		// Create the effect.
		C_CSPlayer *pPlayer = dynamic_cast< C_CSPlayer* >( m_hPlayer.Get() );
		if ( pPlayer && !pPlayer->IsDormant() )
		{
			pPlayer->DoAnimationEvent( (PlayerAnimEvent_t)m_iEvent.Get(), m_nData );
		}
	}

public:
	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

IMPLEMENT_CLIENTCLASS_EVENT( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent );

BEGIN_RECV_TABLE_NOBASE( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_iEvent ) ),
	RecvPropInt( RECVINFO( m_nData ) )
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_CSPlayer )
#ifdef CS_SHIELD_ENABLED
	DEFINE_PRED_FIELD( m_bShieldDrawn, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
#endif
	DEFINE_PRED_FIELD_TOL( m_flStamina, FIELD_FLOAT, FTYPEDESC_INSENDTABLE, 0.1f ),
	DEFINE_PRED_FIELD( m_flCycle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
	DEFINE_PRED_FIELD( m_iShotsFired, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_iDirection, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bResumeZoom, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_iLastZoom, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),

END_PREDICTION_DATA()

vgui::IImage* GetDefaultAvatarImage( C_BasePlayer *pPlayer )
{
	vgui::IImage* result = NULL;

	switch ( pPlayer ? pPlayer->GetTeamNumber() : TEAM_MAXCOUNT )
	{
		case TEAM_TERRORIST: 
			result = vgui::scheme()->GetImage( CSTRIKE_DEFAULT_T_AVATAR, true );
			break;

		case TEAM_CT:		 
			result = vgui::scheme()->GetImage( CSTRIKE_DEFAULT_CT_AVATAR, true );
			break;

		default:
			result = vgui::scheme()->GetImage( CSTRIKE_DEFAULT_AVATAR, true );
			break;
	}

	return result;
}

// ----------------------------------------------------------------------------- //
// Client ragdoll entity.
// ----------------------------------------------------------------------------- //

float g_flDieTranslucentTime = 0.6;

class C_CSRagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS( C_CSRagdoll, C_BaseAnimatingOverlay );
	DECLARE_CLIENTCLASS();

	C_CSRagdoll();
	~C_CSRagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );
	virtual void ClientThink() OVERRIDE;

	int GetPlayerEntIndex() const;
	IRagdoll* GetIRagdoll() const;
	bool GetRagdollInitBoneArrays( matrix3x4_t *pDeltaBones0, matrix3x4_t *pDeltaBones1, matrix3x4_t *pCurrentBones, float boneDt ) OVERRIDE;

	void ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName );

	virtual void ComputeFxBlend();
	virtual bool IsTransparent();
	bool IsInitialized() { return m_bInitialized; }
	// fading ragdolls don't cast shadows
	virtual ShadowType_t ShadowCastType() 
	{ 
		if ( m_flRagdollSinkStart == -1 )
			return BaseClass::ShadowCastType();
		return SHADOWS_NONE;
	}

	virtual void ValidateModelIndex( void );

private:

	C_CSRagdoll( const C_CSRagdoll & ) {}

	void Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity );

	void CreateLowViolenceRagdoll( void );
	void CreateCSRagdoll( void );

private:

	EHANDLE	m_hPlayer;
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
	CNetworkVar(int, m_iDeathPose );
	CNetworkVar(int, m_iDeathFrame );
	float m_flRagdollSinkStart;
	float m_flFadeOutStartTime;
	bool m_bInitialized;
	bool m_bCreatedWhilePlaybackSkipping;
};


IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_CSRagdoll, DT_CSRagdoll, CCSRagdoll )
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
	RecvPropVector( RECVINFO(m_vecRagdollOrigin) ),
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_nModelIndex ) ),
	RecvPropInt( RECVINFO(m_nForceBone) ),
	RecvPropVector( RECVINFO(m_vecForce) ),
	RecvPropVector( RECVINFO( m_vecRagdollVelocity ) ),
	RecvPropInt( RECVINFO(m_iDeathPose) ),
	RecvPropInt( RECVINFO(m_iDeathFrame) ),
	RecvPropInt(RECVINFO(m_iTeamNum)),
	RecvPropInt( RECVINFO(m_bClientSideAnimation)),
END_RECV_TABLE()


C_CSRagdoll::C_CSRagdoll()
{
	m_flRagdollSinkStart = -1;
	m_flFadeOutStartTime = -1;
	m_bInitialized = false;
	m_bCreatedWhilePlaybackSkipping = engine->IsSkippingPlayback();
}

C_CSRagdoll::~C_CSRagdoll()
{
	PhysCleanupFrictionSounds( this );
}

bool C_CSRagdoll::GetRagdollInitBoneArrays( matrix3x4_t *pDeltaBones0, matrix3x4_t *pDeltaBones1, matrix3x4_t *pCurrentBones, float boneDt )
{
	// otherwise use the death pose to set up the ragdoll
	ForceSetupBonesAtTime( pDeltaBones0, gpGlobals->curtime - boneDt );
	GetRagdollCurSequenceWithDeathPose( this, pDeltaBones1, gpGlobals->curtime, m_iDeathPose, m_iDeathFrame );
	return SetupBones( pCurrentBones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, gpGlobals->curtime );
}

void C_CSRagdoll::Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity )
{
	if ( !pSourceEntity )
		return;

	VarMapping_t *pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t *pDest = GetVarMapping();

	// Find all the VarMapEntry_t's that represent the same variable.
	for ( int i = 0; i < pDest->m_Entries.Count(); i++ )
	{
		VarMapEntry_t *pDestEntry = &pDest->m_Entries[i];
		for ( int j=0; j < pSrc->m_Entries.Count(); j++ )
		{
			VarMapEntry_t *pSrcEntry = &pSrc->m_Entries[j];
			if ( !Q_strcmp( pSrcEntry->watcher->GetDebugName(),
				pDestEntry->watcher->GetDebugName() ) )
			{
				pDestEntry->watcher->Copy( pSrcEntry->watcher );
				break;
			}
		}
	}
}

void C_CSRagdoll::ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

	if( !pPhysicsObject )
		return;

	Vector dir = pTrace->endpos - pTrace->startpos;

	if ( iDamageType == DMG_BLAST )
	{
		dir *= 4000;  // adjust impact strenght

		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter( dir );
	}
	else
	{
		Vector hitpos;

		VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
		VectorNormalize( dir );

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset( dir, hitpos );

		// Blood spray!
		FX_CS_BloodSpray( hitpos, dir, 10 );
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}


void C_CSRagdoll::ValidateModelIndex( void )
{
	if ( sv_allowminmodels.GetBool() && cl_minmodels.GetBool() )
	{
		if ( GetTeamNumber() == TEAM_CT )
		{
			int index = cl_min_ct.GetInt() - 1;
			if ( index >= 0 && index < CTPlayerModels.Count() )
			{
				m_nModelIndex = modelinfo->GetModelIndex(CTPlayerModels[index]);
			}
		}
		else if ( GetTeamNumber() == TEAM_TERRORIST )
		{
			int index = cl_min_t.GetInt() - 1;
			if ( index >= 0 && index < TerroristPlayerModels.Count() )
			{
				m_nModelIndex = modelinfo->GetModelIndex(TerroristPlayerModels[index]);
			}
		}
	}

	BaseClass::ValidateModelIndex();
}


void C_CSRagdoll::CreateLowViolenceRagdoll( void )
{
	// Just play a death animation.
	// Find a death anim to play.
	int iMinDeathAnim = 9999, iMaxDeathAnim = -9999;
	for ( int iAnim=1; iAnim < 100; iAnim++ )
	{
		char str[512];
		Q_snprintf( str, sizeof( str ), "death%d", iAnim );
		if ( LookupSequence( str ) == -1 )
			break;

		iMinDeathAnim = MIN( iMinDeathAnim, iAnim );
		iMaxDeathAnim = MAX( iMaxDeathAnim, iAnim );
	}

	if ( iMinDeathAnim == 9999 )
	{
		CreateCSRagdoll();
		return;
	}

	SetNetworkOrigin( m_vecRagdollOrigin );
	SetAbsOrigin( m_vecRagdollOrigin );
	SetAbsVelocity( m_vecRagdollVelocity );

	C_CSPlayer *pPlayer = dynamic_cast< C_CSPlayer* >( m_hPlayer.Get() );
	if ( pPlayer )
	{
		if ( !pPlayer->IsDormant() )
		{
			// move my current model instance to the ragdoll's so decals are preserved.
			pPlayer->SnatchModelInstance( this );
		}

		SetAbsAngles( pPlayer->GetRenderAngles() );
		SetNetworkAngles( pPlayer->GetRenderAngles() );
	}

	int iDeathAnim = RandomInt( iMinDeathAnim, iMaxDeathAnim );
	char str[512];
	Q_snprintf( str, sizeof( str ), "death%d", iDeathAnim );
	SetSequence( LookupSequence( str ) );
	ForceClientSideAnimationOn();

	Interp_Reset( GetVarMapping() );
}


void C_CSRagdoll::CreateCSRagdoll()
{
	// First, initialize all our data. If we have the player's entity on our client,
	// then we can make ourselves start out exactly where the player is.
	C_CSPlayer *pPlayer = dynamic_cast< C_CSPlayer* >( m_hPlayer.Get() );

	// mark this to prevent model changes from overwriting the death sequence with the server sequence
	SetReceivedSequence();

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		// Stop any attached particle effects immediately.
		ParticleProp()->StopEmissionAndDestroyImmediately(NULL);
		ParticleProp()->StopParticlesInvolving(pPlayer);
		// move my current model instance to the ragdoll's so decals are preserved.
		pPlayer->SnatchModelInstance( this );

		VarMapping_t *varMap = GetVarMapping();

		// Copy all the interpolated vars from the player entity.
		// The entity uses the interpolated history to get bone velocity.
		bool bRemotePlayer = (pPlayer != C_BasePlayer::GetLocalPlayer());
		if ( bRemotePlayer )
		{
			Interp_Copy( pPlayer );

			SetAbsAngles( pPlayer->GetRenderAngles() );
			GetRotationInterpolator().Reset();

			m_flAnimTime = pPlayer->m_flAnimTime;
			SetSequence( pPlayer->GetSequence() );
			m_flPlaybackRate = pPlayer->GetPlaybackRate();
		}
		else
		{
			// This is the local player, so set them in a default
			// pose and slam their velocity, angles and origin
			SetAbsOrigin( m_vecRagdollOrigin );

			SetAbsAngles( pPlayer->GetRenderAngles() );

			SetAbsVelocity( m_vecRagdollVelocity );

			int iSeq = LookupSequence( "walk_lower" );
			if ( iSeq == -1 )
			{
				Assert( false );	// missing walk_lower?
				iSeq = 0;
			}

			SetSequence( iSeq );	// walk_lower, basic pose
			SetCycle( 0.0 );

			// go ahead and set these on the player in case the code below decides to set up bones using
			// that entity instead of this one.  The local player may not have valid animation
			pPlayer->SetSequence( iSeq );	// walk_lower, basic pose
			pPlayer->SetCycle( 0.0 );

			Interp_Reset( varMap );
		}
	}
	else
	{
		// overwrite network origin so later interpolation will
		// use this position
		SetNetworkOrigin( m_vecRagdollOrigin );

		SetAbsOrigin( m_vecRagdollOrigin );
		SetAbsVelocity( m_vecRagdollVelocity );

		Interp_Reset( GetVarMapping() );
	}

	// Turn it into a ragdoll.
	if ( cl_ragdoll_physics_enable.GetInt() )
	{
		// Make us a ragdoll..
		m_nRenderFX = kRenderFxRagdoll;

		matrix3x4_t boneDelta0[MAXSTUDIOBONES];
		matrix3x4_t boneDelta1[MAXSTUDIOBONES];
		matrix3x4_t currentBones[MAXSTUDIOBONES];
		const float boneDt = 0.05f;

		//=============================================================================
		// [pfreese], [tj]
		// There are visual problems with the attempted blending of the 
		// death pose animations in C_CSRagdoll::GetRagdollInitBoneArrays. The version
		// in C_BasePlayer::GetRagdollInitBoneArrays doesn't attempt to blend death
		// poses, so if the player is relevant, use that one regardless of whether the 
		// player is the local one or not.
		//=============================================================================
		if ( pPlayer && !pPlayer->IsDormant() )
		{
			pPlayer->GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
		}
		else
		{
			GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
		}

		InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );
		m_flRagdollSinkStart = -1;
	}
	else
	{
		m_flRagdollSinkStart = gpGlobals->curtime;
		DestroyShadow();
		ClientLeafSystem()->SetRenderGroup( GetRenderHandle(), RENDER_GROUP_TRANSLUCENT_ENTITY );
	}
	m_bInitialized = true;
}


void C_CSRagdoll::ComputeFxBlend( void )
{
	if ( m_flRagdollSinkStart == -1 )
	{
		BaseClass::ComputeFxBlend();
	}
	else
	{
		float elapsed = gpGlobals->curtime - m_flRagdollSinkStart;
		float flVal = RemapVal( elapsed, 0, g_flDieTranslucentTime, 255, 0 );
		flVal = clamp( flVal, 0, 255 );
		m_nRenderFXBlend = (int)flVal;

#ifdef _DEBUG
		m_nFXComputeFrame = gpGlobals->framecount;
#endif
	}
}


bool C_CSRagdoll::IsTransparent( void )
{
	if ( m_flRagdollSinkStart == -1 )
	{
		return BaseClass::IsTransparent();
	}
	else
	{
		return true;
	}
}

void C_CSRagdoll::ClientThink()
{
	if ( m_flFadeOutStartTime < 0.0f || IsEffectActive( EF_NODRAW ) )
		return;

	if ( gpGlobals->curtime < m_flFadeOutStartTime )
	{
		SetNextClientThink( m_flFadeOutStartTime );
		return;
	}

	const int fadeSpeed = g_RagdollLVManager.IsLowViolence() ? g_ragdoll_lvfadespeed.GetInt() : g_ragdoll_fadespeed.GetInt();
	int alpha = GetRenderColor().a;
	alpha = MAX( alpha - ( fadeSpeed * gpGlobals->frametime ), 0 );

	SetRenderMode( kRenderTransAlpha );
	SetRenderColorA( alpha );

	if ( alpha <= 0 )
	{
		AddEffects( EF_NODRAW );
		SetNextClientThink( CLIENT_THINK_NEVER );
		return;
	}

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}


void C_CSRagdoll::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		// Prevent replays from creating ragdolls on the first frame of playback after skipping through playback.
		// If a player died (leaving a ragdoll) previous to the first frame of replay playback,
		// their ragdoll wasn't yet initialized because OnDataChanged events are queued but not processed
		// until the first render. 
		if ( engine->IsPlayingDemo() && m_bCreatedWhilePlaybackSkipping )
		{
			Release();
			return;
		}

		if ( g_RagdollLVManager.IsLowViolence() )
		{
			CreateLowViolenceRagdoll();
		}
		else
		{
			CreateCSRagdoll();
		}

		SetRenderColorA( 255 );
		// Normal ragdolls linger for a bit before fading. Low-violence starts fading immediately.
		m_flFadeOutStartTime = gpGlobals->curtime + ( g_RagdollLVManager.IsLowViolence() ? 0.0f : 15.0f );
		SetNextClientThink( m_flFadeOutStartTime );
	}
	else
	{
		if ( !cl_ragdoll_physics_enable.GetInt() )
		{
			// Don't let it set us back to a ragdoll with data from the server.
			m_nRenderFX = kRenderFxNone;
		}
	}
}

IRagdoll* C_CSRagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

//-----------------------------------------------------------------------------
// Purpose: Called when the player toggles nightvision
// Input  : *pData - the int value of the nightvision state
//			*pStruct - the player
//			*pOut -
//-----------------------------------------------------------------------------
void RecvProxy_NightVision( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_CSPlayer *pPlayerData = (C_CSPlayer *) pStruct;

	bool bNightVisionOn = ( pData->m_Value.m_Int > 0 );

	if ( pPlayerData->m_bNightVisionOn != bNightVisionOn )
	{
		if ( bNightVisionOn )
			 pPlayerData->m_flNightVisionAlpha = 1;
	}

	pPlayerData->m_bNightVisionOn = bNightVisionOn;
}

void RecvProxy_FlashTime( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_CSPlayer *pPlayerData = (C_CSPlayer *) pStruct;

	if( pPlayerData != C_BasePlayer::GetLocalPlayer() )
		return;

	if ( (pPlayerData->m_flFlashDuration != pData->m_Value.m_Float) && pData->m_Value.m_Float > 0 )
	{
		pPlayerData->m_flFlashAlpha = 1;
	}

	pPlayerData->m_flFlashDuration = pData->m_Value.m_Float;
	pPlayerData->m_flFlashBangTime = gpGlobals->curtime + pPlayerData->m_flFlashDuration;
}

void RecvProxy_HasDefuser( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_CSPlayer *pPlayerData = (C_CSPlayer *)pStruct;

	if (pPlayerData == NULL)
	{
		return;
	}

	bool drawIcon = false;

	if (pData->m_Value.m_Int == 0)
	{
		pPlayerData->RemoveDefuser();
	}
	else
	{
		if (pPlayerData->HasDefuser() == false)
		{
			drawIcon = true;
		}
		pPlayerData->GiveDefuser();
	}

	if (pPlayerData->IsLocalPlayer() && drawIcon)
	{
		// add to pickup history
		CHudHistoryResource *pHudHR = GET_HUDELEMENT( CHudHistoryResource );

		if ( pHudHR )
		{
			pHudHR->AddToHistory(HISTSLOT_ITEM, "defuser_pickup");
		}
	}
}

void C_CSPlayer::RecvProxy_CycleLatch( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	// This receive proxy looks to see if the server's value is close enough to what we think it should
	// be.  We've been running the same code; this is an error correction for changes we didn't simulate
	// while they were out of PVS.
	C_CSPlayer *pPlayer = (C_CSPlayer *)pStruct;
	if( pPlayer->IsLocalPlayer() )
		return; // Don't need to fixup ourselves.

	float incomingCycle = (float)(pData->m_Value.m_Int) / 16; // Came in as 4 bit fixed point
	float currentCycle = pPlayer->GetCycle();
	bool closeEnough = fabs(currentCycle - incomingCycle) < CycleLatchTolerance;
	if( fabs(currentCycle - incomingCycle) > (1 - CycleLatchTolerance) )
	{
		closeEnough = true;// Handle wrapping around 1->0
	}

	if( !closeEnough )
	{
		// Server disagrees too greatly.  Correct our value.
		if ( pPlayer && pPlayer->GetTeam() )
		{
			DevMsg( 2, "%s %s(%d): Cycle latch wants to correct %.2f in to %.2f.\n",
				pPlayer->GetTeam()->Get_Name(), pPlayer->GetPlayerName(), pPlayer->entindex(), currentCycle, incomingCycle );
		}
		pPlayer->SetServerIntendedCycle( incomingCycle );
	}
}

void __MsgFunc_ReloadEffect( bf_read &msg )
{
	int iPlayer = msg.ReadShort();
	C_CSPlayer *pPlayer = dynamic_cast< C_CSPlayer* >( C_BaseEntity::Instance( iPlayer ) );
	if ( pPlayer )
		pPlayer->PlayReloadEffect();

}
USER_MESSAGE_REGISTER( ReloadEffect );

BEGIN_RECV_TABLE_NOBASE( C_CSPlayer, DT_CSLocalPlayerExclusive )
	RecvPropFloat( RECVINFO(m_flStamina) ),
	RecvPropInt( RECVINFO( m_iDirection ) ),
	RecvPropInt( RECVINFO( m_iShotsFired ) ),
	RecvPropFloat( RECVINFO( m_flVelocityModifier ) ),

	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),

    //=============================================================================
    // HPE_BEGIN:
    // [tj]Set up the receive table for per-client domination data
    //=============================================================================

    RecvPropArray3( RECVINFO_ARRAY( m_bPlayerDominated ), RecvPropBool( RECVINFO( m_bPlayerDominated[0] ) ) ),
    RecvPropArray3( RECVINFO_ARRAY( m_bPlayerDominatingMe ), RecvPropBool( RECVINFO( m_bPlayerDominatingMe[0] ) ) )

    //=============================================================================
    // HPE_END
    //=============================================================================

END_RECV_TABLE()


BEGIN_RECV_TABLE_NOBASE( C_CSPlayer, DT_CSNonLocalPlayerExclusive )
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
END_RECV_TABLE()


IMPLEMENT_CLIENTCLASS_DT( C_CSPlayer, DT_CSPlayer, CCSPlayer )
	RecvPropDataTable( "cslocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_CSLocalPlayerExclusive) ),
	RecvPropDataTable( "csnonlocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_CSNonLocalPlayerExclusive) ),
	RecvPropInt( RECVINFO( m_iAddonBits ) ),
	RecvPropInt( RECVINFO( m_iPrimaryAddon ) ),
	RecvPropInt(RECVINFO(m_iSecondaryAddon)),
	RecvPropInt(RECVINFO(m_zombieClass)),
	RecvPropInt(RECVINFO(m_survivorClass)),
	RecvPropBool(RECVINFO(m_bTankDeathInProgress)),
	RecvPropFloat(RECVINFO(m_flTankDeathPrevCycle)),
	RecvPropInt(RECVINFO(m_nTankDeathSequence)),
	RecvPropInt( RECVINFO( m_iThrowGrenadeCounter ) ),
	RecvPropInt( RECVINFO( m_iPlayerState ) ),
	RecvPropInt( RECVINFO( m_iAccount ) ),
	RecvPropInt( RECVINFO( m_bInBombZone ) ),
	RecvPropInt( RECVINFO( m_bInBuyZone ) ),
	RecvPropInt( RECVINFO( m_iClass ) ),
	RecvPropInt( RECVINFO( m_ArmorValue ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
	RecvPropFloat( RECVINFO( m_flStamina ) ),
	RecvPropInt( RECVINFO( m_bHasDefuser ), 0, RecvProxy_HasDefuser ),
	RecvPropInt( RECVINFO( m_bNightVisionOn), 0, RecvProxy_NightVision ),
	RecvPropBool( RECVINFO( m_bHasNightVision ) ),


    //=============================================================================
    // HPE_BEGIN:
    // [dwenger] Added for fun-fact support
    //=============================================================================

    //RecvPropBool( RECVINFO( m_bPickedUpDefuser ) ),
    //RecvPropBool( RECVINFO( m_bDefusedWithPickedUpKit ) ),

    //=============================================================================
    // HPE_END
    //=============================================================================

    RecvPropBool( RECVINFO( m_bInHostageRescueZone ) ),
	RecvPropInt( RECVINFO( m_ArmorValue ) ),
	RecvPropBool( RECVINFO( m_bIsDefusing ) ),
	RecvPropBool( RECVINFO( m_bResumeZoom ) ),
	RecvPropInt( RECVINFO( m_iLastZoom ) ),

#ifdef CS_SHIELD_ENABLED
	RecvPropBool( RECVINFO( m_bHasShield ) ),
	RecvPropBool( RECVINFO( m_bShieldDrawn ) ),
#endif
	RecvPropInt( RECVINFO( m_bHasHelmet ) ),
	RecvPropVector( RECVINFO( m_vecRagdollVelocity ) ),
	RecvPropFloat( RECVINFO( m_flFlashDuration ), 0, RecvProxy_FlashTime ),
	RecvPropFloat( RECVINFO( m_flFlashMaxAlpha)),
	RecvPropInt( RECVINFO( m_iProgressBarDuration ) ),
	RecvPropFloat( RECVINFO( m_flProgressBarStartTime ) ),
	RecvPropEHandle( RECVINFO( m_hRagdoll ) ),
	RecvPropBool( RECVINFO( m_bIsIT ) ),
	RecvPropBool( RECVINFO( m_bIsGhost ) ),
	RecvPropBool( RECVINFO( m_bIncapacitated ) ),
	RecvPropBool( RECVINFO( m_bBeingRevived ) ),
	RecvPropInt( RECVINFO( m_nIncapacitationCount ) ),
	RecvPropBool( RECVINFO( m_bIncapBlackAndWhite ) ),
	RecvPropBool( RECVINFO( m_bUseSurvivorCalmAnimations ) ),
	RecvPropEHandle( RECVINFO( m_hReviveTarget ) ),
	RecvPropBool( RECVINFO( m_bUsingFirstAidKitOnSelf ) ),
	RecvPropEHandle( RECVINFO( m_hFirstAidKitTarget ) ),
	RecvPropEHandle( RECVINFO( m_pounceVictim ) ),
	RecvPropEHandle( RECVINFO( m_pounceAttacker ) ),
	RecvPropEHandle( RECVINFO( m_chargerVictim ) ),
	RecvPropEHandle( RECVINFO( m_chargerAttacker ) ),
	RecvPropInt( RECVINFO( m_nChargerAction ) ),
	RecvPropInt( RECVINFO( m_nChargerVictimAction ) ),
	RecvPropInt( RECVINFO( m_nChargerStaggerDir ) ),
	RecvPropInt( RECVINFO( m_nDamageStaggerDir ) ),
	RecvPropInt( RECVINFO( m_nTankAction ) ),
	RecvPropInt( RECVINFO( m_cycleLatch ), 0, &C_CSPlayer::RecvProxy_CycleLatch ),

END_RECV_TABLE()



C_CSPlayer::C_CSPlayer() :
	m_iv_angEyeAngles( "C_CSPlayer::m_iv_angEyeAngles" )
{
	m_PlayerAnimState = CreatePlayerAnimState( this, this, LEGANIM_9WAY, true );

	m_angEyeAngles.Init();

	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	m_iLastAddonBits = m_iAddonBits = 0;
	m_iLastPrimaryAddon = m_iLastSecondaryAddon = WEAPON_NONE;
	m_iProgressBarDuration = 0;
	m_flProgressBarStartTime = 0.0f;
	m_ArmorValue = 0;
	m_bHasHelmet = false;
	m_iIDEntIndex = 0;
	m_delayTargetIDTimer.Reset();
	m_iOldIDEntIndex = 0;
	m_holdTargetIDTimer.Reset();
	m_iDirection = 0;
	m_zombieClass = 0;
	m_survivorClass = 0;
	m_bIsIT = false;
	m_bUseSurvivorCalmAnimations = false;
	m_hReviveTarget = NULL;
	m_bUsingFirstAidKitOnSelf = false;
	m_hFirstAidKitTarget = NULL;

	m_Activity = ACT_IDLE;

	m_pFlashlightBeam = NULL;
	m_fNextThinkPushAway = 0.0f;

	m_serverIntendedCycle = -1.0f;

	view->SetScreenOverlayMaterial( NULL );

    m_bPlayingFreezeCamSound = false;

	m_flFootstepEventSuppressUntil = gpGlobals ? ( gpGlobals->curtime + 0.22f ) : 0.22f;

	m_nLastInfectedParticleZombieClass = -1;
	m_nLastInfectedParticleTeam = TEAM_UNASSIGNED;
	m_bLastInfectedParticleAlive = false;
	m_hLastClientActiveWeapon = NULL;
	m_bLastClientActiveWeaponVisible = false;
	m_bLastClientUsingViewModel = false;
	m_hPendingDeployAnimationWeapon = NULL;
	m_flPendingDeployAnimationTime = 0.0f;

	m_bPounceCamHasSavedState = false;
	m_bPounceCamBaseThirdPerson = false;
	m_bPounceCamBaseForcedThirdPerson = false;
	m_bPounceCamBaseOverridingThirdPerson = false;
	m_flPounceCamBlend = 0.0f;
	m_vecPounceCamBaseDesiredOffset.Init();
	m_bStaggerCamHasSavedState = false;
	m_bStaggerCamBaseThirdPerson = false;
	m_bStaggerCamBaseForcedThirdPerson = false;
	m_bStaggerCamBaseOverridingThirdPerson = false;
	m_bStaggerCamInterpolating = false;
	m_flStaggerCamCurrentDist = 0.0f;
	m_flStaggerCamTargetDist = 0.0f;
	m_flStaggerCamCurrentDistUp = 0.0f;
	m_flStaggerCamTargetDistUp = 0.0f;
	m_vecStaggerCamBaseDesiredOffset.Init();
	m_StaggerCameraData.m_flPitch = 0.0f;
	m_StaggerCameraData.m_flYaw = 0.0f;
	m_StaggerCameraData.m_flDist = 0.0f;
	m_StaggerCameraData.m_flLag = 1.0f;
	m_StaggerCameraData.m_vecHullMin.Init( -9.0f, -9.0f, -9.0f );
	m_StaggerCameraData.m_vecHullMax.Init( 9.0f, 9.0f, 9.0f );
	m_bLocalPounceMusicPlaying = false;
	m_hInfectedColorCorrection = INVALID_CLIENT_CCHANDLE;
	m_bTriedCreateInfectedColorCorrection = false;
	m_hGhostColorCorrection = INVALID_CLIENT_CCHANDLE;
	m_bTriedCreateGhostColorCorrection = false;
	m_bLocalHeartbeatPlaying = false;
	m_hBlackAndWhiteColorCorrection = INVALID_CLIENT_CCHANDLE;
	m_bTriedCreateBlackAndWhiteColorCorrection = false;
}

void C_CSPlayer::Spawn(void)
{
	BaseClass::Spawn();
}
C_CSPlayer::~C_CSPlayer()
{
	if ( IsLocalPlayer() && m_bLocalPounceMusicPlaying )
	{
		C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Event.HunterPounce" );
		m_bLocalPounceMusicPlaying = false;
	}

	if ( IsLocalPlayer() && m_bLocalHeartbeatPlaying )
	{
		C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Player.Heartbeat" );
		m_bLocalHeartbeatPlaying = false;
	}

	if ( m_hInfectedColorCorrection != INVALID_CLIENT_CCHANDLE )
	{
		g_pColorCorrectionMgr->RemoveColorCorrection( m_hInfectedColorCorrection );
		m_hInfectedColorCorrection = INVALID_CLIENT_CCHANDLE;
	}

	if ( m_hGhostColorCorrection != INVALID_CLIENT_CCHANDLE )
	{
		g_pColorCorrectionMgr->RemoveColorCorrection( m_hGhostColorCorrection );
		m_hGhostColorCorrection = INVALID_CLIENT_CCHANDLE;
	}

	if ( m_hBlackAndWhiteColorCorrection != INVALID_CLIENT_CCHANDLE )
	{
		g_pColorCorrectionMgr->RemoveColorCorrection( m_hBlackAndWhiteColorCorrection );
		m_hBlackAndWhiteColorCorrection = INVALID_CLIENT_CCHANDLE;
	}

	StopInfectedAmbientParticles();
	RemoveAddonModels();

	ReleaseFlashlight();

	m_PlayerAnimState->Release();
}


bool C_CSPlayer::HasDefuser() const
{
	return m_bHasDefuser;
}

void C_CSPlayer::GiveDefuser()
{
	m_bHasDefuser = true;
}

void C_CSPlayer::RemoveDefuser()
{
	m_bHasDefuser = false;
}

bool C_CSPlayer::HasNightVision() const
{
	return m_bHasNightVision;
}

bool C_CSPlayer::IsVIP() const
{
	C_CS_PlayerResource *pCSPR = (C_CS_PlayerResource*)GameResources();

	if ( !pCSPR )
		return false;

	return pCSPR->IsVIP( entindex() );
}

C_CSPlayer* C_CSPlayer::GetLocalCSPlayer()
{
	return (C_CSPlayer*)C_BasePlayer::GetLocalPlayer();
}


CSPlayerState C_CSPlayer::State_Get() const
{
	return m_iPlayerState;
}


float C_CSPlayer::GetMinFOV() const
{
	// Min FOV for AWP.
	return 10;
}


int C_CSPlayer::GetAccount() const
{
	return m_iAccount;
}


int C_CSPlayer::PlayerClass() const
{
	return m_iClass;
}

bool C_CSPlayer::IsInBuyZone()
{
	return m_bInBuyZone;
}

bool C_CSPlayer::CanShowTeamMenu() const
{
	return true;
}


int C_CSPlayer::ArmorValue() const
{
	return m_ArmorValue;
}

bool C_CSPlayer::HasHelmet() const
{
	return m_bHasHelmet;
}

int C_CSPlayer::GetCurrentAssaultSuitPrice()
{
	// WARNING: This price logic also exists in CCSPlayer::AttemptToBuyAssaultSuit
	// and must be kept in sync if changes are made.

	int fullArmor = ArmorValue() >= 100 ? 1 : 0;
	if ( fullArmor && !HasHelmet() )
	{
		return HELMET_PRICE;
	}
	else if ( !fullArmor && HasHelmet() )
	{
		return KEVLAR_PRICE;
	}
	else
	{
		// NOTE: This applies to the case where you already have both
		// as well as the case where you have neither.  In the case
		// where you have both, the item should still have a price
		// and become disabled when you have little or no money left.
		return ASSAULTSUIT_PRICE;
	}
}

const QAngle& C_CSPlayer::GetRenderAngles()
{
	if ( IsRagdoll() )
	{
		return vec3_angle;
	}
	else
	{
		return m_PlayerAnimState->GetRenderAngles();
	}
}


float g_flFattenAmt = 4;
void C_CSPlayer::GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType )
{
	if ( shadowType == SHADOWS_SIMPLE )
	{
		// Don't let the render bounds change when we're using blobby shadows, or else the shadow
		// will pop and stretch.
		mins = CollisionProp()->OBBMins();
		maxs = CollisionProp()->OBBMaxs();
	}
	else
	{
		GetRenderBounds( mins, maxs );

		// We do this because the normal bbox calculations don't take pose params into account, and
		// the rotation of the guy's upper torso can place his gun a ways out of his bbox, and
		// the shadow will get cut off as he rotates.
		//
		// Thus, we give it some padding here.
		mins -= Vector( g_flFattenAmt, g_flFattenAmt, 0 );
		maxs += Vector( g_flFattenAmt, g_flFattenAmt, 0 );
	}
}


void C_CSPlayer::GetRenderBounds( Vector& theMins, Vector& theMaxs )
{
	// TODO POSTSHIP - this hack/fix goes hand-in-hand with a fix in CalcSequenceBoundingBoxes in utils/studiomdl/simplify.cpp.
	// When we enable the fix in CalcSequenceBoundingBoxes, we can get rid of this.
	//
	// What we're doing right here is making sure it only uses the bbox for our lower-body sequences since,
	// with the current animations and the bug in CalcSequenceBoundingBoxes, are WAY bigger than they need to be.
	C_BaseAnimating::GetRenderBounds( theMins, theMaxs );

	// If we're ducking, we should reduce the render height by the difference in standing and ducking heights.
	// This prevents shadows from drawing above ducking players etc.
	if ( GetFlags() & FL_DUCKING )
	{
		theMaxs.z -= 18.5f;
	}
}


bool C_CSPlayer::GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const
{
	if ( shadowType == SHADOWS_SIMPLE )
	{
		// Blobby shadows should sit directly underneath us.
		pDirection->Init( 0, 0, -1 );
		return true;
	}
	else
	{
		return BaseClass::GetShadowCastDirection( pDirection, shadowType );
	}
}


void C_CSPlayer::VPhysicsUpdate( IPhysicsObject *pPhysics )
{
	BaseClass::VPhysicsUpdate( pPhysics );
}


int C_CSPlayer::GetIDTarget() const
{
	if ( !m_delayTargetIDTimer.IsElapsed() )
		return 0;

	if ( m_iIDEntIndex )
	{
		return m_iIDEntIndex;
	}

	if ( m_iOldIDEntIndex && !m_holdTargetIDTimer.IsElapsed() )
	{
		return m_iOldIDEntIndex;
	}

	return 0;
}


void InitializeAddonModelFromWeapon( CWeaponCSBase *weapon, C_BreakableProp *addon )
{
	if ( !weapon )
	{
		return;
	}

	const CCSWeaponInfo& weaponInfo = weapon->GetCSWpnData();
	if ( weaponInfo.m_szAddonModel[0] == 0 )
	{
		addon->InitializeAsClientEntity( weaponInfo.szWorldModel, RENDER_GROUP_OPAQUE_ENTITY );
	}
	else
	{
		addon->InitializeAsClientEntity( weaponInfo.m_szAddonModel, RENDER_GROUP_OPAQUE_ENTITY );
	}
}

void C_CSPlayer::CreateAddonModel( int i )
{
	COMPILE_TIME_ASSERT( (sizeof( g_AddonInfo ) / sizeof( g_AddonInfo[0] )) == NUM_ADDON_BITS );

	// Create the model entity.
	CAddonInfo *pAddonInfo = &g_AddonInfo[i];

	int iAttachment = LookupAttachment( pAddonInfo->m_pAttachmentName );
	if ( iAttachment <= 0 )
		return;

	C_BreakableProp *pEnt = new C_BreakableProp;

	int addonType = (1<<i);
	if ( addonType == ADDON_PISTOL || addonType == ADDON_PRIMARY )
	{
		CCSWeaponInfo *weaponInfo = GetWeaponInfo( (CSWeaponID)((addonType == ADDON_PRIMARY) ? m_iPrimaryAddon.Get() : m_iSecondaryAddon.Get()) );
		if ( !weaponInfo )
		{
			Warning( "C_CSPlayer::CreateAddonModel: Unable to get weapon info.\n" );
			pEnt->Release();
			return;
		}
		if ( weaponInfo->m_szAddonModel[0] == 0 )
		{
			pEnt->InitializeAsClientEntity( weaponInfo->szWorldModel, RENDER_GROUP_OPAQUE_ENTITY );
		}
		else
		{
			pEnt->InitializeAsClientEntity( weaponInfo->m_szAddonModel, RENDER_GROUP_OPAQUE_ENTITY );
		}
	}
	else if( pAddonInfo->m_pModelName )
	{
		if ( addonType == ADDON_PISTOL2 && !(m_iAddonBits & ADDON_PISTOL ) )
		{
			pEnt->InitializeAsClientEntity( pAddonInfo->m_pHolsterName, RENDER_GROUP_OPAQUE_ENTITY );
		}
		else
		{
			pEnt->InitializeAsClientEntity( pAddonInfo->m_pModelName, RENDER_GROUP_OPAQUE_ENTITY );
		}
	}
	else
	{
		WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( pAddonInfo->m_pWeaponClassName );
		if ( hWpnInfo == GetInvalidWeaponInfoHandle() )
		{
			Assert( false );
			return;
		}

		CCSWeaponInfo *pWeaponInfo = dynamic_cast< CCSWeaponInfo* >( GetFileWeaponInfoFromHandle( hWpnInfo ) );
		if ( pWeaponInfo )
		{
			if ( pWeaponInfo->m_szAddonModel[0] == 0 )
				pEnt->InitializeAsClientEntity( pWeaponInfo->szWorldModel, RENDER_GROUP_OPAQUE_ENTITY );
			else
				pEnt->InitializeAsClientEntity( pWeaponInfo->m_szAddonModel, RENDER_GROUP_OPAQUE_ENTITY );
		}
		else
		{
			pEnt->Release();
			Warning( "C_CSPlayer::CreateAddonModel: Unable to get weapon info for %s.\n", pAddonInfo->m_pWeaponClassName );
			return;
		}
	}

	if ( Q_strcmp( pAddonInfo->m_pAttachmentName, "c4" ) )
	{
		// fade out all attached models except C4
		pEnt->SetFadeMinMax( 400, 500 );
	}

	// Create the addon.
	CAddonModel *pAddon = &m_AddonModels[m_AddonModels.AddToTail()];

	pAddon->m_hEnt = pEnt;
	pAddon->m_iAddon = i;
	pAddon->m_iAttachmentPoint = iAttachment;
	pEnt->SetParent( this, pAddon->m_iAttachmentPoint );
	pEnt->SetLocalOrigin( Vector( 0, 0, 0 ) );
	pEnt->SetLocalAngles( QAngle( 0, 0, 0 ) );
	if ( IsLocalPlayer() )
	{
		pEnt->SetSolid( SOLID_NONE );
		pEnt->RemoveEFlags( EFL_USE_PARTITION_WHEN_NOT_SOLID );
	}
}


void C_CSPlayer::UpdateAddonModels()
{
	int iCurAddonBits = m_iAddonBits;

	// Don't put addon models on the local player unless in third person.
	if ( IsLocalPlayer() && !C_BasePlayer::ShouldDrawLocalPlayer() )
		iCurAddonBits = 0;

	// If the local player is observing this entity in first-person mode, get rid of its addons.
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( pPlayer && pPlayer->GetObserverMode() == OBS_MODE_IN_EYE && pPlayer->GetObserverTarget() == this )
		iCurAddonBits = 0;

	// Any changes to the attachments we should have?
	if ( m_iLastAddonBits == iCurAddonBits &&
		m_iLastPrimaryAddon == m_iPrimaryAddon &&
		m_iLastSecondaryAddon == m_iSecondaryAddon )
	{
		return;
	}

	bool rebuildPistol2Addon = false;
	if ( m_iSecondaryAddon == WEAPON_ELITE && ((m_iLastAddonBits ^ iCurAddonBits) & ADDON_PISTOL) != 0 )
	{
		rebuildPistol2Addon = true;
	}
	m_iLastAddonBits = iCurAddonBits;
	m_iLastPrimaryAddon = m_iPrimaryAddon;
	m_iLastSecondaryAddon = m_iSecondaryAddon;

	// Get rid of any old models.
	int i,iNext;
	for ( i=m_AddonModels.Head(); i != m_AddonModels.InvalidIndex(); i = iNext )
	{
		iNext = m_AddonModels.Next( i );
		CAddonModel *pModel = &m_AddonModels[i];

		int addonBit = 1<<pModel->m_iAddon;
		if ( !( iCurAddonBits & addonBit ) || (rebuildPistol2Addon && addonBit == ADDON_PISTOL2) )
		{
			if ( pModel->m_hEnt.Get() )
				pModel->m_hEnt->Release();

			m_AddonModels.Remove( i );
		}
	}

	// Figure out which models we have now.
	int curModelBits = 0;
	FOR_EACH_LL( m_AddonModels, j )
	{
		curModelBits |= (1<<m_AddonModels[j].m_iAddon);
	}

	// Add any new models.
	for ( i=0; i < NUM_ADDON_BITS; i++ )
	{
		if ( (iCurAddonBits & (1<<i)) && !( curModelBits & (1<<i) ) )
		{
			// Ok, we're supposed to have this one.
			CreateAddonModel( i );
		}
	}

}


void C_CSPlayer::RemoveAddonModels()
{
	m_iAddonBits = 0;
	UpdateAddonModels();
}


void C_CSPlayer::NotifyShouldTransmit( ShouldTransmitState_t state )
{
	// Remove all addon models if we go out of the PVS.
	if ( state == SHOULDTRANSMIT_END )
	{
		RemoveAddonModels();

		if( m_pFlashlightBeam != NULL )
		{
			ReleaseFlashlight();
		}
	}

	BaseClass::NotifyShouldTransmit( state );
}


void C_CSPlayer::UpdateSoundEvents()
{
	int iNext;
	for ( int i=m_SoundEvents.Head(); i != m_SoundEvents.InvalidIndex(); i = iNext )
	{
		iNext = m_SoundEvents.Next( i );

		CCSSoundEvent *pEvent = &m_SoundEvents[i];
		if ( gpGlobals->curtime >= pEvent->m_flEventTime )
		{
			CLocalPlayerFilter filter;
			EmitSound( filter, GetSoundSourceIndex(), STRING( pEvent->m_SoundName ) );

			m_SoundEvents.Remove( i );
		}
	}
}

void C_CSPlayer::TurnOnStaggerCam()
{
	if ( !IsLocalPlayer() || !input )
		return;

	m_bStaggerCamHasSavedState = true;
	m_bStaggerCamBaseThirdPerson = input->CAM_IsThirdPerson();
	m_bStaggerCamBaseForcedThirdPerson = g_ThirdPersonManager.GetForcedThirdPerson();
	m_bStaggerCamBaseOverridingThirdPerson = g_ThirdPersonManager.IsOverridingThirdPerson();
	m_vecStaggerCamBaseDesiredOffset = g_ThirdPersonManager.GetDesiredCameraOffset();

	m_flStaggerCamCurrentDist = 0.0f;
	m_flStaggerCamCurrentDistUp = 0.0f;
	m_flStaggerCamTargetDist = CS_STAGGER_TAUNTCAM_DIST;
	m_flStaggerCamTargetDistUp = CS_STAGGER_TAUNTCAM_DIST_UP;

	m_StaggerCameraData.m_flPitch = 0.0f;
	m_StaggerCameraData.m_flYaw = 0.0f;
	m_StaggerCameraData.m_flDist = m_flStaggerCamTargetDist;
	m_StaggerCameraData.m_flLag = 1.0f;

	g_ThirdPersonManager.SetDesiredCameraOffset( vec3_origin );
	g_ThirdPersonManager.SetOverridingThirdPerson( true );

	input->CAM_ToThirdPerson();
	ThirdPersonSwitch( true );

	m_bStaggerCamInterpolating = true;
}

void C_CSPlayer::TurnOffStaggerCam()
{
	if ( !m_bStaggerCamHasSavedState )
		return;

	m_flStaggerCamTargetDist = 0.0f;
	m_flStaggerCamTargetDistUp = 0.0f;
	m_StaggerCameraData.m_flDist = 0.0f;

	g_ThirdPersonManager.SetOverridingThirdPerson( false );

	if ( m_bStaggerCamBaseForcedThirdPerson )
	{
		TurnOffStaggerCam_Finish();
	}
}

void C_CSPlayer::TurnOffStaggerCam_Finish()
{
	if ( !IsLocalPlayer() || !input )
		return;

	QAngle angles = vec3_angle;
	const Vector &vecOffset = g_ThirdPersonManager.GetCameraOffsetAngles();
	angles[PITCH] = vecOffset[PITCH];
	angles[YAW] = vecOffset[YAW];
	angles[DIST] = vecOffset[DIST];

	input->CAM_SetCameraThirdData( NULL, angles );

	g_ThirdPersonManager.SetDesiredCameraOffset( m_vecStaggerCamBaseDesiredOffset );
	g_ThirdPersonManager.SetOverridingThirdPerson( m_bStaggerCamBaseOverridingThirdPerson );
	g_ThirdPersonManager.SetForcedThirdPerson( m_bStaggerCamBaseForcedThirdPerson );

	if ( !m_bStaggerCamBaseThirdPerson )
	{
		input->CAM_ToFirstPerson();
		ThirdPersonSwitch( false );
		input->CAM_SetCameraThirdData( NULL, vec3_angle );
	}

	m_bStaggerCamHasSavedState = false;
	m_bStaggerCamInterpolating = false;
	m_flStaggerCamCurrentDist = 0.0f;
	m_flStaggerCamTargetDist = 0.0f;
	m_flStaggerCamCurrentDistUp = 0.0f;
	m_flStaggerCamTargetDistUp = 0.0f;
}

void C_CSPlayer::StaggerCamInterpolation()
{
	if ( !IsLocalPlayer() || !input || !m_bStaggerCamInterpolating )
		return;

	m_flStaggerCamCurrentDist = Approach( m_flStaggerCamTargetDist, m_flStaggerCamCurrentDist, gpGlobals->frametime * CS_STAGGER_TAUNTCAM_SPEED );
	m_flStaggerCamCurrentDistUp = Approach( m_flStaggerCamTargetDistUp, m_flStaggerCamCurrentDistUp, gpGlobals->frametime * CS_STAGGER_TAUNTCAM_SPEED );

	const Vector &vecCamOffset = g_ThirdPersonManager.GetCameraOffsetAngles();

	Vector vecOrigin = GetLocalOrigin();
	vecOrigin += GetViewOffset();

	Vector vecForward, vecUp;
	AngleVectors( QAngle( vecCamOffset[PITCH], vecCamOffset[YAW], 0.0f ), &vecForward, NULL, &vecUp );

	trace_t trace;
	UTIL_TraceHull(
		vecOrigin,
		vecOrigin - ( vecForward * m_flStaggerCamCurrentDist ) + ( vecUp * m_flStaggerCamCurrentDistUp ),
		Vector( -9.0f, -9.0f, -9.0f ),
		Vector( 9.0f, 9.0f, 9.0f ),
		MASK_SOLID_BRUSHONLY,
		this,
		COLLISION_GROUP_DEBRIS,
		&trace );

	if ( trace.fraction < 1.0f )
	{
		m_flStaggerCamCurrentDist *= trace.fraction;
	}

	QAngle angCameraOffset( vecCamOffset[PITCH], vecCamOffset[YAW], m_flStaggerCamCurrentDist );
	input->CAM_SetCameraThirdData( &m_StaggerCameraData, angCameraOffset );

	g_ThirdPersonManager.SetDesiredCameraOffset( Vector( m_flStaggerCamCurrentDist, 0.0f, m_flStaggerCamCurrentDistUp ) );

	if ( m_flStaggerCamCurrentDist == m_flStaggerCamTargetDist &&
		 m_flStaggerCamCurrentDistUp == m_flStaggerCamTargetDistUp &&
		 m_flStaggerCamTargetDist == 0.0f )
	{
		TurnOffStaggerCam_Finish();
	}
}

void C_CSPlayer::UpdateStaggerThirdPersonCamera()
{
	if ( !IsLocalPlayer() || !input )
		return;

	const bool wantsStaggerCam =
		IsAlive() &&
		GetObserverMode() == OBS_MODE_NONE &&
		( m_nDamageStaggerDir != PLAYER_STAGGER_DIR_NONE );

	if ( wantsStaggerCam )
	{
		if ( !m_bStaggerCamHasSavedState )
		{
			TurnOnStaggerCam();
		}
	}
	else if ( m_bStaggerCamHasSavedState )
	{
		TurnOffStaggerCam();
	}

	StaggerCamInterpolation();
}

void C_CSPlayer::UpdatePounceThirdPersonCamera()
{
	if ( !IsLocalPlayer() || !input )
		return;

	// Don't interfere with spectator / deathcam / other observer views.
	if ( GetObserverMode() != OBS_MODE_NONE )
	{
		if ( m_bPounceCamHasSavedState )
		{
			g_ThirdPersonManager.SetDesiredCameraOffset( m_vecPounceCamBaseDesiredOffset );
			g_ThirdPersonManager.SetOverridingThirdPerson( m_bPounceCamBaseOverridingThirdPerson );
			g_ThirdPersonManager.SetForcedThirdPerson( m_bPounceCamBaseForcedThirdPerson );

			if ( !m_bPounceCamBaseThirdPerson && input->CAM_IsThirdPerson() )
			{
				input->CAM_ToFirstPerson();
				ThirdPersonSwitch( false );
			}

			m_bPounceCamHasSavedState = false;
			m_flPounceCamBlend = 0.0f;
		}
		return;
	}

	const bool wantsPounceCam = ( m_pounceVictim.Get() != NULL ) || ( m_pounceAttacker.Get() != NULL );
	const bool wantsChargerCam = ( GetTeamNumber() == TEAM_INFECTED && GetZombieClass() == 6 && m_nChargerAction != CHARGER_ACTION_NONE );
	const bool wantsTankThrowCam = ( GetTeamNumber() == TEAM_INFECTED && GetZombieClass() == 8 && m_nTankAction == TANK_ACTION_ROCK_THROW );
	const bool wantsReviveCam = (GetTeamNumber() == TEAM_SURVIVOR && !m_bIncapacitated && m_hReviveTarget.Get() != NULL);
	const bool wantsHealCam = (GetTeamNumber() == TEAM_SURVIVOR && (m_hFirstAidKitTarget.Get() != NULL || m_bUsingFirstAidKitOnSelf));
	const bool wantsAbilityCam = wantsPounceCam || wantsChargerCam || wantsTankThrowCam || wantsReviveCam || wantsHealCam;

	if ( wantsAbilityCam && !m_bPounceCamHasSavedState )
	{
		m_bPounceCamHasSavedState = true;
		m_bPounceCamBaseThirdPerson = input->CAM_IsThirdPerson();
		m_bPounceCamBaseForcedThirdPerson = g_ThirdPersonManager.GetForcedThirdPerson();
		m_bPounceCamBaseOverridingThirdPerson = g_ThirdPersonManager.IsOverridingThirdPerson();
		m_vecPounceCamBaseDesiredOffset = g_ThirdPersonManager.GetDesiredCameraOffset();
		m_flPounceCamBlend = 0.0f;
	}

	if ( !m_bPounceCamHasSavedState )
		return;

	const Vector pounceOffset( 120.0f, 0.0f, 0.0f );

	const float targetBlend = wantsAbilityCam ? 1.0f : 0.0f;
	const float blendTime = ( targetBlend > m_flPounceCamBlend ) ? 0.25f : 0.20f;
	const float step = ( blendTime > 0.0f ) ? ( gpGlobals->frametime / blendTime ) : 1.0f;

	m_flPounceCamBlend = Approach( targetBlend, m_flPounceCamBlend, step );
	m_flPounceCamBlend = clamp( m_flPounceCamBlend, 0.0f, 1.0f );

	float smooth = m_flPounceCamBlend;
	smooth = smooth * smooth * ( 3.0f - 2.0f * smooth );

	Vector desiredOffset;
	VectorLerp( m_vecPounceCamBaseDesiredOffset, pounceOffset, smooth, desiredOffset );
	g_ThirdPersonManager.SetDesiredCameraOffset( desiredOffset );

	const bool shouldBeThirdPerson = m_bPounceCamBaseThirdPerson || ( m_flPounceCamBlend > 0.0f );
	if ( shouldBeThirdPerson )
	{
		// This is a gameplay camera mode (not sv_cheats-gated).
		g_ThirdPersonManager.SetForcedThirdPerson( true );

		if ( !input->CAM_IsThirdPerson() )
		{
			input->CAM_ToThirdPerson();
			ThirdPersonSwitch( true );
		}
	}

	if ( !wantsAbilityCam && m_flPounceCamBlend <= 0.0f )
	{
		// Restore baseline camera state after we finish zooming back in.
		g_ThirdPersonManager.SetDesiredCameraOffset( m_vecPounceCamBaseDesiredOffset );
		g_ThirdPersonManager.SetOverridingThirdPerson( m_bPounceCamBaseOverridingThirdPerson );
		g_ThirdPersonManager.SetForcedThirdPerson( m_bPounceCamBaseForcedThirdPerson );

		if ( !m_bPounceCamBaseThirdPerson && input->CAM_IsThirdPerson() )
		{
			input->CAM_ToFirstPerson();
			ThirdPersonSwitch( false );
		}

		m_bPounceCamHasSavedState = false;
		m_flPounceCamBlend = 0.0f;
	}
}

void C_CSPlayer::UpdatePounceMusic()
{
	if ( !IsLocalPlayer() )
		return;

	// Don't play while spectating / deathcam / observer views.
	if ( GetObserverMode() != OBS_MODE_NONE )
	{
		if ( m_bLocalPounceMusicPlaying )
		{
			C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Event.HunterPounce" );
			m_bLocalPounceMusicPlaying = false;
		}
		return;
	}

	const bool beingPounced = ( m_pounceAttacker.Get() != NULL );
	if ( beingPounced )
	{
		if ( !m_bLocalPounceMusicPlaying )
		{
			CLocalPlayerFilter filter;
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "Event.HunterPounce" );
			m_bLocalPounceMusicPlaying = true;
		}
	}
	else if ( m_bLocalPounceMusicPlaying )
	{
		C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Event.HunterPounce" );
		m_bLocalPounceMusicPlaying = false;
	}
}

void C_CSPlayer::UpdateInfectedColorCorrection()
{
	if ( !IsLocalPlayer() )
		return;

	if ( m_hInfectedColorCorrection == INVALID_CLIENT_CCHANDLE && !m_bTriedCreateInfectedColorCorrection )
	{
		m_bTriedCreateInfectedColorCorrection = true;
		m_hInfectedColorCorrection = g_pColorCorrectionMgr->AddColorCorrection( "infected_cc", "materials/correction/infected.raw" );
	}

	if ( m_hGhostColorCorrection == INVALID_CLIENT_CCHANDLE && !m_bTriedCreateGhostColorCorrection )
	{
		m_bTriedCreateGhostColorCorrection = true;
		m_hGhostColorCorrection = g_pColorCorrectionMgr->AddColorCorrection( "ghost_cc", "materials/correction/ghost.raw" );
	}

	bool shouldEnable = ( GetTeamNumber() == TEAM_INFECTED );
	if ( GetObserverMode() != OBS_MODE_NONE )
	{
		shouldEnable = false;
	}

	const bool enableGhost = shouldEnable && IsGhost();
	const bool enableInfected = shouldEnable && !IsGhost();

	if ( m_hInfectedColorCorrection != INVALID_CLIENT_CCHANDLE )
	{
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_hInfectedColorCorrection, enableInfected ? 1.0f : 0.0f );
	}

	if ( m_hGhostColorCorrection != INVALID_CLIENT_CCHANDLE )
	{
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_hGhostColorCorrection, enableGhost ? 1.0f : 0.0f );
	}
}

void C_CSPlayer::UpdateIncapBlackAndWhiteEffects()
{
	if ( !IsLocalPlayer() )
		return;

	// Don't play while spectating / deathcam / observer views.
	const bool inObserver = ( GetObserverMode() != OBS_MODE_NONE );

	if ( m_hBlackAndWhiteColorCorrection == INVALID_CLIENT_CCHANDLE && !m_bTriedCreateBlackAndWhiteColorCorrection )
	{
		m_bTriedCreateBlackAndWhiteColorCorrection = true;
		m_hBlackAndWhiteColorCorrection = g_pColorCorrectionMgr->AddColorCorrection( "bw_cc", "materials/correction/blackandwhite.raw" );
	}

	const bool shouldEnable = ( !inObserver && IsAlive() && ( GetTeamNumber() == TEAM_SURVIVOR ) && m_bIncapBlackAndWhite );

	if ( m_hBlackAndWhiteColorCorrection != INVALID_CLIENT_CCHANDLE )
	{
		g_pColorCorrectionMgr->SetColorCorrectionWeight( m_hBlackAndWhiteColorCorrection, shouldEnable ? 1.0f : 0.0f );
	}

	if ( shouldEnable )
	{
		if ( !m_bLocalHeartbeatPlaying )
		{
			CLocalPlayerFilter filter;
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "Player.Heartbeat" );
			m_bLocalHeartbeatPlaying = true;
		}
	}
	else if ( m_bLocalHeartbeatPlaying )
	{
		C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Player.Heartbeat" );
		m_bLocalHeartbeatPlaying = false;
	}
}

//-----------------------------------------------------------------------------
void C_CSPlayer::UpdateMinModels( void )
{
	int modelIndex = m_nModelIndex;

	// cl_minmodels convar dependent on sv_allowminmodels convar

	if ( !IsVIP() && sv_allowminmodels.GetBool() && cl_minmodels.GetBool() && !IsLocalPlayer() )
	{
		if ( GetTeamNumber() == TEAM_CT )
		{
			int index = cl_min_ct.GetInt() - 1;
			if ( index >= 0 && index < CTPlayerModels.Count() )
			{
				modelIndex = modelinfo->GetModelIndex( CTPlayerModels[index] );
			}
		}
		else if ( GetTeamNumber() == TEAM_TERRORIST )
		{
			int index = cl_min_t.GetInt() - 1;
			if ( index >= 0 && index < TerroristPlayerModels.Count() )
			{
				modelIndex = modelinfo->GetModelIndex( TerroristPlayerModels[index] );
			}
		}
	}

	SetModelByIndex( modelIndex );
}

// NVNT gate for spectating.
static bool inSpectating_Haptics = false;

#ifdef GLOWS_ENABLE
static Vector CS_GetSurvivorGlowColor( int nHealth )
{
	if ( nHealth >= 40 )
		return Vector( 0.0f, 1.0f, 0.0f ); // bright green

	if ( nHealth >= 20 )
		return Vector( 1.0f, 1.0f, 0.0f ); // yellow
	
	return Vector( 1.0f, 0.0f, 0.0f ); // red
}

static void CS_UpdatePlayerGlow( C_CSPlayer *pPlayer )
{
	if ( !pPlayer )
		return;

	if ( pPlayer->IsDormant() || !pPlayer->IsAlive() || pPlayer->IsLocalPlayer() )
	{
		if ( pPlayer->IsClientSideGlowEnabled() )
			pPlayer->SetClientSideGlowEnabled( false );
		return;
	}

	bool bShouldGlow = false;
	Vector vGlowColor( 1.0f, 1.0f, 1.0f );

	const int nTeam = pPlayer->GetTeamNumber();
	if ( pPlayer->m_bIsIT )
	{
		bShouldGlow = true;
		vGlowColor = Vector( 0.75f, 0.1f, 1.0f );
	}
	else if ( nTeam == TEAM_SURVIVOR )
	{
		bShouldGlow = true;

		if (pPlayer->m_bIncapacitated)
		{
			vGlowColor = Vector(1.0f, 0.0f, 0.0f);
		}
		else {
			vGlowColor = CS_GetSurvivorGlowColor(pPlayer->GetHealth());
		}
	}
	else if ( nTeam == TEAM_INFECTED )
	{
		C_CSPlayer *pLocal = GetLocalOrInEyeCSPlayer();
		if ( pLocal && pLocal->GetTeamNumber() == TEAM_INFECTED )
		{
			bShouldGlow = true;
			if (pLocal->IsGhost()) {
				vGlowColor = Vector(1.0f, 1.0f, 1.0f);
			}
			else {
				vGlowColor = Vector(1.0f, 0.0f, 0.0f);
			}
		}
	}

	if ( bShouldGlow != pPlayer->IsClientSideGlowEnabled() )
	{
		pPlayer->SetClientSideGlowEnabled( bShouldGlow );
	}

	if ( bShouldGlow )
	{
		CGlowObject *pGlow = pPlayer->GetGlowObject();
		if ( pGlow )
		{
			pGlow->SetColor( vGlowColor );
			pGlow->SetAlpha( 1.0f );
			pGlow->SetRenderFlags( true, false );
		}
	}
}
#endif // GLOWS_ENABLE
//-----------------------------------------------------------------------------
void C_CSPlayer::ClientThink()
{
	BaseClass::ClientThink();

	UpdateSoundEvents();
	UpdateInfectedAmbientParticles();

	UpdateAddonModels();

	UpdateIDTarget();

	if ( gpGlobals->curtime >= m_fNextThinkPushAway )
	{
		PerformObstaclePushaway( this );
		m_fNextThinkPushAway =  gpGlobals->curtime + PUSHAWAY_THINK_INTERVAL;
	}

	// NVNT - check for spectating forces
	if ( IsLocalPlayer() )
	{
		if ( GetTeamNumber() == TEAM_SPECTATOR || !this->IsAlive() || GetLocalOrInEyeCSPlayer() != this )
		{
			if (!inSpectating_Haptics)
			{
				if ( haptics )
					haptics->SetNavigationClass("spectate");

				inSpectating_Haptics = true;
			}
		}
		else
		{
			if (inSpectating_Haptics)
			{
				if ( haptics )
					haptics->SetNavigationClass("on_foot");

				inSpectating_Haptics = false;
			}
		}
	}

#ifdef GLOWS_ENABLE
	CS_UpdatePlayerGlow( this );
#endif
}


void C_CSPlayer::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
		m_flFootstepEventSuppressUntil = gpGlobals ? ( gpGlobals->curtime + 0.22f ) : 0.22f;

		if ( IsLocalPlayer() )
		{
			if ( CSGameRules() && CSGameRules()->IsBlackMarket() )
			{
				CSGameRules()->m_pPrices = NULL;
				CSGameRules()->m_StringTableBlackMarket = NULL;
				CSGameRules()->GetBlackMarketPriceList();

				CSGameRules()->SetBlackMarketPrices( false );
			}
		}
	}

	UpdateVisibility();
	UpdateInfectedAmbientParticles();
}

void C_CSPlayer::StopInfectedAmbientParticles()
{
	if ( m_hSmokerSporeTrail )
	{
		m_hSmokerSporeTrail->StopEmission();
		m_hSmokerSporeTrail = NULL;
	}

	if ( m_hSmokerSporeTrailCluster )
	{
		m_hSmokerSporeTrailCluster->StopEmission();
		m_hSmokerSporeTrailCluster = NULL;
	}

	if ( m_hSpitterDrool )
	{
		m_hSpitterDrool->StopEmission();
		m_hSpitterDrool = NULL;
	}

	if ( m_hSpitterSlimeTrail )
	{
		m_hSpitterSlimeTrail->StopEmission();
		m_hSpitterSlimeTrail = NULL;
	}
}

void C_CSPlayer::UpdateInfectedAmbientParticles()
{
	const int team = GetTeamNumber();
	const bool alive = IsAlive() && !IsDormant();
	const int zombieClass = GetZombieClass();

	const bool shouldHaveInfectedParticles = alive && ( team == TEAM_INFECTED ) && ( zombieClass == 1 || zombieClass == 4 );
	const bool needsRefresh =
		( m_nLastInfectedParticleZombieClass != zombieClass ) ||
		( m_nLastInfectedParticleTeam != team ) ||
		( m_bLastInfectedParticleAlive != alive );

	if ( !shouldHaveInfectedParticles )
	{
		if ( needsRefresh )
		{
			StopInfectedAmbientParticles();
		}

		m_nLastInfectedParticleZombieClass = zombieClass;
		m_nLastInfectedParticleTeam = team;
		m_bLastInfectedParticleAlive = alive;
		return;
	}

	if ( !needsRefresh )
	{
		return;
	}

	StopInfectedAmbientParticles();

	int attach = LookupAttachment( "mouth" );
	if ( attach <= 0 )
		attach = LookupAttachment( "head" );
	if ( attach <= 0 )
		attach = LookupAttachment( "eyes" );

	if ( zombieClass == 1 )
	{
		PrecacheParticleSystem( "smoker_spore_trail" );
		PrecacheParticleSystem( "smoker_spore_trail_spores_cluster" );

		if ( attach > 0 )
		{
			m_hSmokerSporeTrail = ParticleProp()->Create( "smoker_spore_trail", PATTACH_POINT_FOLLOW, attach );
			m_hSmokerSporeTrailCluster = ParticleProp()->Create( "smoker_spore_trail_spores_cluster", PATTACH_POINT_FOLLOW, attach );
		}
		else
		{
			m_hSmokerSporeTrail = ParticleProp()->Create( "smoker_spore_trail", PATTACH_ABSORIGIN_FOLLOW );
			m_hSmokerSporeTrailCluster = ParticleProp()->Create( "smoker_spore_trail_spores_cluster", PATTACH_ABSORIGIN_FOLLOW );
		}
	}
	else if ( zombieClass == 4 )
	{
		PrecacheParticleSystem( "spitter_drool" );
		PrecacheParticleSystem( "spitter_slime_trail" );

		if ( attach > 0 )
		{
			m_hSpitterDrool = ParticleProp()->Create( "spitter_drool", PATTACH_POINT_FOLLOW, attach );
			m_hSpitterSlimeTrail = ParticleProp()->Create( "spitter_slime_trail", PATTACH_POINT_FOLLOW, attach );
		}
		else
		{
			m_hSpitterDrool = ParticleProp()->Create( "spitter_drool", PATTACH_ABSORIGIN_FOLLOW );
			m_hSpitterSlimeTrail = ParticleProp()->Create( "spitter_slime_trail", PATTACH_ABSORIGIN_FOLLOW );
		}
	}

	m_nLastInfectedParticleZombieClass = zombieClass;
	m_nLastInfectedParticleTeam = team;
	m_bLastInfectedParticleAlive = alive;
}


void C_CSPlayer::ValidateModelIndex( void )
{
	UpdateMinModels();
}


void C_CSPlayer::PostDataUpdate( DataUpdateType_t updateType )
{
	// C_BaseEntity assumes we're networking the entity's angles, so pretend that it
	// networked the same value we already have.
	SetNetworkAngles( GetLocalAngles() );

	BaseClass::PostDataUpdate( updateType );
}

//-----------------------------------------------------------------------------
// Purpose:
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C_CSPlayer::Interpolate( float currentTime )
{
	if ( !BaseClass::Interpolate( currentTime ) )
		return false;

	if ( CSGameRules()->IsFreezePeriod() )
	{
		// don't interpolate players position during freeze period
		SetAbsOrigin( GetNetworkOrigin() );
	}

	return true;
}

int	C_CSPlayer::GetMaxHealth() const
{
	return 100;
}

//-----------------------------------------------------------------------------
// Purpose: Return the local player, or the player being spectated in-eye
//-----------------------------------------------------------------------------
C_CSPlayer* GetLocalOrInEyeCSPlayer( void )
{
	C_CSPlayer *player = C_CSPlayer::GetLocalCSPlayer();

	if( player && player->GetObserverMode() == OBS_MODE_IN_EYE )
	{
		C_BaseEntity *target = player->GetObserverTarget();

		if( target && target->IsPlayer() )
		{
			return ToCSPlayer( target );
		}
	}
	return player;
}

#define MAX_FLASHBANG_OPACITY 75.0f

//-----------------------------------------------------------------------------
// Purpose: Update this client's targetid entity
//-----------------------------------------------------------------------------
void C_CSPlayer::UpdateIDTarget()
{
	if ( !IsLocalPlayer() )
		return;

	// Clear old target and find a new one
	m_iIDEntIndex = 0;

	// don't show IDs if mp_playerid == 2
	if ( mp_playerid.GetInt() == 2 )
		return;

	// don't show IDs if mp_fadetoblack is on
	if ( mp_fadetoblack.GetBool() && !IsAlive() )
		return;

	// don't show IDs in chase spec mode
	if ( GetObserverMode() == OBS_MODE_CHASE ||
		 GetObserverMode() == OBS_MODE_DEATHCAM )
		 return;

	//Check how much of a screen fade we have.
	//if it's more than 75 then we can't see what's going on so we don't display the id.
	byte color[4];
	bool blend;
	vieweffects->GetFadeParams( &color[0], &color[1], &color[2], &color[3], &blend );

	if ( color[3] > MAX_FLASHBANG_OPACITY && ( IsAlive() || GetObserverMode() == OBS_MODE_IN_EYE ) )
		 return;

	trace_t tr;
	Vector vecStart, vecEnd;
	VectorMA( MainViewOrigin(), 2500, MainViewForward(), vecEnd );
	VectorMA( MainViewOrigin(), 10,   MainViewForward(), vecStart );
	UTIL_TraceLine( vecStart, vecEnd, MASK_VISIBLE_AND_NPCS, GetLocalOrInEyeCSPlayer(), COLLISION_GROUP_NONE, &tr );
	if ( !tr.startsolid && !tr.DidHitNonWorldEntity() )
	{
		CTraceFilterSimple filter( GetLocalOrInEyeCSPlayer(), COLLISION_GROUP_NONE );

		// Check for player hitboxes extending outside their collision bounds
		const float rayExtension = 40.0f;
		UTIL_ClipTraceToPlayers(vecStart, vecEnd + MainViewForward() * rayExtension, MASK_SOLID|CONTENTS_HITBOX, &filter, &tr );
	}

	if ( !tr.startsolid && tr.DidHitNonWorldEntity() )
	{
		C_BaseEntity *pEntity = tr.m_pEnt;

		if ( pEntity && (pEntity != this) )
		{
			if ( mp_playerid.GetInt() == 1 ) // only show team names
			{
				if ( pEntity->GetTeamNumber() != GetTeamNumber() )
				{
					return;
				}
			}

			//Adrian: If there's a smoke cloud in my way, don't display the name
			//We check this AFTER we found a player, just so we don't go thru this for nothing.
			for ( int i = 0; i < m_SmokeGrenades.Count(); i++ )
			{
				C_BaseParticleEntity *pSmokeGrenade = (C_BaseParticleEntity*)m_SmokeGrenades.Element( i );

				if ( pSmokeGrenade )
				{
					float flHit1, flHit2;

					float flRadius = ( SMOKEGRENADE_PARTICLERADIUS * NUM_PARTICLES_PER_DIMENSION + 1 ) * 0.5f;

					Vector vPos = pSmokeGrenade->GetAbsOrigin();

					/*debugoverlay->AddBoxOverlay( pSmokeGrenade->GetAbsOrigin(), Vector( flRadius, flRadius, flRadius ),
					 Vector( -flRadius, -flRadius, -flRadius ), QAngle( 0, 0, 0 ), 255, 0, 0, 255, 0.2 );*/

					if ( IntersectInfiniteRayWithSphere( MainViewOrigin(), MainViewForward(), vPos, flRadius, &flHit1, &flHit2 ) )
					{
						 return;
					}
				}
			}

			if ( !GetIDTarget() && ( !m_iOldIDEntIndex || m_holdTargetIDTimer.IsElapsed() ) )
			{
				// track when we first mouse over the target
				m_delayTargetIDTimer.Start( mp_playerid_delay.GetFloat() );
			}
			m_iIDEntIndex = pEntity->entindex();

			m_iOldIDEntIndex = m_iIDEntIndex;
			m_holdTargetIDTimer.Start( mp_playerid_hold.GetFloat() );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Input handling
//-----------------------------------------------------------------------------
bool C_CSPlayer::CreateMove( float flInputSampleTime, CUserCmd *pCmd )
{
	// Bleh... we will wind up needing to access bones for attachments in here.
	C_BaseAnimating::AutoAllowBoneAccess boneaccess( true, true );

	const bool result = BaseClass::CreateMove( flInputSampleTime, pCmd );

	if ( IsLocalPlayer() && m_nDamageStaggerDir != PLAYER_STAGGER_DIR_NONE )
	{
		pCmd->forwardmove = 0.0f;
		pCmd->sidemove = 0.0f;
		pCmd->upmove = 0.0f;
		pCmd->buttons &= ~( IN_ATTACK | IN_ATTACK2 | IN_JUMP );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose: Flash this entity on the radar
//-----------------------------------------------------------------------------
bool C_CSPlayer::IsInHostageRescueZone()
{
	return 	m_bInHostageRescueZone;
}

CWeaponCSBase* C_CSPlayer::GetActiveCSWeapon() const
{
	return dynamic_cast< CWeaponCSBase* >( GetActiveWeapon() );
}

CWeaponCSBase* C_CSPlayer::GetCSWeapon( CSWeaponID id ) const
{
	for (int i=0;i<MAX_WEAPONS;i++)
	{
		CBaseCombatWeapon *weapon = GetWeapon( i );
		if ( weapon )
		{
			CWeaponCSBase *csWeapon = dynamic_cast< CWeaponCSBase * >( weapon );
			if ( csWeapon )
			{
				if ( id == csWeapon->GetWeaponID() )
				{
					return csWeapon;
				}
			}
		}
	}

	return NULL;
}

//REMOVEME
/*
void C_CSPlayer::SetFireAnimation( PLAYER_ANIM playerAnim )
{
	Activity idealActivity = ACT_WALK;

	// Figure out stuff about the current state.
	float speed = GetAbsVelocity().Length2D();
	bool isMoving = ( speed != 0.0f ) ? true : false;
	bool isDucked = ( GetFlags() & FL_DUCKING ) ? true : false;
	bool isStillJumping = false; //!( GetFlags() & FL_ONGROUND );
	bool isRunning = false;

	if ( speed > ARBITRARY_RUN_SPEED )
	{
		isRunning = true;
	}

	// Now figure out what to do based on the current state and the new state.
	switch ( playerAnim )
	{
	default:
	case PLAYER_RELOAD:
	case PLAYER_ATTACK1:
	case PLAYER_IDLE:
	case PLAYER_WALK:
		// Are we still jumping?
		// If so, keep playing the jump animation.
		if ( !isStillJumping )
		{
			idealActivity = ACT_WALK;

			if ( isDucked )
			{
				idealActivity = !isMoving ? ACT_CROUCHIDLE : ACT_RUN_CROUCH;
			}
			else
			{
				if ( isRunning )
				{
					idealActivity = ACT_RUN;
				}
				else
				{
					idealActivity = isMoving ? ACT_WALK : ACT_IDLE;
				}
			}

			// Allow body yaw to override for standing and turning in place
			idealActivity = m_PlayerAnimState.BodyYawTranslateActivity( idealActivity );
		}
		break;

	case PLAYER_JUMP:
		idealActivity = ACT_HOP;
		break;

	case PLAYER_DIE:
		// Uses Ragdoll now???
		idealActivity = ACT_DIESIMPLE;
		break;

	// FIXME:  Use overlays for reload, start/leave aiming, attacking
	case PLAYER_START_AIMING:
	case PLAYER_LEAVE_AIMING:
		idealActivity = ACT_WALK;
		break;
	}

	CWeaponCSBase *pWeapon = GetActiveCSWeapon();

	if ( pWeapon )
	{
		Activity aWeaponActivity = idealActivity;

		if ( playerAnim == PLAYER_ATTACK1 )
		{
			switch ( idealActivity )
			{
				case ACT_WALK:
				default:
					aWeaponActivity = ACT_PLAYER_WALK_FIRE;
					break;
				case ACT_RUN:
					aWeaponActivity = ACT_PLAYER_RUN_FIRE;
					break;
				case ACT_IDLE:
					aWeaponActivity = ACT_PLAYER_IDLE_FIRE;
					break;
				case ACT_CROUCHIDLE:
					aWeaponActivity = ACT_PLAYER_CROUCH_FIRE;
					break;
				case ACT_RUN_CROUCH:
					aWeaponActivity = ACT_PLAYER_CROUCH_WALK_FIRE;
					break;
			}
		}

		m_PlayerAnimState.SetWeaponLayerSequence( pWeapon->GetCSWpnData().m_szAnimExtension, aWeaponActivity );
	}
}
*/

ShadowType_t C_CSPlayer::ShadowCastType( void )
{
	if ( !IsVisible() )
		 return SHADOWS_NONE;

	// When drawing the local player's world model in first-person, don't cast shadows.
	if ( IsLocalPlayer() && InFirstPersonView() )
		return SHADOWS_NONE;

	return SHADOWS_RENDER_TO_TEXTURE_DYNAMIC;
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether or not we can switch to the given weapon.
// Input  : pWeapon -
//-----------------------------------------------------------------------------
bool C_CSPlayer::Weapon_CanSwitchTo( CBaseCombatWeapon *pWeapon )
{
	if ( !pWeapon->CanDeploy() )
		return false;

	if ( GetActiveWeapon() )
	{
		if ( !GetActiveWeapon()->CanHolster() )
			return false;
	}

	return true;
}


void C_CSPlayer::UpdateClientSideAnimation()
{
	// Update the animation data. It does the local check here so this works when using
	// a third-person camera (and we don't have valid player angles).
	if (this == C_CSPlayer::GetLocalCSPlayer())
	{
		if ( m_nDamageStaggerDir != PLAYER_STAGGER_DIR_NONE )
		{
			m_PlayerAnimState->Update( m_angEyeAngles[YAW], m_angEyeAngles[PITCH] );
		}
		else
		{
			m_PlayerAnimState->Update( EyeAngles()[YAW], m_angEyeAngles[PITCH] );
		}
	}
	else
	{
		m_PlayerAnimState->Update(m_angEyeAngles[YAW], m_angEyeAngles[PITCH]);
	}

	BaseClass::UpdateClientSideAnimation();
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : collisionGroup - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C_CSPlayer::ShouldCollide(int collisionGroup, int contentsMask) const
{
	// Common infected NPCs should not collide with infected-team players (special infected / CT).
	if (GetTeamNumber() == TEAM_INFECTED &&
		(collisionGroup == COLLISION_GROUP_NPC || collisionGroup == COLLISION_GROUP_NPC_ACTOR))
	{
		return false;
	}


	if (collisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT)
	{
		switch (GetTeamNumber())
		{
		case TEAM_SURVIVOR:
			if (!(contentsMask & CONTENTS_TEAM2))
				return false;
			break;

		case TEAM_INFECTED:
			if (!(contentsMask & CONTENTS_TEAM1))
				return false;
			break;
		}
	}
	return BaseClass::ShouldCollide(collisionGroup, contentsMask);
}

float g_flMuzzleFlashScale=1;

void C_CSPlayer::ProcessMuzzleFlashEvent()
{
	CBasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer();

	// Reenable when the weapons have muzzle flash attachments in the right spot.
	if ( this == pLocalPlayer )
		return; // don't show own world muzzle flashs in for localplayer

	if ( pLocalPlayer && pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE )
	{
		// also don't show in 1st person spec mode
		if ( pLocalPlayer->GetObserverTarget() == this )
			return;
	}

	CWeaponCSBase *pWeapon = GetActiveCSWeapon();

	if ( !pWeapon )
		return;

	bool hasMuzzleFlash = (pWeapon->GetMuzzleFlashStyle() != CS_MUZZLEFLASH_NONE);

	Vector vector;
	QAngle angles;

	int iAttachment = LookupAttachment( "muzzle_flash" );

	if ( iAttachment >= 0 )
	{
		bool bFoundAttachment = GetAttachment( iAttachment, vector, angles );
		// If we have an attachment, then stick a light on it.
		if ( bFoundAttachment )
		{
			if ( hasMuzzleFlash )
			{
				dlight_t *el = effects->CL_AllocDlight( LIGHT_INDEX_MUZZLEFLASH + index );
				el->origin = vector;
				el->radius = 70;
				el->decay = el->radius / 0.05f;
				el->die = gpGlobals->curtime + 0.05f;
				el->color.r = 255;
				el->color.g = 192;
				el->color.b = 64;
				el->color.exponent = 5;
			}

			int shellType = GetShellForAmmoType( pWeapon->GetCSWpnData().szAmmo1 );

			QAngle playerAngle = EyeAngles();
			Vector vForward, vRight, vUp;

			AngleVectors( playerAngle, &vForward, &vRight, &vUp );

			QAngle angVelocity;
			Vector vVel = vRight * 100 + vUp * 20;
			VectorAngles( vVel, angVelocity );

			if ( pWeapon->GetMaxClip1() > 0 )
			{
				tempents->CSEjectBrass( vector, angVelocity, 120, shellType, this  );
			}
		}
	}

	if ( hasMuzzleFlash )
	{
		iAttachment = pWeapon->GetMuzzleAttachment();

		if ( iAttachment > 0 )
		{
			float flScale = pWeapon->GetCSWpnData().m_flMuzzleScale;
			flScale *= 0.75;
			FX_MuzzleEffectAttached( flScale, pWeapon->GetRefEHandle(), iAttachment, NULL, false );

		}
	}
}

const QAngle& C_CSPlayer::EyeAngles()
{
	const bool bServerOwnedViewAngles =
		( m_pounceVictim.Get() != NULL ) ||
		( m_pounceAttacker.Get() != NULL );

	// During locked gameplay states the server owns the player's eye angles
	// (for animation/pose), but the local camera is still free to look around in third person.
	if ( IsLocalPlayer() && !g_nKillCamMode && !bServerOwnedViewAngles )
	{
		return BaseClass::EyeAngles();
	}
	else
	{
		return m_angEyeAngles;
	}
}

bool C_CSPlayer::ShouldDraw( void )
{
	// If we're dead, our ragdoll will be drawn for us instead.
	if ( !IsAlive() )
		return false;

	if (IsGhost())
		return false;

	if( GetTeamNumber() == TEAM_SPECTATOR )
		return false;

	if( IsLocalPlayer() )
	{
		if ( IsRagdoll() )
			return true;

		// Always render the local player's world model, even in first-person.
		// Weapon worldmodels remain hidden in first-person, so they won't overlap the viewmodel.
		if ( InFirstPersonView() )
			return C_BaseAnimating::ShouldDraw();
	}

	return BaseClass::ShouldDraw();
}


bool FindWeaponAttachmentBone( C_BaseCombatWeapon *pWeapon, int &iWeaponBone )
{
	if ( !pWeapon )
		return false;

	CStudioHdr *pHdr = pWeapon->GetModelPtr();
	if ( !pHdr )
		return false;

	for ( iWeaponBone=0; iWeaponBone < pHdr->numbones(); iWeaponBone++ )
	{
		if ( stricmp( pHdr->pBone( iWeaponBone )->pszName(), "L_Hand_Attach" ) == 0 )
			break;
	}

	return iWeaponBone != pHdr->numbones();
}


bool FindMyAttachmentBone( C_BaseAnimating *pModel, int &iBone, CStudioHdr *pHdr )
{
	if ( !pHdr )
		return false;

	for ( iBone=0; iBone < pHdr->numbones(); iBone++ )
	{
		if ( stricmp( pHdr->pBone( iBone )->pszName(), "Valvebiped.Bip01_L_Hand" ) == 0 )
			break;
	}

	return iBone != pHdr->numbones();
}


inline bool IsBoneChildOf( CStudioHdr *pHdr, int iBone, int iParent )
{
	if ( iBone == iParent )
		return false;

	while ( iBone != -1 )
	{
		if ( iBone == iParent )
			return true;

		iBone = pHdr->pBone( iBone )->parent;
	}
	return false;
}

void ApplyDifferenceTransformToChildren(
	C_BaseAnimating *pModel,
	const matrix3x4_t &mSource,
	const matrix3x4_t &mDest,
	int iParentBone )
{
	CStudioHdr *pHdr = pModel->GetModelPtr();
	if ( !pHdr )
		return;

	// Build a matrix to go from mOriginalHand to mHand.
	// ( mDest * Inverse( mSource ) ) * mSource = mDest
	matrix3x4_t mSourceInverse, mToDest;
	MatrixInvert( mSource, mSourceInverse );
	ConcatTransforms( mDest, mSourceInverse, mToDest );

	// Now multiply iMyBone and all its children by mToWeaponBone.
	for ( int i=0; i < pHdr->numbones(); i++ )
	{
		if ( IsBoneChildOf( pHdr, i, iParentBone ) )
		{
			matrix3x4_t &mCur = pModel->GetBoneForWrite( i );
			matrix3x4_t mNew;
			ConcatTransforms( mToDest, mCur, mNew );
			mCur = mNew;
		}
	}
}


void GetCorrectionMatrices(
	const matrix3x4_t &mShoulder,
	const matrix3x4_t &mElbow,
	const matrix3x4_t &mHand,
	matrix3x4_t &mShoulderCorrection,
	matrix3x4_t &mElbowCorrection
	)
{
	extern void Studio_AlignIKMatrix( matrix3x4_t &mMat, const Vector &vAlignTo );

	// Get the positions of each node so we can get the direction vectors.
	Vector vShoulder, vElbow, vHand;
	MatrixPosition( mShoulder, vShoulder );
	MatrixPosition( mElbow, vElbow );
	MatrixPosition( mHand, vHand );

	// Get rid of the translation.
	matrix3x4_t mOriginalShoulder = mShoulder;
	matrix3x4_t mOriginalElbow = mElbow;
	MatrixSetColumn( Vector( 0, 0, 0 ), 3, mOriginalShoulder );
	MatrixSetColumn( Vector( 0, 0, 0 ), 3, mOriginalElbow );

	// Let the IK code align them like it would if we did IK on the joint.
	matrix3x4_t mAlignedShoulder = mOriginalShoulder;
	matrix3x4_t mAlignedElbow = mOriginalElbow;
	Studio_AlignIKMatrix( mAlignedShoulder, vElbow-vShoulder );
	Studio_AlignIKMatrix( mAlignedElbow, vHand-vElbow );

	// Figure out the transformation from the aligned bones to the original ones.
	matrix3x4_t mInvAlignedShoulder, mInvAlignedElbow;
	MatrixInvert( mAlignedShoulder, mInvAlignedShoulder );
	MatrixInvert( mAlignedElbow, mInvAlignedElbow );

	ConcatTransforms( mInvAlignedShoulder, mOriginalShoulder, mShoulderCorrection );
	ConcatTransforms( mInvAlignedElbow, mOriginalElbow, mElbowCorrection );
}


void C_CSPlayer::BuildTransformations( CStudioHdr *pHdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed )
{
	// First, setup our model's transformations like normal.
	BaseClass::BuildTransformations( pHdr, pos, q, cameraTransform, boneMask, boneComputed );

	BuildFirstPersonMeathookTransformations(pHdr, pos, q, cameraTransform, boneMask, boneComputed, "ValveBiped.Bip01_Head1");
	BuildFirstPersonMeathookTransformations(pHdr, pos, q, cameraTransform, boneMask, boneComputed, "ValveBiped.Bip01_Head");
	BuildFirstPersonMeathookTransformations(pHdr, pos, q, cameraTransform, boneMask, boneComputed, "bip_head");

	if ( IsLocalPlayer() && !C_BasePlayer::ShouldDrawLocalPlayer() )
		return;

	if ( !cl_left_hand_ik.GetInt() )
		return;

	// If our current weapon has a bone named L_Hand_Attach, then we attach the player's
	// left hand (Valvebiped.Bip01_L_Hand) to it.
	C_BaseCombatWeapon *pWeapon = GetActiveWeapon();

	if ( !pWeapon )
		return;

	// Have the weapon setup its bones.
	pWeapon->SetupBones( NULL, 0, BONE_USED_BY_ANYTHING, gpGlobals->curtime );

	int iWeaponBone = 0;
	if ( FindWeaponAttachmentBone( pWeapon, iWeaponBone ) )
	{
		int iMyBone = 0;
		if ( FindMyAttachmentBone( this, iMyBone, pHdr ) )
		{
			int iHand = iMyBone;
			int iElbow = pHdr->pBone( iHand )->parent;
			int iShoulder = pHdr->pBone( iElbow )->parent;
			matrix3x4_t *pBones = &GetBoneForWrite( 0 );

			// Store off the original hand position.
			matrix3x4_t mSource = pBones[iHand];


			// Figure out the rotation offset from the current shoulder and elbow bone rotations
			// and what the IK code's alignment code is going to produce, because we'll have to
			// re-apply that offset after the IK runs.
			matrix3x4_t mShoulderCorrection, mElbowCorrection;
			GetCorrectionMatrices( pBones[iShoulder], pBones[iElbow], pBones[iHand], mShoulderCorrection, mElbowCorrection );


			// Do the IK solution.
			Vector vHandTarget;
			MatrixPosition( pWeapon->GetBone( iWeaponBone ), vHandTarget );
			Studio_SolveIK( iShoulder, iElbow, iHand, vHandTarget, pBones );


			// Now reapply the rotation correction.
			matrix3x4_t mTempShoulder = pBones[iShoulder];
			matrix3x4_t mTempElbow = pBones[iElbow];
			ConcatTransforms( mTempShoulder, mShoulderCorrection, pBones[iShoulder] );
			ConcatTransforms( mTempElbow, mElbowCorrection, pBones[iElbow] );


			// Now apply the transformation on the hand to the fingers.
			matrix3x4_t &mDest = GetBoneForWrite( iHand );
			ApplyDifferenceTransformToChildren( this, mSource, mDest, iHand );
		}
	}

}


C_BaseAnimating * C_CSPlayer::BecomeRagdollOnClient()
{
	return NULL;
}


IRagdoll* C_CSPlayer::GetRepresentativeRagdoll() const
{
	if ( m_hRagdoll.Get() )
	{
		C_CSRagdoll *pRagdoll = (C_CSRagdoll*)m_hRagdoll.Get();

		return pRagdoll->GetIRagdoll();
	}
	else
	{
		return NULL;
	}
}


void C_CSPlayer::PlayReloadEffect()
{
	// Only play the effect for other players.
	if ( this == C_CSPlayer::GetLocalCSPlayer() )
	{
		Assert( false ); // We shouldn't have been sent this message.
		return;
	}

	// Get the view model for our current gun.
	CWeaponCSBase *pWeapon = GetActiveCSWeapon();
	if ( !pWeapon )
		return;

	// The weapon needs two models, world and view, but can only cache one. Synthesize the other.
	const CCSWeaponInfo &info = pWeapon->GetCSWpnData();
	const model_t *pModel = modelinfo->GetModel( modelinfo->GetModelIndex( info.szViewModel ) );
	if ( !pModel )
		return;
	CStudioHdr studioHdr( modelinfo->GetStudiomodel( pModel ), mdlcache );
	if ( !studioHdr.IsValid() )
		return;

	// Find the reload animation.
	for ( int iSeq=0; iSeq < studioHdr.GetNumSeq(); iSeq++ )
	{
		mstudioseqdesc_t *pSeq = &studioHdr.pSeqdesc( iSeq );

		if ( pSeq->activity == ACT_VM_RELOAD )
		{
			float poseParameters[MAXSTUDIOPOSEPARAM];
			memset( poseParameters, 0, sizeof( poseParameters ) );
			float cyclesPerSecond = Studio_CPS( &studioHdr, *pSeq, iSeq, poseParameters );

			// Now read out all the sound events with their timing
			for ( int iEvent=0; iEvent < pSeq->numevents; iEvent++ )
			{
				mstudioevent_t *pEvent = pSeq->pEvent( iEvent );

				if ( pEvent->event == CL_EVENT_SOUND )
				{
					CCSSoundEvent event;
					event.m_SoundName = pEvent->options;
					event.m_flEventTime = gpGlobals->curtime + pEvent->cycle / cyclesPerSecond;
					m_SoundEvents.AddToTail( event );
				}
			}

			break;
		}
	}
}

void C_CSPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	if ( event == PLAYERANIMEVENT_THROW_GRENADE )
	{
		// Let the server handle this event. It will update m_iThrowGrenadeCounter and the client will
		// pick up the event in CCSPlayerAnimState.
	}
	else
	{
		m_PlayerAnimState->DoAnimationEvent( event, nData );
	}
}

bool C_CSPlayer::StartSceneEvent( CSceneEventInfo *info, CChoreoScene *scene, CChoreoEvent *event, CChoreoActor *actor, CBaseEntity *pTarget )
{
	if ( GetTeamNumber() == TEAM_SURVIVOR && event->GetType() == CChoreoEvent::GESTURE )
	{
		info->m_nSequence = LookupSequence( event->GetParameters() );
		return info->m_nSequence >= 0;
	}

	return BaseClass::StartSceneEvent( info, scene, event, actor, pTarget );
}

bool C_CSPlayer::ProcessSceneEvent( bool bFlexEvents, CSceneEventInfo *info, CChoreoScene *scene, CChoreoEvent *event )
{
	if ( GetTeamNumber() == TEAM_SURVIVOR && event->GetType() == CChoreoEvent::GESTURE )
	{
		if ( bFlexEvents )
			return true;

		if ( !m_PlayerAnimState || !info || !scene || !event || info->m_nSequence < 0 )
			return false;

		const float flDuration = MAX( event->GetDuration(), 0.001f );
		const float flEventCycle = ( scene->GetTime() - event->GetStartTime() ) / flDuration;
		const float flCycle = clamp( event->GetOriginalPercentageFromPlaybackPercentage( flEventCycle ), 0.0f, 1.0f );

		m_PlayerAnimState->SetVCDGestureSequence( info->m_nSequence, flCycle );
		return true;
	}

	return BaseClass::ProcessSceneEvent( bFlexEvents, info, scene, event );
}

bool C_CSPlayer::ClearSceneEvent( CSceneEventInfo *info, bool fastKill, bool canceled )
{
	if ( GetTeamNumber() == TEAM_SURVIVOR && info && info->m_pEvent && info->m_pEvent->GetType() == CChoreoEvent::GESTURE )
	{
		if ( m_PlayerAnimState )
		{
			m_PlayerAnimState->ClearVCDGestureSequence();
		}
		return true;
	}

	return BaseClass::ClearSceneEvent( info, fastKill, canceled );
}

#define PLAYER_HALFWIDTH	 10

void C_CSPlayer::FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options )
{
	// Suppress initial footstep animation events during the first moments after map load.
	if ( ( event == 7001 || event == AE_FOOTSTEP_LEFT || event == AE_FOOTSTEP_RIGHT ) && m_flFootstepEventSuppressUntil > gpGlobals->curtime  )
	{
		BaseClass::FireEvent(origin, angles, event, options);
		return;
	}

	if (event == 7001 || event == AE_FOOTSTEP_LEFT || event == AE_FOOTSTEP_RIGHT)
	{
		// Force a footstep sound
		m_flStepSoundTime = 0;
		Vector vel;
		EstimateAbsVelocity(vel);
		surfacedata_t* t_pSurface = GetGroundSurface();
		UpdateStepSound(t_pSurface, GetAbsOrigin(), vel);


		m_flFootstepEventSuppressUntil = gpGlobals ? (gpGlobals->curtime + 0.22f) : 0.22f;
	}
	else
		BaseClass::FireEvent( origin, angles, event, options );
}


void C_CSPlayer::SetActivity( Activity eActivity )
{
	m_Activity = eActivity;
}


Activity C_CSPlayer::GetActivity() const
{
	return m_Activity;
}


const Vector& C_CSPlayer::GetRenderOrigin( void )
{
	if ( m_hRagdoll.Get() )
	{
		C_CSRagdoll *pRagdoll = (C_CSRagdoll*)m_hRagdoll.Get();
		if ( pRagdoll->IsInitialized() )
			return pRagdoll->GetRenderOrigin();
	}

	return BaseClass::GetRenderOrigin();
}


	void C_CSPlayer::Simulate( void )
	{
		if( this != C_BasePlayer::GetLocalPlayer() )
		{
		if ( IsEffectActive( EF_DIMLIGHT ) && GetTeamNumber() != TEAM_INFECTED )
		{
			QAngle eyeAngles = EyeAngles();
			Vector vForward;
			AngleVectors( eyeAngles, &vForward );

			int iAttachment = LookupAttachment( "muzzle_flash" );

			if ( iAttachment < 0 )
				return;

			Vector vecOrigin;
			QAngle dummy;
			GetAttachment( iAttachment, vecOrigin, dummy );

			trace_t tr;
			UTIL_TraceLine( vecOrigin, vecOrigin + (vForward * 200), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

			if( !m_pFlashlightBeam )
			{
				BeamInfo_t beamInfo;
				beamInfo.m_nType = TE_BEAMPOINTS;
				beamInfo.m_vecStart = tr.startpos;
				beamInfo.m_vecEnd = tr.endpos;
				beamInfo.m_pszModelName = "sprites/glow01.vmt";
				beamInfo.m_pszHaloName = "sprites/glow01.vmt";
				beamInfo.m_flHaloScale = 3.0;
				beamInfo.m_flWidth = 8.0f;
				beamInfo.m_flEndWidth = 35.0f;
				beamInfo.m_flFadeLength = 300.0f;
				beamInfo.m_flAmplitude = 0;
				beamInfo.m_flBrightness = 60.0;
				beamInfo.m_flSpeed = 0.0f;
				beamInfo.m_nStartFrame = 0.0;
				beamInfo.m_flFrameRate = 0.0;
				beamInfo.m_flRed = 255.0;
				beamInfo.m_flGreen = 255.0;
				beamInfo.m_flBlue = 255.0;
				beamInfo.m_nSegments = 8;
				beamInfo.m_bRenderable = true;
				beamInfo.m_flLife = 0.5;
				beamInfo.m_nFlags = FBEAM_FOREVER | FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

				m_pFlashlightBeam = beams->CreateBeamPoints( beamInfo );
			}

			if( m_pFlashlightBeam )
			{
				BeamInfo_t beamInfo;
				beamInfo.m_vecStart = tr.startpos;
				beamInfo.m_vecEnd = tr.endpos;
				beamInfo.m_flRed = 255.0;
				beamInfo.m_flGreen = 255.0;
				beamInfo.m_flBlue = 255.0;

				beams->UpdateBeamInfo( m_pFlashlightBeam, beamInfo );

				dlight_t *el = effects->CL_AllocDlight( 0 );
				el->origin = tr.endpos;
				el->radius = 50;
				el->color.r = 200;
				el->color.g = 200;
				el->color.b = 200;
				el->die = gpGlobals->curtime + 0.1;
			}
		}
		else if ( m_pFlashlightBeam )
		{
			ReleaseFlashlight();
			}
		}
		else
		{
			UpdateStaggerThirdPersonCamera();
			UpdatePounceThirdPersonCamera();
			UpdatePounceMusic();
			UpdateInfectedColorCorrection();
			UpdateIncapBlackAndWhiteEffects();
		}

		BaseClass::Simulate();
	}

void C_CSPlayer::ReleaseFlashlight( void )
{
	if( m_pFlashlightBeam )
	{
		m_pFlashlightBeam->flags = 0;
		m_pFlashlightBeam->die = gpGlobals->curtime - 1;

		m_pFlashlightBeam = NULL;
	}
}

bool C_CSPlayer::HasC4( void )
{
	if( this == C_CSPlayer::GetLocalPlayer() )
	{
		return Weapon_OwnsThisType( "weapon_c4" );
	}
	else
	{
		C_CS_PlayerResource *pCSPR = (C_CS_PlayerResource*)GameResources();

		return pCSPR->HasC4( entindex() );
	}
}

void C_CSPlayer::ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName )
{
	static ConVar *violence_hblood = cvar->FindVar( "violence_hblood" );
	if ( violence_hblood && !violence_hblood->GetBool() )
		return;

	BaseClass::ImpactTrace( pTrace, iDamageType, pCustomImpactName );
}


//-----------------------------------------------------------------------------
void C_CSPlayer::CalcObserverView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	/**
	 * TODO: Fix this!
	// CS:S standing eyeheight is above the collision volume, so we need to pull it
	// down when we go into close quarters.
	float maxEyeHeightAboveBounds = VEC_VIEW_SCALED( this ).z - VEC_HULL_MAX_SCALED( this ).z;
	if ( GetObserverMode() == OBS_MODE_IN_EYE &&
		maxEyeHeightAboveBounds > 0.0f &&
		GetObserverTarget() &&
		GetObserverTarget()->IsPlayer() )
	{
		const float eyeClearance = 12.0f; // eye pos must be this far below the ceiling

		C_CSPlayer *target = ToCSPlayer( GetObserverTarget() );

		Vector offset = eyeOrigin - GetAbsOrigin();

		Vector vHullMin = VEC_HULL_MIN_SCALED( this );
		vHullMin.z = 0.0f;
		Vector vHullMax = VEC_HULL_MAX_SCALED( this );

		Vector start = GetAbsOrigin();
		start.z += vHullMax.z;
		Vector end = start;
		end.z += eyeClearance + VEC_VIEW_SCALED( this ).z - vHullMax_SCALED( this ).z;

		vHullMax.z = 0.0f;

		Vector fudge( 1, 1, 0 );
		vHullMin += fudge;
		vHullMax -= fudge;

		trace_t trace;
		Ray_t ray;
		ray.Init( start, end, vHullMin, vHullMax );
		UTIL_TraceRay( ray, MASK_PLAYERSOLID, target, COLLISION_GROUP_PLAYER_MOVEMENT, &trace );

		if ( trace.fraction < 1.0f )
		{
			float est = start.z + trace.fraction * (end.z - start.z) - GetAbsOrigin().z - eyeClearance;
			if ( ( target->GetFlags() & FL_DUCKING ) == 0 && !target->GetFallVelocity() && !target->IsDucked() )
			{
				offset.z = est;
			}
			else
			{
				offset.z = MIN( est, offset.z );
			}
			eyeOrigin.z = GetAbsOrigin().z + offset.z;
		}
	}
	*/

	BaseClass::CalcObserverView( eyeOrigin, eyeAngles, fov );
}

//=============================================================================
// HPE_BEGIN:
//=============================================================================
// [tj] checks if this player has another given player on their Steam friends list.
bool C_CSPlayer::HasPlayerAsFriend(C_CSPlayer* player)
{
    if (!steamapicontext || !steamapicontext->SteamFriends() || !steamapicontext->SteamUtils() || !player)
    {
        return false;
    }

    player_info_t pi;
    if ( !engine->GetPlayerInfo( player->entindex(), &pi ) )
    {
        return false;
    }

    if ( !pi.friendsID )
    {
        return false;
    }

    // check and see if they're on the local player's friends list
    CSteamID steamID( pi.friendsID, 1, steamapicontext->SteamUtils()->GetConnectedUniverse(), k_EAccountTypeIndividual );
    return steamapicontext->SteamFriends()->HasFriend( steamID, k_EFriendFlagImmediate);
}

// [menglish] Returns whether this player is dominating or is being dominated by the specified player
bool C_CSPlayer::IsPlayerDominated( int iPlayerIndex )
{
	return m_bPlayerDominated.Get( iPlayerIndex );
}

bool C_CSPlayer::IsPlayerDominatingMe( int iPlayerIndex )
{
	return m_bPlayerDominatingMe.Get( iPlayerIndex );
}


// helper interpolation functions
namespace Interpolators
{
	inline float Linear( float t ) { return t; }

	inline float SmoothStep( float t )
	{
		t = 3 * t * t - 2.0f * t * t * t;
		return t;
	}

	inline float SmoothStep2( float t )
	{
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}

	inline float SmoothStepStart( float t )
	{
		t = 0.5f * t;
		t = 3 * t * t - 2.0f * t * t * t;
		t = t* 2.0f;
		return t;
	}

	inline float SmoothStepEnd( float t )
	{
		t = 0.5f * t + 0.5f;
		t = 3 * t * t - 2.0f * t * t * t;
		t = (t - 0.5f) * 2.0f;
		return t;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Calculate the view for the player while he's in freeze frame observer mode
//-----------------------------------------------------------------------------
void C_CSPlayer::CalcFreezeCamView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	C_BaseEntity *pTarget = GetObserverTarget();

	//=============================================================================
	// HPE_BEGIN:
	// [Forrest] Added sv_disablefreezecam check
	//=============================================================================
	static ConVarRef sv_disablefreezecam( "sv_disablefreezecam" );
	if ( !pTarget || cl_disablefreezecam.GetBool() || sv_disablefreezecam.GetBool() )
	//=============================================================================
	// HPE_END
	//=============================================================================
	{
		return CalcDeathCamView( eyeOrigin, eyeAngles, fov );
	}

	// pick a zoom camera target
	Vector vLookAt = pTarget->GetObserverCamOrigin();	// Returns ragdoll origin if they're ragdolled
	vLookAt += GetChaseCamViewOffset( pTarget );

	// look over ragdoll, not through
	if ( !pTarget->IsAlive() )
		vLookAt.z += pTarget->GetBaseAnimating() ? VEC_DEAD_VIEWHEIGHT_SCALED( pTarget->GetBaseAnimating() ).z : VEC_DEAD_VIEWHEIGHT.z;

	// Figure out a view position in front of the target
	Vector vEyeOnPlane = eyeOrigin;
	vEyeOnPlane.z = vLookAt.z;
	Vector vToTarget = vLookAt - vEyeOnPlane;
	VectorNormalize( vToTarget );

	// goal position of camera is pulled away from target by m_flFreezeFrameDistance
	Vector vTargetPos = vLookAt - (vToTarget * m_flFreezeFrameDistance);

	// Now trace out from the target, so that we're put in front of any walls
	trace_t trace;
	C_BaseEntity::PushEnableAbsRecomputations( false ); // HACK don't recompute positions while doing RayTrace
	UTIL_TraceHull( vLookAt, vTargetPos, WALL_MIN, WALL_MAX, MASK_SOLID, pTarget, COLLISION_GROUP_NONE, &trace );
	C_BaseEntity::PopEnableAbsRecomputations();
	if ( trace.fraction < 1.0 )
	{
		// The camera's going to be really close to the target. So we don't end up
		// looking at someone's chest, aim close freezecams at the target's eyes.
		vTargetPos = trace.endpos;

		// To stop all close in views looking up at character's chins, move the view up.
		vTargetPos.z += fabs(vLookAt.z - vTargetPos.z) * 0.85;
		C_BaseEntity::PushEnableAbsRecomputations( false ); // HACK don't recompute positions while doing RayTrace
		UTIL_TraceHull( vLookAt, vTargetPos, WALL_MIN, WALL_MAX, MASK_SOLID, pTarget, COLLISION_GROUP_NONE, &trace );
		C_BaseEntity::PopEnableAbsRecomputations();
		vTargetPos = trace.endpos;
	}

	// Look directly at the target
	vToTarget = vLookAt - vTargetPos;
	VectorNormalize( vToTarget );
	VectorAngles( vToTarget, eyeAngles );

	float fCurTime = gpGlobals->curtime - m_flFreezeFrameStartTime;
	float fInterpolant = clamp( fCurTime / spec_freeze_traveltime.GetFloat(), 0.0f, 1.0f );
	fInterpolant = Interpolators::SmoothStepEnd( fInterpolant );

	// move the eye toward our killer
	VectorLerp( m_vecFreezeFrameStart, vTargetPos, fInterpolant, eyeOrigin );

	if ( fCurTime >= spec_freeze_traveltime.GetFloat() && !m_bSentFreezeFrame )
	{
		IGameEvent *pEvent = gameeventmanager->CreateEvent( "freezecam_started" );
		if ( pEvent )
		{
			gameeventmanager->FireEventClientSide( pEvent );
		}

		m_bSentFreezeFrame = true;
		view->FreezeFrame( spec_freeze_time.GetFloat() );
	}
}

float C_CSPlayer::GetDeathCamInterpolationTime()
{
	static ConVarRef sv_disablefreezecam( "sv_disablefreezecam" );
	if ( cl_disablefreezecam.GetBool() || sv_disablefreezecam.GetBool() || !GetObserverTarget() )
		return spec_freeze_time.GetFloat();
	else
		return CS_DEATH_ANIMATION_TIME;

}


//=============================================================================
// HPE_END
//=============================================================================
