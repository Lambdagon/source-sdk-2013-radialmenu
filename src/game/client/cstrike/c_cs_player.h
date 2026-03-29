//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef C_CS_PLAYER_H
#define C_CS_PLAYER_H
#ifdef _WIN32
#pragma once
#endif


#include "cs_playeranimstate.h"
#include "c_baseplayer.h"
#include "cs_shareddefs.h"
#include "weapon_csbase.h"
#include "baseparticleentity.h"
#include "beamdraw.h"
#include "colorcorrectionmgr.h"
#include "iinput.h"


class C_PhysicsProp;

extern ConVar cl_disablefreezecam;

class CAddonModel
{
public:
	CHandle<C_BaseAnimating> m_hEnt;	// The model for the addon.
	int m_iAddon;						// One of the ADDON_ bits telling which model this is.
	int m_iAttachmentPoint;				// Which attachment point on the player model this guy is on.
};



class C_CSPlayer : public C_BasePlayer, public ICSPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS( C_CSPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_CSPlayer();
	~C_CSPlayer();

	virtual void Simulate();
	virtual void		Spawn();

	bool HasDefuser() const;

	void GiveDefuser();
	void RemoveDefuser();

	bool HasNightVision() const;

	static C_CSPlayer* GetLocalCSPlayer();
	CSPlayerState State_Get() const;

	virtual float GetMinFOV() const;

	// Get how much $$$ this guy has.
	int GetAccount() const;

	// Returns one of the CS_CLASS_ enums.
	int PlayerClass() const;

	bool IsInBuyZone();
	bool CanShowTeamMenu() const;	// Returns true if we're allowed to show the team menu right now.

	// Get the amount of armor the player has.
	int ArmorValue() const;
	bool HasHelmet() const;
	int GetCurrentAssaultSuitPrice();

	virtual const QAngle& EyeAngles();
	virtual const QAngle& GetRenderAngles();
	virtual void CalcObserverView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov );

	virtual void			GetRenderBounds( Vector& theMins, Vector& theMaxs );
	virtual void			GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType );
	virtual bool			GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const;

	virtual void VPhysicsUpdate( IPhysicsObject *pPhysics );

	// Get the ID target entity index. The ID target is the player that is behind our crosshairs, used to
	// display the player's name.
	int GetIDTarget() const;

	virtual void NotifyShouldTransmit( ShouldTransmitState_t state );
	virtual void ClientThink();

	virtual void OnDataChanged( DataUpdateType_t type );
	virtual void PostDataUpdate( DataUpdateType_t updateType );
	virtual bool Interpolate( float currentTime );
	virtual void UpdateStepSound( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity  );
	virtual surfacedata_t * GetFootstepSurface( const Vector &origin, const char *surfaceName );
	virtual void ValidateModelIndex( void );
	virtual void PlayStepSound(Vector& vecOrigin, surfacedata_t* psurface, float fvol, bool force);
	virtual const char* GetFootstepSound(const char* stepType, bool /*unused*/, float side, bool /*unused*/) const;

	virtual int	GetMaxHealth() const;

	bool		Weapon_CanSwitchTo(C_BaseCombatWeapon *pWeapon);

	virtual void UpdateClientSideAnimation();
	virtual void ProcessMuzzleFlashEvent();

	virtual const Vector& GetRenderOrigin( void );

	bool CreateMove( float flInputSampleTime, CUserCmd *pCmd );

	CUtlVector< C_BaseParticleEntity* > m_SmokeGrenades;

	virtual bool ShouldDraw( void );
	virtual void BuildTransformations( CStudioHdr *pStudioHdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed );

	virtual C_BaseAnimating * BecomeRagdollOnClient();
	virtual IRagdoll* GetRepresentativeRagdoll() const;

	void ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName );

	// Have this player play the sounds from his view model's reload animation.
	void PlayReloadEffect();

	virtual void FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options );

	bool		 HasC4( void );
	bool IsGhost(void) const { return m_bIsGhost; }

	virtual void CreateLightEffects( void ) {}	//no dimlight effects

	// Sometimes the server wants to update the client's cycle to get the two to run in sync (for proper hit detection)
	virtual void SetServerIntendedCycle( float intended ) { m_serverIntendedCycle = intended; }
	virtual float GetServerIntendedCycle( void ) { return m_serverIntendedCycle; }

	virtual bool ShouldReceiveProjectedTextures( int flags )
	{
		return ( this != C_BasePlayer::GetLocalPlayer() );
	}

	void ClearSoundEvents()
	{
		m_SoundEvents.RemoveAll();
	}

	//=============================================================================
	// HPE_BEGIN:
	// [menglish] Returns whether this player is dominating or is being dominated by the specified player
	//=============================================================================
	bool IsPlayerDominated( int iPlayerIndex );
	bool IsPlayerDominatingMe( int iPlayerIndex );

	virtual void CalcFreezeCamView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov );

	virtual float GetDeathCamInterpolationTime();
	//=============================================================================
	// HPE_END
	//=============================================================================


