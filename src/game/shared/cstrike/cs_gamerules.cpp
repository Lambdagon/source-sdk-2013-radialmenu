//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The TF Game rules 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "cs_gamerules.h"
#include "cs_ammodef.h"
#include "weapon_csbase.h"
#include "cs_shareddefs.h"
#include "KeyValues.h"
#include "cs_achievement_constants.h"
#include "fmtstr.h"

#ifdef CLIENT_DLL

	#include "networkstringtable_clientdll.h"
	#include "utlvector.h"

#else
	
	#include "bot.h"
	#include "utldict.h"
	#include "cs_player.h"
	#include "cs_team.h"
	#include "recipientfilter.h"
	#include "soundenvelope.h"
	#include "voice_gamemgr.h"
	#include "igamesystem.h"
	#include "weapon_c4.h"
	#include "mapinfo.h"
	#include "shake.h"
	#include "mapentities.h"
	#include "game.h"
	#include "cs_simple_hostage.h"
	#include "cs_gameinterface.h"
	#include "player_resource.h"
	#include "info_view_parameters.h"
	#include "cs_bot_manager.h"
	#include "cs_bot.h"
	#include "eventqueue.h"
	#include "teamplayroundbased_gamerules.h"
	#include "gameweaponmanager.h"
	#include "ai_basenpc.h"
	#include "doors.h"
	#include "BasePropDoor.h"

	#include "cs_gamestats.h"
	#include "cs_urlretrieveprices.h"
	#include "networkstringtable_gamedll.h"
	#include "cs_player_resource.h"
	
#if defined( REPLAY_ENABLED )	
	#include "replay/ireplaysystem.h"
	#include "replay/iserverreplaycontext.h"
	#include "replay/ireplaysessionrecorder.h"
#endif // REPLAY_ENABLED
#include "terror/info_director.h"
#endif


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar survivor_set("survivor_set", "2", FCVAR_ARCHIVE | FCVAR_NOTIFY | FCVAR_REPLICATED, "Which survivor cast to use (1=L4D1, 2=L4D2).", true, 1.0f, true, 2.0f);

#ifndef CLIENT_DLL


#define CS_GAME_STATS_UPDATE 79200 //22 hours
#define CS_GAME_STATS_UPDATE_PERIOD 7200 // 2 hours

extern IUploadGameStats *gamestatsuploader;

#if defined( REPLAY_ENABLED )
extern IReplaySystem *g_pReplay;
#endif // REPLAY_ENABLED

ConVar	sk_plr_dmg_ar2("sk_plr_dmg_ar2", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_ar2("sk_npc_dmg_ar2", "0", FCVAR_REPLICATED);
ConVar	sk_max_ar2("sk_max_ar2", "0", FCVAR_REPLICATED);
ConVar	sk_max_ar2_altfire("sk_max_ar2_altfire", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_alyxgun("sk_plr_dmg_alyxgun", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_alyxgun("sk_npc_dmg_alyxgun", "0", FCVAR_REPLICATED);
ConVar	sk_max_alyxgun("sk_max_alyxgun", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_pistol("sk_plr_dmg_pistol", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_pistol("sk_npc_dmg_pistol", "0", FCVAR_REPLICATED);
ConVar	sk_max_pistol("sk_max_pistol", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_smg1("sk_plr_dmg_smg1", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_smg1("sk_npc_dmg_smg1", "0", FCVAR_REPLICATED);
ConVar	sk_max_smg1("sk_max_smg1", "0", FCVAR_REPLICATED);

// FIXME: remove these
//ConVar	sk_plr_dmg_flare_round	( "sk_plr_dmg_flare_round","0", FCVAR_REPLICATED);
//ConVar	sk_npc_dmg_flare_round	( "sk_npc_dmg_flare_round","0", FCVAR_REPLICATED);
//ConVar	sk_max_flare_round		( "sk_max_flare_round","0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_buckshot("sk_plr_dmg_buckshot", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_buckshot("sk_npc_dmg_buckshot", "0", FCVAR_REPLICATED);
ConVar	sk_max_buckshot("sk_max_buckshot", "0", FCVAR_REPLICATED);
ConVar	sk_plr_num_shotgun_pellets("sk_plr_num_shotgun_pellets", "7", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_rpg_round("sk_plr_dmg_rpg_round", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_rpg_round("sk_npc_dmg_rpg_round", "0", FCVAR_REPLICATED);
ConVar	sk_max_rpg_round("sk_max_rpg_round", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_sniper_round("sk_plr_dmg_sniper_round", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_sniper_round("sk_npc_dmg_sniper_round", "0", FCVAR_REPLICATED);
ConVar	sk_max_sniper_round("sk_max_sniper_round", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_grenade("sk_plr_dmg_grenade", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_grenade("sk_npc_dmg_grenade", "0", FCVAR_REPLICATED);
ConVar	sk_max_grenade("sk_max_grenade", "0", FCVAR_REPLICATED);

#ifdef HL2_EPISODIC
ConVar	sk_max_hopwire("sk_max_hopwire", "3", FCVAR_REPLICATED);
ConVar	sk_max_striderbuster("sk_max_striderbuster", "3", FCVAR_REPLICATED);
#endif

ConVar	sk_plr_dmg_smg1_grenade("sk_plr_dmg_smg1_grenade", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_smg1_grenade("sk_npc_dmg_smg1_grenade", "0", FCVAR_REPLICATED);
ConVar	sk_max_smg1_grenade("sk_max_smg1_grenade", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_357("sk_plr_dmg_357", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_357("sk_npc_dmg_357", "0", FCVAR_REPLICATED);
ConVar	sk_max_357("sk_max_357", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_crossbow("sk_plr_dmg_crossbow", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_crossbow("sk_npc_dmg_crossbow", "0", FCVAR_REPLICATED);
ConVar	sk_max_crossbow("sk_max_crossbow", "0", FCVAR_REPLICATED);

ConVar	sk_dmg_sniper_penetrate_plr("sk_dmg_sniper_penetrate_plr", "0", FCVAR_REPLICATED);
ConVar	sk_dmg_sniper_penetrate_npc("sk_dmg_sniper_penetrate_npc", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_airboat("sk_plr_dmg_airboat", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_airboat("sk_npc_dmg_airboat", "0", FCVAR_REPLICATED);

ConVar	sk_max_gauss_round("sk_max_gauss_round", "0", FCVAR_REPLICATED);

// Gunship & Dropship cannons
ConVar	sk_npc_dmg_gunship("sk_npc_dmg_gunship", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_gunship_to_plr("sk_npc_dmg_gunship_to_plr", "0", FCVAR_REPLICATED);
ConVar	sk_plr_dmg_molotov		( "sk_plr_dmg_molotov","8", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_molotov		( "sk_npc_dmg_molotov","8", FCVAR_REPLICATED);
ConVar	sk_max_molotov			( "sk_max_molotov","1", FCVAR_REPLICATED);

	// Special infected director (server-side): periodically spawn a CT bot near a random Terrorist.
 static ConVar z_special_spawn_interval( "z_special_spawn_interval", "45", FCVAR_GAMEDLL, "Seconds between special infected spawns.", true, 1.0f, true, 600.0f );
static ConVar z_special_spawn_safety_radius( "z_special_spawn_safety_radius", "550", FCVAR_GAMEDLL, "Minimum distance (units) from the survivor when spawning special infected.", true, 0.0f, true, 10000.0f );
static ConVar z_special_spawn_search_radius( "z_special_spawn_search_radius", "3000", FCVAR_GAMEDLL, "How far (units) to search nav areas around the survivor for spawning.", true, 256.0f, true, 20000.0f );
static ConVar z_special_respawn_time( "z_special_respawn_time", "20", FCVAR_GAMEDLL, "Seconds before special infected can respawn after dying.", true, 0.0f, true, 120.0f );
static ConVar z_special_far_cull_distance( "z_special_far_cull_distance", "4500", FCVAR_GAMEDLL, "Special infected bots farther than this from every survivor will be culled after a short grace period.", true, 512.0f, true, 30000.0f );
static ConVar z_special_far_cull_grace( "z_special_far_cull_grace", "5.0", FCVAR_GAMEDLL, "Seconds a special infected bot may remain far from every survivor before it is culled.", true, 0.0f, true, 120.0f );
 static ConVar z_special_kick_dead_delay( "z_special_kick_dead_delay", "5", FCVAR_GAMEDLL, "Seconds after death before a special infected bot is kicked.", true, 0.0f, true, 60.0f );
 static ConVar z_special_spawn_retry_delay( "z_special_spawn_retry_delay", "1.0", FCVAR_GAMEDLL, "Seconds before retrying a failed special infected spawn attempt.", true, 0.1f, true, 30.0f );

// Tank director: spawn tanks near authored map anchors when survivors approach them.
static ConVar z_special_tank_spawn_enabled( "z_special_tank_spawn_enabled", "1", FCVAR_GAMEDLL, "If 1, tanks can spawn near authored tank spawn anchors when survivors approach them." );
static ConVar z_special_tank_spawn_cooldown( "z_special_tank_spawn_cooldown", "180", FCVAR_GAMEDLL, "Minimum seconds between successful tank spawns.", true, 0.0f, true, 3600.0f );
static ConVar z_special_tank_spawn_chance( "z_special_tank_spawn_chance", "0.15", FCVAR_GAMEDLL, "Chance that an eligible natural tank spawn opportunity produces a tank.", true, 0.0f, true, 1.0f );
static ConVar z_tank_spawn_proximity_radius( "z_tank_spawn_proximity_radius", "5600", FCVAR_GAMEDLL, "How close survivors must be to a tank spawn anchor before a tank can spawn there.", true, 128.0f, true, 10000.0f );
static ConVar z_tank_spawn_search_radius( "z_tank_spawn_search_radius", "4500", FCVAR_GAMEDLL, "How far from a tank spawn anchor to search for a valid tank spawn point.", true, 128.0f, true, 5000.0f );

// Tank human takeover: if a tank bot exists and there are human infected, hand the tank to a random human.
static ConVar z_tank_human_takeover_enabled( "z_tank_human_takeover_enabled", "1", FCVAR_GAMEDLL, "If 1, when a tank bot exists and there are human infected players, a random human is told they'll become the tank and takes over within a short delay." );
static ConVar z_tank_human_takeover_delay( "z_tank_human_takeover_delay", "5.0", FCVAR_GAMEDLL, "Seconds between notifying a human infected that they're becoming the tank and actually handing them the tank.", true, 0.0f, true, 30.0f );

// Listen server quality-of-life: when the host joins, auto-join Terrorists and spawn survivor bots.
static ConVar sv_listen_host_autojoin_survivors( "sv_listen_host_autojoin_survivors", "1", FCVAR_GAMEDLL, "If 1, the listen-server host is auto-placed on Terrorists and spawns survivor bots." );
static ConVar sv_listen_host_autojoin_survivor_bots( "sv_listen_host_autojoin_survivor_bots", "3", FCVAR_GAMEDLL, "Number of survivor bots to spawn on the host's team when the listen-server host joins.", true, 0.0f, true, 32.0f );
static ConVar sv_listen_host_autojoin_bot_retry_delay( "sv_listen_host_autojoin_bot_retry_delay", "0.5", FCVAR_GAMEDLL, "Seconds between retrying to spawn survivor bots for listen-server host auto-join.", true, 0.05f, true, 5.0f );

// Common infected director (server-side): keep a population of NPC infected near the survivors.
static ConVar z_spawn_enabled( "z_spawn_enabled", "1", FCVAR_GAMEDLL, "If 1, the director spawns common infected NPCs near survivors." );
static ConVar z_horde_interval_min( "z_horde_interval_min", "60", FCVAR_GAMEDLL, "Minimum seconds between common infected horde events.", true, 1.0f, true, 3600.0f );
static ConVar z_horde_interval_max( "z_horde_interval_max", "240", FCVAR_GAMEDLL, "Maximum seconds between common infected horde events.", true, 1.0f, true, 3600.0f );
static ConVar z_horde_duration_min( "z_horde_duration_min", "60", FCVAR_GAMEDLL, "Minimum seconds a common infected horde lasts.", true, 1.0f, true, 3600.0f );
static ConVar z_horde_duration_max( "z_horde_duration_max", "240", FCVAR_GAMEDLL, "Maximum seconds a common infected horde lasts.", true, 1.0f, true, 3600.0f );
static ConVar z_horde_spawn_batch( "z_horde_spawn_batch", "40", FCVAR_GAMEDLL, "Maximum number of common infected to spawn per batch while a horde is active.", true, 1.0f, true, 128.0f );
static ConVar z_horde_spawn_safety_radius( "z_horde_spawn_safety_radius", "250", FCVAR_GAMEDLL, "Minimum distance (units) from every survivor when spawning common infected for a horde.", true, 0.0f, true, 20000.0f );
static ConVar z_spawn_radius( "z_spawn_radius", "3000", FCVAR_GAMEDLL, "How far (units) to search nav areas around a survivor for spawning common infected.", true, 256.0f, true, 20000.0f );
static ConVar z_spawn_safety_radius( "z_spawn_safety_radius", "350", FCVAR_GAMEDLL, "Minimum distance (units) from a survivor when spawning common infected.", true, 0.0f, true, 10000.0f );
static ConVar z_common_max( "z_common_limit", "40", FCVAR_GAMEDLL, "Maximum number of common infected NPCs on the map.", true, 0.0f, true, 200.0f );
static ConVar z_spawn_interval( "z_spawn_interval", "0.35", FCVAR_GAMEDLL, "Seconds between common infected spawn batches.", true, 0.05f, true, 10.0f );
static ConVar z_spawn_batch( "z_spawn_batch", "20", FCVAR_GAMEDLL, "Maximum number of common infected to spawn per spawn batch.", true, 1.0f, true, 30.0f );
static ConVar z_spawn_require_hidden( "z_spawn_require_hidden", "1", FCVAR_GAMEDLL, "If 1, common infected spawn points must be hidden from survivor line-of-sight." );

// Background/logo maps: populate with wandering infected even when there are no survivors.
static ConVar z_background_populate_enabled( "z_background_populate_enabled", "1", FCVAR_GAMEDLL, "If 1 and the map is a background/logo map, populate it with wandering infected." );
static ConVar z_background_spawn_radius( "z_background_spawn_radius", "2500", FCVAR_GAMEDLL, "Radius (units) around background/logo anchors to spawn wandering infected.", true, 128.0f, true, 20000.0f );
static ConVar z_background_spawn_interval( "z_background_spawn_interval", "0.25", FCVAR_GAMEDLL, "Seconds between background/logo infected spawn batches.", true, 0.05f, true, 10.0f );
static ConVar z_background_special_limit( "z_background_special_limit", "6", FCVAR_GAMEDLL, "Maximum number of special infected bots to keep on background/logo maps.", true, 0.0f, true, 32.0f );

// Survivor squad: keep survivor bots following a leader.
static ConVar survivor_squad_enabled( "survivor_squad_enabled", "1", FCVAR_GAMEDLL, "If 1, survivor bots form a squad and follow a leader (prefer a human player)." );
static ConVar survivor_squad_update_interval( "survivor_squad_update_interval", "0.1", FCVAR_GAMEDLL, "Seconds between survivor squad follow updates.", true, 0.05f, true, 10.0f );

// Saferoom transition: when survivors close the authored saferoom door inside a func_hostage_rescue brush,
// transition to the map's enabled info_changelevel after a short delay.
static ConVar sv_saferoom_change_delay( "sv_saferoom_change_delay", "4.0", FCVAR_GAMEDLL, "Seconds all alive survivors must remain in the saferoom with the door closed before changing to the next level.", true, 0.0f, true, 30.0f );

static float s_flNextSpecialInfectedSpawn = 0.0f;
static float s_flNextTankSpawnAllowed = 0.0f;
static float s_flSpecialInfectedFarCullStartTime[ MAX_PLAYERS + 1 ] = { 0.0f };

static EHANDLE s_hTankTakeoverTankBot;
static EHANDLE s_hTankTakeoverHuman;
static float s_flTankTakeoverExecuteTime = 0.0f;
static int s_nTankTakeoverReservedZombieClass = 0;

static float s_flTankTakeoverReplacementRetryTime = 0.0f;
static float s_flTankTakeoverReplacementExpireTime = 0.0f;
static int s_nTankTakeoverReplacementZombieClass = 0;
static Vector s_vecTankTakeoverReplacementPos( vec3_origin );
static QAngle s_angTankTakeoverReplacementAng( 0, 0, 0 );

static bool s_bListenHostAutoJoinDone = false;
static int s_nListenHostAutoJoinBotsRemaining = 0;
static float s_flListenHostAutoJoinNextBotTry = 0.0f;

static float s_flNextCommonInfectedSpawn = 0.0f;
static float s_flNextCommonHordeStart = 0.0f;
static float s_flCommonHordeEndTime = 0.0f;
static bool s_bCommonHordeSpawnPosValid = false;
static bool s_bCommonHordeInitialBurstDone = false;
static Vector s_vecCommonHordeSpawnPos( vec3_origin );
static QAngle s_angCommonHordeSpawnAng( 0, 0, 0 );
static float s_flNextSurvivorSquadUpdate = 0.0f;
static EHANDLE s_hSurvivorSquadLeader;

static float s_flNextBackgroundPopulate = 0.0f;
static float s_flSaferoomTransitionStart = 0.0f;
static EHANDLE s_hSaferoomTransitionZone;
static EHANDLE s_hSaferoomChangelevel;

// "Safe room" mission start/leave music (No Mercy).
static CSoundPatch *s_pMissionStartNoMercyMusic[MAX_PLAYERS + 1] = { NULL };
static bool s_bSurvivorsLeftSpawnOnce[MAX_PLAYERS + 1] = { false };
static bool s_bMissionStartNoMercyFinished = false;
static float s_flNextLeavingSafetyCheck = 0.0f;
static CUtlVector< Vector > s_vecSurvivorSpawnPoints;
static bool s_bSurvivorSpawnPointsBuilt = false;

// "IT" survivors (covered in boomer vomit) that infected should focus.
static EHANDLE s_hItSurvivor;
static EHANDLE s_hItSurvivors[ MAX_PLAYERS + 1 ];
static float s_flItExpireTimeByPlayer[ MAX_PLAYERS + 1 ] = { 0.0f };

 static CCSPlayer *SelectRandomAliveSurvivorT( void )
 {
	CUtlVector< CCSPlayer * > survivors;
	survivors.EnsureCapacity( 16 );

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( player->GetTeamNumber() != TEAM_TERRORIST )
			continue;

		if ( !player->IsAlive() )
			continue;

		survivors.AddToTail( player );
	}

	if ( survivors.Count() <= 0 )
		return NULL;

 	return survivors[ random->RandomInt( 0, survivors.Count() - 1 ) ];
 }

static void ResetLeavingSafetyMusicState()
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
	for ( int i = 1; i < ARRAYSIZE( s_pMissionStartNoMercyMusic ); ++i )
	{
		if ( s_pMissionStartNoMercyMusic[i] )
		{
			controller.SoundDestroy( s_pMissionStartNoMercyMusic[i] );
			s_pMissionStartNoMercyMusic[i] = NULL;
		}

		s_bSurvivorsLeftSpawnOnce[i] = false;
	}

	s_flNextLeavingSafetyCheck = 0.0f;
	s_bMissionStartNoMercyFinished = false;
	s_vecSurvivorSpawnPoints.RemoveAll();
	s_bSurvivorSpawnPointsBuilt = false;
}

static void BuildSurvivorSpawnPointsIfNeeded()
{
	if ( s_bSurvivorSpawnPointsBuilt )
		return;

	s_vecSurvivorSpawnPoints.RemoveAll();

	CBaseEntity *pEnt = NULL;
	while ( ( pEnt = gEntList.FindEntityByClassname( pEnt, "info_player_terrorist" ) ) != NULL )
	{
		s_vecSurvivorSpawnPoints.AddToTail( pEnt->GetAbsOrigin() );
	}

	// As a last resort, use current survivor positions as "spawn points" so the
	// 200-unit threshold still behaves sensibly on non-CS maps.
	if ( s_vecSurvivorSpawnPoints.Count() <= 0 )
	{
		for ( int i = 1; i <= gpGlobals->maxClients; ++i )
		{
			CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
			if ( !pPlayer )
				continue;

			if ( pPlayer->GetTeamNumber() != TEAM_SURVIVOR )
				continue;

			if ( !pPlayer->IsAlive() )
				continue;

			s_vecSurvivorSpawnPoints.AddToTail( pPlayer->GetAbsOrigin() );
		}
	}

	s_bSurvivorSpawnPointsBuilt = true;
}

static bool HasSurvivorLeftSpawnPoints( CCSPlayer *pPlayer, float thresholdUnits )
{
	if ( !pPlayer || s_vecSurvivorSpawnPoints.Count() <= 0 )
		return false;

	const float thresholdSqr = thresholdUnits * thresholdUnits;

	float closestSqr = FLT_MAX;
	const Vector pos = pPlayer->GetAbsOrigin();
	for ( int s = 0; s < s_vecSurvivorSpawnPoints.Count(); ++s )
	{
		const float distSqr = ( pos - s_vecSurvivorSpawnPoints[ s ] ).LengthSqr();
		if ( distSqr < closestSqr )
			closestSqr = distSqr;
	}

	return closestSqr > thresholdSqr;
}

static bool HaveAllLivingSurvivorsLeftSpawnPoints( float thresholdUnits )
{
	bool foundLivingSurvivor = false;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer || pPlayer->GetTeamNumber() != TEAM_SURVIVOR || !pPlayer->IsAlive() )
			continue;

		foundLivingSurvivor = true;
		if ( !HasSurvivorLeftSpawnPoints( pPlayer, thresholdUnits ) )
			return false;
	}

	return foundLivingSurvivor;
}

static void UpdateLeavingSafetyMusic_NoMercy()
{
	BuildSurvivorSpawnPointsIfNeeded();

	// Start mission music per-player, but keep it active until the whole living survivor team leaves spawn.
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer )
		{
			if ( s_pMissionStartNoMercyMusic[i] )
			{
				CSoundEnvelopeController::GetController().SoundDestroy( s_pMissionStartNoMercyMusic[i] );
				s_pMissionStartNoMercyMusic[i] = NULL;
			}
			s_bSurvivorsLeftSpawnOnce[i] = false;
			continue;
		}

		if ( pPlayer->GetTeamNumber() != TEAM_SURVIVOR )
		{
			if ( s_pMissionStartNoMercyMusic[i] )
			{
				CSoundEnvelopeController::GetController().SoundDestroy( s_pMissionStartNoMercyMusic[i] );
				s_pMissionStartNoMercyMusic[i] = NULL;
			}
			s_bSurvivorsLeftSpawnOnce[i] = false;
			continue;
		}

		// Late-joiners (or edge cases) who are already away from spawn shouldn't start the safe-room music.
		if ( !s_bSurvivorsLeftSpawnOnce[i] && HasSurvivorLeftSpawnPoints( pPlayer, 200.0f ) )
		{
			s_bSurvivorsLeftSpawnOnce[i] = true;
		}

		if ( !pPlayer->IsAlive() )
			continue;

		if ( s_bMissionStartNoMercyFinished )
			continue;

		if ( s_bSurvivorsLeftSpawnOnce[i] )
			continue;

		if ( s_pMissionStartNoMercyMusic[i] )
			continue;

		CSingleUserRecipientFilter filter( pPlayer );
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		s_pMissionStartNoMercyMusic[i] = controller.SoundCreate( filter, SOUND_FROM_WORLD, CHAN_STATIC, "Event.MissionStart_NoMercy", ATTN_NONE );
		if ( s_pMissionStartNoMercyMusic[i] )
		{
			controller.Play( s_pMissionStartNoMercyMusic[i], 1.0f, 100 );
		}
	}

	if ( gpGlobals->curtime < s_flNextLeavingSafetyCheck )
		return;

	s_flNextLeavingSafetyCheck = gpGlobals->curtime + 0.25f;

	if ( s_bMissionStartNoMercyFinished )
		return;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		if ( s_bSurvivorsLeftSpawnOnce[i] )
			continue;

		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer || pPlayer->GetTeamNumber() != TEAM_SURVIVOR || !pPlayer->IsAlive() )
			continue;

		if ( !HasSurvivorLeftSpawnPoints( pPlayer, 200.0f ) )
			continue;

		s_bSurvivorsLeftSpawnOnce[i] = true;
	}

	if ( !HaveAllLivingSurvivorsLeftSpawnPoints( 200.0f ) )
		return;

	s_bMissionStartNoMercyFinished = true;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( s_pMissionStartNoMercyMusic[i] )
		{
			CSoundEnvelopeController::GetController().SoundFadeOut( s_pMissionStartNoMercyMusic[i], 2.0f, true );
			s_pMissionStartNoMercyMusic[i] = NULL;
		}

		if ( !pPlayer || pPlayer->GetTeamNumber() != TEAM_SURVIVOR || !pPlayer->IsAlive() )
			continue;

		CSingleUserRecipientFilter filter( pPlayer );
		CBaseEntity::EmitSound( filter, SOUND_FROM_WORLD, "Event.LeavingSafety_NoMercy" );
	}
}

static void TriggerLogicRelayByName( const char *pszTargetName )
{
	if ( !pszTargetName || !pszTargetName[0] )
		return;

	variant_t emptyVariant;
	CBaseEntity *pRelay = NULL;
	while ( ( pRelay = gEntList.FindEntityByName( pRelay, pszTargetName ) ) != NULL )
	{
		pRelay->AcceptInput( "Trigger", NULL, NULL, emptyVariant, 0 );
	}
}

static void ResetSaferoomTransitionState()
{
	s_flSaferoomTransitionStart = 0.0f;
	s_hSaferoomTransitionZone = NULL;
	s_hSaferoomChangelevel = NULL;
}

static bool IsCampaignSaferoomMap( const CCSGameRules *pRules )
{
	return pRules && pRules->IsHostageRescueMap();
}

static bool HasAnySurvivorLeftSpawnOnce( void )
{
	for ( int i = 1; i < ARRAYSIZE( s_bSurvivorsLeftSpawnOnce ); ++i )
	{
		if ( s_bSurvivorsLeftSpawnOnce[i] )
			return true;
	}

	return false;
}

bool IsEntityInsideSaferoomZone( CBaseEntity *pZone, CBaseEntity *pEntity )
{
	if ( !pZone || !pEntity )
		return false;

	if ( pZone->Intersects( pEntity ) )
		return true;

	return pZone->CollisionProp()->IsPointInBounds( pEntity->WorldSpaceCenter() );
}

bool IsEntityInsideSaferoomEndZone( CBaseEntity *pZone, CBaseEntity *pChangelevel, CBaseEntity *pEntity )
{
	if ( IsEntityInsideSaferoomZone( pZone, pEntity ) )
		return true;

	if ( pChangelevel && pChangelevel != pZone && IsEntityInsideSaferoomZone( pChangelevel, pEntity ) )
		return true;

	return false;
}

static CBaseEntity *FindSaferoomChangeLevelEntity( void )
{
	return gEntList.FindEntityByClassname( NULL, "info_changelevel" );
}

static CBaseEntity *FindSaferoomZoneForChangelevel( CBaseEntity *pChangelevel )
{
	if ( !pChangelevel )
		return NULL;

	CBaseEntity *pBestZone = pChangelevel;
	float flBestDistSqr = FLT_MAX;
	CBaseEntity *pZone = NULL;

	while ( ( pZone = gEntList.FindEntityByClassname( pZone, "func_hostage_rescue" ) ) != NULL )
	{
		if ( IsEntityInsideSaferoomZone( pZone, pChangelevel ) )
			return pZone;

		const float flDistSqr = ( pZone->WorldSpaceCenter() - pChangelevel->GetAbsOrigin() ).LengthSqr();
		if ( flDistSqr < flBestDistSqr )
		{
			flBestDistSqr = flDistSqr;
			pBestZone = pZone;
		}
	}

	return pBestZone;
}

bool IsCheckpointSaferoomDoorModel( const char *pszModelName )
{
	if ( !pszModelName || !pszModelName[0] )
		return false;

	return Q_stricmp( pszModelName, "models/props_doors/checkpoint_door_02.mdl" ) == 0 ||
		   Q_stricmp( pszModelName, "models/props_doors/checkpoint_door_-02.mdl" ) == 0;
}

static bool IsSaferoomDoorClosed( CBaseEntity *pZone, CBaseEntity *pChangelevel )
{
	if ( !pZone && !pChangelevel )
		return false;

	CBaseEntity *pDoorEntity = NULL;
	while ( ( pDoorEntity = gEntList.FindEntityByClassname( pDoorEntity, "prop_door_rotating_checkpoint" ) ) != NULL )
	{
		CBasePropDoor *pDoor = dynamic_cast< CBasePropDoor * >( pDoorEntity );
		if ( !pDoor || !IsCheckpointSaferoomDoorModel( STRING( pDoorEntity->GetModelName() ) ) )
			continue;

		if ( !IsEntityInsideSaferoomEndZone( pZone, pChangelevel, pDoorEntity ) )
			continue;

		if ( pDoor->IsDoorClosed() )
			return true;
	}

	pDoorEntity = NULL;
	while ( ( pDoorEntity = gEntList.FindEntityByClassname( pDoorEntity, "func_door" ) ) != NULL )
	{
		CBaseDoor *pDoor = dynamic_cast< CBaseDoor * >( pDoorEntity );
		if ( !pDoor || !IsEntityInsideSaferoomEndZone( pZone, pChangelevel, pDoorEntity ) )
			continue;

		if ( pDoor->m_toggle_state == TS_AT_BOTTOM )
			return true;
	}

	pDoorEntity = NULL;
	while ( ( pDoorEntity = gEntList.FindEntityByClassname( pDoorEntity, "prop_door_rotating" ) ) != NULL )
	{
		CBasePropDoor *pDoor = dynamic_cast< CBasePropDoor * >( pDoorEntity );
		if ( !pDoor || !IsEntityInsideSaferoomEndZone( pZone, pChangelevel, pDoorEntity ) )
			continue;

		if ( pDoor->IsDoorClosed() )
			return true;
	}

	return false;
}

static bool AreAllAliveSurvivorsInSaferoomEndZone( CBaseEntity *pZone, CBaseEntity *pChangelevel, CCSPlayer **ppActivator )
{
	if ( ppActivator )
	{
		*ppActivator = NULL;
	}

	if ( !pZone && !pChangelevel )
		return false;

	int nAliveSurvivors = 0;
	CCSPlayer *pFirstSurvivor = NULL;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer || pPlayer->GetTeamNumber() != TEAM_SURVIVOR || !pPlayer->IsAlive() )
			continue;

		++nAliveSurvivors;
		if ( !pFirstSurvivor )
		{
			pFirstSurvivor = pPlayer;
		}

		if ( !IsEntityInsideSaferoomEndZone( pZone, pChangelevel, pPlayer ) )
			return false;
	}

	if ( ppActivator )
	{
		*ppActivator = pFirstSurvivor;
	}

	return nAliveSurvivors > 0;
}

bool CCSGameRules::IsPlayerInSaferoom( CCSPlayer *player ) const
{
	if ( !player || player->GetTeamNumber() != TEAM_SURVIVOR || !player->IsAlive() )
		return false;

	if ( !IsCampaignSaferoomMap( this ) )
		return false;

	CBaseEntity *pChangelevel = FindSaferoomChangeLevelEntity();
	CBaseEntity *pZone = FindSaferoomZoneForChangelevel( pChangelevel );
	if ( IsEntityInsideSaferoomEndZone( pZone, pChangelevel, player ) )
		return true;

	BuildSurvivorSpawnPointsIfNeeded();
	return !HasSurvivorLeftSpawnPoints( player, 200.0f );
}

static bool AreAllSpawnedSurvivorsDeadOrIncapacitated( void )
{
	bool bFoundSpawnedSurvivor = false;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer || pPlayer->GetTeamNumber() != TEAM_SURVIVOR )
			continue;

		if ( pPlayer->State_Get() == STATE_PICKINGCLASS )
			continue;

		bFoundSpawnedSurvivor = true;

		if ( !pPlayer->IsAlive() )
			continue;

		if ( pPlayer->IsIncapacitated() )
			continue;

		return false;
	}

	return bFoundSpawnedSurvivor;
}

static bool HasHumanInfectedPlayers( void )
{
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer || pPlayer->IsBot() )
			continue;

		if ( pPlayer->GetTeamNumber() == TEAM_INFECTED )
			return true;
	}

	return false;
}

static void SwapSurvivorAndInfectedTeams( void )
{
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer )
			continue;

		if ( pPlayer->GetTeamNumber() == TEAM_SURVIVOR )
		{
			pPlayer->SwitchTeam( TEAM_INFECTED );
		}
		else if ( pPlayer->GetTeamNumber() == TEAM_INFECTED )
		{
			pPlayer->SwitchTeam( TEAM_SURVIVOR );
		}
	}
}

static void AwardCampaignRoundWin( CCSGameRules *pRules, int iWinningTeam, float flDelay, bool bNeededPlayers, bool bAwardCash )
{
	if ( !pRules || pRules->m_iRoundWinStatus != WINNER_NONE )
		return;

	if ( iWinningTeam == TEAM_SURVIVOR )
	{
		if ( bAwardCash )
		{
			pRules->m_iAccountTerrorist += 3000;
		}

		if ( !bNeededPlayers )
		{
			++pRules->m_iNumTerroristWins;
			pRules->UpdateTeamScores();
		}

		pRules->TerminateRound( flDelay, Terrorists_Win );
		return;
	}

	if ( iWinningTeam == TEAM_INFECTED )
	{
		if ( bAwardCash )
		{
			pRules->m_iAccountCT += 3000;
		}

		if ( !bNeededPlayers )
		{
			++pRules->m_iNumCTWins;
			pRules->UpdateTeamScores();
		}

		pRules->TerminateRound( flDelay, CTs_Win );
	}
}

static void SaferoomTransitionThink( CCSGameRules *pRules, bool bIsRestartingRound )
{
	if ( !gpGlobals || !IsCampaignSaferoomMap( pRules ) || bIsRestartingRound || pRules->IsFreezePeriod() )
	{
		ResetSaferoomTransitionState();
		return;
	}

	if ( !HasAnySurvivorLeftSpawnOnce() )
	{
		ResetSaferoomTransitionState();
		return;
	}

	CBaseEntity *pChangelevel = FindSaferoomChangeLevelEntity();
	CBaseEntity *pZone = FindSaferoomZoneForChangelevel( pChangelevel );
	if ( !pChangelevel )
	{
		ResetSaferoomTransitionState();
		return;
	}

	if ( !IsSaferoomDoorClosed( pZone, pChangelevel ) )
	{
		ResetSaferoomTransitionState();
		return;
	}

	CCSPlayer *pActivator = NULL;
	if ( !AreAllAliveSurvivorsInSaferoomEndZone( pZone, pChangelevel, &pActivator ) )
	{
		ResetSaferoomTransitionState();
		return;
	}

	if ( s_hSaferoomTransitionZone.Get() != pZone || s_hSaferoomChangelevel.Get() != pChangelevel )
	{
		s_flSaferoomTransitionStart = 0.0f;
		s_hSaferoomTransitionZone = pZone;
		s_hSaferoomChangelevel = pChangelevel;
	}

	if ( s_flSaferoomTransitionStart <= 0.0f )
	{
		s_flSaferoomTransitionStart = gpGlobals->curtime;
		return;
	}

	if ( gpGlobals->curtime < ( s_flSaferoomTransitionStart + sv_saferoom_change_delay.GetFloat() ) )
		return;

	if ( HasHumanInfectedPlayers() )
	{
		SwapSurvivorAndInfectedTeams();
	}

	AwardCampaignRoundWin( pRules, TEAM_SURVIVOR, 0.0f, false, false );

	variant_t emptyVariant;
	pChangelevel->AcceptInput( "ChangeLevel", pActivator, pZone, emptyVariant, 0 );
	ResetSaferoomTransitionState();
}

static bool IsValidItSurvivor( CCSPlayer *player )
{
	if ( !player )
		return false;

	if ( player->GetTeamNumber() != TEAM_TERRORIST )
		return false;

	if ( !player->IsAlive() )
		return false;

	return true;
}

static void ClearAllItSurvivors( void )
{
	for ( int i = 0; i < ARRAYSIZE( s_flItExpireTimeByPlayer ); ++i )
	{
		CCSPlayer *player = ToCSPlayer( s_hItSurvivors[i].Get() );
		if ( player )
		{
			player->m_bIsIT = false;
		}

		s_hItSurvivors[i] = NULL;
		s_flItExpireTimeByPlayer[i] = 0.0f;
	}

	s_hItSurvivor = NULL;
}

static bool IsAliveItSurvivorActive( CCSPlayer *player )
{
	if ( !gpGlobals || !IsValidItSurvivor( player ) )
		return false;

	const int playerIndex = player->entindex();
	if ( playerIndex <= 0 || playerIndex >= ARRAYSIZE( s_flItExpireTimeByPlayer ) )
		return false;

	if ( ToCSPlayer( s_hItSurvivors[ playerIndex ].Get() ) != player )
		return false;

	const float flExpireTime = s_flItExpireTimeByPlayer[ playerIndex ];
	return flExpireTime > gpGlobals->curtime;
}

static void PruneExpiredItSurvivors( void )
{
	if ( !gpGlobals )
	{
		ClearAllItSurvivors();
		return;
	}

	for ( int i = 1; i < ARRAYSIZE( s_flItExpireTimeByPlayer ); ++i )
	{
		if ( s_flItExpireTimeByPlayer[i] <= 0.0f )
			continue;

		CCSPlayer *player = ToCSPlayer( s_hItSurvivors[i].Get() );
		if ( !IsAliveItSurvivorActive( player ) )
		{
			if ( player )
			{
				player->m_bIsIT = false;
			}

			s_hItSurvivors[i] = NULL;
			s_flItExpireTimeByPlayer[i] = 0.0f;
		}
	}

	CCSPlayer *preferred = ToCSPlayer( s_hItSurvivor.Get() );
	if ( !IsAliveItSurvivorActive( preferred ) )
	{
		s_hItSurvivor = NULL;
	}
}

static CCSPlayer *GetAliveItSurvivorOrNull( void )
{
	PruneExpiredItSurvivors();

	CCSPlayer *preferred = ToCSPlayer( s_hItSurvivor.Get() );
	if ( IsAliveItSurvivorActive( preferred ) )
		return preferred;

	for ( int i = 1; i < ARRAYSIZE( s_flItExpireTimeByPlayer ); ++i )
	{
		if ( s_flItExpireTimeByPlayer[i] <= 0.0f )
			continue;

		CCSPlayer *player = ToCSPlayer( s_hItSurvivors[i].Get() );
		if ( !IsAliveItSurvivorActive( player ) )
			continue;

		s_hItSurvivor = player;
		return player;
	}

	return NULL;
}

void CCSGameRules::SetItTarget( CCSPlayer *target, float durationSeconds )
{
	if ( !gpGlobals )
	{
		ClearAllItSurvivors();
		return;
	}

	if ( !target )
	{
		ClearAllItSurvivors();
		return;
	}

	if ( !IsValidItSurvivor( target ) )
		return;

	const int playerIndex = target->entindex();
	if ( playerIndex <= 0 || playerIndex >= ARRAYSIZE( s_flItExpireTimeByPlayer ) )
		return;

	s_hItSurvivor = target;
	s_hItSurvivors[ playerIndex ] = target;
	s_flItExpireTimeByPlayer[ playerIndex ] = gpGlobals->curtime + MAX( 0.1f, durationSeconds );
	target->m_bIsIT = true;
}

CCSPlayer *CCSGameRules::GetItTarget( void ) const
{
	return GetAliveItSurvivorOrNull();
}

bool CCSGameRules::IsItTarget( CCSPlayer *target ) const
{
	PruneExpiredItSurvivors();
	return IsAliveItSurvivorActive( target );
}

bool CCSGameRules::IsItActive( void ) const
{
	return GetAliveItSurvivorOrNull() != NULL;
}

static int CollectAliveSurvivorsT( CUtlVector< CCSPlayer * > &outSurvivors )
{
	outSurvivors.RemoveAll();
	outSurvivors.EnsureCapacity( 16 );

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( player->GetTeamNumber() != TEAM_TERRORIST )
			continue;

		if ( !player->IsAlive() )
			continue;

		outSurvivors.AddToTail( player );
	}

	return outSurvivors.Count();
}

static bool GetAliveSurvivorCenter( Vector *outCenter )
{
	if ( !outCenter )
		return false;

	Vector sum( vec3_origin );
	int count = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( player->GetTeamNumber() != TEAM_TERRORIST )
			continue;

		if ( !player->IsAlive() )
			continue;

		sum += player->GetAbsOrigin();
		++count;
	}

	if ( count <= 0 )
		return false;

	*outCenter = sum * ( 1.0f / (float)count );
	return true;
}

static bool IsAnyAliveTank( void )
{
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( !player->IsAlive() )
			continue;

		if ( player->GetTeamNumber() != TEAM_CT )
			continue;

		if ( player->GetZombieClass() == 8 )
			return true;
	}

	return false;
}

static CCSPlayer *FindAliveTankBot( void )
{
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( !player->IsAlive() )
			continue;

		if ( player->GetTeamNumber() != TEAM_CT )
			continue;

		if ( !player->IsBot() )
			continue;

		if ( player->GetZombieClass() != 8 )
			continue;

		return player;
	}

	return NULL;
}

static CCSPlayer *SelectRandomAliveHumanInfected( void )
{
	CUtlVector< CCSPlayer * > humans;
	humans.EnsureCapacity( 8 );

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( !player->IsAlive() )
			continue;

		if ( player->GetTeamNumber() != TEAM_CT )
			continue;

		if ( player->IsBot() )
			continue;

		// Don't pick an existing tank player.
		if ( player->GetZombieClass() == 8 )
			continue;

		humans.AddToTail( player );
	}

	if ( humans.Count() <= 0 )
		return NULL;

	return humans[ random->RandomInt( 0, humans.Count() - 1 ) ];
}

static CCSPlayer *SelectRandomHumanInfected( bool bRequireAlive )
{
	CUtlVector< CCSPlayer * > humans;
	humans.EnsureCapacity( 8 );

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( bRequireAlive && !player->IsAlive() )
			continue;

		if ( player->GetTeamNumber() != TEAM_CT )
			continue;

		if ( player->IsBot() )
			continue;

		if ( player->GetZombieClass() == 8 )
			continue;

		humans.AddToTail( player );
	}

	if ( humans.Count() <= 0 )
		return NULL;

	return humans[ random->RandomInt( 0, humans.Count() - 1 ) ];
}

static void ApplyTankLoadout( CCSPlayer *player, int desiredHealth )
{
	if ( !player )
		return;

	player->SetSpecialInfected( true );
	player->SetSpecialInfectedDeathTimestamp( 0.0f );
	player->SetGhost( false );
	player->ClearPounce();
	player->ClearCharger();
	player->ClearDamageStagger();
	player->ClearTankRockThrow();
	player->SetSurvivorClass( 0 );
	player->SetZombieClass( 8 );
	if ( survivor_set.GetInt() == 1 )
	{
		player->SetModel( "models/infected/hulk_l4d1.mdl" );
	}
	else
	{
		player->SetModel( "models/infected/hulk.mdl" );
	}

	const int maxHealth = 6000;
	player->SetMaxHealth( maxHealth );
	player->SetHealth( clamp( desiredHealth > 0 ? desiredHealth : maxHealth, 1, maxHealth ) );
	player->SetMaxSpeed( 210 );

	player->RemoveAllWeapons();
	player->GiveNamedItem( "weapon_tank_claw" );
}


static void ClearTankTakeoverState( void )
{
	s_hTankTakeoverTankBot = NULL;
	s_hTankTakeoverHuman = NULL;
	s_flTankTakeoverExecuteTime = 0.0f;
	s_nTankTakeoverReservedZombieClass = 0;
}

static bool IsHiddenSpawnFromSurvivorLOS( CCSPlayer *survivor, const Vector &spawnPos )
{
	trace_t tr;
	CTraceFilterSimple filter( survivor, COLLISION_GROUP_NONE );
	UTIL_TraceLine( survivor->EyePosition(), spawnPos + Vector( 0, 0, 36.0f ), MASK_BLOCKLOS, &filter, &tr );

	// If the trace is blocked before reaching the spawn point, it's hidden.
	return ( tr.fraction < 1.0f );
}

static bool IsHiddenSpawnFromAllSurvivorsLOS( const Vector &spawnPos )
{
	bool hasAnySurvivor = false;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *survivor = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !survivor )
			continue;

		if ( survivor->GetTeamNumber() != TEAM_TERRORIST )
			continue;

		if ( !survivor->IsAlive() )
			continue;

		hasAnySurvivor = true;

		// If any survivor has an unblocked LOS to the spawn point, reject it.
		if ( !IsHiddenSpawnFromSurvivorLOS( survivor, spawnPos ) )
			return false;
	}

	return hasAnySurvivor;
}

static int CountAliveSpecialInfectedBots( void )
{
	int count = 0;
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( !player->IsAlive() )
			continue;

		if ( !player->IsBot() || !player->IsSpecialInfected() )
			continue;

		++count;
	}
	return count;
}

static int CountAliveCommonInfectedGlobal( void )
{
	int count = 0;
	CBaseEntity *ent = NULL;
	while ( ( ent = gEntList.FindEntityByClassname( ent, "infected" ) ) != NULL )
	{
		CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( ent );
		if ( !combat || !combat->IsAlive() )
			continue;

		++count;
	}
	return count;
}

static CBaseEntity *PickRandomSpawnAnchorEntity( const char *classname )
{
	if ( !classname )
		return NULL;

	int count = 0;
	CBaseEntity *chosen = NULL;
	CBaseEntity *ent = NULL;
	while ( ( ent = gEntList.FindEntityByClassname( ent, classname ) ) != NULL )
	{
		++count;
		if ( random->RandomInt( 1, count ) == 1 )
		{
			chosen = ent;
		}
	}

	return chosen;
}

static bool PickRandomSpawnAnchor( const char *classname, Vector *outAnchor )
{
	if ( !classname || !outAnchor )
		return false;

	CBaseEntity *ent = PickRandomSpawnAnchorEntity( classname );
	if ( !ent )
		return false;

	*outAnchor = ent->WorldSpaceCenter();
	return true;
}

static bool TryFallbackSpawnPosition( CBasePlayer *pPlayer, const Vector &anchor, const QAngle &angles, Vector *outOrigin, QAngle *outAngles )
{
	if ( !pPlayer || !outOrigin || !outAngles )
		return false;

	Vector testOrigin;
	if ( !EntityPlacementTest( pPlayer, anchor, testOrigin, true ) )
		return false;

	*outOrigin = testOrigin;
	*outAngles = angles;
	return true;
}

static bool FindFallbackPlayerSpawnPosition( CBasePlayer *pPlayer, int team, Vector *outOrigin, QAngle *outAngles )
{
	if ( !pPlayer || !outOrigin || !outAngles )
		return false;

	const char *primaryClass = ( team == TEAM_CT ) ? "info_player_counterterrorist" : "info_player_terrorist";
	const char *secondaryClass = ( team == TEAM_CT ) ? "info_player_terrorist" : "info_player_counterterrorist";
	const char *anchorClasses[] =
	{
		primaryClass,
		secondaryClass,
		"info_player_start",
		"info_player_teamspawn",
		"func_hostage_rescue",
		"info_changelevel",
	};

	for ( int i = 0; i < ARRAYSIZE( anchorClasses ); ++i )
	{
		CBaseEntity *anchor = PickRandomSpawnAnchorEntity( anchorClasses[i] );
		if ( !anchor )
			continue;

		if ( TryFallbackSpawnPosition( pPlayer, anchor->WorldSpaceCenter(), anchor->GetAbsAngles(), outOrigin, outAngles ) )
		{
			return true;
		}
	}

	for ( int pass = 0; pass < 2; ++pass )
	{
		for ( int i = 1; i <= gpGlobals->maxClients; ++i )
		{
			CBasePlayer *other = UTIL_PlayerByIndex( i );
			if ( !other || other == pPlayer )
				continue;

			if ( pass == 0 && other->GetTeamNumber() != team )
				continue;

			if ( !other->IsAlive() )
				continue;

			if ( TryFallbackSpawnPosition( pPlayer, other->GetAbsOrigin(), other->GetAbsAngles(), outOrigin, outAngles ) )
			{
				return true;
			}
		}
	}

	if ( TheNavMesh && TheNavMesh->IsLoaded() && !TheNavMesh->IsGenerating() && TheNavMesh->GetNavAreaCount() > 0 )
	{
		NavAreaCollector collector;
		TheNavMesh->ForAllAreas( collector );
		if ( collector.m_area.Count() > 0 )
		{
			const int maxAttempts = MIN( collector.m_area.Count(), 128 );
			for ( int attempt = 0; attempt < maxAttempts; ++attempt )
			{
				CNavArea *area = collector.m_area[ random->RandomInt( 0, collector.m_area.Count() - 1 ) ];
				if ( !area || area->IsBlocked( TEAM_ANY ) )
					continue;

				if ( TryFallbackSpawnPosition( pPlayer, area->GetRandomPoint(), vec3_angle, outOrigin, outAngles ) )
				{
					return true;
				}
			}
		}
	}

	if ( TryFallbackSpawnPosition( pPlayer, pPlayer->GetAbsOrigin(), vec3_angle, outOrigin, outAngles ) )
	{
		return true;
	}

	if ( TryFallbackSpawnPosition( pPlayer, Vector( 0.0f, 0.0f, 64.0f ), vec3_angle, outOrigin, outAngles ) )
	{
		return true;
	}

	return false;
}

static Vector GetBackgroundInfectedAnchor( void )
{
	Vector anchor( vec3_origin );

	// Prefer the explicit logo spawn(s) if present.
	if ( PickRandomSpawnAnchor( "info_player_logo", &anchor ) )
		return anchor;

	// Fall back to standard spawnpoints if any exist (background maps may omit these).
	if ( PickRandomSpawnAnchor( "info_player_terrorist", &anchor ) )
		return anchor;

	if ( PickRandomSpawnAnchor( "info_player_counterterrorist", &anchor ) )
		return anchor;

	if ( PickRandomSpawnAnchor( "info_player_start", &anchor ) )
		return anchor;

	return anchor;
}

static bool FindBackgroundInfectedSpawnPos( const Vector &anchor, float radius, Vector *outPos, QAngle *outAngles )
{
	if ( !outPos || !outAngles )
		return false;

	// Prefer navmesh points when available.
	if ( TheNavMesh && TheNavMesh->IsLoaded() && !TheNavMesh->IsGenerating() )
	{
		NavAreaCollector collector;
		TheNavMesh->ForAllAreasInRadius( collector, anchor, radius );
		if ( collector.m_area.Count() > 0 )
		{
			const int maxAttempts = 96;
			for ( int attempt = 0; attempt < maxAttempts; ++attempt )
			{
				CNavArea *area = collector.m_area[ random->RandomInt( 0, collector.m_area.Count() - 1 ) ];
				if ( !area )
					continue;

				if ( area->IsBlocked( TEAM_ANY ) )
					continue;

				Vector pos = area->GetRandomPoint();

				// Add a small jitter and clamp back onto the area so spawns don't line up on obvious patterns.
				Vector jittered = pos;
				jittered.x += random->RandomFloat( -75.0f, 75.0f );
				jittered.y += random->RandomFloat( -75.0f, 75.0f );
				area->GetClosestPointOnArea( jittered, &pos );
				pos.z = area->GetZ( pos );

				*outPos = pos;
				outAngles->Init( 0.0f, random->RandomFloat( 0.0f, 360.0f ), 0.0f );
				return true;
			}
		}
	}

	// Fallback: jitter around the anchor and trace to the floor.
	Vector pos = anchor;
	pos.x += random->RandomFloat( -radius, radius );
	pos.y += random->RandomFloat( -radius, radius );

	trace_t tr;
	CTraceFilterWorldOnly filter;
	UTIL_TraceLine( pos + Vector( 0, 0, 512.0f ), pos - Vector( 0, 0, 2048.0f ), MASK_SOLID, &filter, &tr );
	if ( tr.fraction < 1.0f )
	{
		pos = tr.endpos;
	}

	*outPos = pos;
	outAngles->Init( 0.0f, random->RandomFloat( 0.0f, 360.0f ), 0.0f );
	return true;
}

static int CountBotsOnTeam( int teamNum )
{
	int count = 0;
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		if ( !player->IsBot() )
			continue;

		if ( player->GetTeamNumber() != teamNum )
			continue;

		++count;
	}
	return count;
}

static void ListenServerHostAutoJoinThink( CCSGameRules *rules )
{
	if ( !rules || !sv_listen_host_autojoin_survivors.GetBool() )
		return;

	if ( engine->IsDedicatedServer() )
		return;

	CCSPlayer *host = ToCSPlayer( UTIL_GetListenServerHost() );
	if ( !host )
		return;

	// Only run once per map, and only if the host hasn't already joined a team.
	if ( !s_bListenHostAutoJoinDone )
	{
		if ( host->GetTeamNumber() != TEAM_TERRORIST && !CSGameRules()->IsLogoMap() )
		{
			host->ChangeTeam( TEAM_TERRORIST );
			host->RoundRespawn();
		}

		const int desiredBots = MAX( 0, sv_listen_host_autojoin_survivor_bots.GetInt() );
		const int existingBots = CountBotsOnTeam( TEAM_TERRORIST );
		s_nListenHostAutoJoinBotsRemaining = MAX( 0, desiredBots - existingBots );
		s_flListenHostAutoJoinNextBotTry = gpGlobals->curtime;
		s_bListenHostAutoJoinDone = true;
	}

	if ( s_nListenHostAutoJoinBotsRemaining <= 0 )
		return;

	if ( gpGlobals->curtime < s_flListenHostAutoJoinNextBotTry )
		return;

	// Try to add one bot per tick; retry later if the nav mesh isn't ready yet.
	if ( !CSGameRules()->IsLogoMap() ) 
	{ 
		if (!CSGameRules()->TeamFull(TEAM_TERRORIST) && TheCSBots()->BotAddCommand(TEAM_TERRORIST))
		{
			--s_nListenHostAutoJoinBotsRemaining;
			s_flListenHostAutoJoinNextBotTry = gpGlobals->curtime;
		}
		else
		{
			s_flListenHostAutoJoinNextBotTry = gpGlobals->curtime + sv_listen_host_autojoin_bot_retry_delay.GetFloat();
		}
	}
}
bool CCSGameRules::SurvivorNameMatches(CCSPlayer* pPlayer, const char* pszTargetName)
{
	if (!pPlayer || !pszTargetName || pszTargetName[0] == '\0')
		return false;

	int survivorClass = pPlayer->GetSurvivorClass();
	bool isL4D1 = (survivor_set.GetInt() == 1);

	// Official L4D names
	if (isL4D1)
	{
		if (survivorClass == 0 && Q_stricmp(pszTargetName, "NamVet") == 0) return true;
		if (survivorClass == 1 && Q_stricmp(pszTargetName, "Biker") == 0) return true;
		if (survivorClass == 2 && Q_stricmp(pszTargetName, "Manager") == 0) return true;
		if (survivorClass == 3 && Q_stricmp(pszTargetName, "TeenGirl") == 0) return true;
	}
	else
	{
		if (survivorClass == 0 && Q_stricmp(pszTargetName, "Gambler") == 0) return true;
		if (survivorClass == 1 && Q_stricmp(pszTargetName, "Coach") == 0) return true;
		if (survivorClass == 2 && Q_stricmp(pszTargetName, "Mechanic") == 0) return true;
		if (survivorClass == 3 && Q_stricmp(pszTargetName, "Producer") == 0) return true;
	}

	// Friendly names
	if (Q_stricmp(pszTargetName, "bill") == 0)    return (isL4D1 && survivorClass == 0);
	if (Q_stricmp(pszTargetName, "zoey") == 0)    return (isL4D1 && survivorClass == 3);
	if (Q_stricmp(pszTargetName, "francis") == 0) return (isL4D1 && survivorClass == 1);
	if (Q_stricmp(pszTargetName, "louis") == 0)   return (isL4D1 && survivorClass == 2);

	if (Q_stricmp(pszTargetName, "nick") == 0)    return (!isL4D1 && survivorClass == 0);
	if (Q_stricmp(pszTargetName, "coach") == 0)   return (!isL4D1 && survivorClass == 1);
	if (Q_stricmp(pszTargetName, "ellis") == 0)   return (!isL4D1 && survivorClass == 2);
	if (Q_stricmp(pszTargetName, "rochelle") == 0)return (!isL4D1 && survivorClass == 3);

	return false;
}
bool CCSGameRules::FindSpecialInfectedSpawnPos( CCSPlayer *survivor, Vector *outPos, QAngle *outAngles ) const
{
	if ( !survivor || !outPos || !outAngles )
		return false;

	if ( !TheNavMesh || !TheNavMesh->IsLoaded() || TheNavMesh->IsGenerating() )
		return false;

	const Vector survivorOrigin = survivor->GetAbsOrigin();
	const float minDist = z_special_spawn_safety_radius.GetFloat();
	const float searchRadius = z_special_spawn_search_radius.GetFloat();

	NavAreaCollector collector;
	TheNavMesh->ForAllAreasInRadius( collector, survivorOrigin, searchRadius );

	CUtlVector< CNavArea * > candidatesDistanceOnly;
	candidatesDistanceOnly.EnsureCapacity( collector.m_area.Count() );

	FOR_EACH_VEC( collector.m_area, it )
	{
		CNavArea *area = collector.m_area[ it ];
		if ( !area )
			continue;

		if ( area->IsBlocked( TEAM_ANY ) )
			continue;

		const float dist = area->GetCenter().DistTo( survivorOrigin );
		if ( dist < minDist )
			continue;

		candidatesDistanceOnly.AddToTail( area );
	}

	// Prefer areas that are hidden from survivor LOS at the chosen random point.
	const int maxAreaAttempts = 32;
	const int maxPointAttemptsPerArea = 8;

	for ( int attempt = 0; attempt < maxAreaAttempts && candidatesDistanceOnly.Count() > 0; ++attempt )
	{
		const int index = random->RandomInt( 0, candidatesDistanceOnly.Count() - 1 );
		CNavArea *area = candidatesDistanceOnly[ index ];
		candidatesDistanceOnly.FastRemove( index );
		if ( !area )
			continue;

		for ( int j = 0; j < maxPointAttemptsPerArea; ++j )
		{
			Vector pos = area->GetRandomPoint();
			if ( pos.DistTo( survivorOrigin ) < minDist )
				continue;

			if ( IsHiddenSpawnFromAllSurvivorsLOS( pos ) )
			{
				*outPos = pos;
				Vector toSurvivor = survivorOrigin - pos;
				toSurvivor.z = 0.0f;
				VectorAngles( toSurvivor, *outAngles );
				outAngles->x = 0.0f;
				outAngles->z = 0.0f;
				return true;
			}
		}
	}

	return false;
}

static bool IsWithinRadiusOfAnySurvivor( const CUtlVector< CCSPlayer * > &survivors, const Vector &pos, float radiusSqr )
{
	FOR_EACH_VEC( survivors, it )
	{
		CCSPlayer *survivor = survivors[ it ];
		if ( !survivor )
			continue;

		if ( ( survivor->GetAbsOrigin() - pos ).LengthSqr() <= radiusSqr )
			return true;
	}

	return false;
}

static void ResetSpecialInfectedFarCullTimers( void )
{
	for ( int i = 0; i < ARRAYSIZE( s_flSpecialInfectedFarCullStartTime ); ++i )
	{
		s_flSpecialInfectedFarCullStartTime[i] = 0.0f;
	}
}

static void CullFarCommonInfected( const CUtlVector< CCSPlayer * > &survivors, float radius, int maxToCull )
{
	const float radiusSqr = radius * radius;
	int culled = 0;

	CBaseEntity *ent = NULL;
	while ( culled < maxToCull && ( ent = gEntList.FindEntityByClassname( ent, "infected" ) ) != NULL )
	{
		CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( ent );
		if ( !combat || !combat->IsAlive() )
			continue;

		if ( IsWithinRadiusOfAnySurvivor( survivors, ent->GetAbsOrigin(), radiusSqr ) )
			continue;

		UTIL_Remove( ent );
		++culled;
	}
}

static void CullExcessCommonInfectedGlobal( const CUtlVector< CCSPlayer * > *survivors, int maxAlive )
{
	maxAlive = MAX( 0, maxAlive );

	int aliveCommon = CountAliveCommonInfectedGlobal();
	if ( aliveCommon <= maxAlive )
		return;

	if ( survivors && survivors->Count() > 0 )
	{
		CBaseEntity *ent = NULL;
		while ( aliveCommon > maxAlive && ( ent = gEntList.FindEntityByClassname( ent, "infected" ) ) != NULL )
		{
			CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( ent );
			if ( !combat || !combat->IsAlive() )
				continue;

			if ( IsWithinRadiusOfAnySurvivor( *survivors, ent->GetAbsOrigin(), 1.0f ) )
				continue;

			UTIL_Remove( ent );
			--aliveCommon;
		}
	}

	CBaseEntity *ent = NULL;
	while ( aliveCommon > maxAlive && ( ent = gEntList.FindEntityByClassname( ent, "infected" ) ) != NULL )
	{
		CBaseCombatCharacter *combat = dynamic_cast< CBaseCombatCharacter * >( ent );
		if ( !combat || !combat->IsAlive() )
			continue;

		UTIL_Remove( ent );
		--aliveCommon;
	}
}

static CCSPlayer *PickCommonInfectedAnchorSurvivor( const CUtlVector< CCSPlayer * > &survivors )
{
	// Prefer the "IT" survivor if active; otherwise pick a random survivor as the anchor.
	CCSPlayer *survivor = GetAliveItSurvivorOrNull();
	if ( !survivor && survivors.Count() > 0 )
	{
		survivor = survivors[ random->RandomInt( 0, survivors.Count() - 1 ) ];
	}
	return survivor;
}

static float GetRandomHordeInterval( void )
{
	float flMin = MAX( 1.0f, z_horde_interval_min.GetFloat() );
	float flMax = MAX( flMin, z_horde_interval_max.GetFloat() );
	return random->RandomFloat( flMin, flMax );
}

static float GetRandomHordeDuration( void )
{
	float flMin = MAX( 1.0f, z_horde_duration_min.GetFloat() );
	float flMax = MAX( flMin, z_horde_duration_max.GetFloat() );
	return random->RandomFloat( flMin, flMax );
}

static bool IsCommonInfectedHordeActive( void )
{
	return ( gpGlobals != NULL ) && ( s_flCommonHordeEndTime > gpGlobals->curtime );
}

static void ResetCommonInfectedHordeSpawnPos( void )
{
	s_bCommonHordeSpawnPosValid = false;
	s_vecCommonHordeSpawnPos = vec3_origin;
	s_angCommonHordeSpawnAng.Init( 0.0f, 0.0f, 0.0f );
}

static void ResetCommonInfectedHordeState( void )
{
	ResetCommonInfectedHordeSpawnPos();
	s_bCommonHordeInitialBurstDone = false;
}

static void ScheduleNextCommonInfectedHorde( float flBaseTime )
{
	s_flNextCommonHordeStart = flBaseTime + GetRandomHordeInterval();
}

static void StartCommonInfectedHorde( bool bImmediateSpawn )
{
	if ( !gpGlobals )
		return;

	if ( !IsCommonInfectedHordeActive() )
	{
		ResetCommonInfectedHordeState();
	}

	s_flCommonHordeEndTime = MAX( s_flCommonHordeEndTime, gpGlobals->curtime + GetRandomHordeDuration() );
	s_flNextCommonHordeStart = 0.0f;

	if ( bImmediateSpawn )
	{
		s_flNextCommonInfectedSpawn = gpGlobals->curtime;
	}
}

static void DirectCommonInfectedAtSurvivor( CBaseEntity *ent, CCSPlayer *target )
{
	if ( !ent || !target || !target->IsAlive() )
		return;

	CAI_BaseNPC *npc = ent->MyNPCPointer();
	if ( !npc )
		return;

	npc->SetEnemy( target );
	npc->UpdateEnemyMemory( target, target->GetAbsOrigin() );
	npc->SetSchedule( SCHED_CHASE_ENEMY );
}

static void DirectCommonInfectedAtRandomSurvivor( CBaseEntity *ent, const CUtlVector< CCSPlayer * > &survivors )
{
	if ( !ent || survivors.Count() <= 0 )
		return;

	CCSPlayer *target = survivors[ random->RandomInt( 0, survivors.Count() - 1 ) ];
	DirectCommonInfectedAtSurvivor( ent, target );
}

static bool FindCommonInfectedSpawnPosNearSurvivor( CCSPlayer *survivor, float maxDist, Vector *outPos, QAngle *outAngles )
{
	if ( !survivor || !outPos || !outAngles )
		return false;

	if ( !TheNavMesh || !TheNavMesh->IsLoaded() || TheNavMesh->IsGenerating() )
		return false;

	const Vector survivorOrigin = survivor->GetAbsOrigin();
	const float minDist = z_spawn_safety_radius.GetFloat();
	const float searchRadius = MAX( minDist, maxDist );

	NavAreaCollector collector;
	TheNavMesh->ForAllAreasInRadius( collector, survivorOrigin, searchRadius );

	CUtlVector< CNavArea * > candidates;
	candidates.EnsureCapacity( collector.m_area.Count() );

	FOR_EACH_VEC( collector.m_area, it )
	{
		CNavArea *area = collector.m_area[ it ];
		if ( !area )
			continue;

		if ( area->IsBlocked( TEAM_ANY ) )
			continue;

		const float dist = area->GetCenter().DistTo( survivorOrigin );
		if ( dist < minDist || dist > searchRadius )
			continue;

		candidates.AddToTail( area );
	}

	if ( candidates.Count() <= 0 )
		return false;

	const int maxAreaAttempts = 64;
	const int maxPointAttemptsPerArea = 10;

	for ( int attempt = 0; attempt < maxAreaAttempts && candidates.Count() > 0; ++attempt )
	{
		const int index = random->RandomInt( 0, candidates.Count() - 1 );
		CNavArea *area = candidates[ index ];
		candidates.FastRemove( index );
		if ( !area )
			continue;

		for ( int j = 0; j < maxPointAttemptsPerArea; ++j )
		{
			Vector pos = area->GetRandomPoint();
			const float dist = pos.DistTo( survivorOrigin );
			if ( dist < minDist || dist > searchRadius )
				continue;

			if ( z_spawn_require_hidden.GetBool() && !IsHiddenSpawnFromAllSurvivorsLOS( pos ) )
				continue;

			// Add a small jitter and clamp back onto the area so spawns don't line up on obvious patterns.
			Vector jittered = pos;
			jittered.x += random->RandomFloat( -75.0f, 75.0f );
			jittered.y += random->RandomFloat( -75.0f, 75.0f );
			area->GetClosestPointOnArea( jittered, &pos );
			pos.z = area->GetZ( pos );

			if ( pos.DistTo( survivorOrigin ) < minDist )
				continue;

			if ( pos.DistTo( survivorOrigin ) > searchRadius )
				continue;

			*outPos = pos;
			outAngles->Init( 0.0f, random->RandomFloat( 0.0f, 360.0f ), 0.0f );
			return true;
		}
	}

	return false;
}

static float GetCommonInfectedHordeMinSpawnDist( void )
{
	return 550;
}

static bool FindCommonInfectedSpawnPos( const CUtlVector< CCSPlayer * > &survivors, Vector *outPos, QAngle *outAngles, CCSPlayer *anchorSurvivor, float minDistFromAnySurvivor = 0.0f )
{
	if ( survivors.Count() <= 0 || !outPos || !outAngles )
		return false;

	CCSPlayer *survivor = anchorSurvivor ? anchorSurvivor : PickCommonInfectedAnchorSurvivor( survivors );
	if ( !survivor )
		return false;

	if ( !TheNavMesh || !TheNavMesh->IsLoaded() || TheNavMesh->IsGenerating() )
		return false;

	const Vector survivorOrigin = survivor->GetAbsOrigin();
	const float minDist = z_spawn_safety_radius.GetFloat();
	const float searchRadius = z_spawn_radius.GetFloat();
	const float minDistAnySurvivor = clamp( minDistFromAnySurvivor, 0.0f, searchRadius );
	const float minDistAnySurvivorSqr = minDistAnySurvivor * minDistAnySurvivor;

	NavAreaCollector collector;
	TheNavMesh->ForAllAreasInRadius( collector, survivorOrigin, searchRadius );

	CUtlVector< CNavArea * > candidates;
	candidates.EnsureCapacity( collector.m_area.Count() );

	FOR_EACH_VEC( collector.m_area, it )
	{
		CNavArea *area = collector.m_area[ it ];
		if ( !area )
			continue;

		if ( area->IsBlocked( TEAM_ANY ) )
			continue;

		const float dist = area->GetCenter().DistTo( survivorOrigin );
		if ( dist < minDist )
			continue;

		if ( minDistAnySurvivor > 0.0f && IsWithinRadiusOfAnySurvivor( survivors, area->GetCenter(), minDistAnySurvivorSqr ) )
			continue;

		candidates.AddToTail( area );
	}

	if ( candidates.Count() <= 0 )
		return false;

	const int maxAreaAttempts = 96;
	const int maxPointAttemptsPerArea = 12;

	for ( int attempt = 0; attempt < maxAreaAttempts && candidates.Count() > 0; ++attempt )
	{
		const int index = random->RandomInt( 0, candidates.Count() - 1 );
		CNavArea *area = candidates[ index ];
		candidates.FastRemove( index );
		if ( !area )
			continue;

		for ( int j = 0; j < maxPointAttemptsPerArea; ++j )
		{
			Vector pos = area->GetRandomPoint();
			if ( pos.DistTo( survivorOrigin ) < minDist )
				continue;

			if ( pos.DistTo( survivorOrigin ) > searchRadius )
				continue;

			if ( minDistAnySurvivor > 0.0f && IsWithinRadiusOfAnySurvivor( survivors, pos, minDistAnySurvivorSqr ) )
				continue;

			if ( z_spawn_require_hidden.GetBool() && !IsHiddenSpawnFromAllSurvivorsLOS( pos ) )
				continue;

			// Add a small jitter and clamp back onto the area so spawns don't line up on obvious patterns.
			Vector jittered = pos;
			jittered.x += random->RandomFloat( -75.0f, 75.0f );
			jittered.y += random->RandomFloat( -75.0f, 75.0f );
			area->GetClosestPointOnArea( jittered, &pos );
			pos.z = area->GetZ( pos );

			if ( pos.DistTo( survivorOrigin ) < minDist )
				continue;

			if ( pos.DistTo( survivorOrigin ) > searchRadius )
				continue;

			if ( minDistAnySurvivor > 0.0f && IsWithinRadiusOfAnySurvivor( survivors, pos, minDistAnySurvivorSqr ) )
				continue;

			*outPos = pos;
			outAngles->Init( 0.0f, random->RandomFloat( 0.0f, 360.0f ), 0.0f );
			return true;
		}
	}

	return false;
}

static bool IsTankSpawnAnchorEntity( CBaseEntity *ent )
{
	if ( !ent || V_stricmp( ent->GetClassname(), "info_target" ) != 0 )
		return false;

	if ( ent->GetEntityName() == NULL_STRING )
		return false;

	const char *pszName = STRING( ent->GetEntityName() );
	if ( !pszName || !pszName[0] )
		return false;

	return ( Q_strnicmp( pszName, "tank_spawn", 10 ) == 0 ) || ( Q_strnicmp( pszName, "tankspawn", 9 ) == 0 );
}

static bool PickTankSpawnAnchorNearSurvivors( const CUtlVector< CCSPlayer * > &survivors, Vector *outAnchor )
{
	if ( !outAnchor || survivors.Count() <= 0 )
		return false;

	const float flProximitySqr = z_tank_spawn_proximity_radius.GetFloat() * z_tank_spawn_proximity_radius.GetFloat();
	int nEligibleCount = 0;

	for ( CBaseEntity *ent = NULL; ( ent = gEntList.FindEntityByClassname( ent, "info_target" ) ) != NULL; )
	{
		if ( !IsTankSpawnAnchorEntity( ent ) )
			continue;

		if ( !IsWithinRadiusOfAnySurvivor( survivors, ent->GetAbsOrigin(), flProximitySqr ) )
			continue;

		++nEligibleCount;
		if ( random->RandomInt( 1, nEligibleCount ) == 1 )
		{
			*outAnchor = ent->GetAbsOrigin();
		}
	}

	return ( nEligibleCount > 0 );
}

static bool FindTankSpawnPosNearAnchor( const Vector &anchor, const CUtlVector< CCSPlayer * > &survivors, Vector *outPos, QAngle *outAngles )
{
	if ( !outPos || !outAngles || survivors.Count() <= 0 )
		return false;

	if ( !TheNavMesh || !TheNavMesh->IsLoaded() || TheNavMesh->IsGenerating() )
		return false;

	const float flSearchRadius = MAX( 128.0f, z_tank_spawn_search_radius.GetFloat() );
	const float flMinSurvivorDist = z_special_spawn_safety_radius.GetFloat();
	const float flMinSurvivorDistSqr = flMinSurvivorDist * flMinSurvivorDist;

	NavAreaCollector collector;
	TheNavMesh->ForAllAreasInRadius( collector, anchor, flSearchRadius );

	CUtlVector< CNavArea * > candidates;
	candidates.EnsureCapacity( collector.m_area.Count() );

	FOR_EACH_VEC( collector.m_area, it )
	{
		CNavArea *area = collector.m_area[ it ];
		if ( !area || area->IsBlocked( TEAM_ANY ) )
			continue;

		if ( area->GetCenter().DistTo( anchor ) > flSearchRadius )
			continue;

		candidates.AddToTail( area );
	}

	const int maxAreaAttempts = 64;
	const int maxPointAttemptsPerArea = 10;

	for ( int attempt = 0; attempt < maxAreaAttempts && candidates.Count() > 0; ++attempt )
	{
		const int index = random->RandomInt( 0, candidates.Count() - 1 );
		CNavArea *area = candidates[ index ];
		candidates.FastRemove( index );
		if ( !area )
			continue;

		for ( int pointAttempt = 0; pointAttempt < maxPointAttemptsPerArea; ++pointAttempt )
		{
			Vector pos = area->GetRandomPoint();
			if ( pos.DistTo( anchor ) > flSearchRadius )
				continue;

			if ( IsWithinRadiusOfAnySurvivor( survivors, pos, flMinSurvivorDistSqr ) )
				continue;

			if ( !IsHiddenSpawnFromAllSurvivorsLOS( pos ) )
				continue;

			Vector jittered = pos;
			jittered.x += random->RandomFloat( -75.0f, 75.0f );
			jittered.y += random->RandomFloat( -75.0f, 75.0f );
			area->GetClosestPointOnArea( jittered, &pos );
			pos.z = area->GetZ( pos );

			CCSPlayer *target = survivors[ random->RandomInt( 0, survivors.Count() - 1 ) ];
			Vector toSurvivor = target->GetAbsOrigin() - pos;
			toSurvivor.z = 0.0f;
			VectorAngles( toSurvivor, *outAngles );
			outAngles->x = 0.0f;
			outAngles->z = 0.0f;
			*outPos = pos;
			return true;
		}
	}

	return false;
}

static bool IsEntityHullClearAt( CBaseEntity *ent, const Vector &pos, const Vector &mins, const Vector &maxs )
{
	if ( !ent )
		return false;

	trace_t tr;
	UTIL_TraceHull( pos, pos, mins, maxs, MASK_NPCSOLID, ent, ent->GetCollisionGroup(), &tr );
	return ( !tr.startsolid && !tr.allsolid );
}

static bool FixupCommonInfectedSpawnPos( CBaseEntity *ent, const Vector &desiredPos, Vector *outPos )
{
	if ( !ent || !outPos )
		return false;

	const Vector mins = ent->CollisionProp()->OBBMins();
	const Vector maxs = ent->CollisionProp()->OBBMaxs();

	CNavArea *area = NULL;
	if ( TheNavMesh && TheNavMesh->IsLoaded() && !TheNavMesh->IsGenerating() )
	{
		area = TheNavMesh->GetNearestNavArea( desiredPos );
	}

	// Try the desired position, then increasingly large XY rings with a few Z lifts.
	static const float kZOffsets[] = { 0.0f, 8.0f, 16.0f, 24.0f };
	static const float kRadii[] = { 0.0f, 16.0f, 32.0f, 48.0f, 64.0f, 96.0f, 128.0f };

	for ( int iz = 0; iz < ARRAYSIZE( kZOffsets ); ++iz )
	{
		for ( int ir = 0; ir < ARRAYSIZE( kRadii ); ++ir )
		{
			const float radius = kRadii[ ir ];
			const int attemptsThisRing = ( radius <= 0.0f ) ? 1 : 6;

			for ( int a = 0; a < attemptsThisRing; ++a )
			{
				const float yawDeg = random->RandomFloat( 0.0f, 360.0f );
				float s, c;
				SinCos( DEG2RAD( yawDeg ), &s, &c );

				Vector candidate = desiredPos;
				candidate.x += c * radius;
				candidate.y += s * radius;
				candidate.z += kZOffsets[ iz ];

				if ( area )
				{
					area->GetClosestPointOnArea( candidate, &candidate );
					candidate.z = area->GetZ( candidate ) + kZOffsets[ iz ];
				}

				if ( IsEntityHullClearAt( ent, candidate, mins, maxs ) )
				{
					*outPos = candidate;
					return true;
				}
			}
		}
	}

	return false;
}

void CCSGameRules::OnSurvivorVomited( CCSPlayer *victim, CCSPlayer *attacker )
{
	if ( !victim || !victim->IsAlive() || victim->GetTeamNumber() != TEAM_TERRORIST )
		return;

	const float duration = 20.0f;

	// Boomer vomit can immediately trigger or refresh a horde.
	StartCommonInfectedHorde( true );

	if ( IsItTarget( victim ) )
	{
		SetItTarget( victim, duration );
		return;
	}

	victim->SpeakConceptIfAllowed(MP_CONCEPT_JARATE_HIT);
	SetItTarget( victim, duration );

	if ( !TheNavMesh || !TheNavMesh->IsLoaded() || TheNavMesh->IsGenerating() )
		return;

	// Spawn a burst of common infected near the victim (25 total).
	const int kVomitSpawnCount = 25;
	const float spawnMaxDist = MAX( 600.0f, z_spawn_safety_radius.GetFloat() + 900.0f );

	for ( int i = 0; i < kVomitSpawnCount; ++i )
	{
		Vector spawnPos;
		QAngle spawnAng;
		if ( !FindCommonInfectedSpawnPosNearSurvivor( victim, spawnMaxDist, &spawnPos, &spawnAng ) )
			break;

		CBaseEntity *ent = CreateEntityByName( "infected" );
		if ( !ent )
			break;

		ent->SetAbsOrigin( spawnPos );
		ent->SetAbsAngles( spawnAng );
		DispatchSpawn( ent );

		Vector fixedPos;
		if ( FixupCommonInfectedSpawnPos( ent, spawnPos, &fixedPos ) )
		{
			QAngle ang = ent->GetAbsAngles();
			Vector vel( vec3_origin );
			ent->Teleport( &fixedPos, &ang, &vel );
		}
		else
		{
			UTIL_Remove( ent );
			continue;
		}

		CAI_BaseNPC *npc = ent->MyNPCPointer();
		if ( npc )
		{
			DirectCommonInfectedAtSurvivor( ent, victim );
		}

		ent->Activate();

		if ( CountAliveCommonInfectedGlobal() > MAX( 0, z_common_max.GetInt() ) )
		{
			UTIL_Remove( ent );
			break;
		}
	}

	// Nudge special infected to focus too.
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player || !player->IsBot() || !player->IsSpecialInfected() )
			continue;

		if ( player->GetTeamNumber() != TEAM_CT )
			continue;

		CCSBot *bot = dynamic_cast< CCSBot * >( player );
		if ( bot )
		{
			bot->SetBotEnemy( victim );
		}
	}
}

void CCSGameRules::StartScriptedPanicEvent( CCSPlayer *pActivator, bool bRevealActivator )
{
	StartCommonInfectedHorde( true );

	CBroadcastRecipientFilter filter;
	BroadcastSound( "MegaMobIncoming" );

	const char *pszMessage = "The horde has been alerted!";
	CFmtStr panicMessage;

	if ( bRevealActivator )
	{
		if ( pActivator && pActivator->GetTeamNumber() == TEAM_TERRORIST && pActivator->GetPlayerName() && pActivator->GetPlayerName()[0] )
		{
			panicMessage.sprintf( "%s alerted the horde!", pActivator->GetPlayerName() );
		}
		else
		{
			panicMessage.sprintf( "You alerted the horde!" );
		}

		pszMessage = panicMessage.Access();
	}

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player || player->GetTeamNumber() != TEAM_TERRORIST )
			continue;

		ClientPrint( player, HUD_PRINTTALK, pszMessage );
	}
}

static void CommonInfectedDirectorThink( CCSGameRules *rules, bool isRestartingRound )
{
	if ( !rules || !z_spawn_enabled.GetBool() )
		return;

	// Don't spawn during freeze time or while the round is being restarted.
	if ( rules->IsFreezePeriod() || isRestartingRound )
	{
		s_flCommonHordeEndTime = 0.0f;
		ResetCommonInfectedHordeState();
		if ( isRestartingRound )
		{
			s_flNextCommonHordeStart = 0.0f;
		}
		s_flNextCommonInfectedSpawn = gpGlobals->curtime + 1.0f;
		return;
	}

	const bool bHordeActive = IsCommonInfectedHordeActive();
	if ( !bHordeActive )
	{
		if ( s_flCommonHordeEndTime > 0.0f )
		{
			s_flCommonHordeEndTime = 0.0f;
			ResetCommonInfectedHordeState();
		}

		if ( s_flNextCommonHordeStart <= 0.0f )
		{
			ScheduleNextCommonInfectedHorde( gpGlobals->curtime );
		}
		else if ( gpGlobals->curtime >= s_flNextCommonHordeStart )
		{
			StartCommonInfectedHorde( true );
		}
	}

	if ( s_flNextCommonInfectedSpawn <= 0.0f )
	{
		s_flNextCommonInfectedSpawn = gpGlobals->curtime;
	}

	if ( gpGlobals->curtime < s_flNextCommonInfectedSpawn )
		return;

	CUtlVector< CCSPlayer * > survivors;
	if ( CollectAliveSurvivorsT( survivors ) <= 0 )
	{
		CullExcessCommonInfectedGlobal( NULL, z_common_max.GetInt() );
		s_flNextCommonInfectedSpawn = gpGlobals->curtime + 0.5f;
		return;
	}

	if ( bHordeActive && !s_bCommonHordeInitialBurstDone )
	{
		// Horde bursts should arrive as a full pack, not merely top off whatever commons are already alive.
		CullExcessCommonInfectedGlobal( NULL, 0 );
		s_bCommonHordeInitialBurstDone = true;
	}

	// Remove common infected that are no longer within the population radius, so the director keeps pressure near the survivors.
	CullFarCommonInfected( survivors, z_spawn_radius.GetFloat(), z_spawn_batch.GetInt() );
	CullExcessCommonInfectedGlobal( &survivors, z_common_max.GetInt() );

	CCSPlayer *anchor = PickCommonInfectedAnchorSurvivor( survivors );
	const int maxCommon = MAX( 0, z_common_max.GetInt() );
	const int aliveCommon = CountAliveCommonInfectedGlobal();
	const int missing = maxCommon - aliveCommon;
	if ( missing <= 0 )
	{
		s_flNextCommonInfectedSpawn = gpGlobals->curtime + z_spawn_interval.GetFloat();
		return;
	}

	int batch = clamp( z_spawn_batch.GetInt(), 1, 64 );
	if ( bHordeActive )
	{
		batch = clamp( z_horde_spawn_batch.GetInt(), 1, 128 );
	}
	const int toSpawn = MIN( missing, batch );
	int spawned = 0;

	Vector hordeSpawnPos( vec3_origin );
	QAngle hordeSpawnAng( 0, 0, 0 );
	if ( bHordeActive )
	{
		const float minHordeSpawnDist = GetCommonInfectedHordeMinSpawnDist();
		if ( !s_bCommonHordeSpawnPosValid )
		{
			if ( FindCommonInfectedSpawnPos( survivors, &s_vecCommonHordeSpawnPos, &s_angCommonHordeSpawnAng, anchor, minHordeSpawnDist ) )
			{
				s_bCommonHordeSpawnPosValid = true;
			}
		}

		if ( !s_bCommonHordeSpawnPosValid )
		{
			s_flNextCommonInfectedSpawn = gpGlobals->curtime + z_spawn_interval.GetFloat();
			return;
		}

		hordeSpawnPos = s_vecCommonHordeSpawnPos;
		hordeSpawnAng = s_angCommonHordeSpawnAng;
	}

	for ( int i = 0; i < toSpawn; ++i )
	{
		Vector spawnPos;
		QAngle spawnAng;
		if ( bHordeActive )
		{
			spawnPos = hordeSpawnPos;
			spawnAng = hordeSpawnAng;
		}
		else if ( !FindCommonInfectedSpawnPos( survivors, &spawnPos, &spawnAng, anchor ) )
		{
			break;
		}

		CBaseEntity *ent = CreateEntityByName( "infected" );
		if ( !ent )
			break;

		ent->SetAbsOrigin( spawnPos );
		ent->SetAbsAngles( spawnAng );
		DispatchSpawn( ent );

		// If the director picked a point that overlaps a wall/prop, nudge the spawn until the infected fits.
		Vector fixedPos;
		if ( FixupCommonInfectedSpawnPos( ent, spawnPos, &fixedPos ) )
		{
			QAngle ang = ent->GetAbsAngles();
			Vector vel( vec3_origin );
			ent->Teleport( &fixedPos, &ang, &vel );
		}
		else
		{
			UTIL_Remove( ent );
			continue;
		}

		if ( bHordeActive )
		{
			DirectCommonInfectedAtRandomSurvivor( ent, survivors );
		}

		ent->Activate();

		if ( CountAliveCommonInfectedGlobal() > maxCommon )
		{
			UTIL_Remove( ent );
			break;
		}

		++spawned;
	}

	if ( bHordeActive )
	{
		ResetCommonInfectedHordeSpawnPos();
	}

	s_flNextCommonInfectedSpawn = gpGlobals->curtime + z_spawn_interval.GetFloat();
}

static CCSPlayer *SelectSurvivorSquadLeader( const CUtlVector< CCSPlayer * > &survivors, CCSPlayer *currentLeader )
{
	if ( survivors.Count() <= 0 )
		return NULL;

	// Prefer the listen server host if they're an alive human survivor.
	CCSPlayer *host = ToCSPlayer( UTIL_GetListenServerHost() );
	const bool hostIsValidHuman = ( host && host->IsAlive() && host->GetTeamNumber() == TEAM_TERRORIST && !host->IsBot() );

	CUtlVector< CCSPlayer * > bots;
	bots.EnsureCapacity( survivors.Count() );

	CCSPlayer *firstHuman = NULL;
	bool currentLeaderIsAliveSurvivor = false;

	FOR_EACH_VEC( survivors, it )
	{
		CCSPlayer *p = survivors[ it ];
		if ( !p )
			continue;

		if ( p == currentLeader )
			currentLeaderIsAliveSurvivor = true;

		if ( p->IsBot() )
			bots.AddToTail( p );
		else if ( !firstHuman )
			firstHuman = p;
	}

	const bool anyHuman = ( hostIsValidHuman || firstHuman != NULL );

	// Keep current human leader if valid.
	if ( currentLeader && currentLeaderIsAliveSurvivor && !currentLeader->IsBot() )
		return currentLeader;

	// Prefer a human leader if available.
	if ( anyHuman )
		return hostIsValidHuman ? host : firstHuman;

	// If no humans, keep current bot leader if valid.
	if ( currentLeader && currentLeaderIsAliveSurvivor )
		return currentLeader;

	// Otherwise, pick a random survivor bot.
	if ( bots.Count() > 0 )
		return bots[ random->RandomInt( 0, bots.Count() - 1 ) ];

	return NULL;
}

static void SurvivorSquadThink( CCSGameRules *rules )
{
	if ( !rules || !survivor_squad_enabled.GetBool() )
		return;

	if ( s_flNextSurvivorSquadUpdate <= 0.0f )
	{
		s_flNextSurvivorSquadUpdate = gpGlobals->curtime;
	}

	if ( gpGlobals->curtime < s_flNextSurvivorSquadUpdate )
		return;

	CUtlVector< CCSPlayer * > survivors;
	if ( CollectAliveSurvivorsT( survivors ) <= 0 )
	{
		s_hSurvivorSquadLeader = NULL;
		s_flNextSurvivorSquadUpdate = gpGlobals->curtime + survivor_squad_update_interval.GetFloat();
		return;
	}

	CCSPlayer *currentLeader = ToCSPlayer( s_hSurvivorSquadLeader.Get() );
	CCSPlayer *leader = SelectSurvivorSquadLeader( survivors, currentLeader );
	s_hSurvivorSquadLeader = leader;

	FOR_EACH_VEC( survivors, it )
	{
		CCSPlayer *p = survivors[ it ];
		if ( !p || !p->IsBot() )
			continue;

		CCSBot *bot = dynamic_cast< CCSBot * >( p );
		if ( !bot )
			continue;

		// Allow survivor bots to temporarily ignore squad-follow while doing urgent actions
		// like rescuing a pounced teammate or reviving an incapacitated survivor.
		if ( bot->ShouldIgnoreSurvivorSquad() )
		{
			// If we were following the squad leader, break off; otherwise preserve non-squad follow (e.g. revive target).
			if ( leader && bot->IsFollowing() && bot->GetFollowLeader() == leader )
			{
				bot->StopFollowing();
			}
			continue;
		}

		if ( !leader || !leader->IsAlive() )
		{
			if ( bot->IsFollowing() )
				bot->StopFollowing();
			continue;
		}

		if ( bot == leader )
		{
			if ( bot->IsFollowing() )
				bot->StopFollowing();
			continue;
		}

		if ( !bot->IsFollowing() || bot->GetFollowLeader() != leader )
		{
			bot->Follow( leader );
		}
	}

	s_flNextSurvivorSquadUpdate = gpGlobals->curtime + survivor_squad_update_interval.GetFloat();
}

 static CCSBot *SpawnSpecialInfectedBotAt( const Vector &pos, const QAngle &ang, int forcedZombieClass = 0 )
 {
 	if ( !TheBotProfiles )
  		return NULL;

	const BotDifficultyType difficulty = CCSBotManager::GetDifficultyLevel();
	const BotProfile *profile = TheBotProfiles->GetRandomProfile( difficulty, TEAM_CT, WEAPONTYPE_UNKNOWN);
	if ( !profile )
		return NULL;

 	CCSBot *bot = CreateBot< CCSBot >( profile, TEAM_CT );
 	if ( !bot )
 		return NULL;

	// Tag as special infected so we can kick after death.
	bot->SetSpecialInfected( true );

	bot->RoundRespawn();

 	Vector vel( vec3_origin );
 	bot->Teleport( &pos, &ang, &vel );

	// Optionally force a zombie class (used for tanks).
	if (forcedZombieClass > 0)
	{
		bot->SetZombieClass(forcedZombieClass);
		bot->RemoveAllWeapons();

		if (bot->GetZombieClass() == 1) {
			if ( survivor_set.GetInt() == 1 )
			{
				bot->SetModel( "models/infected/smoker_l4d1.mdl" );
			}
			else
			{
				bot->SetModel( "models/infected/smoker.mdl" );
			}
			bot->m_iHealth = 250;
			bot->SetMaxHealth(250);
			bot->SetMaxSpeed(210);
		}
		else if (bot->GetZombieClass() == 2) {
			if ( survivor_set.GetInt() == 1 )
			{
				bot->SetModel( "models/infected/boomer_l4d1.mdl" );
			}
			else
			{
				// 25% chance for a female boomer (boomette).
				if ( random->RandomFloat( 0.0f, 1.0f ) < 0.25f )
				{
					bot->SetModel( "models/infected/boomette.mdl" );
				}
				else
				{
					bot->SetModel( "models/infected/boomer.mdl" );
				}
			}
			bot->m_iHealth = 50;
			bot->SetMaxHealth(50);
			bot->SetMaxSpeed(175);
		}
		else if (bot->GetZombieClass() == 3) {
			if ( survivor_set.GetInt() == 1 )
			{
				bot->SetModel( "models/infected/hunter_l4d1.mdl" );
			}
			else
			{
				bot->SetModel( "models/infected/hunter.mdl" );
			}
			bot->m_iHealth = 250;
			bot->SetMaxHealth(250);
			bot->SetMaxSpeed(250);
		}
		else if (bot->GetZombieClass() == 4) {
			bot->SetModel("models/infected/spitter.mdl");
			bot->m_iHealth = 100;
			bot->SetMaxHealth(100);
			bot->SetMaxSpeed(210);
		}
		else if (bot->GetZombieClass() == 5) {
			bot->SetModel("models/infected/jockey.mdl");
			bot->m_iHealth = 325;
			bot->SetMaxHealth(325);
			bot->SetMaxSpeed(250);
		}
		else if (bot->GetZombieClass() == 6) {
			bot->SetModel("models/infected/charger.mdl");
			bot->m_iHealth = 600;
			bot->SetMaxHealth(600);
		}
		else if (bot->GetZombieClass() == 8) {
			if ( survivor_set.GetInt() == 1 )
			{
				bot->SetModel( "models/infected/hulk_l4d1.mdl" );
			}
			else
			{
				bot->SetModel( "models/infected/hulk.mdl" );
			}
			bot->m_iHealth = 6000;
			bot->SetMaxHealth(6000);
			bot->SetMaxSpeed(210);
		}
		else {
			if ( survivor_set.GetInt() == 1 )
			{
				bot->SetModel( "models/infected/boomer_l4d1.mdl" );
			}
			else
			{
				// Fallback to boomer/boomette model selection.
				if ( random->RandomFloat( 0.0f, 1.0f ) < 0.25f )
				{
					bot->SetModel( "models/infected/boomette.mdl" );
				}
				else
				{
					bot->SetModel( "models/infected/boomer.mdl" );
				}
			}
			bot->m_iHealth = 50;
			bot->SetMaxHealth(50);
			bot->SetMaxSpeed(250);
		}
		if (bot->GetZombieClass() == 1) {
			bot->GiveNamedItem("weapon_smoker_claw");
		}
		else if (bot->GetZombieClass() == 2) {
			bot->GiveNamedItem("weapon_boomer_claw");
		}
		else if (bot->GetZombieClass() == 3) {
			bot->GiveNamedItem("weapon_hunter_claw");
		}
		else if (bot->GetZombieClass() == 4) {
			bot->GiveNamedItem("weapon_spitter_claw");
		}
		else if (bot->GetZombieClass() == 5) {
			bot->GiveNamedItem("weapon_jockey_claw");
		}
		else if (bot->GetZombieClass() == 6) {
			bot->GiveNamedItem("weapon_charger_claw");
		}
		else if (bot->GetZombieClass() == 8) {
			bot->GiveNamedItem("weapon_tank_claw");
		}
	}

	return bot;
}

static bool SpawnNaturalTankAt(const Vector& spawnPos, const QAngle& spawnAng)
{
	if (IsAnyAliveTank())
		return false;

	if (CCSBot* bot = SpawnSpecialInfectedBotAt(spawnPos, spawnAng, 8))
	{
		return true;
	}

	CCSPlayer* human = SelectRandomHumanInfected(false);
	if (!human)
		return false;

	if (!human->IsAlive())
	{
		human->SetSpecialInfected(true);
		human->SetZombieClass(8);
		human->SetSpecialInfectedDeathTimestamp(0.0f);
		human->RoundRespawn();

		if (!human->IsAlive())
			return false;
	}

	Vector vel(vec3_origin);
	human->Teleport(&spawnPos, &spawnAng, &vel);
	ApplyTankLoadout(human, 6000);
	ClientPrint(human, HUD_PRINTTALK, "You are now the TANK!\nAttack the Survivors!");
	return true;
}

static void BackgroundInfectedPopulateThink( CCSGameRules *rules, bool isRestartingRound )
{
	if ( !rules || !rules->IsLogoMap() || !z_background_populate_enabled.GetBool() )
		return;

	// Don't populate while the round is being restarted.
	if ( isRestartingRound )
		return;

	if ( s_flNextBackgroundPopulate <= 0.0f )
	{
		s_flNextBackgroundPopulate = gpGlobals->curtime;
	}

	if ( gpGlobals->curtime < s_flNextBackgroundPopulate )
		return;

	s_flNextBackgroundPopulate = gpGlobals->curtime + z_background_spawn_interval.GetFloat();

	const Vector anchor = GetBackgroundInfectedAnchor();
	const float radius = z_background_spawn_radius.GetFloat();

	// Spawn common infected up to the normal cap.
	const int maxCommon = MAX( 0, z_common_max.GetInt() );
	CullExcessCommonInfectedGlobal( NULL, maxCommon );
	const int aliveCommon = CountAliveCommonInfectedGlobal();
	const int missingCommon = maxCommon - aliveCommon;
	if ( missingCommon > 0 )
	{
		const int batch = clamp( z_spawn_batch.GetInt(), 1, 64 );
		const int toSpawn = MIN( missingCommon, batch );

		for ( int i = 0; i < toSpawn; ++i )
		{
			Vector spawnPos;
			QAngle spawnAng;
			if ( !FindBackgroundInfectedSpawnPos( anchor, radius, &spawnPos, &spawnAng ) )
				break;

			CBaseEntity *ent = CreateEntityByName( "infected" );
			if ( !ent )
				break;

			ent->SetAbsOrigin( spawnPos );
			ent->SetAbsAngles( spawnAng );
			DispatchSpawn( ent );

			// If the chosen point overlaps a wall/prop, nudge until the infected fits.
			Vector fixedPos;
			if ( FixupCommonInfectedSpawnPos( ent, spawnPos, &fixedPos ) )
			{
				QAngle ang = ent->GetAbsAngles();
				Vector vel( vec3_origin );
				ent->Teleport( &fixedPos, &ang, &vel );
			}
			else
			{
				UTIL_Remove( ent );
				continue;
			}

			ent->Activate();
		}
	}

	// Spawn special infected bots up to the background cap and put them into hunt so they roam.
	const int maxSpecial = MAX( 0, z_background_special_limit.GetInt() );
	const int aliveSpecial = CountAliveSpecialInfectedBots();
	const int missingSpecial = maxSpecial - aliveSpecial;
	if ( missingSpecial > 0 )
	{
		// Keep special spawns light to avoid hitching on menu/background maps.
		const int toSpawn = MIN( missingSpecial, 1 );
		for ( int i = 0; i < toSpawn; ++i )
		{
			if ( rules->TeamFull( TEAM_CT ) )
				break;

			Vector spawnPos;
			QAngle spawnAng;
			if ( !FindBackgroundInfectedSpawnPos( anchor, radius, &spawnPos, &spawnAng ) )
				break;

			CCSBot *bot = SpawnSpecialInfectedBotAt( spawnPos, spawnAng );
			if ( !bot )
				break;

			bot->SetRogue( true );
			bot->Hunt();
		}
	}
}

static void SpecialInfectedDirectorThink(CCSGameRules* rules, bool isRestartingRound)
{
	if (!rules)
		return;

	CUtlVector< CCSPlayer * > survivors;
	const int survivorCount = CollectAliveSurvivorsT( survivors );
	const float farCullDistanceSqr = z_special_far_cull_distance.GetFloat() * z_special_far_cull_distance.GetFloat();
	const float farCullGrace = MAX( 0.0f, z_special_far_cull_grace.GetFloat() );

	// Special infected respawn timer (TF2-style): dead specials auto-respawn after a delay.
	const bool allowSpecialRespawnNow = ( !rules->IsFreezePeriod() && !isRestartingRound );
	const float respawnTime = MAX( 0.0f, z_special_respawn_time.GetFloat() );
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CCSPlayer *player = ToCSPlayer( UTIL_PlayerByIndex( i ) );
		if ( !player )
			continue;

		const bool isSpecialInfected = ( player->GetTeamNumber() == TEAM_INFECTED );
		if ( !isSpecialInfected )
			continue;

		const int entIndex = player->entindex();
		if ( entIndex > 0 && entIndex < ARRAYSIZE( s_flSpecialInfectedFarCullStartTime ) )
		{
			const bool shouldTrackFarCull =
				player->IsAlive() &&
				player->IsBot() &&
				player->IsSpecialInfected() &&
				!player->IsGhost() &&
				!player->IsHulkTank() &&
				survivorCount > 0;

			if ( shouldTrackFarCull )
			{
				if ( !IsWithinRadiusOfAnySurvivor( survivors, player->GetAbsOrigin(), farCullDistanceSqr ) )
				{
					if ( s_flSpecialInfectedFarCullStartTime[ entIndex ] <= 0.0f )
					{
						s_flSpecialInfectedFarCullStartTime[ entIndex ] = gpGlobals->curtime;
					}
					else if ( gpGlobals->curtime >= ( s_flSpecialInfectedFarCullStartTime[ entIndex ] + farCullGrace ) )
					{
						s_flSpecialInfectedFarCullStartTime[ entIndex ] = 0.0f;
						player->CommitSuicide( false, true );
						continue;
					}
				}
				else
				{
					s_flSpecialInfectedFarCullStartTime[ entIndex ] = 0.0f;
				}
			}
			else
			{
				s_flSpecialInfectedFarCullStartTime[ entIndex ] = 0.0f;
			}
		}

		if ( player->IsAlive() )
		{
			if ( player->GetSpecialInfectedDeathTimestamp() != 0.0f )
				player->SetSpecialInfectedDeathTimestamp( 0.0f );
			continue;
		}

		float deathTime = player->GetSpecialInfectedDeathTimestamp();
		if ( deathTime <= 0.0f )
		{
			player->SetSpecialInfectedDeathTimestamp( gpGlobals->curtime );
			deathTime = gpGlobals->curtime;
		}

		if ( !allowSpecialRespawnNow )
			continue;

		if ( gpGlobals->curtime >= ( deathTime + respawnTime ) )
		{
			if (player->IsBot()) {
				engine->ServerCommand(UTIL_VarArgs("kickid %d\n", player->GetUserID()));
			}
			else {
				player->SetZombieClass(random->RandomInt(1, 6));
				player->RoundRespawn();
			}
		}
	}

	// Tank spawns: use authored map anchors when survivors get close.
	if (z_special_tank_spawn_enabled.GetBool())
	{
		if (rules->IsFreezePeriod() || isRestartingRound)
		{
			if (isRestartingRound)
			{
				s_flNextTankSpawnAllowed = 0.0f;
			}
		}
		else if ( survivorCount > 0 &&
			!IsAnyAliveTank() &&
			gpGlobals->curtime >= s_flNextTankSpawnAllowed )
		{
			Vector anchor;
			if ( PickTankSpawnAnchorNearSurvivors( survivors, &anchor ) )
			{
				Vector spawnPos;
				QAngle spawnAng;
				if ( FindTankSpawnPosNearAnchor( anchor, survivors, &spawnPos, &spawnAng ) )
				{
					const float flTankSpawnChance = clamp( z_special_tank_spawn_chance.GetFloat(), 0.0f, 1.0f );
					const bool bShouldSpawnTank = ( flTankSpawnChance >= 1.0f ) || ( random->RandomFloat( 0.0f, 1.0f ) <= flTankSpawnChance );
					if ( bShouldSpawnTank )
					{
						SpawnNaturalTankAt( spawnPos, spawnAng );
					}

					// Consume the authored spawn opportunity even on a failed roll so we don't retry the chance every frame.
					s_flNextTankSpawnAllowed = gpGlobals->curtime + z_special_tank_spawn_cooldown.GetFloat();
				}
			}
		}
	}

	if (s_flNextSpecialInfectedSpawn <= 0.0f)
	{
		s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_interval.GetFloat();
	}

	if (gpGlobals->curtime < s_flNextSpecialInfectedSpawn)
		return;

	// Don't spawn during freeze time or while the round is being restarted.
	if (rules->IsFreezePeriod() || isRestartingRound)
	{
		s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_retry_delay.GetFloat();
		return;
	}

	CCSPlayer* survivor = rules ? rules->GetItTarget() : NULL;
	if (!survivor)
	{
		survivor = ( survivorCount > 0 ) ? survivors[ random->RandomInt( 0, survivorCount - 1 ) ] : NULL;
	}
	if (!survivor)
	{
		s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_retry_delay.GetFloat();
		return;
	}

	Vector spawnPos;
	QAngle spawnAng;
	if (!rules->FindSpecialInfectedSpawnPos(survivor, &spawnPos, &spawnAng))
	{
		s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_retry_delay.GetFloat();
		return;
	}

	if (!SpawnSpecialInfectedBotAt(spawnPos, spawnAng))
	{
		s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_retry_delay.GetFloat();
		return;
	}

	s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_interval.GetFloat();
}



static void TankHumanTakeoverThink(CCSGameRules* rules, bool isRestartingRound)
{
	if (!rules || !z_tank_human_takeover_enabled.GetBool())
		return;

	if (rules->IsFreezePeriod() || isRestartingRound)
	{
		ClearTankTakeoverState();
		return;
	}

	// If we owe a replacement bot (to preserve the human's previous class), try to spawn it.
	if (s_nTankTakeoverReplacementZombieClass > 0 && s_flTankTakeoverReplacementExpireTime > 0.0f && gpGlobals->curtime >= s_flTankTakeoverReplacementExpireTime)
	{
		s_nTankTakeoverReplacementZombieClass = 0;
		s_flTankTakeoverReplacementRetryTime = 0.0f;
		s_flTankTakeoverReplacementExpireTime = 0.0f;
		s_vecTankTakeoverReplacementPos = vec3_origin;
		s_angTankTakeoverReplacementAng.Init(0, 0, 0);
	}

	if (s_nTankTakeoverReplacementZombieClass > 0 && gpGlobals->curtime >= s_flTankTakeoverReplacementRetryTime)
	{
		// Only try occasionally so a full server doesn't spam work.
		s_flTankTakeoverReplacementRetryTime = gpGlobals->curtime + 0.25f;

		const int cls = (s_nTankTakeoverReplacementZombieClass > 0) ? s_nTankTakeoverReplacementZombieClass : 0;
		if (!TheBotProfiles)
		{
			s_nTankTakeoverReplacementZombieClass = 0;
			s_flTankTakeoverReplacementRetryTime = 0.0f;
			s_flTankTakeoverReplacementExpireTime = 0.0f;
			s_vecTankTakeoverReplacementPos = vec3_origin;
			s_angTankTakeoverReplacementAng.Init(0, 0, 0);
		}
		else if (cls > 0 && !rules->TeamFull(TEAM_CT))
		{
			CCSBot* bot = SpawnSpecialInfectedBotAt(s_vecTankTakeoverReplacementPos, s_angTankTakeoverReplacementAng, cls);
			if (bot)
			{
				bot->SetRogue(true);
				bot->Hunt();

				s_nTankTakeoverReplacementZombieClass = 0;
				s_flTankTakeoverReplacementExpireTime = 0.0f;
				s_vecTankTakeoverReplacementPos = vec3_origin;
				s_angTankTakeoverReplacementAng.Init(0, 0, 0);
			}
		}
	}

	// If we have a pending takeover, execute once the timer elapses.
	if (s_flTankTakeoverExecuteTime > 0.0f)
	{
		CCSPlayer* human = ToCSPlayer(s_hTankTakeoverHuman.Get());
		CCSPlayer* tankBot = ToCSPlayer(s_hTankTakeoverTankBot.Get());

		// Cancel if the participants are no longer valid.
		if (!human || !tankBot || !human->IsAlive() || !tankBot->IsAlive() || !tankBot->IsBot() || tankBot->GetZombieClass() != 8)
		{
			ClearTankTakeoverState();
			return;
		}

		if (gpGlobals->curtime < s_flTankTakeoverExecuteTime)
			return;

		const int reservedClass = (s_nTankTakeoverReservedZombieClass > 0) ? s_nTankTakeoverReservedZombieClass : random->RandomInt(1, 6);

		const Vector humanPos = human->GetAbsOrigin();
		const QAngle humanAng = human->GetAbsAngles();

		const Vector tankPos = tankBot->GetAbsOrigin();
		const QAngle tankAng = tankBot->GetAbsAngles();
		const int tankHealth = tankBot->GetHealth();

		// Kick the tank bot to make room, then hand the tank to the human.
		engine->ServerCommand(UTIL_VarArgs("kickid %d\n", tankBot->GetUserID()));

		Vector vel(vec3_origin);
		human->Teleport(&tankPos, &tankAng, &vel);
		ApplyTankLoadout(human, tankHealth);

		ClientPrint(human, HUD_PRINTTALK, "You are now the TANK!\nAttack the Survivors!");

		// Reserve their previous class by spawning a replacement bot near where they were.
		s_nTankTakeoverReplacementZombieClass = reservedClass;
		s_vecTankTakeoverReplacementPos = humanPos;
		s_angTankTakeoverReplacementAng = humanAng;
		s_flTankTakeoverReplacementRetryTime = gpGlobals->curtime + 0.25f;
		s_flTankTakeoverReplacementExpireTime = gpGlobals->curtime + 10.0f;

		ClearTankTakeoverState();
		return;
	}

	// No pending takeover: if there's a tank bot and human infected, start one.
	CCSPlayer* tankBot = FindAliveTankBot();
	if (!tankBot)
		return;

	CCSPlayer* human = SelectRandomAliveHumanInfected();
	if (!human)
		return;

	s_hTankTakeoverTankBot = tankBot;
	s_hTankTakeoverHuman = human;
	s_nTankTakeoverReservedZombieClass = human->GetZombieClass();
	s_flTankTakeoverExecuteTime = gpGlobals->curtime + z_tank_human_takeover_delay.GetFloat();

	ClientPrint(human, HUD_PRINTCENTER, "#L4D_tank_take_control");
}

#endif


/**
 * Player hull & eye position for standing, ducking, etc.  This version has a taller
 * player height, but goldsrc-compatible collision bounds.
 */
static CViewVectors g_CSViewVectors(
	Vector( 0, 0, 62 ),		// eye position

	Vector(-16, -16, 0 ),	// hull min
	Vector( 16,  16, 62 ),	// hull max

	Vector(-16, -16, 0 ),	// duck hull min
	Vector( 16,  16, 45 ),	// duck hull max
	Vector( 0, 0, 44 ),		// duck view

	Vector(-10, -10, -10 ),	// observer hull min
	Vector( 10,  10,  10 ),	// observer hull max

	Vector( 0, 0, 14 )		// dead view height
);


#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS(info_player_terrorist, CPointEntity);
LINK_ENTITY_TO_CLASS(info_survivor_rescue, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_counterterrorist,CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_logo,CPointEntity);
#endif

REGISTER_GAMERULES_CLASS( CCSGameRules );


BEGIN_NETWORK_TABLE_NOBASE( CCSGameRules, DT_CSGameRules )
	#ifdef CLIENT_DLL
		RecvPropBool( RECVINFO( m_bFreezePeriod ) ),
		RecvPropInt( RECVINFO( m_iRoundTime ) ),
		RecvPropFloat( RECVINFO( m_fRoundStartTime ) ),
		RecvPropFloat( RECVINFO( m_flGameStartTime ) ),
		RecvPropInt( RECVINFO( m_iHostagesRemaining ) ),
		RecvPropBool( RECVINFO( m_bMapHasBombTarget ) ),
		RecvPropBool( RECVINFO( m_bMapHasRescueZone ) ),
		RecvPropBool( RECVINFO( m_bLogoMap ) ),
		RecvPropBool( RECVINFO( m_bBlackMarket ) )
	#else
		SendPropBool( SENDINFO( m_bFreezePeriod ) ),
		SendPropInt( SENDINFO( m_iRoundTime ), 16 ),
		SendPropFloat( SENDINFO( m_fRoundStartTime ), 32, SPROP_NOSCALE ),
		SendPropFloat( SENDINFO( m_flGameStartTime ), 32, SPROP_NOSCALE ),
		SendPropInt( SENDINFO( m_iHostagesRemaining ), 4 ),
		SendPropBool( SENDINFO( m_bMapHasBombTarget ) ),
		SendPropBool( SENDINFO( m_bMapHasRescueZone ) ),
		SendPropBool( SENDINFO( m_bLogoMap ) ),
		SendPropBool( SENDINFO( m_bBlackMarket ) )
	#endif
END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS( cs_gamerules, CCSGameRulesProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( CSGameRulesProxy, DT_CSGameRulesProxy )


#ifdef CLIENT_DLL
	void RecvProxy_CSGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CCSGameRules *pRules = CSGameRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CCSGameRulesProxy, DT_CSGameRulesProxy )
		RecvPropDataTable( "cs_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_CSGameRules ), RecvProxy_CSGameRules )
	END_RECV_TABLE()
#else
	void* SendProxy_CSGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CCSGameRules *pRules = CSGameRules();
		Assert( pRules );
		return pRules;
	}

	BEGIN_SEND_TABLE( CCSGameRulesProxy, DT_CSGameRulesProxy )
		SendPropDataTable( "cs_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_CSGameRules ), SendProxy_CSGameRules )
	END_SEND_TABLE()
#endif



ConVar ammo_50AE_max( "ammo_50AE_max", "35", FCVAR_REPLICATED );
ConVar ammo_762mm_max( "ammo_762mm_max", "360", FCVAR_REPLICATED );
ConVar ammo_556mm_max( "ammo_556mm_max", "90", FCVAR_REPLICATED );
ConVar ammo_556mm_box_max( "ammo_556mm_box_max", "200", FCVAR_REPLICATED );
ConVar ammo_338mag_max( "ammo_338mag_max", "30", FCVAR_REPLICATED );
ConVar ammo_9mm_max( "ammo_9mm_max", "650", FCVAR_REPLICATED );
ConVar ammo_buckshot_max( "ammo_buckshot_max", "90", FCVAR_REPLICATED );
ConVar ammo_45acp_max( "ammo_45acp_max", "100", FCVAR_REPLICATED );
ConVar ammo_357sig_max( "ammo_357sig_max", "52", FCVAR_REPLICATED );
ConVar ammo_57mm_max( "ammo_57mm_max", "100", FCVAR_REPLICATED );
ConVar ammo_hegrenade_max( "ammo_hegrenade_max", "1", FCVAR_REPLICATED );
ConVar ammo_flashbang_max( "ammo_flashbang_max", "2", FCVAR_REPLICATED );
ConVar ammo_smokegrenade_max( "ammo_smokegrenade_max", "1", FCVAR_REPLICATED );

//ConVar mp_dynamicpricing( "mp_dynamicpricing", "0", FCVAR_REPLICATED, "Enables or Disables the dynamic weapon prices" );


extern ConVar sv_stopspeed;

ConVar mp_buytime( 
	"mp_buytime", 
	"1.5",
	FCVAR_REPLICATED,
	"How many minutes after round start players can buy items for.",
	true, 0.25,
	false, 0 );

ConVar mp_playerid(
	"mp_playerid",
	"0",
	FCVAR_REPLICATED,
	"Controls what information player see in the status bar: 0 all names; 1 team names; 2 no names",
	true, 0,
	true, 2 );

ConVar mp_playerid_delay(
	"mp_playerid_delay",
	"0.5",
	FCVAR_REPLICATED,
	"Number of seconds to delay showing information in the status bar",
	true, 0,
	true, 1 );

ConVar mp_playerid_hold(
	"mp_playerid_hold",
	"0.25",
	FCVAR_REPLICATED,
	"Number of seconds to keep showing old information in the status bar",
	true, 0,
	true, 1 );

ConVar mp_round_restart_delay(
	"mp_round_restart_delay",
	"5.0",
	FCVAR_REPLICATED,
	"Number of seconds to delay before restarting a round after a win",
	true, 0.0f,
	true, 10.0f );

ConVar sv_allowminmodels(
	"sv_allowminmodels",
	"1",
	FCVAR_REPLICATED | FCVAR_NOTIFY,
	"Allow or disallow the use of cl_minmodels on this server." );

#ifdef CLIENT_DLL

ConVar cl_autowepswitch(
	"cl_autowepswitch",
	"1",
	FCVAR_ARCHIVE | FCVAR_USERINFO,
	"Automatically switch to picked up weapons (if more powerful)" );

ConVar cl_autohelp(
	"cl_autohelp",
	"1",
	FCVAR_ARCHIVE | FCVAR_USERINFO,
	"Auto-help" );

#else

	// longest the intermission can last, in seconds
	#define MAX_INTERMISSION_TIME 120

	// Falling damage stuff.
	#define CS_PLAYER_FATAL_FALL_SPEED		1100	// approx 60 feet
	#define CS_PLAYER_MAX_SAFE_FALL_SPEED	580		// approx 20 feet
	#define CS_DAMAGE_FOR_FALL_SPEED		((float)100 / ( CS_PLAYER_FATAL_FALL_SPEED - CS_PLAYER_MAX_SAFE_FALL_SPEED )) // damage per unit per second.

	// These entities are preserved each round restart. The rest are removed and recreated.
	static const char *s_PreserveEnts[] =
	{
		"ai_network",
		"ai_hint",
		"cs_gamerules",
		"cs_team_manager",
		"cs_player_manager",
		"env_soundscape",
		"env_soundscape_proxy",
		"env_soundscape_triggerable",
		"env_sun",
		"env_wind",
		"env_fog_controller",
		"env_tonemap_controller",
		"env_cascade_light",
		"func_brush",
		"func_wall",
		"func_buyzone",
		"func_illusionary",
		"func_hostage_rescue",
		"func_bomb_target",
		"func_elevator",
		"info_elevator_floor",
		"infodecal",
		"info_projecteddecal",
		"info_node",
		"info_target",
		"info_node_hint",
		"info_player_counterterrorist",
		"info_player_terrorist",
		"info_enemy_terrorist_spawn",
		"info_deathmatch_spawn",
		"info_armsrace_counterterrorist",
		"info_armsrace_terrorist",
		"info_map_parameters",
		"keyframe_rope",
		"move_rope",
		"info_ladder",
		"player",
		"point_viewcontrol",
		"point_viewcontrol_multiplayer",
		"point_viewcontrol_survivor",
		"scene_manager",
		"shadow_control",
		"sky_camera",
		"soundent",
		"trigger_soundscape",
		"viewmodel",
		"hand_viewmodel",
		"predicted_viewmodel",
		"worldspawn",
		"point_devshot_camera",
		"logic_choreographed_scene",
		"cfe_player_decal",				// persistent player spray decals must be preserved
		//"logic_auto",					// preserving this will break all of the maps who currently rely on it getting destroyed each time the map entities are recreated
		"info_bomb_target_hint_A",
		"info_bomb_target_hint_B",
		"info_hostage_rescue_zone_hint",
		// for the training map
		"generic_actor",
		"vote_controller",
		"wearable_item",
		"point_hiding_spot",
		"game_coopmission_manager",
		"chicken",
		"", // END Marker
	};


	// --------------------------------------------------------------------------------------------------- //
	// Voice helper
	// --------------------------------------------------------------------------------------------------- //

	class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
	{
	public:
		virtual bool		CanPlayerHearPlayer( CBasePlayer *pListener, CBasePlayer *pTalker, bool &bProximity )
		{
			// Dead players can only be heard by other dead team mates
			if ( pTalker->IsAlive() == false )
			{
				if ( pListener->IsAlive() == false )
					return ( pListener->InSameTeam( pTalker ) );

				return false;
			}

			return ( pListener->InSameTeam( pTalker ) );
		}
	};
	CVoiceGameMgrHelper g_VoiceGameMgrHelper;
	IVoiceGameMgrHelper *g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;



	// --------------------------------------------------------------------------------------------------- //
	// Globals.
	// --------------------------------------------------------------------------------------------------- //

	// NOTE: the indices here must match TEAM_TERRORIST, TEAM_CT, TEAM_SPECTATOR, etc.
	const char *sTeamNames[] =
	{
		"Unassigned",
		"Spectator",
		"TERRORIST",
		"CT"
	};

	extern ConVar mp_maxrounds;

	ConVar mp_startmoney( 
		"mp_startmoney", 
		"800", 
		FCVAR_REPLICATED | FCVAR_NOTIFY,
		"amount of money each player gets when they reset",
		true, 800,
		true, 16000 );	

	ConVar mp_roundtime( 
		"mp_roundtime",
		"2.5",
		FCVAR_REPLICATED | FCVAR_NOTIFY,
		"How many minutes each round takes.",
		true, 1,	// min value
		true, 9		// max value
		);

	ConVar mp_freezetime( 
		"mp_freezetime",
		"6",
		FCVAR_REPLICATED | FCVAR_NOTIFY,
		"how many seconds to keep players frozen when the round starts",
		true, 0,	// min value
		true, 60	// max value
		);

	ConVar mp_c4timer( 
		"mp_c4timer", 
		"45", 
		FCVAR_REPLICATED | FCVAR_NOTIFY,
		"how long from when the C4 is armed until it blows",
		true, 10,	// min value
		true, 90	// max value
		);

	ConVar mp_limitteams( 
		"mp_limitteams", 
		"2", 
		FCVAR_REPLICATED | FCVAR_NOTIFY,
		"Max # of players 1 team can have over another (0 disables check)",
		true, 0,	// min value
		true, 30	// max value
		);

	ConVar mp_tkpunish( 
		"mp_tkpunish", 
		"0", 
		FCVAR_REPLICATED,
		"Will a TK'er be punished in the next round?  {0=no,  1=yes}" );

	ConVar mp_autokick(
		"mp_autokick",
		"1",
		FCVAR_REPLICATED,
		"Kick idle/team-killing players" );

	ConVar mp_spawnprotectiontime(
		"mp_spawnprotectiontime",
		"5",
		FCVAR_REPLICATED,
		"Kick players who team-kill within this many seconds of a round restart." );

	ConVar mp_humanteam( 
		"mp_humanteam", 
		"any", 
		FCVAR_REPLICATED,
		"Restricts human players to a single team {any, CT, T}" );

	ConVar mp_ignore_round_win_conditions(
		"mp_ignore_round_win_conditions",
		"0",
		FCVAR_REPLICATED,
		"Ignore conditions which would end the current round");

	ConCommand EndRound( "endround", &CCSGameRules::EndRound, "End the current round.", FCVAR_CHEAT );


	// --------------------------------------------------------------------------------------------------- //
	// Global helper functions.
	// --------------------------------------------------------------------------------------------------- //

	void InitBodyQue(void)
	{
		// FIXME: Make this work
	}


	Vector DropToGround( 
		CBaseEntity *pMainEnt, 
		const Vector &vPos, 
		const Vector &vMins, 
		const Vector &vMaxs )
	{
		trace_t trace;
		UTIL_TraceHull( vPos, vPos + Vector( 0, 0, -500 ), vMins, vMaxs, MASK_SOLID, pMainEnt, COLLISION_GROUP_NONE, &trace );
		return trace.endpos;
	}


	//-----------------------------------------------------------------------------
	// Purpose: This function can be used to find a valid placement location for an entity.
	//			Given an origin to start looking from and a minimum radius to place the entity at,
	//			it will sweep out a circle around vOrigin and try to find a valid spot (on the ground)
	//			where mins and maxs will fit.
	// Input  : *pMainEnt - Entity to place
	//			&vOrigin - Point to search around
	//			fRadius - Radius to search within
	//			nTries - Number of tries to attempt
	//			&mins - mins of the Entity
	//			&maxs - maxs of the Entity
	//			&outPos - Return point
	// Output : Returns true and fills in outPos if it found a spot.
	//-----------------------------------------------------------------------------
	bool EntityPlacementTest( CBaseEntity *pMainEnt, const Vector &vOrigin, Vector &outPos, bool bDropToGround )
	{
		// This function moves the box out in each dimension in each step trying to find empty space like this:
		//
		//											  X  
		//							   X			  X  
		// Step 1:   X     Step 2:    XXX   Step 3: XXXXX
		//							   X 			  X  
		//											  X  
		//
			 
		Vector mins, maxs;
		pMainEnt->CollisionProp()->WorldSpaceAABB( &mins, &maxs );
		mins -= pMainEnt->GetAbsOrigin();
		maxs -= pMainEnt->GetAbsOrigin();

		// Put some padding on their bbox.
		float flPadSize = 5;
		Vector vTestMins = mins - Vector( flPadSize, flPadSize, flPadSize );
		Vector vTestMaxs = maxs + Vector( flPadSize, flPadSize, flPadSize );

		// First test the starting origin.
		if ( UTIL_IsSpaceEmpty( pMainEnt, vOrigin + vTestMins, vOrigin + vTestMaxs ) )
		{
			if ( bDropToGround )
			{
				outPos = DropToGround( pMainEnt, vOrigin, vTestMins, vTestMaxs );
			}
			else
			{
				outPos = vOrigin;
			}
			return true;
		}

		Vector vDims = vTestMaxs - vTestMins;

		// Keep branching out until we get too far.
		int iCurIteration = 0;
		int nMaxIterations = 15;
		
		int offset = 0;
		do
		{
			for ( int iDim=0; iDim < 3; iDim++ )
			{
				float flCurOffset = offset * vDims[iDim];

				for ( int iSign=0; iSign < 2; iSign++ )
				{
					Vector vBase = vOrigin;
					vBase[iDim] += (iSign*2-1) * flCurOffset;
				
					if ( UTIL_IsSpaceEmpty( pMainEnt, vBase + vTestMins, vBase + vTestMaxs ) )
					{
						// Ensure that there is a clear line of sight from the spawnpoint entity to the actual spawn point.
						// (Useful for keeping things from spawning behind walls near a spawn point)
						trace_t tr;
						UTIL_TraceLine( vOrigin, vBase, MASK_SOLID, pMainEnt, COLLISION_GROUP_NONE, &tr );

						if ( tr.fraction != 1.0 )
						{
							continue;
						}
						
						if ( bDropToGround )
							outPos = DropToGround( pMainEnt, vBase, vTestMins, vTestMaxs );
						else
							outPos = vBase;

						return true;
					}
				}
			}

			++offset;
		} while ( iCurIteration++ < nMaxIterations );

	//	Warning( "EntityPlacementTest for ent %d:%s failed!\n", pMainEnt->entindex(), pMainEnt->GetClassname() );
		return false;
	}

	int UTIL_HumansInGame( bool ignoreSpectators )
	{
		int iCount = 0;

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *entity = CCSPlayer::Instance( i );

			if ( entity && !FNullEnt( entity->edict() ) )
			{
				if ( FStrEq( entity->GetPlayerName(), "" ) )
					continue;

				if ( FBitSet( entity->GetFlags(), FL_FAKECLIENT ) )
					continue;

				if ( ignoreSpectators && entity->GetTeamNumber() != TEAM_TERRORIST && entity->GetTeamNumber() != TEAM_CT )
					continue;

				if ( ignoreSpectators && entity->State_Get() == STATE_PICKINGCLASS )
					continue;

				iCount++;
			}
		}

		return iCount;
	}

	// --------------------------------------------------------------------------------------------------- //
	// CCSGameRules implementation.
	// --------------------------------------------------------------------------------------------------- //

	CCSGameRules::CCSGameRules()
	{
		m_iRoundTime = 0;
		m_iRoundWinStatus = WINNER_NONE;
		m_iFreezeTime = 0;

		m_fRoundStartTime = 0;
		m_bAllowWeaponSwitch = true;
		m_bFreezePeriod = true;
		m_iNumTerrorist = m_iNumCT = 0;	// number of players per team
		m_flRestartRoundTime = 0.1f; // restart first round as soon as possible
		m_iNumSpawnableTerrorist = m_iNumSpawnableCT = 0;
		m_bFirstConnected = false;
		m_bCompleteReset = false;
		m_iAccountTerrorist = m_iAccountCT = 0;
		m_iNumCTWins = 0;
		m_iNumTerroristWins = 0;
		m_iNumConsecutiveCTLoses = 0;
		m_iNumConsecutiveTerroristLoses = 0;
		m_bTargetBombed = false;
		m_bBombDefused = false;
		m_iTotalRoundsPlayed = -1;
		m_iUnBalancedRounds = 0;
		m_flGameStartTime = 0;
		m_iHostagesRemaining = 0;
		m_bLevelInitialized = false;
		m_bLogoMap = false;
		m_tmNextPeriodicThink = 0;

		m_bMapHasBombTarget = false;
		m_bMapHasRescueZone = false;

		m_iSpawnPointCount_Terrorist = 0;
		m_iSpawnPointCount_CT = 0;

		m_bTCantBuy = false;
		m_bCTCantBuy = false;
		m_bMapHasBuyZone = false;

		m_iLoserBonus = 0;

		m_iHostagesRescued = 0;
		m_iHostagesTouched = 0;
		m_flNextHostageAnnouncement = 0.0f;

        //=============================================================================
        // HPE_BEGIN
        // [dwenger] Reset rescue-related achievement values
        //=============================================================================

		// [tj] reset flawless and lossless round related flags
		m_bNoTerroristsKilled = true;
		m_bNoCTsKilled = true;
		m_bNoTerroristsDamaged = true;
		m_bNoCTsDamaged = true;
		m_pFirstKill = NULL;
		m_firstKillTime = 0;

		// [menglish] Reset fun fact values
		m_pFirstBlood = NULL;
		m_firstBloodTime = 0;

        m_bCanDonateWeapons = true;

		// [dwenger] Reset rescue-related achievement values
        m_pLastRescuer = NULL;
        m_iNumRescuers = 0;

		m_hostageWasInjured = false;
		m_hostageWasKilled = false;

		m_pFunFactManager = new CCSFunFactMgr();
		m_pFunFactManager->Init();

        //=============================================================================
        // HPE_END
        //=============================================================================

		m_iHaveEscaped = 0;
		m_bMapHasEscapeZone = false;
		m_iNumEscapers = 0;
		m_iNumEscapeRounds = 0;

		m_iMapHasVIPSafetyZone = 0;
		m_pVIP = NULL;
		m_iConsecutiveVIP = 0;

		m_bMapHasBombZone = false;
		m_bBombDropped = false;
		m_bBombPlanted = false;
		m_pLastBombGuy = NULL;

		m_bAllowWeaponSwitch = true;

		m_flNextHostageAnnouncement = gpGlobals->curtime;	// asap.

		ReadMultiplayCvars();

		m_pPrices = NULL;
		m_bBlackMarket = false;
		m_bDontUploadStats = false;

		// Create the team managers
		for ( int i = 0; i < ARRAYSIZE( sTeamNames ); i++ )
		{
			CTeam *pTeam = static_cast<CTeam*>(CreateEntityByName( "cs_team_manager" ));
			pTeam->Init( sTeamNames[i], i );

			g_Teams.AddToTail( pTeam );
		}

		if ( filesystem->FileExists( UTIL_VarArgs( "maps/cfg/%s.cfg", STRING(gpGlobals->mapname) ) ) )
		{
			// Execute a map specific cfg file - as in Day of Defeat
			// Map names cannot contain quotes or control characters so this is safe but silly that we have to do it.
			engine->ServerCommand( UTIL_VarArgs( "exec \"%s.cfg\" */maps\n", STRING(gpGlobals->mapname) ) );
			engine->ServerExecute();
		}

#ifndef CLIENT_DLL
		// stats

		if ( g_flGameStatsUpdateTime == 0.0f )
		{
			memset( g_iWeaponPurchases, 0, sizeof( g_iWeaponPurchases) );
			memset( g_iTerroristVictories, 0, sizeof( g_iTerroristVictories) );
			memset( g_iCounterTVictories, 0, sizeof( g_iTerroristVictories) );
			g_flGameStatsUpdateTime = CS_GAME_STATS_UPDATE; //Next update is between 22 and 24 hours.
		}
#endif
	}

	void CCSGameRules::AddPricesToTable( weeklyprice_t prices )
	{
		int iIndex = m_StringTableBlackMarket->FindStringIndex( "blackmarket_prices" );

		if ( iIndex == INVALID_STRING_INDEX )
		{
			m_StringTableBlackMarket->AddString( CBaseEntity::IsServer(), "blackmarket_prices", sizeof( weeklyprice_t), &prices );
		}
		else
		{
			m_StringTableBlackMarket->SetStringUserData( iIndex, sizeof( weeklyprice_t), &prices );
		}

		SetBlackMarketPrices( false );
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	CCSGameRules::~CCSGameRules()
	{
		// Note, don't delete each team since they are in the gEntList and will 
		// automatically be deleted from there, instead.
		g_Teams.Purge();
		if( m_pFunFactManager )
		{
			delete m_pFunFactManager;
		}
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	void CCSGameRules::UpdateClientData( CBasePlayer *player )
	{
	}

	//-----------------------------------------------------------------------------
	// Purpose: TF2 Specific Client Commands
	// Input  :
	// Output :
	//-----------------------------------------------------------------------------
	bool CCSGameRules::ClientCommand( CBaseEntity *pEdict, const CCommand &args )
	{
		CCSPlayer *pPlayer = ToCSPlayer( pEdict );

		const char* pcmd = args[0];
		if (FStrEq(pcmd, "voicemenu"))
		{
			if (args.ArgC() < 3)
				return true;

			CBaseMultiplayerPlayer* pMultiPlayerPlayer = dynamic_cast<CBaseMultiplayerPlayer*>(pPlayer);

			if (pMultiPlayerPlayer && pMultiPlayerPlayer->ShouldRunRateLimitedCommand(pcmd))
			{
				int iMenu = atoi(args[1]);
				int iItem = atoi(args[2]);

				VoiceCommand(pMultiPlayerPlayer, iMenu, iItem);
			}

			return true;
		}

		if ( FStrEq( args[0], "changeteam" ) )
		{
			return true;
		}
		else if ( FStrEq( args[0], "nextmap" ) )
		{
			if ( pPlayer->m_iNextTimeCheck < gpGlobals->curtime )
			{
				char szNextMap[32];

				if ( nextlevel.GetString() && *nextlevel.GetString() )
				{
					Q_strncpy( szNextMap, nextlevel.GetString(), sizeof( szNextMap ) );
				}
				else
				{
					GetNextLevelName( szNextMap, sizeof( szNextMap ) );
				}

				ClientPrint( pPlayer, HUD_PRINTTALK, "#game_nextmap", szNextMap);

				pPlayer->m_iNextTimeCheck = gpGlobals->curtime + 1;
			}
			return true;
		}
		else if( pPlayer->ClientCommand( args ) )
		{
			return true;
		}
		else if( BaseClass::ClientCommand( pEdict, args ) )
		{
			return true;
		}
		else if ( TheBots->ServerCommand( args.GetCommandString() ) )
		{
			return true;
		}
		else
		{
			return TheBots->ClientCommand( pPlayer, args );
		}
	}

	//-----------------------------------------------------------------------------
	// Purpose: Player has just spawned. Equip them.
	//-----------------------------------------------------------------------------
	void CCSGameRules::ClientCommandKeyValues( edict_t *pEntity, KeyValues *pKeyValues )
	{
		CCSPlayer *pPlayer = dynamic_cast< CCSPlayer * >( CBaseEntity::Instance( pEntity ) );
		if ( pPlayer )
		{
			char const *pszCommand = pKeyValues->GetName();
			if ( pszCommand && pszCommand[0] )
			{
				if ( FStrEq( pszCommand, "ClanTagChanged" ) )
				{
					pPlayer->SetClanTag( pKeyValues->GetString( "tag", "" ) );

					const char *teamName = "UNKNOWN";
					if ( pPlayer->GetTeam() )
					{
						teamName = pPlayer->GetTeam()->GetName();
					}
					UTIL_LogPrintf("\"%s<%i><%s><%s>\" triggered \"clantag\" (value \"%s\")\n", 
						pPlayer->GetPlayerName(),
						pPlayer->GetUserID(),
						pPlayer->GetNetworkIDString(),
						teamName,
						pKeyValues->GetString( "tag", "unknown" ) );
				}
			}
		}

		BaseClass::ClientCommandKeyValues( pEntity, pKeyValues );
	}

	//-----------------------------------------------------------------------------
	// Purpose: Player has just spawned. Equip them.
	//-----------------------------------------------------------------------------
	void CCSGameRules::PlayerSpawn( CBasePlayer *pBasePlayer )
	{
		CCSPlayer *pPlayer = ToCSPlayer( pBasePlayer );
		if ( !pPlayer )
			Error( "PlayerSpawn" );

		if ( pPlayer->State_Get() != STATE_ACTIVE )
			return;

		pPlayer->EquipSuit();
		
		bool addDefault = true;

		CBaseEntity	*pWeaponEntity = NULL;
		while ( ( pWeaponEntity = gEntList.FindEntityByClassname( pWeaponEntity, "game_player_equip" )) != NULL )
		{
			if ( addDefault )
			{
				// remove all our weapons and armor before touching the first game_player_equip
				pPlayer->RemoveAllItems( true );
			}
			pWeaponEntity->Touch( pPlayer );
			addDefault = false;
		}


		if ( addDefault || pPlayer->m_bIsVIP )
			pPlayer->GiveDefaultItems();
	}

	void CCSGameRules::BroadcastSound( const char *sound, int team )
	{
		CBroadcastRecipientFilter filter;
		filter.MakeReliable();

		if( team != -1 )
		{
			filter.RemoveAllRecipients();
			filter.AddRecipientsByTeam( GetGlobalTeam(team) );
		}

		UserMessageBegin ( filter, "SendAudio" );
			WRITE_STRING( sound );
		MessageEnd();
	}

	//-----------------------------------------------------------------------------
	// Purpose: Player has just spawned. Equip them.
	//-----------------------------------------------------------------------------

	// return a multiplier that should adjust the damage done by a blast at position vecSrc to something at the position
	// vecEnd.  This will take into account the density of an entity that blocks the line of sight from one position to
	// the other.
	//
	// this algorithm was taken from the HL2 version of RadiusDamage.
	float CCSGameRules::GetExplosionDamageAdjustment(Vector & vecSrc, Vector & vecEnd, CBaseEntity *pEntityToIgnore)
	{
		float retval = 0.0;
		trace_t tr;

		UTIL_TraceLine(vecSrc, vecEnd, MASK_SHOT, pEntityToIgnore, COLLISION_GROUP_NONE, &tr);
		if (tr.fraction == 1.0)
		{
			retval = 1.0;
		}
		else if (!(tr.DidHitWorld()) && (tr.m_pEnt != NULL) && (tr.m_pEnt != pEntityToIgnore) && (tr.m_pEnt->GetOwnerEntity() != pEntityToIgnore))
		{
			// if we didn't hit world geometry perhaps there's still damage to be done here.

			CBaseEntity *blockingEntity = tr.m_pEnt;

			// check to see if this part of the player is visible if entities are ignored.
			UTIL_TraceLine(vecSrc, vecEnd, CONTENTS_SOLID, NULL, COLLISION_GROUP_NONE, &tr);

			if (tr.fraction == 1.0)
			{
				if ((blockingEntity != NULL) && (blockingEntity->VPhysicsGetObject() != NULL))
				{
					int nMaterialIndex = blockingEntity->VPhysicsGetObject()->GetMaterialIndex();

					float flDensity;
					float flThickness;
					float flFriction;
					float flElasticity;

					physprops->GetPhysicsProperties( nMaterialIndex, &flDensity,
						&flThickness, &flFriction, &flElasticity );

					const float DENSITY_ABSORB_ALL_DAMAGE = 3000.0;
					float scale = flDensity / DENSITY_ABSORB_ALL_DAMAGE;
					if ((scale >= 0.0) && (scale < 1.0))
					{
						retval = 1.0 - scale;
					}
					else if (scale < 0.0)
					{
						// should never happen, but just in case.
						retval = 1.0;
					}
				}
				else
				{
					retval = 0.75; // we're blocked by something that isn't an entity with a physics module or world geometry, just cut damage in half for now.
				}
			}
		}

		return retval;
	}

	// returns the percentage of the player that is visible from the given point in the world.
	// return value is between 0 and 1.
	float CCSGameRules::GetAmountOfEntityVisible(Vector & vecSrc, CBaseEntity *entity)
	{
		float retval = 0.0;

		const float damagePercentageChest = 0.40;
		const float damagePercentageHead = 0.20;
		const float damagePercentageFeet = 0.20;
		const float damagePercentageRightSide = 0.10;
		const float damagePercentageLeftSide = 0.10;

		if (!(entity->IsPlayer()))
		{
			// the entity is not a player, so the damage is all or nothing.
			Vector vecTarget;
			vecTarget = entity->BodyTarget(vecSrc, false);

			return GetExplosionDamageAdjustment(vecSrc, vecTarget, entity);
		}

		CCSPlayer *player = (CCSPlayer *)entity;

		// check what parts of the player we can see from this point and modify the return value accordingly.
		float chestHeightFromFeet;

		float armDistanceFromChest = HalfHumanWidth;

		// calculate positions of various points on the target player's body
		Vector vecFeet = player->GetAbsOrigin();

		Vector vecChest = player->BodyTarget(vecSrc, false);
		chestHeightFromFeet = vecChest.z - vecFeet.z;  // compute the distance from the chest to the feet. (this accounts for ducking and the like)

		Vector vecHead = player->GetAbsOrigin();
		vecHead.z += HumanHeight;

		Vector vecRightFacing;
		AngleVectors(player->GetAbsAngles(), NULL, &vecRightFacing, NULL);

		vecRightFacing.NormalizeInPlace();
		vecRightFacing = vecRightFacing * armDistanceFromChest;

		Vector vecLeftSide = player->GetAbsOrigin();
		vecLeftSide.x -= vecRightFacing.x;
		vecLeftSide.y -= vecRightFacing.y;
		vecLeftSide.z += chestHeightFromFeet;

		Vector vecRightSide = player->GetAbsOrigin();
		vecRightSide.x += vecRightFacing.x;
		vecRightSide.y += vecRightFacing.y;
		vecRightSide.z += chestHeightFromFeet;

		// check chest
		float damageAdjustment = GetExplosionDamageAdjustment(vecSrc, vecChest, entity);
		retval += (damagePercentageChest * damageAdjustment);

		// check top of head
		damageAdjustment = GetExplosionDamageAdjustment(vecSrc, vecHead, entity);
		retval += (damagePercentageHead * damageAdjustment);

		// check feet
		damageAdjustment = GetExplosionDamageAdjustment(vecSrc, vecFeet, entity);
		retval += (damagePercentageFeet * damageAdjustment);

		// check left "edge"
		damageAdjustment = GetExplosionDamageAdjustment(vecSrc, vecLeftSide, entity);
		retval += (damagePercentageLeftSide * damageAdjustment);

		// check right "edge"
		damageAdjustment = GetExplosionDamageAdjustment(vecSrc, vecRightSide, entity);
		retval += (damagePercentageRightSide * damageAdjustment);

		return retval;
	}

	void CCSGameRules::RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, CBaseEntity * pEntityIgnore )
	{
		RadiusDamage( info, vecSrcIn, flRadius, iClassIgnore, false );
	}

	// Add the ability to ignore the world trace
	void CCSGameRules::RadiusDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore, bool bIgnoreWorld )
	{
		CBaseEntity *pEntity = NULL;
		trace_t		tr;
		float		falloff, damagePercentage;
		Vector		vecSpot;
		Vector		vecToTarget;
		Vector		vecEndPos;

        //=============================================================================
        // HPE_BEGIN:        
        //=============================================================================
         
		// [tj] The number of enemy players this explosion killed
        int numberOfEnemyPlayersKilledByThisExplosion = 0;
		
		// [tj] who we award the achievement to if enough players are killed
		CCSPlayer* pCSExplosionAttacker = ToCSPlayer(info.GetAttacker());

		// [tj] used to determine which achievement to award for sufficient kills
		CBaseEntity* pInflictor = info.GetInflictor();
		bool isGrenade = pInflictor && V_strcmp(pInflictor->GetClassname(), "hegrenade_projectile") == 0;
		bool isBomb = pInflictor && V_strcmp(pInflictor->GetClassname(), "planted_c4") == 0;
         
        //=============================================================================
        // HPE_END
        //=============================================================================
        

		vecEndPos.Init();

		Vector vecSrc = vecSrcIn;

		damagePercentage = 1.0;

		if ( flRadius )
			falloff = info.GetDamage() / flRadius;
		else
			falloff = 1.0;

		int bInWater = (UTIL_PointContents ( vecSrc ) & MASK_WATER) ? true : false;
		
		vecSrc.z += 1;// in case grenade is lying on the ground

		// iterate on all entities in the vicinity.
		for ( CEntitySphereQuery sphere( vecSrc, flRadius ); ( pEntity = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
		{
			//=============================================================================
			// HPE_BEGIN:
			// [tj] We have to save whether or not the player is killed so we don't give credit 
			//		for pre-dead players.
			//=============================================================================
			bool wasAliveBeforeExplosion = false;
			CCSPlayer* pCSExplosionVictim = ToCSPlayer(pEntity);
			if (pCSExplosionVictim)
			{
				wasAliveBeforeExplosion = pCSExplosionVictim->IsAlive();
			}
			//=============================================================================
			// HPE_END
			//=============================================================================
			if ( pEntity->m_takedamage != DAMAGE_NO )
			{
				// UNDONE: this should check a damage mask, not an ignore
				if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
				{// houndeyes don't hurt other houndeyes with their attack
					continue;
				}

				// blasts don't travel into or out of water
				if ( !bIgnoreWorld )
				{
					if (bInWater && pEntity->GetWaterLevel() == 0)
						continue;
					if (!bInWater && pEntity->GetWaterLevel() == 3)
						continue;
				}

				// radius damage can only be blocked by the world
				vecSpot = pEntity->BodyTarget( vecSrc );

				bool bHit = false;

				if( bIgnoreWorld )
				{
					vecEndPos = vecSpot;
					bHit = true;
				}
				else
				{
					// get the percentage of the target entity that is visible from the
					// explosion position.
					damagePercentage = GetAmountOfEntityVisible(vecSrc, pEntity);
					if (damagePercentage > 0.0)
					{
						vecEndPos = vecSpot;

						bHit = true;
					}
				}

				if ( bHit )
				{
					// the explosion can 'see' this entity, so hurt them!
					//vecToTarget = ( vecSrc - vecEndPos );
					vecToTarget = ( vecEndPos - vecSrc );

					// use a Gaussian function to describe the damage falloff over distance, with flRadius equal to 3 * sigma
					// this results in the following values:
					// 
					// Range Fraction  Damage
					//		0.0			100%
					// 		0.1			96%
					// 		0.2			84%
					// 		0.3			67%
					// 		0.4			49%
					// 		0.5			32%
					// 		0.6			20%
					// 		0.7			11%
					// 		0.8			 6%
					// 		0.9			 3%
					// 		1.0			 1%

					float fDist = vecToTarget.Length();
					float fSigma = flRadius / 3.0f; // flRadius specifies 3rd standard deviation (0.0111 damage at this range)
					float fGaussianFalloff = exp(-fDist * fDist / (2.0f * fSigma * fSigma));
					float flAdjustedDamage = info.GetDamage() * fGaussianFalloff * damagePercentage;
				
					if ( flAdjustedDamage > 0 )
					{
						CTakeDamageInfo adjustedInfo = info;
						adjustedInfo.SetDamage( flAdjustedDamage );

						Vector dir = vecToTarget;
						VectorNormalize( dir );

						// If we don't have a damage force, manufacture one
						if ( adjustedInfo.GetDamagePosition() == vec3_origin || adjustedInfo.GetDamageForce() == vec3_origin )
						{
							CalculateExplosiveDamageForce( &adjustedInfo, dir, vecSrc, 1.5	/* explosion scale! */ );
						}
						else
						{
							// Assume the force passed in is the maximum force. Decay it based on falloff.
							float flForce = adjustedInfo.GetDamageForce().Length() * falloff;
							adjustedInfo.SetDamageForce( dir * flForce );
							adjustedInfo.SetDamagePosition( vecSrc );
						}

						Vector vecTarget;
						vecTarget = pEntity->BodyTarget(vecSrc, false);

						UTIL_TraceLine(vecSrc, vecTarget, MASK_SHOT, NULL, COLLISION_GROUP_NONE, &tr);

						// blasts always hit chest
						tr.hitgroup = HITGROUP_GENERIC;

						if (tr.fraction != 1.0)
						{
							// this has to be done to make breakable glass work.
							ClearMultiDamage( );
							pEntity->DispatchTraceAttack( adjustedInfo, dir, &tr );
							ApplyMultiDamage();
						}
						else
						{
							pEntity->TakeDamage( adjustedInfo );
						}
			
						// Now hit all triggers along the way that respond to damage... 
						pEntity->TraceAttackToTriggers( adjustedInfo, vecSrc, vecEndPos, dir );
						//=============================================================================
						// HPE_BEGIN:
						// [sbodenbender] Increment grenade damage stat
						//=============================================================================
						if (pCSExplosionVictim && pCSExplosionAttacker && isGrenade)
						{
							CCS_GameStats.IncrementStat(pCSExplosionAttacker, CSSTAT_GRENADE_DAMAGE, static_cast<int>(adjustedInfo.GetDamage()));
						}
						//=============================================================================
						// HPE_END
						//=============================================================================
					}
				}
			}
            
            //=============================================================================
            // HPE_BEGIN:
            // [tj] Count up victims of area of effect damage for achievement purposes
            //=============================================================================
             
            if (pCSExplosionVictim)
			{
				//If the bomb is exploding, set the attacker to the planter (we can't put this in the CTakeDamageInfo, since
				//players aren't supposed to get credit for bomb kills)
				if (isBomb)
				{
					CPlantedC4* bomb = static_cast<CPlantedC4*> (pInflictor);
					if (bomb)
					{
						pCSExplosionAttacker = bomb->GetPlanter();
					}
				}

				//Count check to make sure we killed an enemy player
				if(	pCSExplosionAttacker &&                  
					!pCSExplosionVictim->IsAlive() && 
					wasAliveBeforeExplosion &&
					pCSExplosionVictim->GetTeamNumber() != pCSExplosionAttacker->GetTeamNumber())               
				{
					numberOfEnemyPlayersKilledByThisExplosion++;
				}
			}             
            //=============================================================================
            // HPE_END
            //=============================================================================
            
		}

		//=============================================================================
		// HPE_BEGIN:
		// [tj] //Depending on which type of explosion it was, award the appropriate achievement.
		//=============================================================================
		
		if (pCSExplosionAttacker && isGrenade && numberOfEnemyPlayersKilledByThisExplosion >= AchievementConsts::GrenadeMultiKill_MinKills)
		{
			pCSExplosionAttacker->AwardAchievement(CSGrenadeMultikill);    
			pCSExplosionAttacker->CheckMaxGrenadeKills(numberOfEnemyPlayersKilledByThisExplosion);

		}
		if (pCSExplosionAttacker && isBomb && numberOfEnemyPlayersKilledByThisExplosion >= AchievementConsts::BombMultiKill_MinKills)
		{
			pCSExplosionAttacker->AwardAchievement(CSBombMultikill);            
		}

		//=============================================================================
		// HPE_END
		//=============================================================================
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *pVictim - 
	//			*pKiller - 
	//			*pInflictor - 
	//-----------------------------------------------------------------------------
	void CCSGameRules::DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info )
	{
		// Work out what killed the player, and send a message to all clients about it
		const char *killer_weapon_name = "world";		// by default, the player is killed by the world
		int killer_ID = 0;

		// Find the killer & the scorer
		CBaseEntity *pInflictor = info.GetInflictor();
		CBaseEntity *pKiller = info.GetAttacker();
		CBasePlayer *pScorer = GetDeathScorer( pKiller, pInflictor );
		CCSPlayer *pCSVictim = (CCSPlayer*)(pVictim);

		bool bHeadshot = false;

		if ( pScorer )	// Is the killer a client?
		{
			killer_ID = pScorer->GetUserID();
		
			if( info.GetDamageType() & DMG_HEADSHOT )
			{
				//to enable drawing the headshot icon as well as the weapon icon, 
				bHeadshot = true;
			}
			
			if ( pInflictor )
			{
				if ( pInflictor == pScorer )
				{
					// If the inflictor is the killer,  then it must be their current weapon doing the damage
					if ( pScorer->GetActiveWeapon() )
					{
						killer_weapon_name = pScorer->GetActiveWeapon()->GetClassname(); //GetDeathNoticeName();
					}
				}
				else
				{
					killer_weapon_name = STRING( pInflictor->m_iClassname );  // it's just that easy
				}
			}
		}
		else
		{
			killer_weapon_name = STRING( pInflictor->m_iClassname );
		}

		// strip the NPC_* or weapon_* from the inflictor's classname
		if ( strncmp( killer_weapon_name, "weapon_", 7 ) == 0 )
		{
			killer_weapon_name += 7;
		}
		else if ( strncmp( killer_weapon_name, "NPC_", 8 ) == 0 )
		{
			killer_weapon_name += 8;
		}
		else if ( strncmp( killer_weapon_name, "func_", 5 ) == 0 )
		{
			killer_weapon_name += 5;
		}
		else if( strncmp( killer_weapon_name, "hegrenade", 9 ) == 0 )	//"hegrenade_projectile"	
		{
			killer_weapon_name = "hegrenade";
		}
		else if( strncmp( killer_weapon_name, "flashbang", 9 ) == 0 )	//"flashbang_projectile"
		{
			killer_weapon_name = "flashbang";
		}

		IGameEvent * event = gameeventmanager->CreateEvent( "player_death" );

		if ( event )
		{
			event->SetInt("userid", pVictim->GetUserID() );
			event->SetInt("attacker", killer_ID );
			event->SetString("weapon", killer_weapon_name );
			event->SetInt("headshot", bHeadshot ? 1 : 0 );
			event->SetInt("priority", bHeadshot ? 8 : 7 );	// HLTV event priority, not transmitted
			if ( pCSVictim->GetDeathFlags() & CS_DEATH_DOMINATION )
			{
				event->SetInt( "dominated", 1 );
			}
			else if ( pCSVictim->GetDeathFlags() & CS_DEATH_REVENGE )
			{
				event->SetInt( "revenge", 1 );
			}
			
			gameeventmanager->FireEvent( event );
		}
	}

	//=========================================================
	//=========================================================
	void CCSGameRules::PlayerKilled( CBasePlayer *pVictim, const CTakeDamageInfo &info )
	{
		CBaseEntity *pInflictor = info.GetInflictor();
		CBaseEntity *pKiller = info.GetAttacker();
		CBasePlayer *pScorer = GetDeathScorer( pKiller, pInflictor );
		CCSPlayer *pCSVictim = (CCSPlayer *)pVictim;
		CCSPlayer *pCSScorer = (CCSPlayer *)pScorer;

		CCS_GameStats.PlayerKilled( pVictim, info );
		//=============================================================================
		// HPE_BEGIN:        
		// [tj] Flag the round as non-lossless for the appropriate team.
		// [menglish] Set the death flags depending on a nemesis system
		//=============================================================================

		if (pVictim->GetTeamNumber() == TEAM_TERRORIST)
		{
			m_bNoTerroristsKilled = false;
			m_bNoTerroristsDamaged = false;            
		}
		if (pVictim->GetTeamNumber() == TEAM_CT)
		{
			m_bNoCTsKilled = false;
			m_bNoCTsDamaged = false;
		}

        m_bCanDonateWeapons = false;

		if ( m_pFirstKill == NULL && pCSScorer != pVictim )
		{
			m_pFirstKill = pCSScorer;
			m_firstKillTime = gpGlobals->curtime - m_fRoundStartTime;
		}

		// determine if this kill affected a nemesis relationship
		int iDeathFlags = 0;
		if ( pScorer )
		{	
            CCS_GameStats.CalculateOverkill( pCSScorer, pCSVictim);
			CCS_GameStats.CalcDominationAndRevenge( pCSScorer, pCSVictim, &iDeathFlags );            
		}
		pCSVictim->SetDeathFlags( iDeathFlags );	
		//=============================================================================
		// HPE_END
		//=============================================================================

		// If we're killed by the C4, we do a subset of BaseClass::PlayerKilled()
		// Specifically, we shouldn't lose any points or show death notices, to match goldsrc
		if ( Q_strcmp(pKiller->GetClassname(), "planted_c4" ) == 0 )
		{
			// dvsents2: uncomment when removing all FireTargets
			// variant_t value;
			// g_EventQueue.AddEvent( "game_playerdie", "Use", value, 0, pVictim, pVictim );
			FireTargets( "game_playerdie", pVictim, pVictim, USE_TOGGLE, 0 );
		}
		else
		{
			BaseClass::PlayerKilled( pVictim, info );
		}

		// check for team-killing, and give monetary rewards/penalties
		// Find the killer & the scorer
		if ( !pScorer )
			return;

		if ( IPointsForKill( pScorer, pVictim ) < 0 )
		{
			// team-killer!
			pCSScorer->AddAccount( -3300 );
			++pCSScorer->m_iTeamKills;
			pCSScorer->m_bJustKilledTeammate = true;

			ClientPrint( pCSScorer, HUD_PRINTCENTER, "#Killed_Teammate" );
			if ( mp_autokick.GetBool() )
			{
				char strTeamKills[64];
				Q_snprintf( strTeamKills, sizeof( strTeamKills ), "%d", pCSScorer->m_iTeamKills );
				ClientPrint( pCSScorer, HUD_PRINTCONSOLE, "#Game_teammate_kills", strTeamKills ); // this includes a " of 3" in it

				if ( pCSScorer->m_iTeamKills >= 3 )
				{
					ClientPrint( pCSScorer, HUD_PRINTCONSOLE, "#Banned_For_Killing_Teammates" );
					engine->ServerCommand( UTIL_VarArgs( "kickid %d\n", pCSScorer->GetUserID() ) );
				}
				else if ( mp_spawnprotectiontime.GetInt() > 0 && GetRoundElapsedTime() < mp_spawnprotectiontime.GetInt() )
				{
					ClientPrint( pCSScorer, HUD_PRINTCONSOLE, "#Banned_For_Killing_Teammates" );
					engine->ServerCommand( UTIL_VarArgs( "kickid %d\n", pCSScorer->GetUserID() ) );
				}
			}

			if ( !(pCSScorer->m_iDisplayHistoryBits & DHF_FRIEND_KILLED) )
			{
				pCSScorer->m_iDisplayHistoryBits |= DHF_FRIEND_KILLED;
				pCSScorer->HintMessage( "#Hint_careful_around_teammates", false );
			}
		}
		else
		{
			//=============================================================================
			// HPE_BEGIN:
			// [tj] Added a check to make sure we don't get money for suicides.
			//=============================================================================
			if (pCSScorer != pCSVictim)
			{
			//=============================================================================
			// HPE_END
			//=============================================================================
				if ( pCSVictim->IsVIP() )
				{
					pCSScorer->HintMessage( "#Hint_reward_for_killing_vip", true );
					pCSScorer->AddAccount( 2500 );
				}
				else			
				{
					pCSScorer->AddAccount( 300 );
				}
			}

			if ( !(pCSScorer->m_iDisplayHistoryBits & DHF_ENEMY_KILLED) )
			{
				pCSScorer->m_iDisplayHistoryBits |= DHF_ENEMY_KILLED;
				pCSScorer->HintMessage( "#Hint_win_round_by_killing_enemy", false );
			}
		}
	}


	void CCSGameRules::InitDefaultAIRelationships()
	{
		//  Allocate memory for default relationships
		CBaseCombatCharacter::AllocateDefaultRelationships();

		// --------------------------------------------------------------
		// First initialize table so we can report missing relationships
		// --------------------------------------------------------------
		int i, j;
		for (i=0;i<NUM_AI_CLASSES;i++)
		{
			for (j=0;j<NUM_AI_CLASSES;j++)
			{
				// By default all relationships are neutral of priority zero
				CBaseCombatCharacter::SetDefaultRelationship( (Class_T)i, (Class_T)j, D_NU, 0 );
			}
		}



		// ------------------------------------------------------------
		//	> CLASS_ANTLION
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_PROTOSNIPER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_ANTLION, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ANTLION, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_BARNACLE
		//
		//  In this case, the relationship D_HT indicates which characters
		//  the barnacle will try to eat.
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_BARNACLE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_MANHACK, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_EARTH_FAUNA, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BARNACLE, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_BULLSEYE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_PLAYER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_ANTLION, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_VORTIGAUNT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_ZOMBIE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_PLAYER_ALLY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_PLAYER_ALLY_VITAL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSEYE, CLASS_HACKED_ROLLERMINE, D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_BULLSQUID
		// ------------------------------------------------------------
		/*
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_ANTLION,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_BARNACLE,			D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_CITIZEN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_CITIZEN_REBEL,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_COMBINE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_COMBINE_HUNTER,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_CONSCRIPT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_HEADCRAB,			D_HT, 1);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_HOUNDEYE,			D_HT, 1);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_MANHACK,			D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_METROPOLICE,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_MILITARY,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_STALKER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_VORTIGAUNT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_ZOMBIE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_HACKED_ROLLERMINE,D_HT, 0);
		*/
		// ------------------------------------------------------------
		//	> CLASS_CITIZEN_PASSIVE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_PLAYER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_BARNACLE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE,	CLASS_BULLSQUID,		D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_COMBINE_HUNTER, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_HEADCRAB, D_FR, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE,	CLASS_HOUNDEYE,			D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_MANHACK, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_MISSILE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_VORTIGAUNT, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_ZOMBIE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_PLAYER_ALLY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_PLAYER_ALLY_VITAL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_PASSIVE, CLASS_HACKED_ROLLERMINE, D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_CITIZEN_REBEL
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_PLAYER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_BARNACLE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL,		CLASS_BULLSQUID,		D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL,		CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_MISSILE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_VORTIGAUNT, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_PLAYER_ALLY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_PLAYER_ALLY_VITAL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CITIZEN_REBEL, CLASS_HACKED_ROLLERMINE, D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_COMBINE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_BARNACLE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_COMBINE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_COMBINE_GUNSHIP, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_COMBINE_HUNTER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_COMBINE_GUNSHIP
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP,		CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_COMBINE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_COMBINE_GUNSHIP, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_COMBINE_HUNTER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP,		CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_MISSILE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_GUNSHIP, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_COMBINE_HUNTER
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER,	CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_COMBINE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_COMBINE_GUNSHIP, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_COMBINE_HUNTER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER,	CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_COMBINE_HUNTER, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_CONSCRIPT
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_PLAYER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_BARNACLE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_VORTIGAUNT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_PLAYER_ALLY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_PLAYER_ALLY_VITAL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_CONSCRIPT, CLASS_HACKED_ROLLERMINE, D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_FLARE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_PLAYER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_ANTLION, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_VORTIGAUNT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_ZOMBIE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_PLAYER_ALLY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_PLAYER_ALLY_VITAL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_FLARE, CLASS_HACKED_ROLLERMINE, D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_HEADCRAB
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB,			CLASS_BULLSQUID,		D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_ZOMBIE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HEADCRAB, CLASS_HACKED_ROLLERMINE, D_FR, 0);

		// ------------------------------------------------------------
		//	> CLASS_HOUNDEYE
		// ------------------------------------------------------------
		/*
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_ANTLION,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_BULLSEYE,			D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_BULLSQUID,		D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_CITIZEN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_CITIZEN_REBEL,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_COMBINE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_COMBINE_HUNTER,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_CONSCRIPT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_METROPOLICE,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_MILITARY,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_VORTIGAUNT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_HACKED_ROLLERMINE,D_HT, 0);
		*/

		// ------------------------------------------------------------
		//	> CLASS_MANHACK
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_HEADCRAB, D_HT, -1);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK,			CLASS_HOUNDEYE,			D_HT,-1);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MANHACK, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_METROPOLICE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE,		CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE,		CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_METROPOLICE, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_MILITARY
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MILITARY, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_MISSILE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MISSILE, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_NONE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_PLAYER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_ANTLION, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE,				CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE,				CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_VORTIGAUNT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_ZOMBIE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_PLAYER_ALLY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_PLAYER_ALLY_VITAL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_NONE, CLASS_HACKED_ROLLERMINE, D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_PLAYER
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_PLAYER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_BARNACLE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_BULLSEYE, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_CITIZEN_PASSIVE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_CITIZEN_REBEL, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_COMBINE_GUNSHIP, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_VORTIGAUNT, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_PROTOSNIPER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_PLAYER_ALLY, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_PLAYER_ALLY_VITAL, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER, CLASS_HACKED_ROLLERMINE, D_LI, 0);

		// ------------------------------------------------------------
		//	> CLASS_PLAYER_ALLY
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_PLAYER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_BARNACLE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_HEADCRAB, D_FR, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_VORTIGAUNT, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_ZOMBIE, D_FR, 1);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_PROTOSNIPER, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_PLAYER_ALLY, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_PLAYER_ALLY_VITAL, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY, CLASS_HACKED_ROLLERMINE, D_LI, 0);

		// ------------------------------------------------------------
		//	> CLASS_PLAYER_ALLY_VITAL
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_PLAYER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_BARNACLE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL,	CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_COMBINE_HUNTER, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL,	CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_VORTIGAUNT, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_PROTOSNIPER, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_PLAYER_ALLY, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_PLAYER_ALLY_VITAL, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_ALLY_VITAL, CLASS_HACKED_ROLLERMINE, D_LI, 0);

		// ------------------------------------------------------------
		//	> CLASS_SCANNER
		// ------------------------------------------------------------	
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_COMBINE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_COMBINE_GUNSHIP, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_COMBINE_HUNTER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_MANHACK, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_METROPOLICE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_MILITARY, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_SCANNER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_STALKER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_ZOMBIE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_PROTOSNIPER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_SCANNER, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_STALKER
		// ------------------------------------------------------------	
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_ZOMBIE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_STALKER, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_VORTIGAUNT
		// ------------------------------------------------------------	
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_PLAYER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_BARNACLE, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT,		CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_CITIZEN_PASSIVE, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_CITIZEN_REBEL, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT,		CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_VORTIGAUNT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_PLAYER_ALLY, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_PLAYER_ALLY_VITAL, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_VORTIGAUNT, CLASS_HACKED_ROLLERMINE, D_LI, 0);

		// ------------------------------------------------------------
		//	> CLASS_ZOMBIE
		// ------------------------------------------------------------	
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_HEADCRAB, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_MANHACK, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_MILITARY, D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_ZOMBIE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ZOMBIE, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_PROTOSNIPER
		// ------------------------------------------------------------	
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_COMBINE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_METROPOLICE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_MILITARY, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_MISSILE, D_NU, 5);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_STALKER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PROTOSNIPER, CLASS_HACKED_ROLLERMINE, D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_EARTH_FAUNA
		//
		// Hates pretty much everything equally except other earth fauna.
		// This will make the critter choose the nearest thing as its enemy.
		// ------------------------------------------------------------	
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_NONE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_PLAYER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_CITIZEN_PASSIVE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_CITIZEN_REBEL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_COMBINE_GUNSHIP, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_COMBINE_HUNTER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_CONSCRIPT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_FLARE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_MANHACK, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_MISSILE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_SCANNER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_VORTIGAUNT, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_ZOMBIE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_PROTOSNIPER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_EARTH_FAUNA, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_PLAYER_ALLY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_PLAYER_ALLY_VITAL, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_EARTH_FAUNA, CLASS_HACKED_ROLLERMINE, D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_HACKED_ROLLERMINE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_NONE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_PLAYER, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_ANTLION, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_BARNACLE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_BULLSEYE, D_NU, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE,			CLASS_BULLSQUID,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_CITIZEN_PASSIVE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_CITIZEN_REBEL, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_COMBINE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_COMBINE_GUNSHIP, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_COMBINE_HUNTER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_CONSCRIPT, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_FLARE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_HEADCRAB, D_HT, 0);
		//CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE,			CLASS_HOUNDEYE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_MANHACK, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_METROPOLICE, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_MILITARY, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_MISSILE, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_SCANNER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_STALKER, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_VORTIGAUNT, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_ZOMBIE, D_HT, 1);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_PROTOSNIPER, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_EARTH_FAUNA, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_PLAYER_ALLY, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_PLAYER_ALLY_VITAL, D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HACKED_ROLLERMINE, CLASS_HACKED_ROLLERMINE, D_LI, 0);
	}

	//------------------------------------------------------------------------------
	// Purpose : Return classify text for classify type
	//------------------------------------------------------------------------------
	const char *CCSGameRules::AIClassText(int classType)
	{
		switch (classType)
		{
			case CLASS_NONE:			return "CLASS_NONE";
			case CLASS_PLAYER:			return "CLASS_PLAYER";
			default:					return "MISSING CLASS in ClassifyText()";
		}
	}

	//-----------------------------------------------------------------------------
	// Purpose: When gaining new technologies in TF, prevent auto switching if we
	//  receive a weapon during the switch
	// Input  : *pPlayer - 
	//			*pWeapon - 
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool CCSGameRules::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBaseCombatWeapon *pWeapon )
	{
		bool bIsBeingGivenItem = false;
		CCSPlayer *pCSPlayer = ToCSPlayer( pPlayer );
		if ( pCSPlayer && pCSPlayer->IsBeingGivenItem() )
			bIsBeingGivenItem = true;

		if ( pPlayer->GetActiveWeapon() && pPlayer->IsNetClient() && !bIsBeingGivenItem )
		{
			// Player has an active item, so let's check cl_autowepswitch.
			const char *cl_autowepswitch = engine->GetClientConVarValue( engine->IndexOfEdict( pPlayer->edict() ), "cl_autowepswitch" );
			if ( cl_autowepswitch && atoi( cl_autowepswitch ) <= 0 )
			{
				return false;
			}
		}

		if ( pPlayer->IsBot() && !bIsBeingGivenItem )
		{
			return false;
		}

		if ( !GetAllowWeaponSwitch() )
		{
			return false;
		}

		return BaseClass::FShouldSwitchWeapon( pPlayer, pWeapon );
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : allow - 
	//-----------------------------------------------------------------------------
	void CCSGameRules::SetAllowWeaponSwitch( bool allow )
	{
		m_bAllowWeaponSwitch = allow;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool CCSGameRules::GetAllowWeaponSwitch()
	{
		return m_bAllowWeaponSwitch;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : *pPlayer - 
	// Output : const char
	//-----------------------------------------------------------------------------
	const char *CCSGameRules::SetDefaultPlayerTeam( CBasePlayer *pPlayer )
	{
		Assert( pPlayer );
		return BaseClass::SetDefaultPlayerTeam( pPlayer );
	}


	void CCSGameRules::LevelInitPreEntity()
	{
		BaseClass::LevelInitPreEntity();

		// TODO for CZ-style hostages: TheHostageChatter->Precache();
	}


	void CCSGameRules::LevelInitPostEntity()
	{
		BaseClass::LevelInitPostEntity();

		m_bLevelInitialized = false; // re-count CT and T start spots now that they exist

		// Figure out from the entities in the map what kind of map this is (bomb run, prison escape, etc).
		CheckMapConditions();

		// Start special infected spawns fresh each map.
		s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_interval.GetFloat();
		ResetSpecialInfectedFarCullTimers();

		// Start listen host auto-join fresh each map.
		s_bListenHostAutoJoinDone = false;
		s_nListenHostAutoJoinBotsRemaining = 0;
		s_flListenHostAutoJoinNextBotTry = 0.0f;

		// Start common infected + survivor squad fresh each map.
		s_flNextCommonInfectedSpawn = 0.0f;
		s_flNextCommonHordeStart = 0.0f;
		s_flCommonHordeEndTime = 0.0f;
		ResetCommonInfectedHordeState();
		s_flNextSurvivorSquadUpdate = 0.0f;
		s_hSurvivorSquadLeader = NULL;

		// Start background/logo population fresh each map.
		s_flNextBackgroundPopulate = 0.0f;
		s_flNextTankSpawnAllowed = 0.0f;

		ResetLeavingSafetyMusicState();
		ResetSaferoomTransitionState();
	}
	
	INetworkStringTable *g_StringTableBlackMarket = NULL;

	void CCSGameRules::CreateCustomNetworkStringTables( void )
	{
		m_StringTableBlackMarket = g_StringTableBlackMarket;

		if ( 0 )//mp_dynamicpricing.GetBool() )
		{

			if ( m_bBlackMarket == false )
			{
				Msg( "ERROR: mp_dynamicpricing set to 1 but couldn't download the price list!\n" );
			}
		}
		else
		{
			m_bBlackMarket = false;
			SetBlackMarketPrices( true );
		}
	}

	float CCSGameRules::FlPlayerFallDamage( CBasePlayer *pPlayer )
	{
		float fFallVelocity = pPlayer->m_Local.m_flFallVelocity - CS_PLAYER_MAX_SAFE_FALL_SPEED;
		float fallDamage = fFallVelocity * CS_DAMAGE_FOR_FALL_SPEED * 1.25;

		if ( fallDamage > 0.0f )
		{
			// let the bots know
			IGameEvent * event = gameeventmanager->CreateEvent( "player_falldamage" );
			if ( event )
			{
				event->SetInt( "userid", pPlayer->GetUserID() );
				event->SetFloat( "damage", fallDamage );
				event->SetInt( "priority", 4 );	// HLTV event priority, not transmitted
				
				gameeventmanager->FireEvent( event );
			}
		}

		return fallDamage;
	} 

	
	void CCSGameRules::ClientDisconnected( edict_t *pClient )
	{
		BaseClass::ClientDisconnected( pClient );

        //=============================================================================
        // HPE_BEGIN:
        // [tj] Clear domination data when a player disconnects
        //=============================================================================
         
        CCSPlayer *pPlayer = ToCSPlayer( GetContainingEntity( pClient ) );
        if ( pPlayer )
        {
            pPlayer->RemoveNemesisRelationships();
        }
         
        //=============================================================================
        // HPE_END
        //=============================================================================
        

		CheckWinConditions();
	}


	// Called when game rules are destroyed by CWorld
	void CCSGameRules::LevelShutdown()
	{
		ResetLeavingSafetyMusicState();
		ResetSaferoomTransitionState();
		ResetSpecialInfectedFarCullTimers();
		s_flNextCommonHordeStart = 0.0f;
		s_flCommonHordeEndTime = 0.0f;
		ResetCommonInfectedHordeState();
		s_flNextCommonInfectedSpawn = 0.0f;
		s_flNextTankSpawnAllowed = 0.0f;

		int iLevelIndex = GetCSLevelIndex( STRING( gpGlobals->mapname ) );

		if ( iLevelIndex != -1 )
		{
			g_iTerroristVictories[iLevelIndex] += m_iNumTerroristWins;
			g_iCounterTVictories[iLevelIndex] += m_iNumCTWins;
		}

		BaseClass::LevelShutdown();
	}

	
	//---------------------------------------------------------------------------------------------------
	/**
	 * Check if the scenario has been won/lost.
	 * Return true if the scenario is over, false if the scenario is still in progress
	 */
	bool CCSGameRules::CheckWinConditions( void )
	{
		if ( mp_ignore_round_win_conditions.GetBool() )
		{
			return false;
		}

		// If a winner has already been determined.. then get the heck out of here
		if (m_iRoundWinStatus != WINNER_NONE)
		{
			// still check if we lost players to where we need to do a full reset next round...
			int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
			InitializePlayerCounts( NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT );

			bool bNeededPlayers = false;
			NeededPlayersCheck( bNeededPlayers );

			return true;
		}

		// Initialize the player counts..
		int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
		InitializePlayerCounts( NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT );


		/***************************** OTHER PLAYER's CHECK *********************************************************/
		bool bNeededPlayers = false;
		if ( NeededPlayersCheck( bNeededPlayers ) )
			return false;

		/****************************** ASSASINATION/VIP SCENARIO CHECK *******************************************************/
		if ( VIPRoundEndCheck( bNeededPlayers ) )
			return true;

		/****************************** PRISON ESCAPE CHECK *******************************************************/
		if ( PrisonRoundEndCheck() )
			return true;


		/****************************** BOMB CHECK ********************************************************/
		if ( BombRoundEndCheck( bNeededPlayers ) )
			return true;

		/****************************** CAMPAIGN SURVIVOR WIPE CHECK *************************************/
		if ( IsCampaignSaferoomMap( this ) && AreAllSpawnedSurvivorsDeadOrIncapacitated() )
		{
			AwardCampaignRoundWin( this, TEAM_INFECTED, mp_round_restart_delay.GetFloat(), bNeededPlayers, true );
			return true;
		}


		/***************************** TEAM EXTERMINATION CHECK!! *********************************************************/
		// CounterTerrorists won by virture of elimination
		if ( TeamExterminationCheck( NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT, bNeededPlayers ) )
			return true;

		
		/******************************** HOSTAGE RESCUE CHECK ******************************************************/
		if ( HostageRescueRoundEndCheck( bNeededPlayers ) )
			return true;

		// scenario not won - still in progress
		return false;
	}


	bool CCSGameRules::NeededPlayersCheck( bool &bNeededPlayers )
	{
		return false;
	}


	void CCSGameRules::InitializePlayerCounts(
		int &NumAliveTerrorist,
		int &NumAliveCT,
		int &NumDeadTerrorist,
		int &NumDeadCT
		)
	{
		NumAliveTerrorist = NumAliveCT = NumDeadCT = NumDeadTerrorist = 0;
		m_iNumTerrorist = m_iNumCT = m_iNumSpawnableTerrorist = m_iNumSpawnableCT = 0;
		m_iHaveEscaped = 0;

		// Count how many dead players there are on each team.
		for ( int iTeam=0; iTeam < GetNumberOfTeams(); iTeam++ )
		{
			CTeam *pTeam = GetGlobalTeam( iTeam );

			for ( int iPlayer=0; iPlayer < pTeam->GetNumPlayers(); iPlayer++ )
			{
				CCSPlayer *pPlayer = ToCSPlayer( pTeam->GetPlayer( iPlayer ) );
				Assert( pPlayer );
				if ( !pPlayer )
					continue;

				Assert( pPlayer->GetTeamNumber() == pTeam->GetTeamNumber() );

				switch ( pTeam->GetTeamNumber() )
				{
				case TEAM_CT:
					m_iNumCT++;

					if ( pPlayer->State_Get() != STATE_PICKINGCLASS )
						m_iNumSpawnableCT++;

					if ( pPlayer->m_lifeState != LIFE_ALIVE )
						NumDeadCT++;
					else
						NumAliveCT++;

					break;

				case TEAM_TERRORIST:
					m_iNumTerrorist++;

					if ( pPlayer->State_Get() != STATE_PICKINGCLASS )
						m_iNumSpawnableTerrorist++;

					if ( pPlayer->m_lifeState != LIFE_ALIVE )
						NumDeadTerrorist++;
					else
						NumAliveTerrorist++;

					// Check to see if this guy escaped.
					if ( pPlayer->m_bEscaped == true )
						m_iHaveEscaped++;

					break;
				}
			}
		}
	}

	bool CCSGameRules::HostageRescueRoundEndCheck( bool bNeededPlayers )
	{
		// Check to see if 50% of the hostages have been rescued.
		CHostage* hostage = NULL;

		int iNumHostages = g_Hostages.Count();
		int iNumLeftToRescue = 0;
		int i;

		for ( i=0; i<iNumHostages; i++ )
		{
			hostage = g_Hostages[i];

			if ( hostage->m_iHealth > 0 && !hostage->IsRescued() ) // We've found a live hostage. don't end the round
				iNumLeftToRescue++;
		}

		m_iHostagesRemaining = iNumLeftToRescue;

		if ( (iNumLeftToRescue == 0) && (iNumHostages > 0) )
		{
			if ( m_iHostagesRescued >= (iNumHostages * 0.5)	)
			{
				m_iAccountCT += 2500;

				if ( !bNeededPlayers )
				{
					m_iNumCTWins ++;
					// Update the clients team score
					UpdateTeamScores();
				}
				CCS_GameStats.Event_AllHostagesRescued();
				// tell the bots all the hostages have been rescued
				IGameEvent * event = gameeventmanager->CreateEvent( "hostage_rescued_all" );
				if ( event )
				{
					gameeventmanager->FireEvent( event );
				}

				TerminateRound( mp_round_restart_delay.GetFloat(), All_Hostages_Rescued );
				return true;
			}
		}

		return false;
	}


	bool CCSGameRules::PrisonRoundEndCheck()
	{
		//MIKETODO: get this working when working on prison escape
		/*
		if (m_bMapHasEscapeZone == true)
		{
			float flEscapeRatio;

			flEscapeRatio = (float) m_iHaveEscaped / (float) m_iNumEscapers;

			if (flEscapeRatio >= m_flRequiredEscapeRatio)
			{
				BroadcastSound( "Event.TERWin" );
				m_iAccountTerrorist += 3150;

				if ( !bNeededPlayers )
				{
					m_iNumTerroristWins ++;
					// Update the clients team score
					UpdateTeamScores();
				}
				EndRoundMessage( "#Terrorists_Escaped", Terrorists_Escaped );
				TerminateRound( mp_round_restart_delay.GetFloat(), WINNER_TER );
				return;
			}
			else if ( NumAliveTerrorist == 0 && flEscapeRatio < m_flRequiredEscapeRatio)
			{
				BroadcastSound( "Event.CTWin" );
				m_iAccountCT += (1 - flEscapeRatio) * 3500; // CTs are rewarded based on how many terrorists have escaped...
				
				if ( !bNeededPlayers )
				{
					m_iNumCTWins++;
					// Update the clients team score
					UpdateTeamScores();
				}
				EndRoundMessage( "#CTs_PreventEscape", CTs_PreventEscape );
				TerminateRound( mp_round_restart_delay.GetFloat(), WINNER_CT );
				return;
			}

			else if ( NumAliveTerrorist == 0 && NumDeadTerrorist != 0 && m_iNumSpawnableCT > 0 )
			{
				BroadcastSound( "Event.CTWin" );
				m_iAccountCT += (1 - flEscapeRatio) * 3250; // CTs are rewarded based on how many terrorists have escaped...
				
				if ( !bNeededPlayers )
				{
					m_iNumCTWins++;
					// Update the clients team score
					UpdateTeamScores();
				}
				EndRoundMessage( "#Escaping_Terrorists_Neutralized", Escaping_Terrorists_Neutralized );
				TerminateRound( mp_round_restart_delay.GetFloat(), WINNER_CT );
				return;
			}
			// else return;    
		}
		*/

		return false;
	}


	bool CCSGameRules::VIPRoundEndCheck( bool bNeededPlayers )
	{
		if (m_iMapHasVIPSafetyZone != 1)
			return false;

		if (m_pVIP == NULL)
			return false;

		if (m_pVIP->m_bEscaped == true)
		{
			m_iAccountCT += 3500;

			if ( !bNeededPlayers )
			{
				m_iNumCTWins ++;
				// Update the clients team score
				UpdateTeamScores();
			}

			//MIKETODO: get this working when working on VIP scenarios
			/*
			MessageBegin( MSG_SPEC, SVC_DIRECTOR );
				WRITE_BYTE ( 9 );	// command length in bytes
				WRITE_BYTE ( DRC_CMD_EVENT );	// VIP rescued
				WRITE_SHORT( ENTINDEX(m_pVIP->edict()) );	// index number of primary entity
				WRITE_SHORT( 0 );	// index number of secondary entity
				WRITE_LONG( 15 | DRC_FLAG_FINAL);   // eventflags (priority and flags)
			MessageEnd();
			*/

			// tell the bots the VIP got out
			IGameEvent * event = gameeventmanager->CreateEvent( "vip_escaped" );
			if ( event )
			{
				event->SetInt( "userid", m_pVIP->GetUserID() );
				event->SetInt( "priority", 9 );
				gameeventmanager->FireEvent( event );
			}

			//=============================================================================
			// HPE_BEGIN:
			// [menglish] If the VIP has escaped award him an MVP
			//=============================================================================
			 
			m_pVIP->IncrementNumMVPs( CSMVP_UNDEFINED );
			 
			//=============================================================================
			// HPE_END
			//=============================================================================

			TerminateRound( mp_round_restart_delay.GetFloat(), VIP_Escaped );
			return true;
		}
		else if ( m_pVIP->m_lifeState == LIFE_DEAD )   // The VIP is dead
		{
			m_iAccountTerrorist += 3250;

			if ( !bNeededPlayers )
			{
				m_iNumTerroristWins ++;
				// Update the clients team score
				UpdateTeamScores();
			}

			// tell the bots the VIP was killed
			IGameEvent * event = gameeventmanager->CreateEvent( "vip_killed" );
			if ( event )
			{
				event->SetInt( "userid", m_pVIP->GetUserID() );
				event->SetInt( "priority", 9 );
				gameeventmanager->FireEvent( event );
			}

			TerminateRound( mp_round_restart_delay.GetFloat(), VIP_Assassinated );
			return true;
		}

		return false;
	}


	bool CCSGameRules::BombRoundEndCheck( bool bNeededPlayers )
	{
		// Check to see if the bomb target was hit or the bomb defused.. if so, then let's end the round!
		if ( ( m_bTargetBombed == true ) && ( m_bMapHasBombTarget == true ) )
		{
			m_iAccountTerrorist += 3500;

			if ( !bNeededPlayers )
			{
				m_iNumTerroristWins ++;
				// Update the clients team score
				UpdateTeamScores();
			}

			TerminateRound( mp_round_restart_delay.GetFloat(), Target_Bombed );
			return true;
		}
		else
		if ( ( m_bBombDefused == true ) && ( m_bMapHasBombTarget == true ) )
		{
			m_iAccountCT += 3250;

			m_iAccountTerrorist += 800; // give the T's a little bonus for planting the bomb even though it was defused.

			if ( !bNeededPlayers )
			{
				m_iNumCTWins++;
				// Update the clients team score
				UpdateTeamScores();
			}

			TerminateRound( mp_round_restart_delay.GetFloat(), Bomb_Defused );
			return true;
		}

		return false;
	}


	bool CCSGameRules::TeamExterminationCheck(
		int NumAliveTerrorist,
		int NumAliveCT,
		int NumDeadTerrorist,
		int NumDeadCT,
		bool bNeededPlayers
	)
	{
		if ( ( m_iNumCT > 0 && m_iNumSpawnableCT > 0 ) && ( m_iNumTerrorist > 0 && m_iNumSpawnableTerrorist > 0 ) )
		{
			if ( NumAliveTerrorist == 0 && NumDeadTerrorist != 0 && m_iNumSpawnableCT > 0 )
			{
				bool nowin = false;
					
				for ( int iGrenade=0; iGrenade < g_PlantedC4s.Count(); iGrenade++ )
				{
					CPlantedC4 *pC4 = g_PlantedC4s[iGrenade];

					if ( pC4->IsBombActive() )
						nowin = true;
				}

				if ( !nowin )
				{
					if ( m_bMapHasBombTarget )
						m_iAccountCT += 3250;
					else
						m_iAccountCT += 3000;

					if ( !bNeededPlayers )
					{
						m_iNumCTWins++;
						// Update the clients team score
						UpdateTeamScores();
					}

					return true;
				}
			}
		
			// Terrorists WON
			if ( NumAliveCT == 0 && NumDeadCT != 0 && m_iNumSpawnableTerrorist > 0 )
			{
				if ( m_bMapHasBombTarget )
					m_iAccountTerrorist += 3250;
				else
					m_iAccountTerrorist += 3000;

				if ( !bNeededPlayers )
				{
					m_iNumTerroristWins++;
					// Update the clients team score
					UpdateTeamScores();
				}

				return true;
			}
		}
		else if ( NumAliveCT == 0 && NumAliveTerrorist == 0 )
		{
			return true;
		}

		return false;
	}


	void CCSGameRules::PickNextVIP()
	{
		// MIKETODO: work on this when getting VIP maps running.
		/*
		if (IsVIPQueueEmpty() != true)
		{
			// Remove the current VIP from his VIP status and make him a regular CT.
			if (m_pVIP != NULL)
				ResetCurrentVIP();

			for (int i = 0; i <= 4; i++)
			{
				if (VIPQueue[i] != NULL)
				{
					m_pVIP = VIPQueue[i];
					m_pVIP->MakeVIP();

					VIPQueue[i] = NULL;		// remove this player from the VIP queue
					StackVIPQueue();		// and re-organize the queue
					m_iConsecutiveVIP = 0;
					return;
				}
			}
		}
		else if (m_iConsecutiveVIP >= 3)	// If it's been the same VIP for 3 rounds already.. then randomly pick a new one
		{
			m_iLastPick++;

			if (m_iLastPick > m_iNumCT)
				m_iLastPick = 1;

			int iCount = 1;

			CBaseEntity* pPlayer = NULL;
			CBasePlayer* player = NULL;
			CBasePlayer* pLastPlayer = NULL;

			pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" );
			while (		(pPlayer != NULL) && (!FNullEnt(pPlayer->edict()))	)
			{
				if (	!(pPlayer->pev->flags & FL_DORMANT)	)
				{
					player = GetClassPtr((CBasePlayer *)pPlayer->pev);
					
					if (	(player->m_iTeam == CT) && (iCount == m_iLastPick)	)
					{
						if (	(player == m_pVIP) && (pLastPlayer != NULL)	)
							player = pLastPlayer;

						// Remove the current VIP from his VIP status and make him a regular CT.
						if (m_pVIP != NULL)
							ResetCurrentVIP();

						player->MakeVIP();
						m_iConsecutiveVIP = 0;

						return;
					}
					else if ( player->m_iTeam == CT )
						iCount++;

					if ( player->m_iTeam != SPECTATOR )
						pLastPlayer = player;
				}
				pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" );
			}
		}
		else if (m_pVIP == NULL)  // There is no VIP and there is no one waiting to be the VIP.. therefore just pick the first CT player we can find.
		{
			CBaseEntity* pPlayer = NULL;
			CBasePlayer* player = NULL;

			pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" );
			while (		(pPlayer != NULL) && (!FNullEnt(pPlayer->edict()))	)
			{
				if ( pPlayer->pev->flags != FL_DORMANT	)
				{
					player = GetClassPtr((CBasePlayer *)pPlayer->pev);
		
					if ( player->m_iTeam == CT )
					{
						player->MakeVIP();
						m_iConsecutiveVIP = 0;
						return;
					}
				}
				pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" );
			}
		}
		*/
	}


	void CCSGameRules::ReadMultiplayCvars()
	{
		m_iRoundTime = (int)(mp_roundtime.GetFloat() * 60);
		m_iFreezeTime = 0;
	}


	void CCSGameRules::RestartRound()
	{
		ResetLeavingSafetyMusicState();

#if defined( REPLAY_ENABLED )
		if ( g_pReplay )
		{
			// Write replay and stop recording if appropriate
			if ( g_pReplay->IsRecording() )
			{
				g_pReplay->SV_EndRecordingSession();
			}
			
			int nActivePlayerCount = m_iNumTerrorist + m_iNumCT;
			if ( nActivePlayerCount && g_pReplay->SV_ShouldBeginRecording( false ) )
			{
				// Tell the replay manager that it should begin recording the new round as soon as possible
				g_pReplay->SV_GetContext()->GetSessionRecorder()->StartRecording();
			}
		}
#endif
		//=============================================================================
		// HPE_BEGIN:
		// [tj] Notify players that the round is about to be reset
		//=============================================================================
        for ( int clientIndex = 1; clientIndex <= gpGlobals->maxClients; clientIndex++ )
		{
			CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( clientIndex );
			if(pPlayer)
			{
				pPlayer->OnPreResetRound();
			}
		}

		//=============================================================================
		// HPE_END
		//=============================================================================    

		if ( !IsFinite( gpGlobals->curtime ) )
		{
			Warning( "NaN curtime in RestartRound\n" );
			gpGlobals->curtime = 0.0f;
		}

		int i;

		m_iTotalRoundsPlayed++;
		
		//ClearBodyQue();

		// Hardlock the player accelaration to 5.0
		//CVAR_SET_FLOAT( "sv_accelerate", 5.0 );
		//CVAR_SET_FLOAT( "sv_friction", 4.0 );
		//CVAR_SET_FLOAT( "sv_stopspeed", 75 );

		sv_stopspeed.SetValue( 75.0f );

		// Tabulate the number of players on each team.
		int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
		InitializePlayerCounts( NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT );
		
		m_bBombDropped = false;
		m_bBombPlanted = false;
		
		if ( GetHumanTeam() != TEAM_UNASSIGNED )
		{
			MoveHumansToHumanTeam();
		}

		/*************** AUTO-BALANCE CODE *************/
		if ( mp_autoteambalance.GetInt() != 0 &&
			(m_iUnBalancedRounds >= 1) )
		{
			if ( GetHumanTeam() == TEAM_UNASSIGNED )
			{
				BalanceTeams();
			}
		}

		if ( ((m_iNumSpawnableCT - m_iNumSpawnableTerrorist) >= 2) ||
			((m_iNumSpawnableTerrorist - m_iNumSpawnableCT) >= 2)	)
		{
			m_iUnBalancedRounds++;
		}
		else
		{
			m_iUnBalancedRounds = 0;
		}

		// Warn the players of an impending auto-balance next round...
		if ( mp_autoteambalance.GetInt() != 0 &&
			(m_iUnBalancedRounds == 1)	)
		{
			if ( GetHumanTeam() == TEAM_UNASSIGNED )
			{
				UTIL_ClientPrintAll( HUD_PRINTCENTER,"#Auto_Team_Balance_Next_Round");
			}
		}

		/*************** AUTO-BALANCE CODE *************/

		if ( m_bCompleteReset )
		{
			// bounds check
			if ( mp_timelimit.GetInt() < 0 )
			{
				mp_timelimit.SetValue( 0 );
			}
			m_flGameStartTime = gpGlobals->curtime;
			if ( !IsFinite( m_flGameStartTime.Get() ) )
			{
				Warning( "Trying to set a NaN game start time\n" );
				m_flGameStartTime.GetForModify() = 0.0f;
			}

			// Reset total # of rounds played
			m_iTotalRoundsPlayed = 0;

			// Reset score info
			m_iNumTerroristWins				= 0;
			m_iNumCTWins					= 0;
			m_iNumConsecutiveTerroristLoses	= 0;
			m_iNumConsecutiveCTLoses		= 0;


			// Reset team scores
			UpdateTeamScores();


			// Reset the player stats
			for ( i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CCSPlayer *pPlayer = CCSPlayer::Instance( i );

				if ( pPlayer && !FNullEnt( pPlayer->edict() ) )
					pPlayer->Reset();
			}
		}

		m_bFreezePeriod = true;

		ReadMultiplayCvars();

		// Check to see if there's a mapping info paramater entity
		if ( g_pMapInfo )
		{
			switch ( g_pMapInfo->m_iBuyingStatus )
			{
				case 0: 
					m_bCTCantBuy = false; 
					m_bTCantBuy = false; 
					Msg( "EVERYONE CAN BUY!\n" );
					break;
				
				case 1: 
					m_bCTCantBuy = false; 
					m_bTCantBuy = true; 
					Msg( "Only CT's can buy!!\n" );
					break;

				case 2: 
					m_bCTCantBuy = true; 
					m_bTCantBuy = false; 
					Msg( "Only T's can buy!!\n" );
					break;
				
				case 3: 
					m_bCTCantBuy = true; 
					m_bTCantBuy = true; 
					Msg( "No one can buy!!\n" );
					break;

				default: 
					m_bCTCantBuy = false; 
					m_bTCantBuy = false; 
					break;
			}
		}
		else
		{
			// by default everyone can buy
			m_bCTCantBuy = false; 
			m_bTCantBuy = false; 
		}
		
		
		// Check to see if this map has a bomb target in it

		if ( gEntList.FindEntityByClassname( NULL, "func_bomb_target" ) )
		{
			m_bMapHasBombTarget		= true;
			m_bMapHasBombZone		= true;
		}
		else if ( gEntList.FindEntityByClassname( NULL, "info_bomb_target" ) )
		{
			m_bMapHasBombTarget		= true;
			m_bMapHasBombZone		= false;
		}
		else
		{
			m_bMapHasBombTarget		= false;
			m_bMapHasBombZone		= false;
		}

		// Check to see if this map has hostage rescue zones

		if ( gEntList.FindEntityByClassname( NULL, "func_hostage_rescue" ) )
			m_bMapHasRescueZone = true;
		else
			m_bMapHasRescueZone = false;


		// See if the map has func_buyzone entities
		// Used by CBasePlayer::HandleSignals() to support maps without these entities
		
		if ( gEntList.FindEntityByClassname( NULL, "func_buyzone" ) )
			m_bMapHasBuyZone = true;
		else
			m_bMapHasBuyZone = false;


		// GOOSEMAN : See if this map has func_escapezone entities
		if ( gEntList.FindEntityByClassname( NULL, "func_escapezone" ) )
		{
			m_bMapHasEscapeZone = true;
			m_iHaveEscaped = 0;
			m_iNumEscapers = 0; // Will increase this later when we count how many Ts are starting
			if (m_iNumEscapeRounds >= 3)
			{
				SwapAllPlayers();
				m_iNumEscapeRounds = 0;
			}

			m_iNumEscapeRounds++;  // Increment the number of rounds played... After 8 rounds, the players will do a whole sale switch..
		}
		else
			m_bMapHasEscapeZone = false;

		// Check to see if this map has VIP safety zones
		if ( gEntList.FindEntityByClassname( NULL, "func_vip_safetyzone" ) )
		{
			PickNextVIP();
			m_iConsecutiveVIP++;
			m_iMapHasVIPSafetyZone = 1;
		}
		else
			m_iMapHasVIPSafetyZone = 2;

		// Update accounts based on number of hostages remaining.. 
		int iRescuedHostageBonus = 0;

		for ( int iHostage=0; iHostage < g_Hostages.Count(); iHostage++ )
		{
			CHostage *pHostage = g_Hostages[iHostage];

			if( pHostage->IsRescuable() )	//Alive and not rescued
			{
				iRescuedHostageBonus += 150;
			}
			
			if ( iRescuedHostageBonus >= 2000 )
				break;
		}

		//*******Catch up code by SupraFiend. Scale up the loser bonus when teams fall into losing streaks
		if (m_iRoundWinStatus == WINNER_TER) // terrorists won
		{
			//check to see if they just broke a losing streak
			if(m_iNumConsecutiveTerroristLoses > 1)
				m_iLoserBonus = 1500;//this is the default losing bonus

			m_iNumConsecutiveTerroristLoses = 0;//starting fresh
			m_iNumConsecutiveCTLoses++;//increment the number of wins the CTs have had
		}
		else if (m_iRoundWinStatus == WINNER_CT) // CT Won
		{
			//check to see if they just broke a losing streak
			if(m_iNumConsecutiveCTLoses > 1)
				m_iLoserBonus = 1500;//this is the default losing bonus

			m_iNumConsecutiveCTLoses = 0;//starting fresh
			m_iNumConsecutiveTerroristLoses++;//increment the number of wins the Terrorists have had
		}

		//check if the losing team is in a losing streak & that the loser bonus hasen't maxed out.
		if((m_iNumConsecutiveTerroristLoses > 1) && (m_iLoserBonus < 3000))
			m_iLoserBonus += 500;//help out the team in the losing streak
		else
		if((m_iNumConsecutiveCTLoses > 1) && (m_iLoserBonus < 3000))
			m_iLoserBonus += 500;//help out the team in the losing streak

		// assign the wining and losing bonuses
		if (m_iRoundWinStatus == WINNER_TER) // terrorists won
		{
			m_iAccountTerrorist += iRescuedHostageBonus;
			m_iAccountCT += m_iLoserBonus;
		}
		else if (m_iRoundWinStatus == WINNER_CT) // CT Won
		{
			m_iAccountCT += iRescuedHostageBonus;
			if (m_bMapHasEscapeZone == false)	// only give them the bonus if this isn't an escape map
				m_iAccountTerrorist += m_iLoserBonus;
		}
		

		//Update CT account based on number of hostages rescued
		m_iAccountCT += m_iHostagesRescued * 750;


		// Update individual players accounts and respawn players

		//**********new code by SupraFiend
		//##########code changed by MartinO 
		//the round time stamp must be set before players are spawned
		m_fRoundStartTime = gpGlobals->curtime + m_iFreezeTime;

		if ( !IsFinite( m_fRoundStartTime.Get() ) )
		{
			Warning( "Trying to set a NaN round start time\n" );
			m_fRoundStartTime.GetForModify() = 0.0f;
		}
		
		//Adrian - No cash for anyone at first rounds! ( well, only the default. )
		if ( m_bCompleteReset )
		{
			m_iAccountTerrorist = m_iAccountCT = 0; //No extra cash!.

			//We are starting fresh. So it's like no one has ever won or lost.
			m_iNumTerroristWins				= 0; 
			m_iNumCTWins					= 0;
			m_iNumConsecutiveTerroristLoses	= 0;
			m_iNumConsecutiveCTLoses		= 0;
			m_iLoserBonus					= 1400;
		}

		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( i );

			if ( !pPlayer )
				continue;

			pPlayer->m_iNumSpawns	= 0;
			pPlayer->m_bTeamChanged	= false;
				
			if ( pPlayer->GetTeamNumber() == TEAM_CT )
			{
				if (pPlayer->DoesPlayerGetRoundStartMoney())
				{
					pPlayer->AddAccount( m_iAccountCT );
				}
			}
			else if ( pPlayer->GetTeamNumber() == TEAM_TERRORIST )
			{
				m_iNumEscapers++;	// Add another potential escaper to the mix!
				if (pPlayer->DoesPlayerGetRoundStartMoney())
				{
					pPlayer->AddAccount( m_iAccountTerrorist );
				}
			}

			// tricky, make players non solid while moving to their spawn points
			if ( (pPlayer->GetTeamNumber() == TEAM_CT) || (pPlayer->GetTeamNumber() == TEAM_TERRORIST) )
			{
				pPlayer->AddSolidFlags( FSOLID_NOT_SOLID );
			}
		}
        
        //=============================================================================
        // HPE_BEGIN:
        // [tj] Keep track of number of players per side and if they have the same uniform
        //=============================================================================
 
        int terroristUniform = -1;
        bool allTerroristsWearingSameUniform = true;
        int numberOfTerrorists = 0;
        int ctUniform = -1;
        bool allCtsWearingSameUniform = true;
        int numberOfCts = 0;
 
        //=============================================================================
        // HPE_END
        //=============================================================================

		// know respawn all players
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( i );

			if ( !pPlayer )
				continue;

			if ( pPlayer->GetTeamNumber() == TEAM_CT )
			{
                //=============================================================================
                // HPE_BEGIN:
                // [tj] Increment CT count and check CT uniforms.
                //=============================================================================
                
                numberOfCts++;
                if (ctUniform == -1)
                {
                    ctUniform = pPlayer->PlayerClass();
                }
                else if (pPlayer->PlayerClass() != ctUniform)
                {
                    allCtsWearingSameUniform = false;
                }
                 
                //=============================================================================
                // HPE_END
                //=============================================================================
                
				pPlayer->RoundRespawn();
			}

			if ( pPlayer->GetTeamNumber() == TEAM_TERRORIST )
			{
                //=============================================================================
                // HPE_BEGIN:
                // [tj] Increment terrorist count and check terrorist uniforms
                //=============================================================================
                 
                numberOfTerrorists++;
                if (terroristUniform == -1)
                {
                    terroristUniform = pPlayer->PlayerClass();
                }
                else if (pPlayer->PlayerClass() != terroristUniform)
                {
                    allTerroristsWearingSameUniform = false;
                }
                 
                //=============================================================================
                // HPE_END
                //=============================================================================
                
				pPlayer->RoundRespawn();
			}
			else
			{
				pPlayer->ObserverRoundRespawn();
			}

			if ( pPlayer->m_iAccount > pPlayer->m_iShouldHaveCash )
			{
				m_bDontUploadStats = true;
			}
		}

        //=============================================================================
        // HPE_BEGIN:
        //=============================================================================

        // [tj] Award same uniform achievement for qualifying teams
        for ( i = 1; i <= gpGlobals->maxClients; i++ )
        {
            CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( i );

            if ( !pPlayer )
                continue;

            if ( pPlayer->GetTeamNumber() == TEAM_CT && allCtsWearingSameUniform && numberOfCts >= AchievementConsts::SameUniform_MinPlayers)
            {
                pPlayer->AwardAchievement(CSSameUniform);
            }

            if ( pPlayer->GetTeamNumber() == TEAM_TERRORIST && allTerroristsWearingSameUniform && numberOfTerrorists >= AchievementConsts::SameUniform_MinPlayers)
            {
                pPlayer->AwardAchievement(CSSameUniform);
            }
        }

		// [menglish] reset per-round achievement variables for each player
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( i );
			if( pPlayer )
			{
				pPlayer->ResetRoundBasedAchievementVariables();
			}
		}

		// [pfreese] Reset all round or match stats, depending on type of restart
		if ( m_bCompleteReset )
		{
			CCS_GameStats.ResetAllStats();
			CCS_GameStats.ResetPlayerClassMatchStats();
		}
		else
		{
			CCS_GameStats.ResetRoundStats();
		}

		//=============================================================================
		// HPE_END
		//=============================================================================

		// Respawn entities (glass, doors, etc..)
		CleanUpMap();

		// now run a tkpunish check, after the map has been cleaned up
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( i );

			if ( !pPlayer )
				continue;

			if ( pPlayer->GetTeamNumber() == TEAM_CT && pPlayer->PlayerClass() >= FIRST_CT_CLASS && pPlayer->PlayerClass() <= LAST_CT_CLASS )
			{
				pPlayer->CheckTKPunishment();
			}
			if ( pPlayer->GetTeamNumber() == TEAM_TERRORIST && pPlayer->PlayerClass() >= FIRST_T_CLASS && pPlayer->PlayerClass() <= LAST_T_CLASS )
			{
				pPlayer->CheckTKPunishment();
			}
		}

		// Give C4 to the terrorists
		if (m_bMapHasBombTarget == true	)
			GiveC4();

		// Reset game variables
		m_flIntermissionEndTime = 0;
		m_flRestartRoundTime = 0.0;
		s_flNextSpecialInfectedSpawn = gpGlobals->curtime + z_special_spawn_interval.GetFloat();
		m_iAccountTerrorist = m_iAccountCT = 0;
		m_iHostagesRescued = 0;
		m_iHostagesTouched = 0;

        //=============================================================================
        // HPE_BEGIN
        // [dwenger] Reset rescue-related achievement values
        //=============================================================================

		// [tj] reset flawless and lossless round related flags
		m_bNoTerroristsKilled = true;
		m_bNoCTsKilled = true;
		m_bNoTerroristsDamaged = true;
		m_bNoCTsDamaged = true;
		m_pFirstKill = NULL;
		m_pFirstBlood = NULL;

        m_bCanDonateWeapons = true;

		// [dwenger] Reset rescue-related achievement values
        m_iHostagesRemaining = 0;
        m_pLastRescuer = NULL;

		m_hostageWasInjured = false;
		m_hostageWasKilled = false;

        //=============================================================================
        // HPE_END
        //=============================================================================

        m_iNumRescuers = 0;
		m_iRoundWinStatus = WINNER_NONE;
		m_bTargetBombed = m_bBombDefused = false;
		m_bCompleteReset = false;
		m_flNextHostageAnnouncement = gpGlobals->curtime;

		m_iHostagesRemaining = g_Hostages.Count();

		// fire global game event
		IGameEvent * event = gameeventmanager->CreateEvent( "round_start" );
		if ( event )
		{
			event->SetInt("timelimit", m_iRoundTime );
			event->SetInt("fraglimit", 0 );
			event->SetInt( "priority", 6 ); // HLTV event priority, not transmitted
		
			if ( m_bMapHasRescueZone )
			{
				event->SetString("objective","HOSTAGE RESCUE");
			}
			else if ( m_bMapHasEscapeZone )
			{
				event->SetString("objective","PRISON ESCAPE");
			}
			else if ( m_iMapHasVIPSafetyZone == 1 )
			{
				event->SetString("objective","VIP RESCUE");
			}
			else if ( m_bMapHasBombTarget || m_bMapHasBombZone )
			{
				event->SetString("objective","BOMB TARGET");
			}
			else
			{
				event->SetString("objective","DEATHMATCH");
			}

			gameeventmanager->FireEvent( event );
		}
	
		UploadGameStats();

		//=============================================================================
		// HPE_BEGIN:
		// [pfreese] I commented out this call to CreateWeaponManager, as the 
		// CGameWeaponManager object doesn't appear to be actually used by the CSS
		// code, and in any case, the weapon manager does not support wildcards in 
		// entity names (as seemingly indicated) below. When the manager fails to 
		// create its factory, it removes itself in any case.
		//=============================================================================

		// CreateWeaponManager( "weapon_*", gpGlobals->maxClients * 2 );
		
		//=============================================================================
		// HPE_END
		//=============================================================================


		if (g_pDirector)
			g_pDirector->m_OnGameplayStart.FireOutput(NULL, g_pDirector);
	}

	void CCSGameRules::GiveC4()
	{
		enum {
			ALL_TERRORISTS = 0,
			HUMAN_TERRORISTS,
		};
		int iTerrorists[2][ABSOLUTE_PLAYER_LIMIT];
		int numAliveTs[2] = { 0, 0 };
		int lastBombGuyIndex[2] = { -1, -1 };

		//Create an array of the indeces of bomb carrier candidates
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( i ) );

			if( pPlayer && pPlayer->IsAlive() && pPlayer->GetTeamNumber() == TEAM_TERRORIST && numAliveTs[ALL_TERRORISTS] < ABSOLUTE_PLAYER_LIMIT  )
			{
				if ( pPlayer == m_pLastBombGuy )
				{
					lastBombGuyIndex[ALL_TERRORISTS] = numAliveTs[ALL_TERRORISTS];
					lastBombGuyIndex[HUMAN_TERRORISTS] = numAliveTs[HUMAN_TERRORISTS];
				}

				iTerrorists[ALL_TERRORISTS][numAliveTs[ALL_TERRORISTS]] = i;
				numAliveTs[ALL_TERRORISTS]++;
				if ( !pPlayer->IsBot() )
				{
					iTerrorists[HUMAN_TERRORISTS][numAliveTs[HUMAN_TERRORISTS]] = i;
					numAliveTs[HUMAN_TERRORISTS]++;
				}
			}
		}

		int which = cv_bot_defer_to_human.GetBool();
		if ( numAliveTs[HUMAN_TERRORISTS] == 0 )
		{
			which = ALL_TERRORISTS;
		}
		/*
		//pick one of the candidates randomly
		if( numAliveTs[which] > 0 )
		{
			int index = random->RandomInt(0,numAliveTs[which]-1);
			if ( lastBombGuyIndex[which] >= 0 )
			{
				// give the C4 sequentially
				index = (lastBombGuyIndex[which] + 1) % numAliveTs[which];
			}
			CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex( iTerrorists[which][index] ) );

			Assert( pPlayer && pPlayer->GetTeamNumber() == TEAM_TERRORIST && pPlayer->IsAlive() );

			pPlayer->GiveNamedItem( WEAPON_C4_CLASSNAME );
			m_pLastBombGuy = pPlayer;

			//pPlayer->SetBombIcon();
			//pPlayer->pev->body = 1;
			
			pPlayer->m_iDisplayHistoryBits |= DHF_BOMB_RETRIEVED;
			pPlayer->HintMessage( "#Hint_you_have_the_bomb", false, true );

			// Log this information
			//UTIL_LogPrintf("\"%s<%i><%s><TERRORIST>\" triggered \"Spawned_With_The_Bomb\"\n", 
			//	STRING( pPlayer->GetPlayerName() ),
			//	GETPLAYERUSERID( pPlayer->edict() ),
			//	GETPLAYERAUTHID( pPlayer->edict() ) );
		}*/

		m_bBombDropped = false;
	}

	void CCSGameRules::Think()
	{
		CGameRules::Think();

		UpdateLeavingSafetyMusic_NoMercy();

		PruneExpiredItSurvivors();

		for ( int i = 0; i < GetNumberOfTeams(); i++ )
		{
			GetGlobalTeam( i )->Think();
		}

		///// Check game rules /////
		if ( CheckGameOver() )
		{
			return;
		}

		// have we hit the max rounds?
		if ( CheckMaxRounds() )
		{
			return;
		}

		// did somebaody hit the fraglimit ?
		if ( CheckFragLimit() )
		{
			return;
		}

		if ( CheckWinLimit() )
		{
			return;
		}

		
		// Check for the end of the round.
		if ( IsFreezePeriod() )
		{
			CheckFreezePeriodExpired();
		}
		else 
		{
			CheckRoundTimeExpired();
		}

		CheckLevelInitialized();
		
		if ( m_flRestartRoundTime > 0.0f && m_flRestartRoundTime <= gpGlobals->curtime )
		{
			bool botSpeaking = false;
			for ( int i=1; i <= gpGlobals->maxClients; ++i )
			{
				CBasePlayer *player = UTIL_PlayerByIndex( i );
				if (player == NULL)
					continue;

				if (!player->IsBot())
					continue;
				
				CCSBot *bot = dynamic_cast< CCSBot * >(player);
				if ( !bot )
					continue;

				if ( bot->IsUsingVoice() )
				{
					if ( gpGlobals->curtime > m_flRestartRoundTime + 10.0f )
					{
						Msg( "Ignoring speaking bot %s at round end\n", bot->GetPlayerName() );
					}
					else
					{
						botSpeaking = true;
						break;
					}
				}
			}

			if ( !botSpeaking )
			{
				RestartRound();
			}
		}
		
		if ( gpGlobals->curtime > m_tmNextPeriodicThink )
		{
			CheckRestartRound();
			m_tmNextPeriodicThink = gpGlobals->curtime + 1.0;
		}

		const bool isRestartingRound = ( m_flRestartRoundTime > 0.0f );
		ListenServerHostAutoJoinThink( this );
		SurvivorSquadThink( this );
		CommonInfectedDirectorThink( this, isRestartingRound );
		SpecialInfectedDirectorThink( this, isRestartingRound );
		TankHumanTakeoverThink( this, isRestartingRound );
		BackgroundInfectedPopulateThink( this, isRestartingRound );
		SaferoomTransitionThink( this, isRestartingRound );
	}


	// The bots do their processing after physics simulation etc so their visibility checks don't recompute
	// bone positions multiple times a frame.
	void CCSGameRules::EndGameFrame( void )
	{
		TheBots->StartFrame();

		BaseClass::EndGameFrame();
	}

	bool CCSGameRules::CheckGameOver()
	{
		if ( g_fGameOver )   // someone else quit the game already
		{
			//=============================================================================
			// HPE_BEGIN:
			// [Forrest] Calling ChangeLevel multiple times was causing IncrementMapCycleIndex
			// to skip over maps in the list.  Avoid this using a technique from CTeamplayRoundBasedRules::Think.
			//=============================================================================
			// check to see if we should change levels now
			if ( m_flIntermissionEndTime && ( m_flIntermissionEndTime < gpGlobals->curtime ) )
			{
				ChangeLevel(); // intermission is over

				// Don't run this code again
				m_flIntermissionEndTime = 0.f;
			}
			//=============================================================================
			// HPE_END
			//=============================================================================

			return true;
		}

		return false;
	}

	bool CCSGameRules::CheckFragLimit()
	{
		if ( fraglimit.GetInt() <= 0 )
			return false;

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );

			if ( pPlayer && pPlayer->FragCount() >= fraglimit.GetInt() )
			{
				const char *teamName = "UNKNOWN";
				if ( pPlayer->GetTeam() )
				{
					teamName = pPlayer->GetTeam()->GetName();
				}
				UTIL_LogPrintf("\"%s<%i><%s><%s>\" triggered \"Intermission_Kill_Limit\"\n", 
					pPlayer->GetPlayerName(),
					pPlayer->GetUserID(),
					pPlayer->GetNetworkIDString(),
					teamName
					);
				GoToIntermission();
				return true;
			}
		}

		return false;
	}

	bool CCSGameRules::CheckMaxRounds()
	{
		if ( mp_maxrounds.GetInt() != 0 )
		{
			if ( m_iTotalRoundsPlayed >= mp_maxrounds.GetInt() )
			{
				UTIL_LogPrintf("World triggered \"Intermission_Round_Limit\"\n");
				GoToIntermission();
				return true;
			}
		}
		
		return false;
	}


	bool CCSGameRules::CheckWinLimit()
	{
		// has one team won the specified number of rounds?
		if ( mp_winlimit.GetInt() != 0 )
		{
			if ( m_iNumCTWins >= mp_winlimit.GetInt() )
			{
				UTIL_LogPrintf("Team \"CT\" triggered \"Intermission_Win_Limit\"\n");
				GoToIntermission();
				return true;
			}
			if ( m_iNumTerroristWins >= mp_winlimit.GetInt() )
			{
				UTIL_LogPrintf("Team \"TERRORIST\" triggered \"Intermission_Win_Limit\"\n");
				GoToIntermission();
				return true;
			}
		}

		return false;
	}


	void CCSGameRules::CheckFreezePeriodExpired()
	{
		float startTime = m_fRoundStartTime;
		if ( !IsFinite( startTime ) )
		{
			Warning( "Infinite round start time!\n" );
			m_fRoundStartTime.GetForModify() = gpGlobals->curtime;
		}

		if ( IsFinite( startTime ) && gpGlobals->curtime < startTime )
		{
			return; // not time yet to start round
		}

		// Log this information
		UTIL_LogPrintf("World triggered \"Round_Start\"\n");

		char CT_sentence[40];
		char T_sentence[40];
		
		switch ( random->RandomInt( 0, 3 ) )
		{
		case 0:
			Q_strncpy(CT_sentence,"radio.moveout", sizeof( CT_sentence ) ); 
			Q_strncpy(T_sentence ,"radio.moveout", sizeof( T_sentence ) ); 
			break;

		case 1:
			Q_strncpy(CT_sentence, "radio.letsgo", sizeof( CT_sentence ) ); 
			Q_strncpy(T_sentence , "radio.letsgo", sizeof( T_sentence ) ); 
			break;

		case 2:
			Q_strncpy(CT_sentence , "radio.locknload", sizeof( CT_sentence ) );
			Q_strncpy(T_sentence , "radio.locknload", sizeof( T_sentence ) );
			break;

		default:
			Q_strncpy(CT_sentence , "radio.go", sizeof( CT_sentence ) );
			Q_strncpy(T_sentence , "radio.go", sizeof( T_sentence ) );
			break;
		}

		// More specific radio commands for the new scenarios : Prison & Assasination
		if (m_bMapHasEscapeZone == TRUE)
		{
			Q_strncpy(CT_sentence , "radio.elim", sizeof( CT_sentence ) );
			Q_strncpy(T_sentence , "radio.getout", sizeof( T_sentence ) );
		}
		else if (m_iMapHasVIPSafetyZone == 1)
		{
			Q_strncpy(CT_sentence , "radio.vip", sizeof( CT_sentence ) );
			Q_strncpy(T_sentence , "radio.locknload", sizeof( T_sentence ) );
		}

		// Freeze period expired: kill the flag
		m_bFreezePeriod = false;
		TriggerLogicRelayByName( "relay_intro_start" );

		IGameEvent * event = gameeventmanager->CreateEvent( "round_freeze_end" );
		if ( event )
		{
			gameeventmanager->FireEvent( event );
		}

		// Update the timers for all clients and play a sound
		bool bCTPlayed = false;
		bool bTPlayed = false;

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = CCSPlayer::Instance( i );
			if ( pPlayer && !FNullEnt( pPlayer->edict() ) )
			{
				if ( pPlayer->State_Get() == STATE_ACTIVE )
				{
					if ( (pPlayer->GetTeamNumber() == TEAM_CT) && !bCTPlayed )
					{
						pPlayer->Radio( CT_sentence );
						bCTPlayed = true;
					}
					else if ( (pPlayer->GetTeamNumber() == TEAM_TERRORIST) && !bTPlayed )
					{
						pPlayer->Radio( T_sentence );
						bTPlayed = true;
					}

				}
				
				//pPlayer->SyncRoundTimer();
			}
		}
	}


	void CCSGameRules::CheckRoundTimeExpired()
	{
		if ( mp_ignore_round_win_conditions.GetBool() )
			return;

		if ( GetRoundRemainingTime() > 0 || m_iRoundWinStatus != WINNER_NONE ) 
			return; //We haven't completed other objectives, so go for this!.

		if( !m_bFirstConnected )
			return;

		// New code to get rid of round draws!!

		if ( m_bMapHasBombTarget )
		{
			//If the bomb is planted, don't let the round timer end the round.
			//keep going until the bomb explodes or is defused
			if( !m_bBombPlanted )
			{
				m_iAccountCT += 3250;
				
				m_iNumCTWins++;
				TerminateRound( mp_round_restart_delay.GetFloat(), Target_Saved );
				UpdateTeamScores();
				MarkLivingPlayersOnTeamAsNotReceivingMoneyNextRound(TEAM_TERRORIST);
			}
		}
		else if ( m_bMapHasRescueZone )
		{
			m_iAccountTerrorist += 3250; 
			
			m_iNumTerroristWins++;
			TerminateRound( mp_round_restart_delay.GetFloat(), Hostages_Not_Rescued );
			UpdateTeamScores();
			MarkLivingPlayersOnTeamAsNotReceivingMoneyNextRound(TEAM_CT);
		}
		else if ( m_bMapHasEscapeZone )
		{
			m_iNumCTWins++;
			TerminateRound( mp_round_restart_delay.GetFloat(), Terrorists_Not_Escaped );
			UpdateTeamScores();
		}
		else if ( m_iMapHasVIPSafetyZone == 1 )
		{
			m_iAccountTerrorist += 3250;
			m_iNumTerroristWins++;

			TerminateRound( mp_round_restart_delay.GetFloat(), VIP_Not_Escaped );
			UpdateTeamScores();
		}

#if defined( REPLAY_ENABLED )
		if ( g_pReplay )
		{
			// Write replay and stop recording if appropriate
			g_pReplay->SV_EndRecordingSession();
		}
#endif
	}

	void CCSGameRules::GoToIntermission( void )
	{
		Msg( "Going to intermission...\n" );

		IGameEvent *winEvent = gameeventmanager->CreateEvent( "cs_win_panel_match" );

		if( winEvent )
		{
			for ( int teamIndex = TEAM_TERRORIST; teamIndex <= TEAM_CT; teamIndex++ )
			{
				CTeam *team = GetGlobalTeam( teamIndex );
				if ( team )
				{
					float kills = CCS_GameStats.GetTeamStats(teamIndex)[CSSTAT_KILLS];
					float deaths = CCS_GameStats.GetTeamStats(teamIndex)[CSSTAT_DEATHS];
					// choose dialog variables to set depending on team
					switch ( teamIndex )
					{
					case TEAM_TERRORIST:
						winEvent->SetInt( "t_score", team->GetScore() );
						if(deaths == 0)
						{
							winEvent->SetFloat( "t_kd", kills );
						}
						else
						{
							winEvent->SetFloat( "t_kd", kills / deaths );
						}										
						winEvent->SetInt( "t_objectives_done", CCS_GameStats.GetTeamStats(teamIndex)[CSSTAT_OBJECTIVES_COMPLETED] );
						winEvent->SetInt( "t_money_earned", CCS_GameStats.GetTeamStats(teamIndex)[CSSTAT_MONEY_EARNED] );
						break;
					case TEAM_CT:
						winEvent->SetInt( "ct_score", team->GetScore() );
						if(deaths == 0)
						{
							winEvent->SetFloat( "ct_kd", kills );
						}
						else
						{
							winEvent->SetFloat( "ct_kd", kills / deaths );
						}
						winEvent->SetInt( "ct_objectives_done", CCS_GameStats.GetTeamStats(teamIndex)[CSSTAT_OBJECTIVES_COMPLETED] );
						winEvent->SetInt( "ct_money_earned", CCS_GameStats.GetTeamStats(teamIndex)[CSSTAT_MONEY_EARNED] );
						break;
					default:
						Assert( false );
						break;
					}
				}
			}

			gameeventmanager->FireEvent( winEvent );
		}

		BaseClass::GoToIntermission();

		// set all players to FL_FROZEN
		for ( int i = 1; i <= MAX_PLAYERS; i++ )
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );

			if ( pPlayer )
			{
				pPlayer->AddFlag( FL_FROZEN );
			}
		}

		// freeze players while in intermission
		m_bFreezePeriod = true;
	}

	int PlayerScoreInfoSort( const playerscore_t *p1, const playerscore_t *p2 )
	{
		// check frags
		if ( p1->iScore > p2->iScore )
			return -1;
		if ( p2->iScore > p1->iScore )
			return 1;

		// check index
		if ( p1->iPlayerIndex < p2->iPlayerIndex )
			return -1;

		return 1;
	}

#if defined (_DEBUG)
	void TestRoundWinpanel( void )
	{
		IGameEvent *event = gameeventmanager->CreateEvent( "round_end" );
		event->SetInt( "winner", TEAM_TERRORIST );

		if ( event )
		{
			gameeventmanager->FireEvent( event );
		}


		IGameEvent *event2 = gameeventmanager->CreateEvent( "player_death" );
		if ( event2 )
		{
			CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex(1) );
			
			// pCappingPlayers is a null terminated list of player indeces
			event2->SetInt("userid", pPlayer->GetUserID() );
			event2->SetInt("attacker", pPlayer->GetUserID() );
			event2->SetString("weapon", "Bare Hands" );
			event2->SetInt("headshot", 1 );
			event2->SetInt( "revenge", 1 );

			gameeventmanager->FireEvent( event2 );
		}

		IGameEvent *winEvent = gameeventmanager->CreateEvent( "cs_win_panel_round" );

		if ( winEvent )
		{
			if ( 1 )
			{
				if ( 0 /*team == m_iTimerWinTeam */)
				{
					// timer expired, defenders win
					// show total time that was defended
					winEvent->SetBool( "show_timer_defend", true );
					winEvent->SetInt( "timer_time", 0 /*m_pRoundTimer->GetTimerMaxLength() */);
				}
				else
				{
					// attackers win
					// show time it took for them to win
					winEvent->SetBool( "show_timer_attack", true );

					int iTimeElapsed = 90; //m_pRoundTimer->GetTimerMaxLength() - (int)m_pRoundTimer->GetTimeRemaining();
					winEvent->SetInt( "timer_time", iTimeElapsed );
				}
			}
			else
			{
				winEvent->SetBool( "show_timer_attack", false );
				winEvent->SetBool( "show_timer_defend", false );
			}

			int iLastEvent = Terrorists_Win;

			winEvent->SetInt( "final_event", iLastEvent );

			// Set the fun fact data in the event
			winEvent->SetString( "funfact_token", "#funfact_first_blood" );
			winEvent->SetInt( "funfact_player", 1 );
			winEvent->SetInt( "funfact_data1", 20 );
			winEvent->SetInt( "funfact_data2", 31 );
			winEvent->SetInt( "funfact_data3", 45 );

			gameeventmanager->FireEvent( winEvent );
		}
	}
	ConCommand test_round_winpanel( "test_round_winpanel", TestRoundWinpanel, "", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT );

	void TestMatchWinpanel( void )
	{
		IGameEvent *event = gameeventmanager->CreateEvent( "round_end" );
		event->SetInt( "winner", TEAM_TERRORIST );

		if ( event )
		{
			gameeventmanager->FireEvent( event );
		}

		IGameEvent *winEvent = gameeventmanager->CreateEvent( "cs_win_panel_match" );

		if ( winEvent )
		{
			winEvent->SetInt( "t_score", 4 );
			winEvent->SetInt( "ct_score", 1 );

			winEvent->SetFloat( "t_kd", 1.8f );
			winEvent->SetFloat( "ct_kd", 0.4f );

			winEvent->SetInt( "t_objectives_done", 5 );
			winEvent->SetInt( "ct_objectives_done", 2 );

			winEvent->SetInt( "t_money_earned", 30000 );
			winEvent->SetInt( "ct_money_earned", 19999 );

			gameeventmanager->FireEvent( winEvent );
		}
	}
	ConCommand test_match_winpanel( "test_match_winpanel", TestMatchWinpanel, "", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT );

	void TestFreezePanel( void )
	{
		IGameEvent *event = gameeventmanager->CreateEvent( "freezecam_started" );
		if ( event )
		{
			gameeventmanager->FireEvent( event );
		}

		IGameEvent *winEvent = gameeventmanager->CreateEvent( "show_freezepanel" );

		if ( winEvent )
		{
			winEvent->SetInt( "killer", 1 );
			gameeventmanager->FireEvent( winEvent );
		}
	}
	ConCommand test_freezepanel( "test_freezepanel", TestFreezePanel, "", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT );
#endif // _DEBUG

	static void PrintToConsole( CBasePlayer *player, const char *text )
	{
		if ( player )
		{
			ClientPrint( player, HUD_PRINTCONSOLE, text );
		}
		else
		{
			Msg( "%s", text );
		}
	}

	void CCSGameRules::DumpTimers( void ) const
	{
		extern ConVar bot_join_delay;
		CBasePlayer *player = UTIL_GetCommandClient();
		CFmtStr str;

		PrintToConsole( player, str.sprintf( "Timers and related info at %f:\n", gpGlobals->curtime ) );
		PrintToConsole( player, str.sprintf( "m_bCompleteReset: %d\n", m_bCompleteReset ) );
		PrintToConsole( player, str.sprintf( "m_iTotalRoundsPlayed: %d\n", m_iTotalRoundsPlayed ) );
		PrintToConsole( player, str.sprintf( "m_iRoundTime: %d\n", m_iRoundTime.Get() ) );
		PrintToConsole( player, str.sprintf( "m_iRoundWinStatus: %d\n", m_iRoundWinStatus ) );

		PrintToConsole( player, str.sprintf( "first connected: %d\n", m_bFirstConnected ) );
		PrintToConsole( player, str.sprintf( "intermission end time: %f\n", m_flIntermissionEndTime ) );
		PrintToConsole( player, str.sprintf( "freeze period: %d\n", m_bFreezePeriod.Get() ) );
		PrintToConsole( player, str.sprintf( "round restart time: %f\n", m_flRestartRoundTime ) );
		PrintToConsole( player, str.sprintf( "game start time: %f\n", m_flGameStartTime.Get() ) );
		PrintToConsole( player, str.sprintf( "m_fRoundStartTime: %f\n", m_fRoundStartTime.Get() ) );
		PrintToConsole( player, str.sprintf( "freeze time: %d\n", m_iFreezeTime ) );
		PrintToConsole( player, str.sprintf( "next think: %f\n", m_tmNextPeriodicThink ) );

		PrintToConsole( player, str.sprintf( "fraglimit: %d\n", fraglimit.GetInt() ) );
		PrintToConsole( player, str.sprintf( "mp_maxrounds: %d\n", mp_maxrounds.GetInt() ) );
		PrintToConsole( player, str.sprintf( "mp_winlimit: %d\n", mp_winlimit.GetInt() ) );
		PrintToConsole( player, str.sprintf( "bot_quota: %d\n", cv_bot_quota.GetInt() ) );
		PrintToConsole( player, str.sprintf( "bot_quota_mode: %s\n", cv_bot_quota_mode.GetString() ) );
		PrintToConsole( player, str.sprintf( "bot_join_after_player: %d\n", cv_bot_join_after_player.GetInt() ) );
		PrintToConsole( player, str.sprintf( "bot_join_delay: %d\n", bot_join_delay.GetInt() ) );
		PrintToConsole( player, str.sprintf( "nextlevel: %s\n", nextlevel.GetString() ) );

		int humansInGame = UTIL_HumansInGame( true );
		int botsInGame = UTIL_BotsInGame();
		PrintToConsole( player, str.sprintf( "%d humans and %d bots in game\n", humansInGame, botsInGame ) );

		PrintToConsole( player, str.sprintf( "num CTs (spawnable): %d (%d)\n", m_iNumCT, m_iNumSpawnableCT ) );
		PrintToConsole( player, str.sprintf( "num Ts (spawnable): %d (%d)\n", m_iNumTerrorist, m_iNumSpawnableTerrorist ) );

		if ( g_fGameOver )
		{
			PrintToConsole( player, str.sprintf( "Game is over!\n" ) );
		}
		PrintToConsole( player, str.sprintf( "\n" ) );
	}

	CON_COMMAND( mp_dump_timers, "Prints round timers to the console for debugging" )
	{
		if ( !UTIL_IsCommandIssuedByServerAdmin() )
			return;

		if ( CSGameRules() )
		{
			CSGameRules()->DumpTimers();
		}
	}


	// living players on the given team need to be marked as not receiving any money
	// next round.
	void CCSGameRules::MarkLivingPlayersOnTeamAsNotReceivingMoneyNextRound(int team)
	{
		int playerNum;
		for (playerNum = 1; playerNum <= gpGlobals->maxClients; ++playerNum)
		{
			CCSPlayer *player = (CCSPlayer *)UTIL_PlayerByIndex(playerNum);
			if (player == NULL)
			{
				continue;
			}

			if ((player->GetTeamNumber() == team) && (player->IsAlive()))
			{
				player->MarkAsNotReceivingMoneyNextRound();
			}
		}
	}


	void CCSGameRules::CheckLevelInitialized( void )
	{
		if ( !m_bLevelInitialized )
		{
			// Count the number of spawn points for each team
			// This determines the maximum number of players allowed on each

			CBaseEntity* ent = NULL; 
			
			m_iSpawnPointCount_Terrorist	= 0;
			m_iSpawnPointCount_CT			= 0;

			while ( ( ent = gEntList.FindEntityByClassname( ent, "info_player_terrorist" ) ) != NULL )
			{
				if ( IsSpawnPointValid( ent, NULL ) )
				{
					m_iSpawnPointCount_Terrorist++;
				}
				else
				{
					Warning("Invalid terrorist spawnpoint at (%.1f,%.1f,%.1f)\n",
						ent->GetAbsOrigin()[0],ent->GetAbsOrigin()[2],ent->GetAbsOrigin()[2] );
				}
			}

			ent = NULL;
			
			while ( ( ent = gEntList.FindEntityByClassname( ent, "info_player_counterterrorist" ) ) != NULL )
			{
				if ( IsSpawnPointValid( ent, NULL ) ) 
				{
					m_iSpawnPointCount_CT++;
				}
				else
				{
					Warning("Invalid counterterrorist spawnpoint at (%.1f,%.1f,%.1f)\n",
						ent->GetAbsOrigin()[0],ent->GetAbsOrigin()[2],ent->GetAbsOrigin()[2] );
				}
			}

			// Is this a logo map?
			if ( gEntList.FindEntityByClassname( NULL, "info_player_logo" ) || gpGlobals->eLoadType == MapLoad_Background )
				m_bLogoMap = true;

			m_bLevelInitialized = true;
		}
	}

	void CCSGameRules::ShowSpawnPoints( void )
	{
		CBaseEntity* ent = NULL;
		
		while ( ( ent = gEntList.FindEntityByClassname( ent, "info_player_terrorist" ) ) != NULL )
		{
			if ( IsSpawnPointValid( ent, NULL ) )
			{
				NDebugOverlay::Box( ent->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX, 0, 255, 0, 200, 600 );
			}
			else
			{
				NDebugOverlay::Box( ent->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX, 255, 0, 0, 200, 600);
			}
		}

		ent = NULL;

		while ( ( ent = gEntList.FindEntityByClassname( ent, "info_player_counterterrorist" ) ) != NULL )
		{
			if ( IsSpawnPointValid( ent, NULL ) ) 
			{
				NDebugOverlay::Box( ent->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX, 0, 255, 0, 200, 600 );
			}
			else
			{
				NDebugOverlay::Box( ent->GetAbsOrigin(), VEC_HULL_MIN, VEC_HULL_MAX, 255, 0, 0, 200, 600 );
			}
		}
	}

	void CCSGameRules::CheckRestartRound( void )
	{
		// Restart the game if specified by the server
		int iRestartDelay = mp_restartgame.GetInt();

		if ( iRestartDelay > 0 )
		{
			if ( iRestartDelay > 60 )
				iRestartDelay = 60;

			// log the restart
			UTIL_LogPrintf( "World triggered \"Restart_Round_(%i_%s)\"\n", iRestartDelay, iRestartDelay == 1 ? "second" : "seconds" );

			UTIL_LogPrintf( "Team \"CT\" scored \"%i\" with \"%i\" players\n", m_iNumCTWins, m_iNumCT );
			UTIL_LogPrintf( "Team \"TERRORIST\" scored \"%i\" with \"%i\" players\n", m_iNumTerroristWins, m_iNumTerrorist );

			// let the players know
			char strRestartDelay[64];
			Q_snprintf( strRestartDelay, sizeof( strRestartDelay ), "%d", iRestartDelay );
			UTIL_ClientPrintAll( HUD_PRINTCENTER, "#Game_will_restart_in", strRestartDelay, iRestartDelay == 1 ? "SECOND" : "SECONDS" );
			UTIL_ClientPrintAll( HUD_PRINTCONSOLE, "#Game_will_restart_in", strRestartDelay, iRestartDelay == 1 ? "SECOND" : "SECONDS" );

			m_flRestartRoundTime = gpGlobals->curtime + iRestartDelay;
			m_bCompleteReset = true;
			mp_restartgame.SetValue( 0 );
		}
	}


	class SetHumanTeamFunctor
	{
	public:
		SetHumanTeamFunctor( int targetTeam )
		{
			m_targetTeam = targetTeam;
			m_sourceTeam = ( m_targetTeam == TEAM_CT ) ? TEAM_TERRORIST : TEAM_CT;

			m_traitors.MakeReliable();
			m_loyalists.MakeReliable();
			m_loyalists.AddAllPlayers();
		}

		bool operator()( CBasePlayer *basePlayer )
		{
			CCSPlayer *player = ToCSPlayer( basePlayer );
			if ( !player )
				return true;

			if ( player->IsBot() )
				return true;

			if ( player->GetTeamNumber() != m_sourceTeam )
				return true;

			if ( player->State_Get() == STATE_PICKINGCLASS )
				return true;

			if ( CSGameRules()->TeamFull( m_targetTeam ) )
				return false;

			if ( CSGameRules()->TeamStacked( m_targetTeam, m_sourceTeam ) )
				return false;

			player->SwitchTeam( m_targetTeam );
			m_traitors.AddRecipient( player );
			m_loyalists.RemoveRecipient( player );

			return true;
		}

		void SendNotice( void )
		{
			if ( m_traitors.GetRecipientCount() > 0 )
			{
				UTIL_ClientPrintFilter( m_traitors, HUD_PRINTCENTER, "#Player_Balanced" );
				UTIL_ClientPrintFilter( m_loyalists, HUD_PRINTCENTER, "#Teams_Balanced" );
			}
		}

	private:
		int m_targetTeam;
		int m_sourceTeam;

		CRecipientFilter m_traitors;
		CRecipientFilter m_loyalists;
	};


	void CCSGameRules::MoveHumansToHumanTeam( void )
	{
		int targetTeam = GetHumanTeam();
		if ( targetTeam != TEAM_TERRORIST && targetTeam != TEAM_CT )
			return;

		SetHumanTeamFunctor setTeam( targetTeam );
		ForEachPlayer( setTeam );

		setTeam.SendNotice();
	}


	void CCSGameRules::BalanceTeams( void )
	{
		int iTeamToSwap = TEAM_UNASSIGNED;
		int iNumToSwap;

		if (m_iMapHasVIPSafetyZone == 1) // The ratio for teams is different for Assasination maps
		{
			int iDesiredNumCT, iDesiredNumTerrorist;
			
			if ( (m_iNumCT + m_iNumTerrorist)%2 != 0)	// uneven number of players
				iDesiredNumCT			= (int)((m_iNumCT + m_iNumTerrorist) * 0.55) + 1;
			else
				iDesiredNumCT			= (int)((m_iNumCT + m_iNumTerrorist)/2);
			iDesiredNumTerrorist	= (m_iNumCT + m_iNumTerrorist) - iDesiredNumCT;

			if ( m_iNumCT < iDesiredNumCT )
			{
				iTeamToSwap = TEAM_TERRORIST;
				iNumToSwap = iDesiredNumCT - m_iNumCT;
			}
			else if ( m_iNumTerrorist < iDesiredNumTerrorist )
			{
				iTeamToSwap = TEAM_CT;
				iNumToSwap = iDesiredNumTerrorist - m_iNumTerrorist;
			}
			else
				return;
		}
		else
		{
			if (m_iNumCT > m_iNumTerrorist)
			{
				iTeamToSwap = TEAM_CT;
				iNumToSwap = (m_iNumCT - m_iNumTerrorist)/2;
				
			}
			else if (m_iNumTerrorist > m_iNumCT)
			{
				iTeamToSwap = TEAM_TERRORIST;
				iNumToSwap = (m_iNumTerrorist - m_iNumCT)/2;
			}
			else
			{
				return;	// Teams are even.. Get out of here.
			}
		}

		if (iNumToSwap > 3) // Don't swap more than 3 players at a time.. This is a naive method of avoiding infinite loops.
			iNumToSwap = 3;

		int iTragetTeam = TEAM_UNASSIGNED;

		if ( iTeamToSwap == TEAM_CT )
		{
			iTragetTeam = TEAM_TERRORIST;
		}
		else if ( iTeamToSwap == TEAM_TERRORIST )
		{
			iTragetTeam = TEAM_CT;
		}
		else
		{
			// no valid team to swap
			return;
		}

		CRecipientFilter traitors;
		CRecipientFilter loyalists;

		traitors.MakeReliable();
		loyalists.MakeReliable();
		loyalists.AddAllPlayers();

		for (int i = 0; i < iNumToSwap; i++)
		{
			// last person to join the server
			int iHighestUserID = -1;
			CCSPlayer *pPlayerToSwap = NULL;

			// check if target team is full, exit if so
			if ( TeamFull(iTragetTeam) )
				break;

			// search for player with highest UserID = most recently joined to switch over
			for ( int j = 1; j <= gpGlobals->maxClients; j++ )
			{
				CCSPlayer *pPlayer = (CCSPlayer *)UTIL_PlayerByIndex( j );

				if ( !pPlayer )
					continue;

				CCSBot *bot = dynamic_cast< CCSBot * >(pPlayer);
				if ( bot )
					continue; // don't swap bots - the bot system will handle that

				if ( pPlayer &&
					 ( m_pVIP != pPlayer ) && 
					 ( pPlayer->GetTeamNumber() == iTeamToSwap ) && 
					 ( engine->GetPlayerUserId( pPlayer->edict() ) > iHighestUserID ) &&
					 ( pPlayer->State_Get() != STATE_PICKINGCLASS ) )
					{
						iHighestUserID = engine->GetPlayerUserId( pPlayer->edict() );
						pPlayerToSwap = pPlayer;
					}
			}

			if ( pPlayerToSwap != NULL )
			{
				traitors.AddRecipient( pPlayerToSwap );
				loyalists.RemoveRecipient( pPlayerToSwap );
				pPlayerToSwap->SwitchTeam( iTragetTeam );
			}
		}

		if ( traitors.GetRecipientCount() > 0 )
		{
			UTIL_ClientPrintFilter( traitors, HUD_PRINTCENTER, "#Player_Balanced" );
			UTIL_ClientPrintFilter( loyalists, HUD_PRINTCENTER, "#Teams_Balanced" );
		}
	}


	bool CCSGameRules::TeamFull( int team_id )
	{
		CheckLevelInitialized();

		switch ( team_id )
		{
		case TEAM_TERRORIST:
			if ( m_iSpawnPointCount_Terrorist <= 0 )
				return false;
			return m_iNumTerrorist >= m_iSpawnPointCount_Terrorist;

		case TEAM_CT:
			if ( m_iSpawnPointCount_CT <= 0 )
				return false;
			return m_iNumCT >= m_iSpawnPointCount_CT;
		}

		return false;
	}
	
	int CCSGameRules::GetHumanTeam()
	{
		if ( FStrEq( "CT", mp_humanteam.GetString() ) )
		{
			return TEAM_CT;
		}
		else if ( FStrEq( "T", mp_humanteam.GetString() ) )
		{
			return TEAM_TERRORIST;
		}
		
		return TEAM_UNASSIGNED;
	}

	int CCSGameRules::SelectDefaultTeam( bool ignoreBots /*= false*/ )
	{
		if ( ignoreBots && ( FStrEq( cv_bot_join_team.GetString(), "T" ) || FStrEq( cv_bot_join_team.GetString(), "CT" ) ) )
		{
			ignoreBots = false;	// don't ignore bots when they can't switch teams
		}

		if ( ignoreBots && !mp_autoteambalance.GetBool() )
		{
			ignoreBots = false;	// don't ignore bots when they can't switch teams
		}

		int team = TEAM_UNASSIGNED;
		int numTerrorists = m_iNumTerrorist;
		int numCTs = m_iNumCT;
		if ( ignoreBots )
		{
			numTerrorists = UTIL_HumansOnTeam( TEAM_TERRORIST );
			numCTs = UTIL_HumansOnTeam( TEAM_CT );
		}

		// Choose the team that's lacking players
		if ( numTerrorists < numCTs )
		{
			team = TEAM_TERRORIST;
		}
		else if ( numTerrorists > numCTs )
		{
			team = TEAM_CT;
		}
		// Choose the team that's losing
		else if ( m_iNumTerroristWins < m_iNumCTWins )
		{
			team = TEAM_TERRORIST;
		}
		else if ( m_iNumCTWins < m_iNumTerroristWins )
		{
			team = TEAM_CT;
		}
		else
		{
			// Teams and scores are equal, pick a random team
			if ( random->RandomInt( 0, 1 ) == 0 )
			{
				team = TEAM_CT;
			}
			else
			{
				team = TEAM_TERRORIST;
			}
		}

		if ( TeamFull( team ) )
		{
			// Pick the opposite team
			if ( team == TEAM_TERRORIST )
			{
				team = TEAM_CT;
			}
			else
			{
				team = TEAM_TERRORIST;
			}

			// No choices left
			if ( TeamFull( team ) )
				return TEAM_UNASSIGNED;
		}

		return team;
	}

	//checks to see if the desired team is stacked, returns true if it is
	bool CCSGameRules::TeamStacked( int newTeam_id, int curTeam_id  )
	{
		//players are allowed to change to their own team
		if(newTeam_id == curTeam_id)
			return false;

		// if mp_limitteams is 0, don't check
		if ( mp_limitteams.GetInt() == 0 )
			return false;

		switch ( newTeam_id )
		{
		case TEAM_TERRORIST:
			if(curTeam_id != TEAM_UNASSIGNED && curTeam_id != TEAM_SPECTATOR)
			{
				if((m_iNumTerrorist + 1) > (m_iNumCT + mp_limitteams.GetInt() - 1))
					return true;
				else
					return false;
			}
			else
			{
				if((m_iNumTerrorist + 1) > (m_iNumCT + mp_limitteams.GetInt()))
					return true;
				else
					return false;
			}
			break;
		case TEAM_CT:
			if(curTeam_id != TEAM_UNASSIGNED && curTeam_id != TEAM_SPECTATOR)
			{
				if((m_iNumCT + 1) > (m_iNumTerrorist + mp_limitteams.GetInt() - 1))
					return true;
				else
					return false;
			}
			else
			{
				if((m_iNumCT + 1) > (m_iNumTerrorist + mp_limitteams.GetInt()))
					return true;
				else
					return false;
			}
			break;
		}

		return false;
	}


	//=========================================================
	//=========================================================
	bool CCSGameRules::FPlayerCanRespawn( CBasePlayer *pBasePlayer )
	{
		CCSPlayer *pPlayer = ToCSPlayer( pBasePlayer );
		if ( !pPlayer )
			Error( "FPlayerCanRespawn: pPlayer=0" );

		const bool isSpecialInfected = ( pPlayer->GetTeamNumber() == TEAM_INFECTED && pPlayer->GetZombieClass() > 0 );

		// Special infected can respawn repeatedly based on their own respawn timer.
		if ( isSpecialInfected )
		{
			if ( gpGlobals->curtime < m_flRestartRoundTime )
				return false;

			// Only valid team members can spawn
			if ( pPlayer->GetTeamNumber() != TEAM_CT && pPlayer->GetTeamNumber() != TEAM_TERRORIST )
				return false;

			// Only players with a valid class can spawn
			if ( pPlayer->GetClass() == CS_CLASS_NONE )
				return false;

			// Allow initial spawn (joining mid-round) and allow repeated spawns after the respawn timer.
			if ( pPlayer->IsAlive() )
				return true;

			const float respawnTime = MAX( 0.0f, z_special_respawn_time.GetFloat() );
			const float deathTime = pPlayer->GetSpecialInfectedDeathTimestamp();
			return ( deathTime <= 0.0f ) || ( gpGlobals->curtime >= ( deathTime + respawnTime ) );
		}

		// Player cannot respawn twice in a round
		if ( pPlayer->m_iNumSpawns > 0 && m_bFirstConnected )
			return false;

		// If they're dead after the map has ended, and it's about to start the next round,
		// wait for the round restart to respawn them.
		if ( gpGlobals->curtime < m_flRestartRoundTime )
			return false;

		// Only valid team members can spawn
		if ( pPlayer->GetTeamNumber() != TEAM_CT && pPlayer->GetTeamNumber() != TEAM_TERRORIST )
			return false;

		// Only players with a valid class can spawn
		if ( pPlayer->GetClass() == CS_CLASS_NONE )
			return false;

		// Player cannot respawn until next round if more than 20 seconds in

		// Tabulate the number of players on each team.
		m_iNumCT = GetGlobalTeam( TEAM_CT )->GetNumPlayers();
		m_iNumTerrorist = GetGlobalTeam( TEAM_TERRORIST )->GetNumPlayers();

		if ( m_iNumTerrorist > 0 && m_iNumCT > 0 )
		{
			if ( gpGlobals->curtime > (m_fRoundStartTime + 20) )
			{
				//If this player just connected and fadetoblack is on, then maybe
				//the server admin doesn't want him peeking around.
				color32_s clr = {0,0,0,255};
				if ( mp_fadetoblack.GetBool() )
				{
					UTIL_ScreenFade( pPlayer, clr, 3, 3, FFADE_OUT | FFADE_STAYOUT );
				}

				return false;
			}
		}

		// Player cannot respawn while in the Choose Appearance menu
		//if ( pPlayer->m_iMenu == Menu_ChooseAppearance )
		//	return false;

		return true;
	}

	void CCSGameRules::TerminateRound(float tmDelay, int iReason )
	{
		variant_t emptyVariant;
		int iWinnerTeam = WINNER_NONE;
		const char *text = "UNKNOWN";
				
		// UTIL_ClientPrintAll( HUD_PRINTCENTER, sentence );

		switch ( iReason )
		{
// Terror wins:
			case Target_Bombed:	
				text = "#Target_Bombed";
				iWinnerTeam = WINNER_TER;
				break;

			case VIP_Assassinated:
				text = "#VIP_Assassinated";
				iWinnerTeam = WINNER_TER;
				break;

			case Terrorists_Escaped:
				text = "#Terrorists_Escaped";
				iWinnerTeam = WINNER_TER;
				break;

			case Terrorists_Win:
				text = "#Terrorists_Win";
				iWinnerTeam = WINNER_TER;
				break;

			case Hostages_Not_Rescued:
				text = "#Hostages_Not_Rescued";
				iWinnerTeam = WINNER_TER;
				break;

			case VIP_Not_Escaped:
				text = "#VIP_Not_Escaped";
				iWinnerTeam = WINNER_TER;
				break;
// CT wins:
			case VIP_Escaped:
				text = "#VIP_Escaped";
				iWinnerTeam = WINNER_CT;
				break;

			case CTs_PreventEscape:
				text = "#CTs_PreventEscape";
				iWinnerTeam = WINNER_CT;
				break;

			case Escaping_Terrorists_Neutralized:
				text = "#Escaping_Terrorists_Neutralized";
				iWinnerTeam = WINNER_CT;
				break;

			case Bomb_Defused:
				text = "#Bomb_Defused";
				iWinnerTeam = WINNER_CT;
				break;

			case CTs_Win:
				text = "#CTs_Win";
				iWinnerTeam = WINNER_CT;
				break;

			case All_Hostages_Rescued:
				text = "#All_Hostages_Rescued";
				iWinnerTeam = WINNER_CT;
				break;

			case Target_Saved:
				text = "#Target_Saved";
				iWinnerTeam = WINNER_CT;
				break;

			case Terrorists_Not_Escaped:
				text = "#Terrorists_Not_Escaped";
				iWinnerTeam = WINNER_CT;
				break;
// no winners:
			case Game_Commencing:
				text = "#Game_Commencing";
				iWinnerTeam = WINNER_DRAW;
				break;

			case Round_Draw:
				text = "#Round_Draw";
				iWinnerTeam = WINNER_DRAW;
				break;

			default:
				DevMsg("TerminateRound: unknown round end ID %i\n", iReason );
				break;
		}

		m_iRoundWinStatus = iWinnerTeam;
		m_flRestartRoundTime = gpGlobals->curtime + tmDelay;

		if ( iWinnerTeam == WINNER_CT )
		{
			for( int i=0;i<g_Hostages.Count();i++ )
				g_Hostages[i]->AcceptInput( "CTsWin", NULL, NULL, emptyVariant, 0 );
		}

		else if ( iWinnerTeam == WINNER_TER )
		{
			for( int i=0;i<g_Hostages.Count();i++ )
				g_Hostages[i]->AcceptInput( "TerroristsWin", NULL, NULL, emptyVariant, 0 );
		}
		else
		{
			Assert( iWinnerTeam == WINNER_NONE || iWinnerTeam == WINNER_DRAW );
		}

		//=============================================================================
		// HPE_BEGIN:		
		//=============================================================================

		// [tj] Check for any non-player-specific achievements.
		ProcessEndOfRoundAchievements(iWinnerTeam, iReason);

		if( iReason != Game_Commencing )
		{
			// [pfreese] Setup and send win panel event (primarily funfact data)

			FunFact funfact;
			funfact.szLocalizationToken = "";
			funfact.iPlayer = 0;
			funfact.iData1 = 0;
			funfact.iData2 = 0;
			funfact.iData3 = 0;

			m_pFunFactManager->GetRoundEndFunFact( iWinnerTeam, iReason, funfact);

			//Send all the info needed for the win panel
			IGameEvent *winEvent = gameeventmanager->CreateEvent( "cs_win_panel_round" );

			if ( winEvent )
			{
				// determine what categories to send
				if ( GetRoundRemainingTime() <= 0 )
				{
					// timer expired, defenders win
					// show total time that was defended
					winEvent->SetBool( "show_timer_defend", true );
					winEvent->SetInt( "timer_time", m_iRoundTime );
				}
				else
				{
					// attackers win
					// show time it took for them to win
					winEvent->SetBool( "show_timer_attack", true );

					int iTimeElapsed = m_iRoundTime - GetRoundRemainingTime();
					winEvent->SetInt( "timer_time", iTimeElapsed );
				}

				winEvent->SetInt( "final_event", iReason );

				// Set the fun fact data in the event
				winEvent->SetString( "funfact_token", funfact.szLocalizationToken);
				winEvent->SetInt( "funfact_player", funfact.iPlayer );
				winEvent->SetInt( "funfact_data1", funfact.iData1 );
				winEvent->SetInt( "funfact_data2", funfact.iData2 );
				winEvent->SetInt( "funfact_data3", funfact.iData3 );
				gameeventmanager->FireEvent( winEvent );
			}
		}

		// [tj] Inform players that the round is over
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( i );
			if(pPlayer)
			{
				pPlayer->OnRoundEnd(iWinnerTeam, iReason);
			}
		}
		//=============================================================================
		// HPE_END
		//=============================================================================

		IGameEvent * event = gameeventmanager->CreateEvent( "round_end" );
		if ( event )
		{
			event->SetInt( "winner", iWinnerTeam );
			event->SetInt( "reason", iReason );
			event->SetString( "message", text );
			event->SetInt( "priority", 6 ); // HLTV event priority, not transmitted
			gameeventmanager->FireEvent( event );
		}

		if ( GetMapRemainingTime() == 0.0f  )
		{
			UTIL_LogPrintf("World triggered \"Intermission_Time_Limit\"\n");
			GoToIntermission();
		}
	}

	//=============================================================================
	// HPE_BEGIN:	
	//=============================================================================

	// Helper to determine if all players on a team are playing for the same clan
	static bool IsClanTeam( CTeam *pTeam )
	{
		uint32 iTeamClan = 0;
		for ( int iPlayer = 0; iPlayer < pTeam->GetNumPlayers(); iPlayer++ )
		{
			CBasePlayer *pPlayer = pTeam->GetPlayer( iPlayer );
			if ( !pPlayer )
				return false;

			const char *pClanID = engine->GetClientConVarValue( pPlayer->entindex(), "cl_clanid" );
			uint32 iPlayerClan = atoi( pClanID );
			if ( iPlayer == 0 )
			{
				// Initialize the team clan
				iTeamClan = iPlayerClan;
			}
			else
			{
				if ( iPlayerClan != iTeamClan || iPlayerClan == 0 )
					return false;
			}
		}
		return iTeamClan != 0;
	}

	// [tj] This is where we check non-player-specific that occur at the end of the round
	void CCSGameRules::ProcessEndOfRoundAchievements(int iWinnerTeam, int iReason)
	{
		if (iWinnerTeam == WINNER_CT || iWinnerTeam == WINNER_TER)
		{
			int losingTeamId = (iWinnerTeam == TEAM_CT) ? TEAM_TERRORIST : TEAM_CT;
			CTeam* losingTeam = GetGlobalTeam(losingTeamId);

			
			//Check for players we should ignore when checking team size.
			int ignoreCount = 0;
			
			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CCSPlayer* pPlayer = (CCSPlayer*)UTIL_PlayerByIndex( i );
				if (pPlayer)
				{
					int teamNum = pPlayer->GetTeamNumber();
					if ( teamNum == losingTeamId )
					{
						if (pPlayer->WasNotKilledNaturally())
						{
							ignoreCount++;
						}
					}
				}
			}


			// [tj] Check extermination with no losses achievement
			if ( ( ( iReason == CTs_Win && m_bNoCTsKilled ) || ( iReason == Terrorists_Win && m_bNoTerroristsKilled ) ) 
				&& losingTeam && losingTeam->GetNumPlayers() - ignoreCount >= AchievementConsts::DefaultMinOpponentsForAchievement)
			{
				CTeam *pTeam = GetGlobalTeam( iWinnerTeam );

				for ( int iPlayer=0; iPlayer < pTeam->GetNumPlayers(); iPlayer++ )
				{
					CCSPlayer *pPlayer = ToCSPlayer( pTeam->GetPlayer( iPlayer ) );
					Assert( pPlayer );
					if ( !pPlayer )
						continue;

					pPlayer->AwardAchievement(CSLosslessExtermination);
				}
			}

			// [tj] Check flawless victory achievement - currently requiring extermination
			if (((iReason == CTs_Win && m_bNoCTsDamaged) || (iReason == Terrorists_Win && m_bNoTerroristsDamaged))
				&& losingTeam && losingTeam->GetNumPlayers() - ignoreCount >= AchievementConsts::DefaultMinOpponentsForAchievement)
			{
				CTeam *pTeam = GetGlobalTeam( iWinnerTeam );

				for ( int iPlayer=0; iPlayer < pTeam->GetNumPlayers(); iPlayer++ )
				{
					CCSPlayer *pPlayer = ToCSPlayer( pTeam->GetPlayer( iPlayer ) );
					Assert( pPlayer );
					if ( !pPlayer )
						continue;

					pPlayer->AwardAchievement(CSFlawlessVictory);
				}
			}

			// [tj] Check bloodless victory achievement
			if (((iWinnerTeam == TEAM_TERRORIST && m_bNoCTsKilled) || (iWinnerTeam == Terrorists_Win && m_bNoTerroristsKilled))
				&& losingTeam && losingTeam->GetNumPlayers() >= AchievementConsts::DefaultMinOpponentsForAchievement)
			{
				CTeam *pTeam = GetGlobalTeam( iWinnerTeam );

				for ( int iPlayer=0; iPlayer < pTeam->GetNumPlayers(); iPlayer++ )
				{
					CCSPlayer *pPlayer = ToCSPlayer( pTeam->GetPlayer( iPlayer ) );
					Assert( pPlayer );
					if ( !pPlayer )
						continue;

					pPlayer->AwardAchievement(CSBloodlessVictory);
				}
			}

			// Check the clan match achievement
			CTeam *pWinningTeam = GetGlobalTeam( iWinnerTeam );
			if ( pWinningTeam && pWinningTeam->GetNumPlayers() >= AchievementConsts::DefaultMinOpponentsForAchievement &&
				 losingTeam && losingTeam->GetNumPlayers() - ignoreCount >= AchievementConsts::DefaultMinOpponentsForAchievement &&
				 IsClanTeam( pWinningTeam ) && IsClanTeam( losingTeam ) )
			{
				for ( int iPlayer=0; iPlayer < pWinningTeam->GetNumPlayers(); iPlayer++ )
				{
					CCSPlayer *pPlayer = ToCSPlayer( pWinningTeam->GetPlayer( iPlayer ) );
					if ( !pPlayer )
						continue;

					pPlayer->AwardAchievement( CSWinClanMatch );
				}
			}
		}
	}

	//[tj] Counts the number of players in each category in the struct (dead, alive, etc...)
	void CCSGameRules::GetPlayerCounts(TeamPlayerCounts teamCounts[TEAM_MAXCOUNT])
	{
		memset(teamCounts, 0, sizeof(TeamPlayerCounts) * TEAM_MAXCOUNT);

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer* pPlayer = (CCSPlayer*)UTIL_PlayerByIndex( i );
			if (pPlayer)
			{
				int iTeam = pPlayer->GetTeamNumber();

				if (iTeam >= 0 && iTeam < TEAM_MAXCOUNT)
				{
					++teamCounts[iTeam].totalPlayers;
					if (pPlayer->IsAlive())
					{
						++teamCounts[iTeam].totalAlivePlayers;
					}
					else
					{
						++teamCounts[iTeam].totalDeadPlayers;

						//If the player has joined a team bit isn't in the game yet
						if (pPlayer->State_Get() == STATE_PICKINGCLASS)
						{
							++teamCounts[iTeam].unenteredPlayers;
						}
						else if (pPlayer->WasNotKilledNaturally())
						{
							++teamCounts[iTeam].suicidedPlayers;
						}
						else
						{
							++teamCounts[iTeam].killedPlayers;
						}						
					}
				}
			}
		}
	}
	//=============================================================================
	// HPE_END
	//=============================================================================

	void CCSGameRules::UpdateTeamScores()
	{
		CTeam *pTerrorists = GetGlobalTeam( TEAM_TERRORIST );
		CTeam *pCTs = GetGlobalTeam( TEAM_CT );

		Assert( pTerrorists && pCTs );

		if( pTerrorists )
			pTerrorists->SetScore( m_iNumTerroristWins );

		if( pCTs )
			pCTs->SetScore( m_iNumCTWins );
	}


	void CCSGameRules::CheckMapConditions()
	{
		// Check to see if this map has a bomb target in it
		if ( gEntList.FindEntityByClassname( NULL, "func_bomb_target" ) )
		{
			m_bMapHasBombTarget		= true;
			m_bMapHasBombZone		= true;
		}
		else if ( gEntList.FindEntityByClassname( NULL, "info_bomb_target" ) )
		{
			m_bMapHasBombTarget		= true;
			m_bMapHasBombZone		= false;
		}
		else
		{
			m_bMapHasBombTarget		= false;
			m_bMapHasBombZone		= false;
		}

		// See if the map has func_buyzone entities
		// Used by CBasePlayer::HandleSignals() to support maps without these entities
		if ( gEntList.FindEntityByClassname( NULL, "func_buyzone" ) )
		{
			m_bMapHasBuyZone = true;
		}
		else
		{
			m_bMapHasBuyZone = false;
		}

		// Check to see if this map has hostage rescue zones
		if ( gEntList.FindEntityByClassname( NULL, "func_hostage_rescue" ) )
		{
			m_bMapHasRescueZone = true;
		}
		else
		{
			m_bMapHasRescueZone = false;
		}

		// GOOSEMAN : See if this map has func_escapezone entities
		if ( gEntList.FindEntityByClassname( NULL, "func_escapezone" ) )
		{
			m_bMapHasEscapeZone = true;
		}
		else
		{
			m_bMapHasEscapeZone = false;
		}

		// Check to see if this map has VIP safety zones
		if ( gEntList.FindEntityByClassname( NULL, "func_vip_safetyzone" ) )
		{
			m_iMapHasVIPSafetyZone = 1;
		}
		else
		{
			m_iMapHasVIPSafetyZone = 2;
		}
	}


	void CCSGameRules::SwapAllPlayers()
	{
		// MOTODO we have to make sure that enought spaning points exits
		Assert ( 0 );
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			/* CCSPlayer *pPlayer = CCSPlayer::Instance( i );
			if ( pPlayer && !FNullEnt( pPlayer->edict() ) )
				pPlayer->SwitchTeam(); */
		}

		// Swap Team victories
		int iTemp;

		iTemp = m_iNumCTWins;
		m_iNumCTWins = m_iNumTerroristWins;
		m_iNumTerroristWins = iTemp;
		
		// Update the clients team score
		UpdateTeamScores();
	}


	bool CS_FindInList( const char **pStrings, const char *pToFind )
	{
		return FindInList( pStrings, pToFind );
	}

	void CCSGameRules::CleanUpMap()
	{
		if (IsLogoMap())
			return;

		// Recreate all the map entities from the map data (preserving their indices),
		// then remove everything else except the players.

		// Get rid of all entities except players.
		CBaseEntity *pCur = gEntList.FirstEnt();
		while ( pCur )
		{
			CWeaponCSBase *pWeapon = dynamic_cast< CWeaponCSBase* >( pCur );
			// Weapons with owners don't want to be removed..
			if ( pWeapon )
			{
                //=============================================================================
                // HPE_BEGIN:
                // [dwenger] Handle round restart processing for the weapon.
                //=============================================================================

                pWeapon->OnRoundRestart();

                //=============================================================================
                // HPE_END
                //=============================================================================

                if ( pWeapon->ShouldRemoveOnRoundRestart() )
				{
					UTIL_Remove( pCur );
				}
			}
			// remove entities that has to be restored on roundrestart (breakables etc)
			else if ( !CS_FindInList( s_PreserveEnts, pCur->GetClassname() ) )
			{
				UTIL_Remove( pCur );
			}
			
			pCur = gEntList.NextEnt( pCur );
		}
		
		// Really remove the entities so we can have access to their slots below.
		gEntList.CleanupDeleteList();

		// Cancel all queued events, in case a func_bomb_target fired some delayed outputs that
		// could kill respawning CTs
		g_EventQueue.Clear();

		// Now reload the map entities.
		class CCSMapEntityFilter : public IMapEntityFilter
		{
		public:
			virtual bool ShouldCreateEntity( const char *pClassname )
			{
				// Don't recreate the preserved entities.
				if ( !CS_FindInList( s_PreserveEnts, pClassname ) )
				{
					return true;
				}
				else
				{
					// Increment our iterator since it's not going to call CreateNextEntity for this ent.
					if ( m_iIterator != g_MapEntityRefs.InvalidIndex() )
						m_iIterator = g_MapEntityRefs.Next( m_iIterator );
				
					return false;
				}
			}


			virtual CBaseEntity* CreateNextEntity( const char *pClassname )
			{
				if ( m_iIterator == g_MapEntityRefs.InvalidIndex() )
				{
					// This shouldn't be possible. When we loaded the map, it should have used 
					// CCSMapLoadEntityFilter, which should have built the g_MapEntityRefs list
					// with the same list of entities we're referring to here.
					Assert( false );
					return NULL;
				}
				else
				{
					CMapEntityRef &ref = g_MapEntityRefs[m_iIterator];
					m_iIterator = g_MapEntityRefs.Next( m_iIterator );	// Seek to the next entity.

					if ( ref.m_iEdict == -1 || engine->PEntityOfEntIndex( ref.m_iEdict ) )
					{
						// Doh! The entity was delete and its slot was reused.
						// Just use any old edict slot. This case sucks because we lose the baseline.
						return CreateEntityByName( pClassname );
					}
					else
					{
						// Cool, the slot where this entity was is free again (most likely, the entity was 
						// freed above). Now create an entity with this specific index.
						return CreateEntityByName( pClassname, ref.m_iEdict );
					}
				}
			}

		public:
			int m_iIterator; // Iterator into g_MapEntityRefs.
		};
		CCSMapEntityFilter filter;
		filter.m_iIterator = g_MapEntityRefs.Head();

		// DO NOT CALL SPAWN ON info_node ENTITIES!

		MapEntity_ParseAllEntities( engine->GetMapEntitiesString(), &filter, true );
	}


	bool CCSGameRules::IsThereABomber()
	{
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CCSPlayer *pPlayer = CCSPlayer::Instance( i );

			if ( pPlayer && !FNullEnt( pPlayer->edict() ) )
			{
				if ( pPlayer->GetTeamNumber() == TEAM_CT )
					continue;

				if ( pPlayer->HasC4() )
					 return true; //There you are.
			}
		}

		//Didn't find a bomber.
		return false;
	}


	void CCSGameRules::EndRound()
	{
		// fake a round end
		CSGameRules()->TerminateRound( 0.0f, Round_Draw );
	}

	CBaseEntity *CCSGameRules::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
	{
		// gat valid spwan point
		CBaseEntity *pSpawnSpot = pPlayer->EntSelectSpawnPoint();

		Vector spawnOrigin = vec3_origin;
		QAngle spawnAngles = vec3_angle;

		if ( pSpawnSpot &&
			 pSpawnSpot != CBaseEntity::Instance( INDEXENT( 0 ) ) &&
			 IsSpawnPointValid( pSpawnSpot, pPlayer ) )
		{
			spawnOrigin = pSpawnSpot->GetAbsOrigin();
			spawnAngles = pSpawnSpot->GetLocalAngles();
		}
		else if ( !FindFallbackPlayerSpawnPosition( pPlayer, pPlayer->GetTeamNumber(), &spawnOrigin, &spawnAngles ) )
		{
			spawnOrigin.Init( 0.0f, 0.0f, 64.0f );
			spawnAngles.Init();
		}

		// Move the player to the place it said.
		pPlayer->Teleport( &spawnOrigin, &spawnAngles, &vec3_origin );
		pPlayer->m_Local.m_vecPunchAngle = vec3_angle;
		
		return ( pSpawnSpot && pSpawnSpot != CBaseEntity::Instance( INDEXENT( 0 ) ) ) ? pSpawnSpot : pPlayer;
	}
	
	// checks if the spot is clear of players
	bool CCSGameRules::IsSpawnPointValid( CBaseEntity *pSpot, CBasePlayer *pPlayer )
	{
		if ( !pSpot->IsTriggered( pPlayer ) )
		{
			return false;
		}

		Vector mins = GetViewVectors()->m_vHullMin;
		Vector maxs = GetViewVectors()->m_vHullMax;

		Vector vTestMins = pSpot->GetAbsOrigin() + mins;
		Vector vTestMaxs = pSpot->GetAbsOrigin() + maxs;
		
		// First test the starting origin.
		return UTIL_IsSpaceEmpty( pPlayer, vTestMins, vTestMaxs );
	}


	bool CCSGameRules::IsThereABomb()
	{
		bool bBombFound = false;

		/* are there any bombs, either laying around, or in someone's inventory? */
		if( gEntList.FindEntityByClassname( NULL, WEAPON_C4_CLASSNAME ) != 0 )
		{
			bBombFound = true;
		}
		/* what about planted bombs!? */
		else if( gEntList.FindEntityByClassname( NULL, PLANTED_C4_CLASSNAME ) != 0 )
		{
			bBombFound = true;
		}
		
		return bBombFound;
	}

	void CCSGameRules::HostageTouched()
	{
		if( gpGlobals->curtime > m_flNextHostageAnnouncement && m_iRoundWinStatus == WINNER_NONE )
		{
			//BroadcastSound( "Event.HostageTouched" );
			m_flNextHostageAnnouncement = gpGlobals->curtime + 60.0;
		}		
	}

	void CCSGameRules::CreateStandardEntities()
	{
		// Create the player resource
		g_pPlayerResource = (CPlayerResource*)CBaseEntity::Create( "cs_player_manager", vec3_origin, vec3_angle );
	
		// Create the entity that will send our data to the client.
#ifdef DBGFLAG_ASSERT
		CBaseEntity *pEnt = 
#endif
			CBaseEntity::Create( "cs_gamerules", vec3_origin, vec3_angle );
		Assert( pEnt );
	}

#define MY_USHRT_MAX	0xffff
#define MY_UCHAR_MAX	0xff

bool DataHasChanged( void )
{
	for ( int i = 0; i < CS_NUM_LEVELS; i++ )
	{
		if ( g_iTerroristVictories[i] != 0 || g_iCounterTVictories[i] != 0 )
			return true;
	}

	for ( int i = 0; i < WEAPON_MAX; i++ )
	{
		if ( g_iWeaponPurchases[i] != 0 )
			return true;
	}

	return false;
}

void CCSGameRules::UploadGameStats( void )
{
	g_flGameStatsUpdateTime -= gpGlobals->curtime;

	if ( g_flGameStatsUpdateTime > 0 )
		return;

	if ( IsBlackMarket() == false )
		return;

	if ( m_bDontUploadStats == true )
		return;

	if ( DataHasChanged() == true )
	{
		cs_gamestats_t stats;
		memset( &stats, 0, sizeof(stats) );

		// Header
		stats.header.iVersion = CS_STATS_BLOB_VERSION;
		Q_strncpy( stats.header.szGameName, "cstrike", sizeof(stats.header.szGameName) );
		Q_strncpy( stats.header.szMapName, STRING( gpGlobals->mapname ), sizeof( stats.header.szMapName ) );

		ConVar *hostip = cvar->FindVar( "hostip" );
		if ( hostip )
		{
			int ip = hostip->GetInt();
			stats.header.ipAddr[0] = ip >> 24;
			stats.header.ipAddr[1] = ( ip >> 16 ) & MY_UCHAR_MAX;
			stats.header.ipAddr[2] = ( ip >> 8 ) & MY_UCHAR_MAX;
			stats.header.ipAddr[3] = ( ip ) & MY_UCHAR_MAX;
		}			

		ConVar *hostport = cvar->FindVar( "hostip" );
		if ( hostport )
		{
			stats.header.port = hostport->GetInt();
		}			

		stats.header.serverid = 0;

		stats.iMinutesPlayed = clamp( (short)( gpGlobals->curtime / 60 ), 0, MY_USHRT_MAX ); 

		memcpy( stats.iTerroristVictories, g_iTerroristVictories, sizeof( g_iTerroristVictories) );
		memcpy( stats.iCounterTVictories, g_iCounterTVictories, sizeof( g_iCounterTVictories) );
		memcpy( stats.iBlackMarketPurchases, g_iWeaponPurchases, sizeof( g_iWeaponPurchases) );

		stats.iAutoBuyPurchases = g_iAutoBuyPurchases;
		stats.iReBuyPurchases = g_iReBuyPurchases;

		stats.iAutoBuyM4A1Purchases = g_iAutoBuyM4A1Purchases;
		stats.iAutoBuyAK47Purchases = g_iAutoBuyAK47Purchases;
		stats.iAutoBuyFamasPurchases = g_iAutoBuyFamasPurchases;
		stats.iAutoBuyGalilPurchases = g_iAutoBuyGalilPurchases;
		stats.iAutoBuyVestHelmPurchases = g_iAutoBuyVestHelmPurchases;
		stats.iAutoBuyVestPurchases = g_iAutoBuyVestPurchases;

		const void *pvBlobData = ( const void * )( &stats );
		unsigned int uBlobSize = sizeof( stats );

		if ( gamestatsuploader )
		{
			gamestatsuploader->UploadGameStats( 
				STRING( gpGlobals->mapname ),
				CS_STATS_BLOB_VERSION,
				uBlobSize,
				pvBlobData );
		}


		memset( g_iWeaponPurchases, 0, sizeof( g_iWeaponPurchases) );
		memset( g_iTerroristVictories, 0, sizeof( g_iTerroristVictories) );
		memset( g_iCounterTVictories, 0, sizeof( g_iTerroristVictories) );

		g_iAutoBuyPurchases = 0;
		g_iReBuyPurchases = 0;

		g_iAutoBuyM4A1Purchases = 0;
		g_iAutoBuyAK47Purchases = 0;
		g_iAutoBuyFamasPurchases = 0;
		g_iAutoBuyGalilPurchases = 0;
		g_iAutoBuyVestHelmPurchases = 0;
		g_iAutoBuyVestPurchases = 0;
	}

	g_flGameStatsUpdateTime = CS_GAME_STATS_UPDATE; //Next update is between 22 and 24 hours.
}
#endif	// CLIENT_DLL

CBaseCombatWeapon *CCSGameRules::GetNextBestWeapon( CBaseCombatCharacter *pPlayer, CBaseCombatWeapon *pCurrentWeapon )
{
	CBaseCombatWeapon *bestWeapon = NULL;

	// search all the weapons looking for the closest next
	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		CBaseCombatWeapon *weapon = pPlayer->GetWeapon(i);
		if ( !weapon )
			continue;

		if ( !weapon->CanBeSelected() || weapon == pCurrentWeapon )
			continue;

#ifndef CLIENT_DLL
		CCSPlayer *csPlayer = ToCSPlayer(pPlayer);
		CWeaponCSBase *csWeapon = static_cast< CWeaponCSBase * >(weapon);
		if ( csPlayer && csPlayer->IsBot() && !TheCSBots()->IsWeaponUseable( csWeapon ) )
			continue;
#endif // CLIENT_DLL

		if ( bestWeapon )
		{
			if ( weapon->GetSlot() < bestWeapon->GetSlot() )
			{
				bestWeapon = weapon;
			}
			else if ( weapon->GetSlot() == bestWeapon->GetSlot() && weapon->GetPosition() < bestWeapon->GetPosition() )
			{
				bestWeapon = weapon;
			}
		}
		else
		{
			bestWeapon = weapon;
		}
	}

	return bestWeapon;
}

float CCSGameRules::GetMapRemainingTime()
{
#ifdef GAME_DLL
	if ( nextlevel.GetString() && *nextlevel.GetString() )
	{
		return 0;
	}
#endif

	// if timelimit is disabled, return -1
	if ( mp_timelimit.GetInt() <= 0 )
		return -1;

	// timelimit is in minutes
	float flTimeLeft =  ( m_flGameStartTime + mp_timelimit.GetInt() * 60 ) - gpGlobals->curtime;

	// never return a negative value
	if ( flTimeLeft < 0 )
		flTimeLeft = 0;

	return flTimeLeft;
}

float CCSGameRules::GetMapElapsedTime( void )
{
	return gpGlobals->curtime;
}

float CCSGameRules::GetRoundRemainingTime()
{
	return (float) (m_fRoundStartTime + m_iRoundTime); 
}

float CCSGameRules::GetRoundStartTime()
{
	return m_fRoundStartTime;
}


float CCSGameRules::GetRoundElapsedTime()
{
	return gpGlobals->curtime - m_fRoundStartTime;
}


bool CCSGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		::V_swap(collisionGroup0,collisionGroup1);
	}

	// Common infected crowding: allow NPCs (including common infected) to pass through each other,
	// while navigation traces (often COLLISION_GROUP_NONE) can still treat them as obstacles.
	if ( ( collisionGroup0 == COLLISION_GROUP_NPC || collisionGroup0 == COLLISION_GROUP_NPC_ACTOR ) &&
		 ( collisionGroup1 == COLLISION_GROUP_NPC || collisionGroup1 == COLLISION_GROUP_NPC_ACTOR ) )
	{
		return false;
	}
	
	//Don't stand on COLLISION_GROUP_WEAPONs
	if( collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT &&
		collisionGroup1 == COLLISION_GROUP_WEAPON )
	{
		return false;
	}

	// TODO: make a CS-SPECIFIC COLLISION GROUP FOR PHYSICS PROPS THAT USE THIS COLLISION BEHAVIOR.

	
	if ( (collisionGroup0 == COLLISION_GROUP_PLAYER || collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT) &&
		collisionGroup1 == COLLISION_GROUP_PUSHAWAY )
	{
		return false;
	}

	if ( collisionGroup0 == COLLISION_GROUP_DEBRIS && collisionGroup1 == COLLISION_GROUP_PUSHAWAY )
	{
		// let debris and multiplayer objects collide
		return true;
	}

	return BaseClass::ShouldCollide( collisionGroup0, collisionGroup1 ); 
}


bool CCSGameRules::IsFreezePeriod()
{
	return m_bFreezePeriod;
}


bool CCSGameRules::IsVIPMap() const
{
	//MIKETODO: VIP mode
	return false;
}


bool CCSGameRules::IsBombDefuseMap() const
{
	return m_bMapHasBombTarget;
}

bool CCSGameRules::IsHostageRescueMap() const
{
	return m_bMapHasRescueZone;
}

bool CCSGameRules::IsLogoMap() const
{
	return m_bLogoMap;
}

float CCSGameRules::GetBuyTimeLength() const
{
	return ( mp_buytime.GetFloat() * 60 );
}

bool CCSGameRules::IsBuyTimeElapsed()
{
	return ( GetRoundElapsedTime() > GetBuyTimeLength() );
}

int CCSGameRules::DefaultFOV()
{
	return 90;
}

const CViewVectors* CCSGameRules::GetViewVectors() const
{
	return &g_CSViewVectors;
}


//-----------------------------------------------------------------------------
// Purpose: Init CS ammo definitions
//-----------------------------------------------------------------------------

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			1	

// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)


static CCSAmmoDef ammoDef;
CCSAmmoDef* GetCSAmmoDef()
{
	GetAmmoDef(); // to initialize the ammo info
	return &ammoDef;
}

CAmmoDef* GetAmmoDef()
{
	static bool bInitted = false;

	if ( !bInitted )
	{
		bInitted = true;

		ammoDef.AddAmmoType("AR2", DMG_BULLET, TRACER_LINE_AND_WHIZ, "sk_plr_dmg_ar2", "sk_npc_dmg_ar2", "sk_max_ar2", BULLET_IMPULSE(200, 1225), 0);
		ammoDef.AddAmmoType("AlyxGun", DMG_BULLET, TRACER_LINE, "sk_plr_dmg_alyxgun", "sk_npc_dmg_alyxgun", "sk_max_alyxgun", BULLET_IMPULSE(200, 1225), 0);
		ammoDef.AddAmmoType("Pistol", DMG_BULLET, TRACER_LINE_AND_WHIZ, "sk_plr_dmg_pistol", "sk_npc_dmg_pistol", "sk_max_pistol", BULLET_IMPULSE(200, 1225), 0);
		ammoDef.AddAmmoType("SMG1", DMG_BULLET, TRACER_LINE_AND_WHIZ, "sk_plr_dmg_smg1", "sk_npc_dmg_smg1", "sk_max_smg1", BULLET_IMPULSE(200, 1225), 0);
		ammoDef.AddAmmoType("357", DMG_BULLET, TRACER_LINE_AND_WHIZ, "sk_plr_dmg_357", "sk_npc_dmg_357", "sk_max_357", BULLET_IMPULSE(800, 5000), 0);
		ammoDef.AddAmmoType("XBowBolt", DMG_BULLET, TRACER_LINE, "sk_plr_dmg_crossbow", "sk_npc_dmg_crossbow", "sk_max_crossbow", BULLET_IMPULSE(800, 8000), 0);

		ammoDef.AddAmmoType("Buckshot", DMG_BULLET | DMG_BUCKSHOT, TRACER_LINE, "sk_plr_dmg_buckshot", "sk_npc_dmg_buckshot", "sk_max_buckshot", BULLET_IMPULSE(400, 1200), 0);
		ammoDef.AddAmmoType("RPG_Round", DMG_BURN, TRACER_NONE, "sk_plr_dmg_rpg_round", "sk_npc_dmg_rpg_round", "sk_max_rpg_round", 0, 0);
		ammoDef.AddAmmoType("SMG1_Grenade", DMG_BURN, TRACER_NONE, "sk_plr_dmg_smg1_grenade", "sk_npc_dmg_smg1_grenade", "sk_max_smg1_grenade", 0, 0);
		ammoDef.AddAmmoType("SniperRound", DMG_BULLET, TRACER_NONE, "sk_plr_dmg_sniper_round", "sk_npc_dmg_sniper_round", "sk_max_sniper_round", BULLET_IMPULSE(650, 6000), 0);
		ammoDef.AddAmmoType("SniperPenetratedRound", DMG_BULLET, TRACER_NONE, "sk_dmg_sniper_penetrate_plr", "sk_dmg_sniper_penetrate_npc", "sk_max_sniper_round", BULLET_IMPULSE(150, 6000), 0);
		ammoDef.AddAmmoType("Grenade", DMG_BURN, TRACER_NONE, "sk_plr_dmg_grenade", "sk_npc_dmg_grenade", "sk_max_grenade", 0, 0);
		ammoDef.AddAmmoType("Thumper", DMG_SONIC, TRACER_NONE, 10, 10, 2, 0, 0);
		ammoDef.AddAmmoType("Gravity", DMG_CLUB, TRACER_NONE, 0, 0, 8, 0, 0);
		//		ammoDef.AddAmmoType("Extinguisher",		DMG_BURN,					TRACER_NONE,			0,	0, 100, 0, 0 );
		ammoDef.AddAmmoType("Battery", DMG_CLUB, TRACER_NONE, NULL, NULL, NULL, 0, 0);
		ammoDef.AddAmmoType("GaussEnergy", DMG_SHOCK, TRACER_NONE, "sk_jeep_gauss_damage", "sk_jeep_gauss_damage", "sk_max_gauss_round", BULLET_IMPULSE(650, 8000), 0); // hit like a 10kg weight at 400 in/s
		ammoDef.AddAmmoType("CombineCannon", DMG_BULLET, TRACER_LINE, "sk_npc_dmg_gunship_to_plr", "sk_npc_dmg_gunship", NULL, 1.5 * 750 * 12, 0); // hit like a 1.5kg weight at 750 ft/s
		ammoDef.AddAmmoType("AirboatGun", DMG_AIRBOAT, TRACER_LINE, "sk_plr_dmg_airboat", "sk_npc_dmg_airboat", NULL, BULLET_IMPULSE(10, 600), 0);

		//=====================================================================
		// STRIDER MINIGUN DAMAGE - Pull up a chair and I'll tell you a tale.
		//
		// When we shipped Half-Life 2 in 2004, we were unaware of a bug in
		// CAmmoDef::NPCDamage() which was returning the MaxCarry field of
		// an ammotype as the amount of damage that should be done to a NPC
		// by that type of ammo. Thankfully, the bug only affected Ammo Types 
		// that DO NOT use ConVars to specify their parameters. As you can see,
		// all of the important ammotypes use ConVars, so the effect of the bug
		// was limited. The Strider Minigun was affected, though.
		//
		// According to my perforce Archeology, we intended to ship the Strider
		// Minigun ammo type to do 15 points of damage per shot, and we did. 
		// To achieve this we, unaware of the bug, set the Strider Minigun ammo 
		// type to have a maxcarry of 15, since our observation was that the 
		// number that was there before (8) was indeed the amount of damage being
		// done to NPC's at the time. So we changed the field that was incorrectly
		// being used as the NPC Damage field.
		//
		// The bug was fixed during Episode 1's development. The result of the 
		// bug fix was that the Strider was reduced to doing 5 points of damage
		// to NPC's, since 5 is the value that was being assigned as NPC damage
		// even though the code was returning 15 up to that point.
		//
		// Now as we go to ship Orange Box, we discover that the Striders in 
		// Half-Life 2 are hugely ineffective against citizens, causing big
		// problems in maps 12 and 13. 
		//
		// In order to restore balance to HL2 without upsetting the delicate 
		// balance of ep2_outland_12, I have chosen to build Episodic binaries
		// with 5 as the Strider->NPC damage, since that's the value that has
		// been in place for all of Episode 2's development. Half-Life 2 will
		// build with 15 as the Strider->NPC damage, which is how HL2 shipped
		// originally, only this time the 15 is located in the correct field
		// now that the AmmoDef code is behaving correctly.
		//
		//=====================================================================
#ifdef HL2_EPISODIC
		ammoDef.AddAmmoType("StriderMinigun", DMG_BULLET, TRACER_LINE, 5, 5, 15, 1.0 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED); // hit like a 1.0kg weight at 750 ft/s
#else
		ammoDef.AddAmmoType("StriderMinigun", DMG_BULLET, TRACER_LINE, 5, 15, 15, 1.0 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED); // hit like a 1.0kg weight at 750 ft/s
#endif//HL2_EPISODIC

		ammoDef.AddAmmoType("StriderMinigunDirect", DMG_BULLET, TRACER_LINE, 2, 2, 15, 1.0 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED); // hit like a 1.0kg weight at 750 ft/s
		ammoDef.AddAmmoType("HelicopterGun", DMG_BULLET, TRACER_LINE_AND_WHIZ, "sk_npc_dmg_helicopter_to_plr", "sk_npc_dmg_helicopter", "sk_max_smg1", BULLET_IMPULSE(400, 1225), AMMO_FORCE_DROP_IF_CARRIED | AMMO_INTERPRET_PLRDAMAGE_AS_DAMAGE_TO_PLAYER);
		ammoDef.AddAmmoType("AR2AltFire", DMG_DISSOLVE, TRACER_NONE, 0, 0, "sk_max_ar2_altfire", 0, 0);
		ammoDef.AddAmmoType("Grenade", DMG_BURN, TRACER_NONE, "sk_plr_dmg_grenade", "sk_npc_dmg_grenade", "sk_max_grenade", 0, 0);
#ifdef HL2_EPISODIC
		ammoDef.AddAmmoType("Hopwire", DMG_BLAST, TRACER_NONE, "sk_plr_dmg_grenade", "sk_npc_dmg_grenade", "sk_max_hopwire", 0, 0);
		ammoDef.AddAmmoType("CombineHeavyCannon", DMG_BULLET, TRACER_LINE, 40, 40, NULL, 10 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED); // hit like a 10 kg weight at 750 ft/s
		ammoDef.AddAmmoType("ammo_proto1", DMG_BULLET, TRACER_LINE, 0, 0, 10, 0, 0);
#endif // HL2_EPISODIC

		ammoDef.AddAmmoType( BULLET_PLAYER_50AE,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_50AE_max",		2400 * BULLET_IMPULSE_EXAGGERATION, 0, 10, 14 );
		ammoDef.AddAmmoType( BULLET_PLAYER_762MM,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_762mm_max",	2400 * BULLET_IMPULSE_EXAGGERATION, 0, 10, 14 );
		ammoDef.AddAmmoType( BULLET_PLAYER_556MM,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_556mm_max",	2400 * BULLET_IMPULSE_EXAGGERATION, 0, 10, 14 );
		ammoDef.AddAmmoType( BULLET_PLAYER_556MM_BOX,	DMG_BULLET, TRACER_LINE, 0, 0, "ammo_556mm_box_max",2400 * BULLET_IMPULSE_EXAGGERATION, 0, 10, 14 );
		ammoDef.AddAmmoType( BULLET_PLAYER_338MAG,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_338mag_max",	2800 * BULLET_IMPULSE_EXAGGERATION, 0, 12, 16 );
		ammoDef.AddAmmoType( BULLET_PLAYER_9MM,			DMG_BULLET, TRACER_LINE, 0, 0, "ammo_9mm_max",		2000 * BULLET_IMPULSE_EXAGGERATION, 0, 5, 10 );
		ammoDef.AddAmmoType( BULLET_PLAYER_BUCKSHOT,	DMG_BULLET, TRACER_LINE, 0, 0, "ammo_buckshot_max", 600 * BULLET_IMPULSE_EXAGGERATION,  0, 3, 6 );
		ammoDef.AddAmmoType( BULLET_PLAYER_45ACP,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_45acp_max",	2100 * BULLET_IMPULSE_EXAGGERATION, 0, 6, 10 );
		ammoDef.AddAmmoType( BULLET_PLAYER_357SIG,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_357sig_max",	2000 * BULLET_IMPULSE_EXAGGERATION, 0, 4, 8 );
		ammoDef.AddAmmoType( BULLET_PLAYER_57MM,		DMG_BULLET, TRACER_LINE, 0, 0, "ammo_57mm_max",		2000 * BULLET_IMPULSE_EXAGGERATION, 0, 4, 8 );
		ammoDef.AddAmmoType( AMMO_TYPE_HEGRENADE,		DMG_BLAST,	TRACER_LINE, 0, 0, "ammo_hegrenade_max", 1, 0 );
		ammoDef.AddAmmoType( AMMO_TYPE_FLASHBANG,		0,			TRACER_LINE, 0,	0, "ammo_flashbang_max", 1, 0 );
		ammoDef.AddAmmoType( AMMO_TYPE_SMOKEGRENADE,	0,			TRACER_LINE, 0, 0, "ammo_smokegrenade_max", 1, 0 );

		//Adrian: I set all the prices to 0 just so the rest of the buy code works
		//This should be revisited.
		ammoDef.AddAmmoCost( BULLET_PLAYER_50AE, 0, 7 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_762MM, 0, 30 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_556MM, 0, 30 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_556MM_BOX, 0, 30 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_338MAG, 0, 10 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_9MM, 0, 30 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_BUCKSHOT, 0, 8 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_45ACP, 0, 25 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_357SIG, 0, 13 );
		ammoDef.AddAmmoCost( BULLET_PLAYER_57MM, 0, 50 );
	}

	return &ammoDef;
}

#ifndef CLIENT_DLL
const char *CCSGameRules::GetChatPrefix( bool bTeamOnly, CBasePlayer *pPlayer )
{
	const char *pszPrefix = NULL;

	if ( !pPlayer )  // dedicated server output
	{
		pszPrefix = "";
	}
	else
	{
		// team only
		if ( bTeamOnly == TRUE )
		{
			if ( pPlayer->GetTeamNumber() == TEAM_CT )
			{
				if ( pPlayer->m_lifeState == LIFE_ALIVE )
				{
					pszPrefix = "(Counter-Terrorist)";
				}
				else 
				{
					pszPrefix = "*DEAD*(Counter-Terrorist)";
				}
			}
			else if ( pPlayer->GetTeamNumber() == TEAM_TERRORIST )
			{
				if ( pPlayer->m_lifeState == LIFE_ALIVE )
				{
					pszPrefix = "(Terrorist)";
				}
				else
				{
					pszPrefix = "*DEAD*(Terrorist)";
				}
			}
			else if ( pPlayer->GetTeamNumber() == TEAM_SPECTATOR )
			{
				pszPrefix = "(Spectator)";
			}
		}
		// everyone
		else
		{
			if ( pPlayer->m_lifeState == LIFE_ALIVE )
			{
				pszPrefix = "";
			}
			else
			{
				if ( pPlayer->GetTeamNumber() != TEAM_SPECTATOR )
				{
					pszPrefix = "*DEAD*";	
				}
				else
				{
					pszPrefix = "*SPEC*";
				}
			}
		}
	}

	return pszPrefix;
}

const char *CCSGameRules::GetChatLocation( bool bTeamOnly, CBasePlayer *pPlayer )
{
	if ( !pPlayer )  // dedicated server output
	{
		return NULL;
	}

	// only teammates see locations
	if ( !bTeamOnly )
		return NULL;

	// only living players have locations
	if ( pPlayer->GetTeamNumber() != TEAM_CT && pPlayer->GetTeamNumber() != TEAM_TERRORIST )
		return NULL;

	if ( !pPlayer->IsAlive() )
		return NULL;

	return pPlayer->GetLastKnownPlaceName();
}

const char *CCSGameRules::GetChatFormat( bool bTeamOnly, CBasePlayer *pPlayer )
{
	if ( !pPlayer )  // dedicated server output
	{
		return NULL;
	}

	const char *pszFormat = NULL;

	// team only
	if ( bTeamOnly == TRUE )
	{
		if ( pPlayer->GetTeamNumber() == TEAM_CT )
		{
			if ( pPlayer->m_lifeState == LIFE_ALIVE )
			{
				const char *chatLocation = GetChatLocation( bTeamOnly, pPlayer );
				if ( chatLocation && *chatLocation )
				{
					pszFormat = "Cstrike_Chat_CT_Loc";
				}
				else
				{
					pszFormat = "Cstrike_Chat_CT";
				}
			}
			else 
			{
				pszFormat = "Cstrike_Chat_CT_Dead";
			}
		}
		else if ( pPlayer->GetTeamNumber() == TEAM_TERRORIST )
		{
			if ( pPlayer->m_lifeState == LIFE_ALIVE )
			{
				const char *chatLocation = GetChatLocation( bTeamOnly, pPlayer );
				if ( chatLocation && *chatLocation )
				{
					pszFormat = "Cstrike_Chat_T_Loc";
				}
				else
				{
					pszFormat = "Cstrike_Chat_T";
				}
			}
			else
			{
				pszFormat = "Cstrike_Chat_T_Dead";
			}
		}
		else if ( pPlayer->GetTeamNumber() == TEAM_SPECTATOR )
		{
			pszFormat = "Cstrike_Chat_Spec";
		}
	}
	// everyone
	else
	{
		if ( pPlayer->m_lifeState == LIFE_ALIVE )
		{
			pszFormat = "Cstrike_Chat_All";
		}
		else
		{
			if ( pPlayer->GetTeamNumber() != TEAM_SPECTATOR )
			{
				pszFormat = "Cstrike_Chat_AllDead";	
			}
			else
			{
				pszFormat = "Cstrike_Chat_AllSpec";
			}
		}
	}

	return pszFormat;
}

void CCSGameRules::ClientSettingsChanged( CBasePlayer *pPlayer )
{
	const char *pszNewName = engine->GetClientConVarValue( pPlayer->entindex(), "name" );
	const char *pszOldName = pPlayer->GetPlayerName();
	CCSPlayer *pCSPlayer = (CCSPlayer*)pPlayer;		
	if ( pszOldName[0] != 0 && Q_strncmp( pszOldName, pszNewName, MAX_PLAYER_NAME_LENGTH-1 ) )		
	{
		pCSPlayer->ChangeName( pszNewName );		
	}

	pCSPlayer->m_bShowHints = true;
	if ( pCSPlayer->IsNetClient() )
	{
		const char *pShowHints = engine->GetClientConVarValue( engine->IndexOfEdict( pCSPlayer->edict() ), "cl_autohelp" );
		if ( pShowHints && atoi( pShowHints ) <= 0 )
		{
			pCSPlayer->m_bShowHints = false;
		}
	}
}

bool CCSGameRules::FAllowNPCs( void )
{
	return true;
}

bool CCSGameRules::IsFriendlyFireOn( void )
{
	return friendlyfire.GetBool();
}

#ifndef CLIENT_DLL
static bool ZSpawnIsAllowed( void )
{
	ConVarRef sv_cheats( "sv_cheats" );
	ConVarRef developer( "developer" );

	const bool cheats = sv_cheats.IsValid() && ( sv_cheats.GetInt() != 0 );
	const bool dev = developer.IsValid() && ( developer.GetInt() != 0 );
	return cheats || dev;
}

static int ZSpawnParseZombieClass( const char *pszType )
{
	if ( !pszType || !pszType[0] )
		return 0;

	// Common infected aliases.
	if ( !V_stricmp( pszType, "common" ) || !V_stricmp( pszType, "infected" ) )
		return 0;

	// Special infected.
	if ( !V_stricmp( pszType, "smoker" ) )
		return 1;
	if ( !V_stricmp( pszType, "boomer" ) )
		return 2;
	if ( !V_stricmp( pszType, "hunter" ) )
		return 3;
	if ( !V_stricmp( pszType, "spitter" ) )
		return 4;
	if ( !V_stricmp( pszType, "jockey" ) )
		return 5;
	if ( !V_stricmp( pszType, "charger" ) )
		return 6;
	if ( !V_stricmp( pszType, "tank" ) )
		return 8;

	return -1;
}

static bool ZSpawnGetCrosshairPos( CCSPlayer *player, Vector *outPos )
{
	if ( !player || !outPos )
		return false;

	Vector forward;
	AngleVectors( player->EyeAngles(), &forward );

	const Vector start = player->EyePosition();
	const Vector end = start + forward * 8192.0f;

	trace_t tr;
	CTraceFilterSimple filter( player, COLLISION_GROUP_NONE );
	UTIL_TraceLine( start, end, MASK_SHOT, &filter, &tr );

	*outPos = ( tr.fraction < 1.0f ) ? tr.endpos : end;
	return true;
}

CON_COMMAND( z_spawn, "Spawn infected. Usage: z_spawn [common|smoker|boomer|hunter|spitter|jockey|charger|tank] [auto]" )
{
	if ( !ZSpawnIsAllowed() )
	{
		Msg( "z_spawn is restricted: enable sv_cheats 1 or developer 1.\n" );
		return;
	}

	CCSPlayer *player = ToCSPlayer( UTIL_GetCommandClient() );
	if ( !player )
	{
		Msg( "z_spawn requires a command client (a player).\n" );
		return;
	}

	const int argc = args.ArgC();
	const char *arg1 = ( argc > 1 ) ? args.Arg( 1 ) : NULL;
	const char *arg2 = ( argc > 2 ) ? args.Arg( 2 ) : NULL;

	// "auto" can be provided as the 2nd arg (L4D-style), or as the only arg for common infected.
	bool autoMode = false;
	if ( arg2 && !V_stricmp( arg2, "auto" ) )
	{
		autoMode = true;
	}
	else if ( arg1 && !V_stricmp( arg1, "auto" ) && argc == 2 )
	{
		autoMode = true;
		arg1 = NULL;
	}

	const int zombieClass = ZSpawnParseZombieClass( arg1 );
	if ( zombieClass < 0 )
	{
		Msg( "Unknown infected type '%s'.\n", arg1 ? arg1 : "" );
		Msg( "Usage: z_spawn [common|smoker|boomer|hunter|spitter|jockey|charger|tank] [auto]\n" );
		return;
	}

	Vector crosshairPos;
	if ( !ZSpawnGetCrosshairPos( player, &crosshairPos ) )
	{
		Msg( "z_spawn failed to find a crosshair position.\n" );
		return;
	}

	Vector spawnPos = crosshairPos;
	if ( autoMode )
	{
		if ( !TheNavMesh || !TheNavMesh->IsLoaded() || TheNavMesh->IsGenerating() )
		{
			Msg( "z_spawn auto requires a loaded nav mesh.\n" );
			return;
		}

		CNavArea *area = TheNavMesh->GetNearestNavArea( crosshairPos );
		if ( !area )
		{
			Msg( "z_spawn auto could not find a nav area at the crosshair.\n" );
			return;
		}

		area->GetClosestPointOnArea( crosshairPos, &spawnPos );
		spawnPos.z = area->GetZ( spawnPos );
	}
	else
	{
		// Snap to the floor so "shooting a wall" doesn't embed the spawn in geometry.
		trace_t tr;
		CTraceFilterWorldOnly filter;
		UTIL_TraceLine( crosshairPos + Vector( 0, 0, 256.0f ), crosshairPos - Vector( 0, 0, 2048.0f ), MASK_SOLID, &filter, &tr );
		if ( tr.fraction < 1.0f )
		{
			spawnPos = tr.endpos;
		}
	}

	QAngle spawnAng( 0.0f, random->RandomFloat( 0.0f, 360.0f ), 0.0f );

	// Special infected are CT bots.
	if ( zombieClass > 0 )
	{
		// If the command client is a dead human infected player, use z_spawn to respawn
		// them as the requested class instead of creating another infected bot.
		if ( !player->IsBot() && !player->IsAlive() && player->GetTeamNumber() == TEAM_INFECTED )
		{
			player->SetZombieClass( zombieClass );
			player->SetSpecialInfectedDeathTimestamp( 0.0f );
			player->RoundRespawn();

			if ( !player->IsAlive() )
			{
				Msg( "z_spawn failed to respawn the infected player.\n" );
				return;
			}

			Vector vel( vec3_origin );
			player->Teleport( &spawnPos, &spawnAng, &vel );
			return;
		}

		if ( CSGameRules() && CSGameRules()->TeamFull( TEAM_CT ) )
		{
			Msg( "z_spawn failed: infected team is full.\n" );
			return;
		}

		CCSBot *bot = SpawnSpecialInfectedBotAt( spawnPos, spawnAng, zombieClass );
		if ( !bot )
		{
			Msg( "z_spawn failed to spawn special infected.\n" );
			return;
		}

		// Ensure it roams on background/logo maps or with no targets.
		bot->SetRogue( true );
		bot->Hunt();
		return;
	}

	// Common infected are NPC entities.
	CBaseEntity *ent = CreateEntityByName( "infected" );
	if ( !ent )
	{
		Msg( "z_spawn failed to create common infected.\n" );
		return;
	}

	ent->SetAbsOrigin( spawnPos );
	ent->SetAbsAngles( spawnAng );
	DispatchSpawn( ent );

	// If the chosen point overlaps a wall/prop, nudge the spawn until the infected fits.
	Vector fixedPos;
	if ( FixupCommonInfectedSpawnPos( ent, spawnPos, &fixedPos ) )
	{
		QAngle ang = ent->GetAbsAngles();
		Vector vel( vec3_origin );
		ent->Teleport( &fixedPos, &ang, &vel );
	}
	else
	{
		UTIL_Remove( ent );
		Msg( "z_spawn failed: no clear spot found.\n" );
		return;
	}

	ent->Activate();

	if ( CountAliveCommonInfectedGlobal() > MAX( 0, z_common_max.GetInt() ) )
	{
		UTIL_Remove( ent );
		Msg( "z_spawn deleted the common infected because z_common_limit was exceeded.\n" );
	}
}
#endif // !CLIENT_DLL


CON_COMMAND( map_showspawnpoints, "Shows player spawn points (red=invalid)" )
{
	CSGameRules()->ShowSpawnPoints();
}

void DrawSphere( const Vector& pos, float radius, int r, int g, int b, float lifetime )
{
	Vector edge, lastEdge;
	NDebugOverlay::Line( pos, pos + Vector( 0, 0, 50 ), r, g, b, true, lifetime );

	lastEdge = Vector( radius + pos.x, pos.y, pos.z );
	float angle;
	for( angle=0.0f; angle <= 360.0f; angle += 22.5f )
	{
		edge.x = radius * BotCOS( angle ) + pos.x;
		edge.y = pos.y;
		edge.z = radius * BotSIN( angle ) + pos.z;

		NDebugOverlay::Line( edge, lastEdge, r, g, b, true, lifetime );

		lastEdge = edge;
	}

	lastEdge = Vector( pos.x, radius + pos.y, pos.z );
	for( angle=0.0f; angle <= 360.0f; angle += 22.5f )
	{
		edge.x = pos.x;
		edge.y = radius * BotCOS( angle ) + pos.y;
		edge.z = radius * BotSIN( angle ) + pos.z;

		NDebugOverlay::Line( edge, lastEdge, r, g, b, true, lifetime );

		lastEdge = edge;
	}

	lastEdge = Vector( pos.x, radius + pos.y, pos.z );
	for( angle=0.0f; angle <= 360.0f; angle += 22.5f )
	{
		edge.x = radius * BotCOS( angle ) + pos.x;
		edge.y = radius * BotSIN( angle ) + pos.y;
		edge.z = pos.z;

		NDebugOverlay::Line( edge, lastEdge, r, g, b, true, lifetime );

		lastEdge = edge;
	}
}

CON_COMMAND_F( map_showbombradius, "Shows bomb radius from the center of each bomb site and planted bomb.", FCVAR_CHEAT )
{
	float flBombDamage = 500.0f;
	if ( g_pMapInfo )
		flBombDamage = g_pMapInfo->m_flBombRadius;
	float flBombRadius = flBombDamage * 3.5f;
	Msg( "Bomb Damage is %.0f, Radius is %.0f\n", flBombDamage, flBombRadius );

	CBaseEntity* ent = NULL;
	while ( ( ent = gEntList.FindEntityByClassname( ent, "func_bomb_target" ) ) != NULL )
	{
		const Vector &pos = ent->WorldSpaceCenter();
		DrawSphere( pos, flBombRadius, 255, 255, 0, 10 );
	}

	ent = NULL;
	while ( ( ent = gEntList.FindEntityByClassname( ent, "planted_c4" ) ) != NULL )
	{
		const Vector &pos = ent->WorldSpaceCenter();
		DrawSphere( pos, flBombRadius, 255, 0, 0, 10 );
	}
}

CON_COMMAND_F( map_setbombradius, "Sets the bomb radius for the map.", FCVAR_CHEAT )
{
	if ( args.ArgC() != 2 )
		return;

	if ( !UTIL_IsCommandIssuedByServerAdmin() )
		return;

	if ( !g_pMapInfo )
		CBaseEntity::Create( "info_map_parameters", vec3_origin, vec3_angle );

	if ( !g_pMapInfo )
		return;

	g_pMapInfo->m_flBombRadius = atof( args[1] );
	map_showbombradius( args );
}

void CreateBlackMarketString( void )
{
	g_StringTableBlackMarket = networkstringtable->CreateStringTable( "BlackMarketTable" , 1 );
}

int CCSGameRules::GetStartMoney( void )
{
	if ( IsBlackMarket() )
	{
		return atoi( mp_startmoney.GetDefault() );
	}

	return mp_startmoney.GetInt();
}



//=============================================================================
// HPE_BEGIN:
// [menglish] Set up anything for all players that changes based on new players spawning mid-game
//				Find and return fun fact data
//=============================================================================
 
//-----------------------------------------------------------------------------
// Purpose: Called when a player joins the game after it's started yet can still spawn in
//-----------------------------------------------------------------------------
void CCSGameRules::SpawningLatePlayer( CCSPlayer* pLatePlayer )
{
	//Reset the round kills number of enemies for the opposite team
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CCSPlayer *pPlayer = (CCSPlayer*) UTIL_PlayerByIndex( i );
		if(pPlayer)
		{
			if(pPlayer->GetTeamNumber() == pLatePlayer->GetTeamNumber())
			{
				continue;
			}
			pPlayer->m_NumEnemiesAtRoundStart++;
		}
	}
}

//=============================================================================
// HPE_END
//=============================================================================

//=============================================================================
// HPE_BEGIN:
// [pfreese] Test for "pistol" round, defined as the default starting round
// when players cannot purchase anything primary weapons
//=============================================================================

bool CCSGameRules::IsPistolRound()
{
	return m_iTotalRoundsPlayed == 0 && GetStartMoney() <= 800;
}

//=============================================================================
// HPE_END
//=============================================================================

//=============================================================================
// HPE_BEGIN:
// [tj] So game rules can react to damage taken
// [menglish]
//=============================================================================

void CCSGameRules::PlayerTookDamage(CCSPlayer* player, const CTakeDamageInfo &damageInfo)
{
	CBaseEntity *pInflictor = damageInfo.GetInflictor();
	CBaseEntity *pAttacker = damageInfo.GetAttacker();
	CCSPlayer *pCSScorer = (CCSPlayer *)(GetDeathScorer( pAttacker, pInflictor ));

	if ( player && pCSScorer )
	{
		if (player->GetTeamNumber() == TEAM_CT)
		{
			m_bNoCTsDamaged = false;
		}

		if (player->GetTeamNumber() == TEAM_TERRORIST)
		{
			m_bNoTerroristsDamaged = false;
		}
		// set the first blood if this is the first and the victim is on a different team then the player
		if ( m_pFirstBlood == NULL && pCSScorer != player && pCSScorer->GetTeamNumber() != player ->GetTeamNumber() )
		{
			m_pFirstBlood = pCSScorer;
			m_firstBloodTime = gpGlobals->curtime - m_fRoundStartTime;
		}
	}
}


//=============================================================================
// HPE_END
//=============================================================================
#endif

bool CCSGameRules::IsConnectedUserInfoChangeAllowed( CBasePlayer *pPlayer )
{
#ifdef GAME_DLL
	if( pPlayer )
	{
		int iPlayerTeam = pPlayer->GetTeamNumber();
		if( ( iPlayerTeam == TEAM_CT ) || ( iPlayerTeam == TEAM_TERRORIST ) )
			return false;
	}
#else
	int iLocalPlayerTeam = GetLocalPlayerTeam();
	if( ( iLocalPlayerTeam == TEAM_CT ) || ( iLocalPlayerTeam == TEAM_TERRORIST ) )
			return false;
#endif

	return true;
}

#ifdef GAME_DLL

struct convar_tags_t
{
	const char *pszConVar;
	const char *pszTag;
};

// The list of convars that automatically turn on tags when they're changed.
// Convars in this list need to have the FCVAR_NOTIFY flag set on them, so the
// tags are recalculated and uploaded to the master server when the convar is changed.
convar_tags_t convars_to_check_for_tags[] =
{
	{ "mp_friendlyfire", "friendlyfire" },
	{ "bot_quota", "bots" },
	{ "sv_nostats", "nostats" },
	{ "mp_startmoney", "startmoney" },
	{ "sv_allowminmodels", "nominmodels" },
	{ "sv_enablebunnyhopping", "bunnyhopping" },
	{ "sv_competitive_minspec", "compspec" },
	{ "mp_holiday_nogifts", "nogifts" },
};

//-----------------------------------------------------------------------------
// Purpose: Engine asks for the list of convars that should tag the server
//-----------------------------------------------------------------------------
void CCSGameRules::GetTaggedConVarList( KeyValues *pCvarTagList )
{
	BaseClass::GetTaggedConVarList( pCvarTagList );

	for ( int i = 0; i < ARRAYSIZE(convars_to_check_for_tags); i++ )
	{
		KeyValues *pKV = new KeyValues( "tag" );
		pKV->SetString( "convar", convars_to_check_for_tags[i].pszConVar );
		pKV->SetString( "tag", convars_to_check_for_tags[i].pszTag );

		pCvarTagList->AddSubKey( pKV );
	}
}

#endif


int CCSGameRules::GetBlackMarketPriceForWeapon( int iWeaponID )
{
	if ( m_pPrices == NULL )
	{
		GetBlackMarketPriceList();
	}

	if ( m_pPrices )
		return m_pPrices->iCurrentPrice[iWeaponID];
	else
		return 0;
}

int CCSGameRules::GetBlackMarketPreviousPriceForWeapon( int iWeaponID )
{
	if ( m_pPrices == NULL )
	{
		GetBlackMarketPriceList();
	}

	if ( m_pPrices )
		return m_pPrices->iPreviousPrice[iWeaponID];
	else
		return 0;
}

const weeklyprice_t *CCSGameRules::GetBlackMarketPriceList( void )
{
	if ( m_StringTableBlackMarket == NULL )
	{
		m_StringTableBlackMarket = networkstringtable->FindTable( CS_GAMERULES_BLACKMARKET_TABLE_NAME);
	}

	if ( m_pPrices == NULL )
	{
		int iSize = 0;
		INetworkStringTable *pTable = m_StringTableBlackMarket;
		if ( pTable && pTable->GetNumStrings() > 0 )
		{
			m_pPrices = (const weeklyprice_t *)pTable->GetStringUserData( 0, &iSize );
		}
	}

	if ( m_pPrices )
	{
		PrepareEquipmentInfo();
	}
	
	return m_pPrices;
}

void CCSGameRules::SetBlackMarketPrices( bool bSetDefaults )
{
	for ( int i = 1; i < WEAPON_MAX; i++ )
	{
		if ( i == WEAPON_SHIELDGUN )
			continue;

		CCSWeaponInfo *info = GetWeaponInfo( (CSWeaponID)i );

		if ( info == NULL )
			continue;

		if ( bSetDefaults == false )
		{
			info->SetWeaponPrice( GetBlackMarketPriceForWeapon( i ) );
			info->SetPreviousPrice( GetBlackMarketPreviousPriceForWeapon( i ) );
		}
		else
		{
			info->SetWeaponPrice( info->GetDefaultPrice() );
		}
	}
}

#ifdef CLIENT_DLL

CCSGameRules::CCSGameRules()
{
	CSGameRules()->m_StringTableBlackMarket = NULL;
	m_pPrices = NULL;
	m_bBlackMarket = false;
}

void TestTable( void )
{
	CSGameRules()->m_StringTableBlackMarket = networkstringtable->FindTable( CS_GAMERULES_BLACKMARKET_TABLE_NAME);

	if ( CSGameRules()->m_StringTableBlackMarket == NULL )
		return;

	int iIndex = CSGameRules()->m_StringTableBlackMarket->FindStringIndex( "blackmarket_prices" );
	int iSize = 0;

	const weeklyprice_t *pPrices = NULL;
	
	pPrices = (const weeklyprice_t *)(CSGameRules()->m_StringTableBlackMarket)->GetStringUserData( iIndex, &iSize );
}

#ifdef DEBUG
ConCommand cs_testtable( "cs_testtable", TestTable );
#endif

//-----------------------------------------------------------------------------
// Enforce certain values on the specified convar.
//-----------------------------------------------------------------------------
void EnforceCompetitiveCVar( const char *szCvarName, float fMinValue, float fMaxValue = FLT_MAX, int iArgs = 0, ... )
{
	// Doing this check first because OK values might be outside the min/max range
	ConVarRef competitiveConvar(szCvarName);
	float fValue = competitiveConvar.GetFloat();
	va_list vl;
	va_start(vl, iArgs);
	for( int i=0; i< iArgs; ++i )
	{
		if( (int)fValue == (int)va_arg(vl,double) )
			return;
	}
	va_end(vl);

	if( fValue < fMinValue || fValue > fMaxValue )
	{
		float fNewValue = MAX( MIN( fValue, fMaxValue ), fMinValue );
		competitiveConvar.SetValue( fNewValue );
		DevMsg( "Convar %s enforced by server (see sv_competitive_minspec.) Set to %2f.\n", szCvarName, fNewValue );
	}
}

//-----------------------------------------------------------------------------
// An interface used by ENABLE_COMPETITIVE_CONVAR macro that lets the classes
// defined in the macro to be stored and acted on.
//-----------------------------------------------------------------------------
class ICompetitiveConvar
{
public:
	// It is a best practice to always have a virtual destructor in an interface
	// class. Otherwise if the derived classes have destructors they will not be
	// called.
	virtual ~ICompetitiveConvar() {}
	virtual void BackupConvar() = 0;
	virtual void EnforceRestrictions() = 0;
	virtual void RestoreOriginalValue() = 0;
	virtual void InstallChangeCallback() = 0;
};

//-----------------------------------------------------------------------------
// A manager for all enforced competitive convars.
//-----------------------------------------------------------------------------
class CCompetitiveCvarManager : public CAutoGameSystem
{
public:
	typedef CUtlVector<ICompetitiveConvar*> CompetitiveConvarList_t;
	static void AddConvarToList( ICompetitiveConvar* pCVar )
	{
		GetConvarList()->AddToTail( pCVar );
	}

	static void BackupAllConvars()
	{
		FOR_EACH_VEC( *GetConvarList(), i )
		{
			(*GetConvarList())[i]->BackupConvar();
		}
	}

	static void EnforceRestrictionsOnAllConvars()
	{
		FOR_EACH_VEC( *GetConvarList(), i )
		{
			(*GetConvarList())[i]->EnforceRestrictions();
		}
	}

	static void RestoreAllOriginalValues()
	{
		FOR_EACH_VEC( *GetConvarList(), i )
		{
			(*GetConvarList())[i]->RestoreOriginalValue();
		}
	}

	static CompetitiveConvarList_t* GetConvarList()
	{
		if( !s_pCompetitiveConvars )
		{
			s_pCompetitiveConvars = new CompetitiveConvarList_t();
		}
		return s_pCompetitiveConvars;
	}

	static KeyValues* GetConVarBackupKV()
	{
		if( !s_pConVarBackups )
		{
			s_pConVarBackups = new KeyValues("ConVarBackups");
		}
		return s_pConVarBackups;
	}

	virtual bool Init() 
	{ 
		FOR_EACH_VEC( *GetConvarList(), i )
		{
			(*GetConvarList())[i]->InstallChangeCallback();
		}
		return true;
	}

	virtual void Shutdown()
	{
		FOR_EACH_VEC( *GetConvarList(), i )
		{
			delete (*GetConvarList())[i];
		}
		delete s_pCompetitiveConvars; 
		s_pCompetitiveConvars = null;
		s_pConVarBackups->deleteThis(); 
		s_pConVarBackups = null;
	}
private:
	static CompetitiveConvarList_t* s_pCompetitiveConvars;
	static KeyValues* s_pConVarBackups;
};
static CCompetitiveCvarManager *s_pCompetitiveCvarManager = new CCompetitiveCvarManager();
CCompetitiveCvarManager::CompetitiveConvarList_t* CCompetitiveCvarManager::s_pCompetitiveConvars = null;
KeyValues* CCompetitiveCvarManager::s_pConVarBackups = null;

//-----------------------------------------------------------------------------
// Macro to define restrictions on convars with "sv_competitive_minspec 1"
// Usage: ENABLE_COMPETITIVE_CONVAR( convarName, minValue, maxValue, optionalValues, opVal1, opVal2, ...
//-----------------------------------------------------------------------------
#define ENABLE_COMPETITIVE_CONVAR( convarName, ... ) \
class CCompetitiveMinspecConvar##convarName : public ICompetitiveConvar { \
public: \
	CCompetitiveMinspecConvar##convarName(){ CCompetitiveCvarManager::AddConvarToList(this);} \
	static void on_changed_##convarName( IConVar *var, const char *pOldValue, float flOldValue ){ \
		if( sv_competitive_minspec.GetBool() ) { \
			EnforceCompetitiveCVar( #convarName , __VA_ARGS__  ); }\
		else {\
			CCompetitiveCvarManager::GetConVarBackupKV()->SetFloat( #convarName, ConVarRef( #convarName ).GetFloat() ); } } \
	virtual void BackupConvar() { CCompetitiveCvarManager::GetConVarBackupKV()->SetFloat( #convarName, ConVarRef( #convarName ).GetFloat() ); } \
	virtual void EnforceRestrictions() { EnforceCompetitiveCVar( #convarName , __VA_ARGS__  ); } \
	virtual void RestoreOriginalValue() { ConVarRef(#convarName).SetValue(CCompetitiveCvarManager::GetConVarBackupKV()->GetFloat( #convarName ) ); } \
	virtual void InstallChangeCallback() { static_cast<ConVar*>(ConVarRef( #convarName ).GetLinkedConVar())->InstallChangeCallback( CCompetitiveMinspecConvar##convarName::on_changed_##convarName); } \
}; \
static CCompetitiveMinspecConvar##convarName *s_pCompetitiveConvar##convarName = new CCompetitiveMinspecConvar##convarName();

//-----------------------------------------------------------------------------
// Callback function for sv_competitive_minspec convar value change.
//-----------------------------------------------------------------------------
void sv_competitive_minspec_changed_f( IConVar *var, const char *pOldValue, float flOldValue )
{
	ConVar *pCvar = static_cast<ConVar*>(var);

	if( pCvar->GetBool() == true && (bool)flOldValue == false )
	{
		// Backup the values of each cvar and enforce new ones
		CCompetitiveCvarManager::BackupAllConvars();
		CCompetitiveCvarManager::EnforceRestrictionsOnAllConvars();
	}
	else if( pCvar->GetBool() == false && (bool)flOldValue == true )
	{
		// If sv_competitive_minspec is disabled, restore old client values
		CCompetitiveCvarManager::RestoreAllOriginalValues();
	}
}
#endif

static ConVar sv_competitive_minspec( "sv_competitive_minspec",
									 "0",
									 FCVAR_REPLICATED | FCVAR_NOTIFY,
									 "Enable to force certain client convars to minimum/maximum values to help prevent competitive advantages:\n \
	r_drawdetailprops = 1\n \
	r_staticprop_lod = minimum -1 maximum 3\n \
	fps_max minimum 59 (0 works too)\n \
	cl_detailfade minimum 400\n \
	cl_detaildist minimum 1200\n \
	cl_interp_ratio = minimum 1 maximum 2\n \
	cl_interp = minimum 0 maximum 0.031\n \
	"
#ifdef CLIENT_DLL
									 ,sv_competitive_minspec_changed_f
#endif
									 );

#ifdef CLIENT_DLL

ENABLE_COMPETITIVE_CONVAR( r_drawdetailprops, true, true ); // force r_drawdetailprops on
ENABLE_COMPETITIVE_CONVAR( r_staticprop_lod, -1, 3 );		// force r_staticprop_lod from -1 to 3
ENABLE_COMPETITIVE_CONVAR( fps_max, 59, FLT_MAX, 1, 0 );	// force fps_max above 59. One additional value (0) works
ENABLE_COMPETITIVE_CONVAR( cl_detailfade, 400 );			// force cl_detailfade above 400.
ENABLE_COMPETITIVE_CONVAR( cl_detaildist, 1200 );			// force cl_detaildist above 1200.
ENABLE_COMPETITIVE_CONVAR( cl_interp_ratio, 1, 2 );			// force cl_interp_ratio from 1 to 2
ENABLE_COMPETITIVE_CONVAR( cl_interp, 0, 0.031 );		// force cl_interp from 0.0152 to 0.031

// Stubs for replay client code
const char *GetMapDisplayName( const char *pMapName )
{
	return pMapName;
}

bool IsTakingAFreezecamScreenshot()
{
	return false;
}



#endif

IMPLEMENT_NETWORKCLASS_ALIASED(SurvivorPosition, DT_SurvivorPosition)

BEGIN_NETWORK_TABLE(CSurvivorPosition, DT_SurvivorPosition)
#ifdef CLIENT_DLL
RecvPropInt(RECVINFO(m_order))
#else
SendPropInt(SENDINFO(m_order))
#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
BEGIN_DATADESC(CSurvivorPosition)
DEFINE_INPUTFUNC(FIELD_STRING, "SetViewControl", InputSetViewControl),

DEFINE_KEYFIELD(m_iszSurvivorName, FIELD_STRING, "SurvivorName"),
DEFINE_KEYFIELD(m_order, FIELD_INTEGER, "Order")
END_DATADESC()

LINK_ENTITY_TO_CLASS(info_survivor_position, CSurvivorPosition)
#endif

void CSurvivorPosition::Spawn(void)
{
	BaseClass::Spawn();
}

#ifdef GAME_DLL
void CSurvivorPosition::Precache(void)
{

}

bool CSurvivorPosition::MatchesPlayer(CCSPlayer* pPlayer)
{
	if (!pPlayer)
		return false;

	if (m_iszSurvivorName == NULL_STRING)
		return false;

	return CSGameRules()->SurvivorNameMatches(
		pPlayer,
		STRING(m_iszSurvivorName)
	);
}

void CSurvivorPosition::InputSetViewControl(inputdata_t& inputdata)
{
	AssertMsg1(m_hPlayer, "Survivor position %s has no player", STRING(m_iszSurvivorName));
	//AssertMsg( m_hPlayer != UTIL_GetListenServerHost(), "Try controlling the player" );
	if (m_hPlayer)
	{
		const char* name = inputdata.value.String();
		CBaseEntity* pViewEntity = gEntList.FindEntityByName(NULL, name);

		//AssertMsg( m_hPlayer != UTIL_GetListenServerHost(), "Found player" );
		if (pViewEntity)
		{
			variant_t dummy;
			pViewEntity->AcceptInput("Enable", m_hPlayer, NULL, dummy, 0);
			//AssertMsg( m_hPlayer != UTIL_GetListenServerHost(), "Controlling the player" );
		}
	}
}

int CSurvivorPosition::UpdateTransmitState(void)
{
	return CBaseEntity::SetTransmitState(FL_EDICT_ALWAYS);
}

#endif
#include "debugoverlay_shared.h"
#ifdef CLIENT_DLL
void CSurvivorPosition::ClientThink(void)
{
	// Some debugoverlay code would be here, but that shouldn't matter much

	Vector origin = WorldSpaceCenter(); // WorldSpaceCenter may not be correct
	QAngle angles = GetRenderAngles(); // May not be accurate either

	Vector mins(-10, -10, -10);
	Vector maxs(10, 10, 10);
	debugoverlay->AddBoxOverlay(origin, mins, maxs, angles, 255, 50, 50, 255, 0.01);
}
#endif