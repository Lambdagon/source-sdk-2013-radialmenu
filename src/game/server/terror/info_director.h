//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Minimal Left 4 Dead-style director relay entity.
//
//=============================================================================//

#include "cbase.h"
#include "cs_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

namespace
{
	const float INFO_DIRECTOR_PANIC_EVENT_DELAY = 2.5f;
}

class CInfoDirector : public CPointEntity
{
public:
	DECLARE_CLASS( CInfoDirector, CPointEntity );
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );

	void InputPanicEvent( inputdata_t &inputdata );
	void InputForcePanicEvent( inputdata_t &inputdata );
	void InputForceSurvivorPositions(inputdata_t& inputdata);
	void InputReleaseSurvivorPositions(inputdata_t& inputdata);

	void StartScriptedSurvivorPositions(bool bLock);

	void InputEndCustomScriptedStage(inputdata_t& inputdata);

	void InputScriptedPanicEvent(inputdata_t& inputdata);

	void InputFireConceptToAny(inputdata_t& inputdata);

	void InputIncrementTeamScore(inputdata_t& inputdata);

	void InputStartIntro(inputdata_t& inputdata);

	void InputFinishIntro(inputdata_t& inputdata);

	void InputBeginScript(inputdata_t& inputdata);

	void InputEndScript(inputdata_t& inputdata);

	void InputCreateNewJournal(inputdata_t& inputdata);

	void InputWriteToJournal(inputdata_t& inputdata);

	void InputExecuteJournal(inputdata_t& inputdata);

	void InputEnableTankFrustration(inputdata_t& inputdata);

	void InputDisableTankFrustration(inputdata_t& inputdata);

	void DoSurvivorPositionTeleport(bool bLock);
	void TeleportSurvivorToPosition(CCSPlayer* pPlayer, CSurvivorPosition* pPos, bool bLock);
	int  CountSurvivorsAlive();
	void SurvivorPositionRetryThink(void);

	bool m_bLockPositions;
	int  m_iRetryCount;

	COutputEvent m_OnGameplayStart;
	COutputEvent m_OnCustomPanicStageFinished;
	COutputEvent m_OnPanicEventFinished;
	COutputEvent m_OnTeamScored;
	COutputEvent m_OnScavengeRoundStart;
	COutputEvent m_OnScavengeOvertimeStart;
	COutputEvent m_OnScavengeOvertimeCancel;
	COutputEvent m_OnScavengeTimerExpired;
	COutputEvent m_OnScavengeIntensityChanged;
	COutputEvent m_OnUserDefinedScriptEvent1;
	COutputEvent m_OnUserDefinedScriptEvent2;
	COutputEvent m_OnUserDefinedScriptEvent3;
	COutputEvent m_OnUserDefinedScriptEvent4;

private:
	void QueuePanicEvent( CBaseEntity *pActivator, bool bRevealActivator );
	void PanicEventThink( void );

	EHANDLE m_hPendingActivator;
	bool m_bRevealActivator;
};

extern CInfoDirector* g_pDirector;