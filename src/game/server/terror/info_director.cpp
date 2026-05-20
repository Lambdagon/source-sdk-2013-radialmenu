//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Minimal Left 4 Dead-style director relay entity.
//
//=============================================================================//

#include "cbase.h"
#include "info_director.h"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CInfoDirector* g_pDirector = NULL;

LINK_ENTITY_TO_CLASS( info_director, CInfoDirector );

BEGIN_DATADESC( CInfoDirector )

	DEFINE_FIELD( m_hPendingActivator, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bRevealActivator, FIELD_BOOLEAN ),

	DEFINE_INPUTFUNC( FIELD_VOID, "PanicEvent", InputPanicEvent ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ForcePanicEvent", InputForcePanicEvent ),

	DEFINE_THINKFUNC( PanicEventThink ),

END_DATADESC()

void CInfoDirector::Spawn( void )
{
	Precache();

	SetMoveType( MOVETYPE_NONE );
	SetSolid( SOLID_NONE );
	AddEffects( EF_NODRAW );

	m_hPendingActivator = NULL;
	m_bRevealActivator = false;

	SetThink( NULL );
	SetNextThink( TICK_NEVER_THINK );
}

void CInfoDirector::Precache( void )
{
	PrecacheScriptSound( "MegaMobIncoming" );
}

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
