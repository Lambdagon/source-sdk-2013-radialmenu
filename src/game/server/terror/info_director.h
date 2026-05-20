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

private:
	void QueuePanicEvent( CBaseEntity *pActivator, bool bRevealActivator );
	void PanicEventThink( void );

	EHANDLE m_hPendingActivator;
	bool m_bRevealActivator;
};

extern CInfoDirector* g_pDirector;