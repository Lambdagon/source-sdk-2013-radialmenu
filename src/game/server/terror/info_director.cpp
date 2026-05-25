//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Minimal Left 4 Dead-style director relay entity.
//
//=============================================================================//

#include "cbase.h"
#include "info_director.h"
#include "cs_player.h"
#include "cs_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CInfoDirector* g_pDirector = NULL;

LINK_ENTITY_TO_CLASS( info_director, CInfoDirector );

// ======================
// CInfoDirector
// ======================

BEGIN_DATADESC( CInfoDirector )
    DEFINE_FIELD( m_hPendingActivator, FIELD_EHANDLE ),
    DEFINE_FIELD( m_bRevealActivator,  FIELD_BOOLEAN ),
    DEFINE_FIELD( m_bLockPositions,    FIELD_BOOLEAN ),
    DEFINE_FIELD( m_iRetryCount,       FIELD_INTEGER ),

    DEFINE_INPUTFUNC( FIELD_VOID, "PanicEvent",               InputPanicEvent ),
    DEFINE_INPUTFUNC( FIELD_VOID, "ForcePanicEvent",          InputForcePanicEvent ),
    DEFINE_INPUTFUNC( FIELD_VOID, "ForceSurvivorPositions",   InputForceSurvivorPositions ),
    DEFINE_INPUTFUNC( FIELD_VOID, "ReleaseSurvivorPositions", InputReleaseSurvivorPositions ),

    DEFINE_INPUTFUNC(FIELD_VOID, "EndCustomScriptedStage", InputEndCustomScriptedStage),
    DEFINE_INPUTFUNC(FIELD_STRING, "ScriptedPanicEvent", InputScriptedPanicEvent),
    DEFINE_INPUTFUNC(FIELD_STRING, "FireConceptToAny", InputFireConceptToAny),
    DEFINE_INPUTFUNC(FIELD_INTEGER, "IncrementTeamScore", InputIncrementTeamScore),
    DEFINE_INPUTFUNC(FIELD_VOID, "StartIntro", InputStartIntro),
    DEFINE_INPUTFUNC(FIELD_VOID, "FinishIntro", InputFinishIntro),
    DEFINE_INPUTFUNC(FIELD_STRING, "BeginScript", InputBeginScript),
    DEFINE_INPUTFUNC(FIELD_VOID, "EndScript", InputEndScript),
    DEFINE_INPUTFUNC(FIELD_STRING, "CreateNewJournal", InputCreateNewJournal),
    DEFINE_INPUTFUNC(FIELD_STRING, "WriteToJournal", InputWriteToJournal),
    DEFINE_INPUTFUNC(FIELD_STRING, "ExecuteJournal", InputExecuteJournal),
    DEFINE_INPUTFUNC(FIELD_VOID, "EnableTankFrustration", InputEnableTankFrustration),
    DEFINE_INPUTFUNC(FIELD_VOID, "DisableTankFrustration", InputDisableTankFrustration),

    DEFINE_OUTPUT(m_OnGameplayStart, "OnGameplayStart"),
    DEFINE_OUTPUT(m_OnCustomPanicStageFinished, "OnCustomPanicStageFinished"),
    DEFINE_OUTPUT(m_OnPanicEventFinished, "OnPanicEventFinished"),
    DEFINE_OUTPUT(m_OnTeamScored, "OnTeamScored"),
    DEFINE_OUTPUT(m_OnScavengeRoundStart, "OnScavengeRoundStart"),
    DEFINE_OUTPUT(m_OnScavengeOvertimeStart, "OnScavengeOvertimeStart"),
    DEFINE_OUTPUT(m_OnScavengeOvertimeCancel, "OnScavengeOvertimeCancel"),
    DEFINE_OUTPUT(m_OnScavengeTimerExpired, "OnScavengeTimerExpired"),
    DEFINE_OUTPUT(m_OnScavengeIntensityChanged, "OnScavengeIntensityChanged"),
    DEFINE_OUTPUT(m_OnUserDefinedScriptEvent1, "OnUserDefinedScriptEvent1"),
    DEFINE_OUTPUT(m_OnUserDefinedScriptEvent2, "OnUserDefinedScriptEvent2"),
    DEFINE_OUTPUT(m_OnUserDefinedScriptEvent3, "OnUserDefinedScriptEvent3"),
    DEFINE_OUTPUT(m_OnUserDefinedScriptEvent4, "OnUserDefinedScriptEvent4"),

    DEFINE_THINKFUNC( PanicEventThink ),
    DEFINE_THINKFUNC( SurvivorPositionRetryThink ),
END_DATADESC()

void CInfoDirector::Spawn( void )
{
    Precache();

    SetMoveType( MOVETYPE_NONE );
    SetSolid( SOLID_NONE );
    AddEffects( EF_NODRAW );

    m_hPendingActivator = NULL;
    m_bRevealActivator = false;
    m_bLockPositions = false;
    m_iRetryCount = 0;

    SetThink( NULL );
    SetNextThink( TICK_NEVER_THINK );
}

void CInfoDirector::Precache( void )
{
    PrecacheScriptSound( "MegaMobIncoming" );
}