// Called by shared code.
public:

	// ICSPlayerAnimState overrides.
	virtual CWeaponCSBase* CSAnim_GetActiveWeapon();
	virtual bool CSAnim_CanMove();


	void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );


// Implemented in shared code.
public:
	virtual float GetPlayerMaxSpeed();

	void GetBulletTypeParameters(
		int iBulletType,
		float &fPenetrationPower,
		float &flPenetrationDistance );

	void FireBullet(
		Vector vecSrc,
		const QAngle &shootAngles,
		float flDistance,
		int iPenetration,
		int iBulletType,
		int iDamage,
		float flRangeModifier,
		CBaseEntity *pevAttacker,
		bool bDoEffects,
		float xSpread, float ySpread );

	void KickBack(
		float up_base,
		float lateral_base,
		float up_modifier,
		float lateral_modifier,
		float up_max,
		float lateral_max,
		int direction_change );

	// Returns true if the player is allowed to move.
	bool CanMove() const;

	void OnJump( float fImpulse );
	void OnLand( float fVelocity );

	bool HasC4() const;	// Is this player carrying a C4 bomb?
	bool IsVIP() const;	// Is this player the VIP?

	virtual void SetAnimation( PLAYER_ANIM playerAnim );


public:

	void UpdateIDTarget( void );
	void RemoveAddonModels( void );
	void UpdateMinModels( void );

	void SetActivity( Activity eActivity );
	Activity GetActivity( void ) const;

	ICSPlayerAnimState *GetPlayerAnimState() { return m_PlayerAnimState; }

public:

	ICSPlayerAnimState *m_PlayerAnimState;

	// Used to control animation state.
	Activity m_Activity;

	// Predicted variables.
	CNetworkVar( bool, m_bResumeZoom );
	CNetworkVar( int , m_iLastZoom ); // after firing a shot, set the FOV to 90, and after showing the animation, bring the FOV back to last zoom level.
	CNetworkVar( CSPlayerState, m_iPlayerState );	// SupraFiend: this gives the current state in the joining process, the states are listed above
	CNetworkVar( bool, m_bIsDefusing );			// tracks whether this player is currently defusing a bomb
	CNetworkVar( bool, m_bInBombZone );
	CNetworkVar( bool, m_bInBuyZone );
	CNetworkVar( int, m_iThrowGrenadeCounter );	// used to trigger grenade throw animations.

	bool IsInHostageRescueZone( void );

	// This is a combination of the ADDON_ flags in cs_shareddefs.h.
	CNetworkVar( int, m_iAddonBits );

	// Clients don't know about holstered weapons, so we need to be told about them here
	CNetworkVar( int, m_iPrimaryAddon );
	CNetworkVar( int, m_iSecondaryAddon );
	CNetworkVar(int, m_zombieClass);
	CNetworkVar(int, m_survivorClass);

	// How long the progress bar takes to get to the end. If this is 0, then the progress bar
	// should not be drawn.
	CNetworkVar( int, m_iProgressBarDuration );

	// When the progress bar should start.
	CNetworkVar( float, m_flProgressBarStartTime );

	CNetworkVar( float, m_flStamina );
	CNetworkVar( int, m_iDirection );	// The current lateral kicking direction; 1 = right,  0 = left
	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently
	CNetworkVar( bool, m_bNightVisionOn );
	CNetworkVar( bool, m_bHasNightVision );

    //=============================================================================
    // HPE_BEGIN:
    // [dwenger] Added for fun-fact support
    //=============================================================================

    //CNetworkVar( bool, m_bPickedUpDefuser );
    //CNetworkVar( bool, m_bDefusedWithPickedUpKit );

    //=============================================================================
    // HPE_END
    //=============================================================================

    CNetworkVar( float, m_flVelocityModifier );

	bool		m_bDetected;

	EHANDLE	m_hRagdoll;

	// Survivor incapacitation (networked).
	CNetworkVar( bool, m_bIsIT );
	CNetworkVar( bool, m_bIsGhost );
	CNetworkVar( bool, m_bIncapacitated );
	CNetworkVar( bool, m_bBeingRevived );
	CNetworkVar( int, m_nIncapacitationCount );
	CNetworkVar( bool, m_bIncapBlackAndWhite );

	EHANDLE	m_pounceVictim;
	EHANDLE	m_pounceAttacker;

	EHANDLE	m_chargerVictim;
	EHANDLE	m_chargerAttacker;
	CNetworkVar( int, m_nChargerAction );
	CNetworkVar( int, m_nChargerVictimAction );
	CNetworkVar( int, m_nChargerStaggerDir );
	CNetworkVar( int, m_nDamageStaggerDir );
	CNetworkVar( int, m_nTankAction );

	CWeaponCSBase* GetActiveCSWeapon() const;
	CWeaponCSBase* GetCSWeapon( CSWeaponID id ) const;

	virtual ShadowType_t		ShadowCastType();

