//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "baseviewmodel_shared.h"
#include "datacache/imdlcache.h"

#if defined( CLIENT_DLL )
#include "iprediction.h"
#include "prediction.h"
#include "client_virtualreality.h"
#include "sourcevr/isourcevirtualreality.h"
#else
#include "vguiscreen.h"
#endif

#if defined( CLIENT_DLL ) && defined( SIXENSE )
#include "sixense/in_sixense.h"
#include "sixense/sixense_convars_extern.h"
#endif

#ifdef SIXENSE
extern ConVar in_forceuser;
#include "iclientmode.h"
#endif

#include "weapon_csbase.h"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define VIEWMODEL_ANIMATION_PARITY_BITS 3
#define SCREEN_OVERLAY_MATERIAL "vgui/screens/vgui_overlay"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseViewModel::CBaseViewModel()
{
#if defined( CLIENT_DLL )
	// NOTE: We do this here because the color is never transmitted for the view model.
	m_nOldAnimationParity = 0;
	m_nOldViewModelLayerParity = 0;
	m_EntClientFlags |= ENTCLIENTFLAG_ALWAYS_INTERPOLATE;
#endif
	SetRenderColor( 255, 255, 255, 255 );

	// View model of this weapon
	m_sVMName			= NULL_STRING;		
	// Prefix of the animations that should be used by the player carrying this weapon
	m_sAnimationPrefix	= NULL_STRING;

	m_nViewModelIndex	= 0;

	m_nAnimationParity	= 0;
	m_nViewModelLayerSequence = -1;
	m_nViewModelLayerParity = 0;
	m_nAnimationLayerIndex = -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseViewModel::~CBaseViewModel()
{
}

void CBaseViewModel::UpdateOnRemove( void )
{
	BaseClass::UpdateOnRemove();

	DestroyControlPanels();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseViewModel::Precache( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseViewModel::Spawn( void )
{
	Precache( );
	SetSize( Vector( -8, -4, -2), Vector(8, 4, 2) );
	SetSolid( SOLID_NONE );
}


#if defined ( CSTRIKE_DLL ) && !defined ( CLIENT_DLL )
#define VGUI_CONTROL_PANELS
#endif

#if defined ( TF_DLL )
#define VGUI_CONTROL_PANELS
#endif

#ifdef INVASION_DLL
#define VGUI_CONTROL_PANELS
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseViewModel::SetControlPanelsActive( bool bState )
{
#if defined( VGUI_CONTROL_PANELS )
	// Activate control panel screens
	for ( int i = m_hScreens.Count(); --i >= 0; )
	{
		if (m_hScreens[i].Get())
		{
			m_hScreens[i]->SetActive( bState );
		}
	}
#endif
}

//-----------------------------------------------------------------------------
// This is called by the base object when it's time to spawn the control panels
//-----------------------------------------------------------------------------
void CBaseViewModel::SpawnControlPanels()
{
#if defined( VGUI_CONTROL_PANELS )
	char buf[64];

	// Destroy existing panels
	DestroyControlPanels();

	CBaseCombatWeapon *weapon = m_hWeapon.Get();

	if ( weapon == NULL )
	{
		return;
	}

	MDLCACHE_CRITICAL_SECTION();

	// FIXME: Deal with dynamically resizing control panels?

	// If we're attached to an entity, spawn control panels on it instead of use
	CBaseAnimating *pEntityToSpawnOn = this;
	char *pOrgLL = "controlpanel%d_ll";
	char *pOrgUR = "controlpanel%d_ur";
	char *pAttachmentNameLL = pOrgLL;
	char *pAttachmentNameUR = pOrgUR;
	/*
	if ( IsBuiltOnAttachment() )
	{
		pEntityToSpawnOn = dynamic_cast<CBaseAnimating*>((CBaseEntity*)m_hBuiltOnEntity.Get());
		if ( pEntityToSpawnOn )
		{
			char sBuildPointLL[64];
			char sBuildPointUR[64];
			Q_snprintf( sBuildPointLL, sizeof( sBuildPointLL ), "bp%d_controlpanel%%d_ll", m_iBuiltOnPoint );
			Q_snprintf( sBuildPointUR, sizeof( sBuildPointUR ), "bp%d_controlpanel%%d_ur", m_iBuiltOnPoint );
			pAttachmentNameLL = sBuildPointLL;
			pAttachmentNameUR = sBuildPointUR;
		}
		else
		{
			pEntityToSpawnOn = this;
		}
	}
	*/

	Assert( pEntityToSpawnOn );

	// Lookup the attachment point...
	int nPanel;
	for ( nPanel = 0; true; ++nPanel )
	{
		Q_snprintf( buf, sizeof( buf ), pAttachmentNameLL, nPanel );
		int nLLAttachmentIndex = pEntityToSpawnOn->LookupAttachment(buf);
		if (nLLAttachmentIndex <= 0)
		{
			// Try and use my panels then
			pEntityToSpawnOn = this;
			Q_snprintf( buf, sizeof( buf ), pOrgLL, nPanel );
			nLLAttachmentIndex = pEntityToSpawnOn->LookupAttachment(buf);
			if (nLLAttachmentIndex <= 0)
				return;
		}

		Q_snprintf( buf, sizeof( buf ), pAttachmentNameUR, nPanel );
		int nURAttachmentIndex = pEntityToSpawnOn->LookupAttachment(buf);
		if (nURAttachmentIndex <= 0)
		{
			// Try and use my panels then
			Q_snprintf( buf, sizeof( buf ), pOrgUR, nPanel );
			nURAttachmentIndex = pEntityToSpawnOn->LookupAttachment(buf);
			if (nURAttachmentIndex <= 0)
				return;
		}

		const char *pScreenName;
		weapon->GetControlPanelInfo( nPanel, pScreenName );
		if (!pScreenName)
			continue;

		const char *pScreenClassname;
		weapon->GetControlPanelClassName( nPanel, pScreenClassname );
		if ( !pScreenClassname )
			continue;

		// Compute the screen size from the attachment points...
		matrix3x4_t	panelToWorld;
		pEntityToSpawnOn->GetAttachment( nLLAttachmentIndex, panelToWorld );

		matrix3x4_t	worldToPanel;
		MatrixInvert( panelToWorld, worldToPanel );

		// Now get the lower right position + transform into panel space
		Vector lr, lrlocal;
		pEntityToSpawnOn->GetAttachment( nURAttachmentIndex, panelToWorld );
		MatrixGetColumn( panelToWorld, 3, lr );
		VectorTransform( lr, worldToPanel, lrlocal );

		float flWidth = lrlocal.x;
		float flHeight = lrlocal.y;

		CVGuiScreen *pScreen = CreateVGuiScreen( pScreenClassname, pScreenName, pEntityToSpawnOn, this, nLLAttachmentIndex );
		pScreen->ChangeTeam( GetTeamNumber() );
		pScreen->SetActualSize( flWidth, flHeight );
		pScreen->SetActive( false );
		pScreen->MakeVisibleOnlyToTeammates( false );
	
#ifdef INVASION_DLL
		pScreen->SetOverlayMaterial( SCREEN_OVERLAY_MATERIAL );
#endif
		pScreen->SetAttachedToViewModel( true );
		int nScreen = m_hScreens.AddToTail( );
		m_hScreens[nScreen].Set( pScreen );
	}
#endif
}

void CBaseViewModel::DestroyControlPanels()
{
#if defined( VGUI_CONTROL_PANELS )
	// Kill the control panels
	int i;
	for ( i = m_hScreens.Count(); --i >= 0; )
	{
		DestroyVGuiScreen( m_hScreens[i].Get() );
	}
	m_hScreens.RemoveAll();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEntity - 
//-----------------------------------------------------------------------------
void CBaseViewModel::SetOwner( CBaseEntity *pEntity )
{
	m_hOwner = pEntity;
#if !defined( CLIENT_DLL )
	// Make sure we're linked into hierarchy
	//SetParent( pEntity );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : nIndex - 
//-----------------------------------------------------------------------------
void CBaseViewModel::SetIndex( int nIndex )
{
	m_nViewModelIndex = nIndex;
	Assert( m_nViewModelIndex < (1 << VIEWMODEL_INDEX_BITS) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CBaseViewModel::ViewModelIndex( ) const
{
	return m_nViewModelIndex;
}

//-----------------------------------------------------------------------------
// Purpose: Pass our visibility on to our child screens
//-----------------------------------------------------------------------------
void CBaseViewModel::AddEffects( int nEffects )
{
	if ( nEffects & EF_NODRAW )
	{
		SetControlPanelsActive( false );
	}

	BaseClass::AddEffects( nEffects );
}

//-----------------------------------------------------------------------------
// Purpose: Pass our visibility on to our child screens
//-----------------------------------------------------------------------------
void CBaseViewModel::RemoveEffects( int nEffects )
{
	if ( nEffects & EF_NODRAW )
	{
		SetControlPanelsActive( true );
	}

	BaseClass::RemoveEffects( nEffects );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *modelname - 
//-----------------------------------------------------------------------------
void CBaseViewModel::SetWeaponModel( const char *modelname, CBaseCombatWeapon *weapon )
{
	m_hWeapon = weapon;

#if defined( CLIENT_DLL )
	SetModel( modelname );
#else
	string_t str;
	if ( modelname != NULL )
	{
		str = MAKE_STRING( modelname );
	}
	else
	{
		str = NULL_STRING;
	}

	if ( str != m_sVMName )
	{
		// Msg( "SetWeaponModel %s at %f\n", modelname, gpGlobals->curtime );
		m_sVMName = str;
		SetModel( STRING( m_sVMName ) );

		// Create any vgui control panels associated with the weapon
		SpawnControlPanels();

		bool showControlPanels = weapon && weapon->ShouldShowControlPanels();
		SetControlPanelsActive( showControlPanels );
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : CBaseCombatWeapon
//-----------------------------------------------------------------------------
CBaseCombatWeapon *CBaseViewModel::GetOwningWeapon( void )
{
	return m_hWeapon.Get();
}

void CBaseViewModel::SetPlaybackRate( float flPlaybackRate )
{
	BaseClass::SetPlaybackRate( flPlaybackRate );

	if ( !HasViewModelAnimationLayer() )
		return;

#if defined( CLIENT_DLL )
	m_AnimOverlay[m_nAnimationLayerIndex].m_flPlaybackRate = flPlaybackRate;
#else
	SetLayerPlaybackRate( m_nAnimationLayerIndex, flPlaybackRate );
#endif
}

bool CBaseViewModel::HasViewModelAnimationLayer()
{
#if defined( CLIENT_DLL )
	return m_nAnimationLayerIndex >= 0 && m_nAnimationLayerIndex < m_AnimOverlay.Count();
#else
	return IsValidLayer( m_nAnimationLayerIndex );
#endif
}

void CBaseViewModel::ClearViewModelAnimationLayer()
{
	if ( !HasViewModelAnimationLayer() )
	{
		m_nAnimationLayerIndex = -1;
#if !defined( CLIENT_DLL )
		if ( m_nViewModelLayerSequence != -1 )
		{
			m_nViewModelLayerSequence = -1;
			m_nViewModelLayerParity = ( m_nViewModelLayerParity + 1 ) & ( (1<<VIEWMODEL_ANIMATION_PARITY_BITS) - 1 );
		}
#endif
		return;
	}

#if defined( CLIENT_DLL )
	C_AnimationLayer &layer = m_AnimOverlay[m_nAnimationLayerIndex];
	layer.Reset();
	layer.m_nOrder = CBaseAnimatingOverlay::MAX_OVERLAYS;
	m_flOverlayPrevEventCycle[m_nAnimationLayerIndex] = -1.0f;
#else
	FastRemoveLayer( m_nAnimationLayerIndex );
	m_nViewModelLayerSequence = -1;
	m_nViewModelLayerParity = ( m_nViewModelLayerParity + 1 ) & ( (1<<VIEWMODEL_ANIMATION_PARITY_BITS) - 1 );
#endif

	m_nAnimationLayerIndex = -1;
}

void CBaseViewModel::SetViewModelBaseSequence( int sequence )
{
	ClearViewModelAnimationLayer();

	SetSequence( sequence );
	m_nAnimationParity = ( m_nAnimationParity + 1 ) & ( (1<<VIEWMODEL_ANIMATION_PARITY_BITS) - 1 );

#if defined( CLIENT_DLL )
	m_nOldAnimationParity = m_nAnimationParity;
	m_flAnimTime = gpGlobals->curtime;
#endif

	SetCycle( 0 );
	ResetSequenceInfo();
}

void CBaseViewModel::EnsureViewModelIdleSequence()
{
	if ( GetSequenceActivity( GetSequence() ) == ACT_VM_IDLE )
		return;

	const int idleSequence = SelectWeightedSequence( ACT_VM_IDLE );
	if ( idleSequence == ACTIVITY_NOT_AVAILABLE )
		return;

	SetSequence( idleSequence );
#if defined( CLIENT_DLL )
	m_flAnimTime = gpGlobals->curtime;
#endif
	SetCycle( 0 );
	ResetSequenceInfo();
}

void CBaseViewModel::AddViewModelAnimationLayer( int sequence )
{
	ClearViewModelAnimationLayer();

#if defined( CLIENT_DLL )
	m_nAnimationLayerIndex = 0;

	if ( m_AnimOverlay.Count() <= m_nAnimationLayerIndex )
	{
		m_AnimOverlay.AddMultipleToTail( m_nAnimationLayerIndex + 1 - m_AnimOverlay.Count() );
	}

	if ( m_iv_AnimOverlay.Count() <= m_nAnimationLayerIndex )
	{
		const int nOldInterpCount = m_iv_AnimOverlay.Count();
		m_iv_AnimOverlay.AddMultipleToTail( m_nAnimationLayerIndex + 1 - m_iv_AnimOverlay.Count() );
		for ( int i = nOldInterpCount; i < m_iv_AnimOverlay.Count(); ++i )
		{
			AddVar( &m_AnimOverlay.Element( i ), &m_iv_AnimOverlay.Element( i ), LATCH_ANIMATION_VAR, true );
		}
	}

	C_AnimationLayer &layer = m_AnimOverlay[m_nAnimationLayerIndex];
	layer.Reset();
	layer.m_nSequence = sequence;
	layer.m_flPrevCycle = 0.0f;
	layer.m_flCycle = 0.0f;
	layer.m_flPlaybackRate = GetPlaybackRate();
	layer.m_flWeight = 1.0f;
	layer.m_nOrder = 1;
	layer.m_flLayerAnimtime = gpGlobals->curtime;
	m_flOverlayPrevEventCycle[m_nAnimationLayerIndex] = -1.0f;
#else
	m_nAnimationLayerIndex = AddGestureSequence( sequence, true );
	SetLayerPlaybackRate( m_nAnimationLayerIndex, GetPlaybackRate() );
	m_nViewModelLayerSequence = sequence;
	m_nViewModelLayerParity = ( m_nViewModelLayerParity + 1 ) & ( (1<<VIEWMODEL_ANIMATION_PARITY_BITS) - 1 );
#endif
}

bool CBaseViewModel::IsViewModelSequenceFinished()
{
	if ( HasViewModelAnimationLayer() )
	{
#if defined( CLIENT_DLL )
		return m_AnimOverlay[m_nAnimationLayerIndex].m_flWeight == 0.0f;
#else
		return m_AnimOverlay[m_nAnimationLayerIndex].m_bSequenceFinished;
#endif
	}

	return BaseClass::IsSequenceFinished();
}

float CBaseViewModel::GetViewModelSequenceDuration()
{
	if ( HasViewModelAnimationLayer() )
	{
#if defined( CLIENT_DLL )
		const C_AnimationLayer &layer = m_AnimOverlay[m_nAnimationLayerIndex];
		float flPlaybackRate = layer.m_flPlaybackRate;
		if ( flPlaybackRate < 0.0f )
		{
			flPlaybackRate = -flPlaybackRate;
		}
		return flPlaybackRate > 0.0f ? SequenceDuration( layer.m_nSequence ) / flPlaybackRate : 0.0f;
#else
		const CAnimationLayer &layer = m_AnimOverlay[m_nAnimationLayerIndex];
		float flPlaybackRate = layer.m_flPlaybackRate;
		if ( flPlaybackRate < 0.0f )
		{
			flPlaybackRate = -flPlaybackRate;
		}
		return flPlaybackRate > 0.0f ? SequenceDuration( layer.m_nSequence ) / flPlaybackRate : 0.0f;
#endif
	}

	return SequenceDuration();
}

#if defined( CLIENT_DLL )
void CBaseViewModel::AdvanceViewModelAnimationLayer( float currentTime )
{
	if ( !HasViewModelAnimationLayer() )
		return;

	CStudioHdr *pStudioHdr = GetModelPtr();
	if ( pStudioHdr == NULL )
	{
		ClearViewModelAnimationLayer();
		return;
	}

	C_AnimationLayer &layer = m_AnimOverlay[m_nAnimationLayerIndex];
	if ( layer.m_nSequence == -1 )
	{
		ClearViewModelAnimationLayer();
		return;
	}

	if ( layer.m_flWeight == 0.0f )
	{
		ClearViewModelAnimationLayer();
		return;
	}

	const float flInterval = currentTime - layer.m_flLayerAnimtime;
	if ( flInterval <= 0.0f )
		return;

	layer.m_flPrevCycle = layer.m_flCycle;
	layer.m_flCycle += flInterval * GetSequenceCycleRate( pStudioHdr, layer.m_nSequence ) * layer.m_flPlaybackRate;
	layer.m_flLayerAnimtime = currentTime;

	if ( layer.m_flCycle >= 1.0f )
	{
		if ( IsSequenceLooping( layer.m_nSequence ) )
		{
			layer.m_flCycle = 0.0f;
		}
		else
		{
			layer.m_flCycle = 1.0f;
		}

		layer.m_flWeight = 0.0f;
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : sequence - 
//-----------------------------------------------------------------------------
void CBaseViewModel::SendViewModelMatchingSequence( int sequence )
{
	if ( sequence < 0 )
		return;

	CBaseCombatWeapon* weapon = m_hWeapon.Get();
#if !defined( CLIENT_DLL )
	bool showControlPanels = weapon && weapon->ShouldShowControlPanels();
	SetControlPanelsActive( showControlPanels );
#endif

	if ( GetSequenceActivity( sequence ) == ACT_VM_IDLE )
	{
		SetViewModelBaseSequence( sequence );
		return;
	}

	EnsureViewModelIdleSequence();
	weapon->SetViewModel();
	AddViewModelAnimationLayer(sequence);
}

#if defined( CLIENT_DLL )
#include "ivieweffects.h"
#endif
extern float g_verticalBob;

void CBaseViewModel::CalcViewModelView( CBasePlayer *owner, const Vector& eyePosition, const QAngle& eyeAngles )
{
	// UNDONE: Calc this on the server?  Disabled for now as it seems unnecessary to have this info on the server
#if defined( CLIENT_DLL )
	QAngle vmangoriginal = eyeAngles;
	QAngle vmangles = eyeAngles;
	Vector vmorigin = eyePosition;

	CBaseCombatWeapon *pWeapon = m_hWeapon.Get();
	//Allow weapon lagging
	if ( pWeapon != NULL )
	{
#if defined( CLIENT_DLL )
		if ( !prediction->InPrediction() )
#endif
		{
			// add weapon-specific bob 
			pWeapon->AddViewmodelBob( this, vmorigin, vmangles );

			CalcViewModelLag( vmorigin, vmangles, vmangoriginal );
		}
	}
	// Add model-specific bob even if no weapon associated (for head bob for off hand models)
	AddViewModelBob( owner, vmorigin, vmangles );
	CalcViewModelLag(vmorigin, vmangles, vmangoriginal);

#if defined( CLIENT_DLL )
	if ( !prediction->InPrediction() )
	{
		// Let the viewmodel shake at about 10% of the amplitude of the player's view
		vieweffects->ApplyShake( vmorigin, vmangles, 0.1 );	
	}
#endif

	if( UseVR() )
	{
		g_ClientVirtualReality.OverrideViewModelTransform( vmorigin, vmangles, pWeapon && pWeapon->ShouldUseLargeViewModelVROverride() );
	}


	SetLocalOrigin( vmorigin );
	SetLocalAngles( vmangles );

#ifdef SIXENSE
	if( g_pSixenseInput->IsEnabled() && (owner->GetObserverMode()==OBS_MODE_NONE) && !UseVR() )
	{
		const float max_gun_pitch = 20.0f;

		float viewmodel_fov_ratio = g_pClientMode->GetViewModelFOV()/owner->GetFOV();
		QAngle gun_angles = g_pSixenseInput->GetViewAngleOffset() * -viewmodel_fov_ratio;

		// Clamp pitch a bit to minimize seeing back of viewmodel
		if( gun_angles[PITCH] < -max_gun_pitch )
		{ 
			gun_angles[PITCH] = -max_gun_pitch; 
		}

#ifdef WIN32 // ShouldFlipViewModel comes up unresolved on osx? Mabye because it's defined inline? fixme
		if( ShouldFlipViewModel() ) 
		{
			gun_angles[YAW] *= -1.0f;
		}
#endif

		vmangles = EyeAngles() +  gun_angles;

		SetLocalAngles( vmangles );
	}
#endif
#endif

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float g_fMaxViewModelLag = 1.5f;

ConVar sv_viewmodel_lag_do_angles( "sv_viewmodel_lag_do_angles", "1", FCVAR_CHEAT | FCVAR_REPLICATED );

void CBaseViewModel::CalcViewModelLag( Vector& origin, QAngle& angles, QAngle& original_angles )
{
	Vector vOriginalOrigin = origin;
	QAngle vOriginalAngles = angles;

	// Calculate our drift
	Vector	forward;
	AngleVectors( angles, &forward, NULL, NULL );

	if ( gpGlobals->frametime != 0.0f )
	{
		Vector vDifference;
		VectorSubtract( forward, m_vecLastFacing, vDifference );

		float flSpeed = 5.0f;

		// If we start to lag too far behind, we'll increase the "catch up" speed.  Solves the problem with fast cl_yawspeed, m_yaw or joysticks
		//  rotating quickly.  The old code would slam lastfacing with origin causing the viewmodel to pop to a new position
		float flDiff = vDifference.Length();
		if ( (flDiff > g_fMaxViewModelLag) && (g_fMaxViewModelLag > 0.0f) )
		{
			float flScale = flDiff / g_fMaxViewModelLag;
			flSpeed *= flScale;
		}

		// FIXME:  Needs to be predictable?
		VectorMA( m_vecLastFacing, flSpeed * gpGlobals->frametime, vDifference, m_vecLastFacing );
		// Make sure it doesn't grow out of control!!!
		VectorNormalize( m_vecLastFacing );
		VectorMA( origin, 5.0f, vDifference * -1.0f, origin );

		Assert( m_vecLastFacing.IsValid() );
	}

	if ( sv_viewmodel_lag_do_angles.GetBool() )
	{
		Vector right, up;
		AngleVectors( original_angles, &forward, &right, &up );

		float pitch = original_angles[ PITCH ];
		if ( pitch > 180.0f )
			pitch -= 360.0f;
		else if ( pitch < -180.0f )
			pitch += 360.0f;

		if ( g_fMaxViewModelLag == 0.0f )
		{
			origin = vOriginalOrigin;
			angles = vOriginalAngles;
		}

		//FIXME: These are the old settings that caused too many exposed polys on some models
		VectorMA( origin, -pitch * 0.035f,	forward,	origin );
		VectorMA( origin, -pitch * 0.03f,		right,	origin );
		VectorMA( origin, -pitch * 0.02f,		up,		origin);
	}
}

//-----------------------------------------------------------------------------
// Stub to keep networking consistent for DEM files
//-----------------------------------------------------------------------------
#if defined( CLIENT_DLL )
  extern void RecvProxy_EffectFlags( const CRecvProxyData *pData, void *pStruct, void *pOut );
  void RecvProxy_SequenceNum( const CRecvProxyData *pData, void *pStruct, void *pOut );
#endif

//-----------------------------------------------------------------------------
// Purpose: Resets anim cycle when the server changes the weapon on us
//-----------------------------------------------------------------------------
#if defined( CLIENT_DLL )
static void RecvProxy_Weapon( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	CBaseViewModel *pViewModel = ((CBaseViewModel*)pStruct);
	CBaseCombatWeapon *pOldWeapon = pViewModel->GetOwningWeapon();

	// Chain through to the default recieve proxy ...
	RecvProxy_IntToEHandle( pData, pStruct, pOut );

	// ... and reset our cycle index if the server is switching weapons on us
	CBaseCombatWeapon *pNewWeapon = pViewModel->GetOwningWeapon();
	if ( pNewWeapon != pOldWeapon )
	{
		// Restart animation at frame 0
		pViewModel->SetCycle( 0 );
		pViewModel->m_flAnimTime = gpGlobals->curtime;
	}
}
#endif


LINK_ENTITY_TO_CLASS( viewmodel, CBaseViewModel );

IMPLEMENT_NETWORKCLASS_ALIASED( BaseViewModel, DT_BaseViewModel )

BEGIN_NETWORK_TABLE_NOBASE(CBaseViewModel, DT_BaseViewModel)
#if !defined( CLIENT_DLL )
	SendPropModelIndex(SENDINFO(m_nModelIndex)),
	SendPropInt		(SENDINFO(m_nBody), 8),
	SendPropInt		(SENDINFO(m_nSkin), 10),
	SendPropInt		(SENDINFO(m_nSequence),	8, SPROP_UNSIGNED),
	SendPropInt		(SENDINFO(m_nViewModelIndex), VIEWMODEL_INDEX_BITS, SPROP_UNSIGNED),
	SendPropFloat	(SENDINFO(m_flPlaybackRate),	8,	SPROP_ROUNDUP,	-4.0,	12.0f),
	SendPropInt		(SENDINFO(m_fEffects),		10, SPROP_UNSIGNED),
	SendPropInt		(SENDINFO(m_nAnimationParity), 3, SPROP_UNSIGNED ),
	SendPropInt		(SENDINFO(m_nViewModelLayerSequence), 14),
	SendPropInt		(SENDINFO(m_nViewModelLayerActivity), 13, SPROP_UNSIGNED),
	SendPropInt		(SENDINFO(m_nViewModelLayerParity), 3, SPROP_UNSIGNED ),
	SendPropEHandle (SENDINFO(m_hWeapon)),
	SendPropEHandle (SENDINFO(m_hOwner)),

	SendPropInt( SENDINFO( m_nNewSequenceParity ), EF_PARITY_BITS, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_nResetEventsParity ), EF_PARITY_BITS, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_nMuzzleFlashParity ), EF_MUZZLEFLASH_BITS, SPROP_UNSIGNED ),
	// In CS, viewmodel animation layers are mirrored and advanced clientside (see C_BaseViewModel::Interpolate).
	// Don't network overlay vars from the server, or the server will drive layer cycles at tickrate.
	SendPropDataTable( "overlay_vars", 0, &REFERENCE_SEND_TABLE( DT_OverlayVars ) ),

	SendPropArray	(SendPropFloat(SENDINFO_ARRAY(m_flPoseParameter),	8, 0, 0.0f, 1.0f), m_flPoseParameter),
#else
	RecvPropInt		(RECVINFO(m_nModelIndex)),
	RecvPropInt		(RECVINFO(m_nSkin)),
	RecvPropInt		(RECVINFO(m_nBody)),
	RecvPropInt		(RECVINFO(m_nSequence), 0, RecvProxy_SequenceNum ),
	RecvPropInt		(RECVINFO(m_nViewModelIndex)),
	RecvPropFloat	(RECVINFO(m_flPlaybackRate)),
	RecvPropInt		(RECVINFO(m_fEffects), 0, RecvProxy_EffectFlags ),
	RecvPropInt		(RECVINFO(m_nAnimationParity)),
	RecvPropInt		(RECVINFO(m_nViewModelLayerSequence)),
	RecvPropInt		(RECVINFO(m_nViewModelLayerActivity)),
	RecvPropInt		(RECVINFO(m_nViewModelLayerParity)),
	RecvPropEHandle (RECVINFO(m_hWeapon), RecvProxy_Weapon ),
	RecvPropEHandle (RECVINFO(m_hOwner)),

	RecvPropInt( RECVINFO( m_nNewSequenceParity )),
	RecvPropInt( RECVINFO( m_nResetEventsParity )),
	RecvPropInt( RECVINFO( m_nMuzzleFlashParity )),
	RecvPropDataTable( "overlay_vars", 0, 0, &REFERENCE_RECV_TABLE( DT_OverlayVars ) ),

	RecvPropArray(RecvPropFloat(RECVINFO(m_flPoseParameter[0]) ), m_flPoseParameter ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL

BEGIN_PREDICTION_DATA( CBaseViewModel )

	// Networked
	DEFINE_PRED_FIELD( m_nModelIndex, FIELD_SHORT, FTYPEDESC_INSENDTABLE | FTYPEDESC_MODELINDEX ),
	DEFINE_PRED_FIELD( m_nSkin, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nBody, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nSequence, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nViewModelIndex, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD_TOL( m_flPlaybackRate, FIELD_FLOAT, FTYPEDESC_INSENDTABLE, 0.125f ),
	DEFINE_PRED_FIELD( m_fEffects, FIELD_INTEGER, FTYPEDESC_INSENDTABLE | FTYPEDESC_OVERRIDE ),
	DEFINE_PRED_FIELD( m_nAnimationParity, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nViewModelLayerSequence, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nViewModelLayerParity, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_hWeapon, FIELD_EHANDLE, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_flAnimTime, FIELD_FLOAT, 0 ),

	DEFINE_FIELD( m_hOwner, FIELD_EHANDLE ),
	DEFINE_FIELD( m_flTimeWeaponIdle, FIELD_FLOAT ),
	DEFINE_FIELD( m_Activity, FIELD_INTEGER ),
	DEFINE_FIELD( m_nAnimationLayerIndex, FIELD_INTEGER ),
	DEFINE_PRED_FIELD( m_flCycle, FIELD_FLOAT, FTYPEDESC_PRIVATE | FTYPEDESC_OVERRIDE | FTYPEDESC_NOERRORCHECK ),

END_PREDICTION_DATA()

void RecvProxy_SequenceNum( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	CBaseViewModel *model = (CBaseViewModel *)pStruct;
	if (pData->m_Value.m_Int != model->GetSequence())
	{
		MDLCACHE_CRITICAL_SECTION();

		model->SetSequence(pData->m_Value.m_Int);
		model->m_flAnimTime = gpGlobals->curtime;
		model->SetCycle(0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CBaseViewModel::LookupAttachment( const char *pAttachmentName )
{
	if ( m_hWeapon.Get() && m_hWeapon.Get()->WantsToOverrideViewmodelAttachments() )
		return m_hWeapon.Get()->LookupAttachment( pAttachmentName );

	return BaseClass::LookupAttachment( pAttachmentName );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseViewModel::GetAttachment( int number, matrix3x4_t &matrix )
{
	if ( m_hWeapon.Get() && m_hWeapon.Get()->WantsToOverrideViewmodelAttachments() )
		return m_hWeapon.Get()->GetAttachment( number, matrix );

	return BaseClass::GetAttachment( number, matrix );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseViewModel::GetAttachment( int number, Vector &origin )
{
	if ( m_hWeapon.Get() && m_hWeapon.Get()->WantsToOverrideViewmodelAttachments() )
		return m_hWeapon.Get()->GetAttachment( number, origin );

	return BaseClass::GetAttachment( number, origin );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseViewModel::GetAttachment( int number, Vector &origin, QAngle &angles )
{
	if ( m_hWeapon.Get() && m_hWeapon.Get()->WantsToOverrideViewmodelAttachments() )
		return m_hWeapon.Get()->GetAttachment( number, origin, angles );

	return BaseClass::GetAttachment( number, origin, angles );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseViewModel::GetAttachmentVelocity( int number, Vector &originVel, Quaternion &angleVel )
{
	if ( m_hWeapon.Get() && m_hWeapon.Get()->WantsToOverrideViewmodelAttachments() )
		return m_hWeapon.Get()->GetAttachmentVelocity( number, originVel, angleVel );

	return BaseClass::GetAttachmentVelocity( number, originVel, angleVel );
}

#endif