// ----------------------------------------------------------------------
// Panic Events 
// ----------------------------------------------------------------------
void CInfoDirector::InputPanicEvent( inputdata_t &inputdata )
{
    QueuePanicEvent( inputdata.pActivator, true );
}

void CInfoDirector::InputForcePanicEvent( inputdata_t &inputdata )
{
    QueuePanicEvent( inputdata.pActivator, false );
}

void CInfoDirector::QueuePanicEvent( CBaseEntity *pActivator, bool bRevealActivator )
{
    m_hPendingActivator = pActivator;
    m_bRevealActivator = bRevealActivator;

    SetThink( &CInfoDirector::PanicEventThink );
    SetNextThink( gpGlobals->curtime + INFO_DIRECTOR_PANIC_EVENT_DELAY );
}

void CInfoDirector::PanicEventThink( void )
{
    CCSGameRules *pRules = CSGameRules();
    if ( pRules )
    {
        pRules->StartScriptedPanicEvent( ToCSPlayer( m_hPendingActivator.Get() ), m_bRevealActivator );
    }

    m_hPendingActivator = NULL;
    m_bRevealActivator = false;
    SetThink( NULL );
    SetNextThink( TICK_NEVER_THINK );
}

// ----------------------------------------------------------------------
// Survivor Position System
// ----------------------------------------------------------------------
void CInfoDirector::InputForceSurvivorPositions( inputdata_t &inputdata )
{
    StartScriptedSurvivorPositions( true );
}

void CInfoDirector::InputReleaseSurvivorPositions( inputdata_t &inputdata )
{
    StartScriptedSurvivorPositions( false );
}

void CInfoDirector::StartScriptedSurvivorPositions( bool bLock )
{
    if ( CountSurvivorsAlive() == 0 )
    {
        m_bLockPositions = bLock;
        m_iRetryCount = 0;

        SetThink( &CInfoDirector::SurvivorPositionRetryThink );
        SetNextThink( gpGlobals->curtime + 0.2f );
        return;
    }

    DoSurvivorPositionTeleport( bLock );
}

void CInfoDirector::SurvivorPositionRetryThink( void )
{
    m_iRetryCount++;

    if ( CountSurvivorsAlive() > 0 || m_iRetryCount >= 20 ) 
    {
        DoSurvivorPositionTeleport( m_bLockPositions );
        SetThink( NULL );
        SetNextThink( TICK_NEVER_THINK );
    }
    else
    {
        SetNextThink( gpGlobals->curtime + 0.2f );
    }
}

int CInfoDirector::CountSurvivorsAlive()
{
    int count = 0;
    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CCSPlayer *pPlayer = ToCSPlayer( UTIL_PlayerByIndex(i) );
        if ( pPlayer && pPlayer->IsAlive() && pPlayer->GetTeamNumber() == TEAM_SURVIVOR )
            count++;
    }
    return count;
}

void CInfoDirector::DoSurvivorPositionTeleport(bool bLock)
{
    CBaseEntity* pEntity = NULL;

    while ((pEntity = gEntList.FindEntityByClassname(
        pEntity,
        "info_survivor_position")) != NULL)
    {
        CSurvivorPosition* pPos =
            dynamic_cast<CSurvivorPosition*>(pEntity);

        if (!pPos)
            continue;

        for (int i = 1; i <= gpGlobals->maxClients; i++)
        {
            CCSPlayer* pPlayer =
                ToCSPlayer(UTIL_PlayerByIndex(i));

            if (!pPlayer)
                continue;

            if (!pPlayer->IsAlive())
                continue;

            if (pPlayer->GetTeamNumber() != TEAM_SURVIVOR)
                continue;

            if (pPos->MatchesPlayer(pPlayer))
            {
                TeleportSurvivorToPosition(
                    pPlayer,
                    pPos,
                    bLock
                );

                pPos->m_hPlayer = pPlayer;

                break;
            }
        }
    }
}

void CInfoDirector::TeleportSurvivorToPosition(
    CCSPlayer *pPlayer,
    CSurvivorPosition *pPos,
    bool bLock )
{
    if ( !pPlayer || !pPos ) return;

    pPlayer->Teleport( &pPos->GetAbsOrigin(), &pPos->GetAbsAngles(), &vec3_origin );

    if ( bLock )
    {
        pPlayer->AddFlag( FL_FROZEN | FL_GODMODE );
    }
    else
    {
        pPlayer->RemoveFlag( FL_FROZEN | FL_GODMODE );
    }

}

void CInfoDirector::InputEndCustomScriptedStage(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputScriptedPanicEvent(inputdata_t& inputdata)
{
	if (CSGameRules())
    {
        CSGameRules()->StartScriptedPanicEvent( ToCSPlayer( inputdata.pActivator ), false);
	}
}

void CInfoDirector::InputFireConceptToAny(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputIncrementTeamScore(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputStartIntro(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputFinishIntro(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputBeginScript(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputEndScript(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputCreateNewJournal(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputWriteToJournal(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputExecuteJournal(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputEnableTankFrustration(inputdata_t& inputdata)
{
    // TODO: Implement 
}

void CInfoDirector::InputDisableTankFrustration(inputdata_t& inputdata)
{
    // TODO: Implement 
}