#ifdef CS_SHIELD_ENABLED
	bool HasShield( void ) { return m_bHasShield; }
	bool IsShieldDrawn( void ) { return m_bShieldDrawn;	}
	void SetShieldDrawnState( bool bState ) { m_bShieldDrawn = bState; }
#else
	bool HasShield( void ) { return false; }
	bool IsShieldDrawn( void ) { return false; }
	void SetShieldDrawnState( bool bState ) {}
#endif

	float m_flNightVisionAlpha;

	float m_flFlashAlpha;
	float m_flFlashBangTime;
	CNetworkVar( float, m_flFlashMaxAlpha );
	CNetworkVar( float, m_flFlashDuration );

	CNetworkVar(bool, m_bTankDeathInProgress);
	CNetworkVar(float, m_flTankDeathPrevCycle);
	CNetworkVar(int, m_nTankDeathSequence);

	// Having the RecvProxy in the player allows us to keep the var private
	static void RecvProxy_CycleLatch( const CRecvProxyData *pData, void *pStruct, void *pOut );

	// Bots and hostages auto-duck during jumps
	bool m_duckUntilOnGround;

	Vector m_lastStandingPos; // used by the gamemovement code for finding ladders

	void SurpressLadderChecks( const Vector& pos, const Vector& normal );
	bool CanGrabLadder( const Vector& pos, const Vector& normal );
	int GetSurvivorClass() const;
	void SetSurvivorClass(int zombieClass);
	int GetZombieClass() const;
	void SetZombieClass(int zombieClass);

//=============================================================================
// HPE_BEGIN:
//=============================================================================

// [tj] checks if this player has another given player on their Steam friends list.
	bool HasPlayerAsFriend(C_CSPlayer* player);

private:
	CountdownTimer m_ladderSurpressionTimer;
	Vector m_lastLadderNormal;
	Vector m_lastLadderPos;

	bool ShouldCollide(int collisionGroup, int contentsMask) const;

	void UpdateRadar();
	void UpdateSoundEvents();
	void UpdateStaggerThirdPersonCamera();
	void TurnOnStaggerCam();
	void TurnOffStaggerCam();
	void TurnOffStaggerCam_Finish();
	void StaggerCamInterpolation();
	void UpdatePounceThirdPersonCamera();
	void UpdatePounceMusic();
	void UpdateInfectedColorCorrection();
	void UpdateIncapBlackAndWhiteEffects();

	void UpdateInfectedAmbientParticles();
	void StopInfectedAmbientParticles();

	void CreateAddonModel( int i );
	void UpdateAddonModels();

	void PushawayThink();

	int		m_iAccount;
	bool	m_bHasHelmet;
	int		m_iClass;
	int		m_ArmorValue;
	QAngle	m_angEyeAngles;
	bool	m_bHasDefuser;
	bool	m_bInHostageRescueZone;
	float	m_fNextThinkPushAway;

    bool    m_bPlayingFreezeCamSound;

#ifdef CS_SHIELD_ENABLED
	bool	m_bHasShield;
	bool	m_bShieldDrawn;
#endif

	Vector m_vecRagdollVelocity;

	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;

	// ID Target
	int					m_iIDEntIndex;
	CountdownTimer		m_delayTargetIDTimer;

	// Show the ID target after the cursor leaves the entity
	int					m_iOldIDEntIndex;
	CountdownTimer		m_holdTargetIDTimer;

	// Footstep animation events can fire during initial entity creation; suppress for a short time.
	float				m_flFootstepEventSuppressUntil;

	CUtlReference< CNewParticleEffect > m_hSmokerSporeTrail;
	CUtlReference< CNewParticleEffect > m_hSmokerSporeTrailCluster;
	CUtlReference< CNewParticleEffect > m_hSpitterDrool;
	CUtlReference< CNewParticleEffect > m_hSpitterSlimeTrail;

	int m_nLastInfectedParticleZombieClass;
	int m_nLastInfectedParticleTeam;
	bool m_bLastInfectedParticleAlive;
	CHandle<C_BaseCombatWeapon> m_hLastClientActiveWeapon;
	bool m_bLastClientActiveWeaponVisible;
	bool m_bLastClientUsingViewModel;
	CHandle<C_BaseCombatWeapon> m_hPendingDeployAnimationWeapon;
	float m_flPendingDeployAnimationTime;

	bool m_bPounceCamHasSavedState;
	bool m_bPounceCamBaseThirdPerson;
	bool m_bPounceCamBaseForcedThirdPerson;
	bool m_bPounceCamBaseOverridingThirdPerson;
	float m_flPounceCamBlend;
	Vector m_vecPounceCamBaseDesiredOffset;
	bool m_bStaggerCamHasSavedState;
	bool m_bStaggerCamBaseThirdPerson;
	bool m_bStaggerCamBaseForcedThirdPerson;
	bool m_bStaggerCamBaseOverridingThirdPerson;
	bool m_bStaggerCamInterpolating;
	CameraThirdData_t m_StaggerCameraData;
	float m_flStaggerCamCurrentDist;
	float m_flStaggerCamTargetDist;
	float m_flStaggerCamCurrentDistUp;
	float m_flStaggerCamTargetDistUp;
	Vector m_vecStaggerCamBaseDesiredOffset;
	bool m_bLocalPounceMusicPlaying;
	ClientCCHandle_t m_hInfectedColorCorrection;
	bool m_bTriedCreateInfectedColorCorrection;
	ClientCCHandle_t m_hGhostColorCorrection;
	bool m_bTriedCreateGhostColorCorrection;
	bool m_bLocalHeartbeatPlaying;
	ClientCCHandle_t m_hBlackAndWhiteColorCorrection;
	bool m_bTriedCreateBlackAndWhiteColorCorrection;

	void ReleaseFlashlight( void );
	Beam_t	*m_pFlashlightBeam;

	class CCSSoundEvent
	{
	public:
		string_t m_SoundName;
		float m_flEventTime;	// Play the event when gpGlobals->curtime goes past this.
	};
	CUtlLinkedList<CCSSoundEvent,int> m_SoundEvents;


	// This is the list of addons hanging off the guy (grenades, C4, nightvision, etc).
	CUtlLinkedList<CAddonModel, int> m_AddonModels;
	int m_iLastAddonBits;
	int m_iLastPrimaryAddon;
	int m_iLastSecondaryAddon;

	int m_cycleLatch;				// server periodically updates this to fix up our anims, here it is a 4 bit fixed point
	float m_serverIntendedCycle;	// server periodically updates this to fix up our anims, here it is the float we want, or -1 for no override



    //=============================================================================
    // HPE_BEGIN:
    // [tj] Network variables that track who are dominating and being dominated by
    //=============================================================================

    CNetworkArray( bool, m_bPlayerDominated, MAX_PLAYERS+1 );		// array of state per other player whether player is dominating other players
    CNetworkArray( bool, m_bPlayerDominatingMe, MAX_PLAYERS+1 );	// array of state per other player whether other players are dominating this player

    //=============================================================================
    // HPE_END
    //=============================================================================



	C_CSPlayer( const C_CSPlayer & );
};

inline int CCSPlayer::GetZombieClass() const
{
	return m_zombieClass;
}
inline void CCSPlayer::SetZombieClass(int zombieClass)
{
	m_zombieClass = zombieClass;
}

inline int CCSPlayer::GetSurvivorClass() const
{
	return m_survivorClass;
}
inline void CCSPlayer::SetSurvivorClass(int zombieClass)
{
	m_survivorClass = zombieClass;
}
C_CSPlayer* GetLocalOrInEyeCSPlayer( void );

inline C_CSPlayer *ToCSPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

	return dynamic_cast<C_CSPlayer*>( pEntity );
}

namespace vgui
{
	class IImage;
}

vgui::IImage* GetDefaultAvatarImage( C_BasePlayer *pPlayer );




#endif // C_CS_PLAYER_H
