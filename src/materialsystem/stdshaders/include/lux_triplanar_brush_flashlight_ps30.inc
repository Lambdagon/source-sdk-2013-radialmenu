// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($MODE_BUMP == 0 && $SSBUMP != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_triplanar_brush_flashlight_ps30_Static_Index
{
	unsigned int m_nTRIPLANAR_BASE : 2;
	unsigned int m_nMODE_BUMP : 2;
	unsigned int m_nSSBUMP : 2;
	unsigned int m_nMODE_DETAIL : 2;
#ifdef _DEBUG
	bool m_bTRIPLANAR_BASE : 1;
	bool m_bMODE_BUMP : 1;
	bool m_bSSBUMP : 1;
	bool m_bMODE_DETAIL : 1;
#endif	// _DEBUG
public:
	void SetTRIPLANAR_BASE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRIPLANAR_BASE = i;
#ifdef _DEBUG
		m_bTRIPLANAR_BASE = true;
#endif	// _DEBUG
	}

	void SetMODE_BUMP( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nMODE_BUMP = i;
#ifdef _DEBUG
		m_bMODE_BUMP = true;
#endif	// _DEBUG
	}

	void SetSSBUMP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSSBUMP = i;
#ifdef _DEBUG
		m_bSSBUMP = true;
#endif	// _DEBUG
	}

	void SetMODE_DETAIL( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nMODE_DETAIL = i;
#ifdef _DEBUG
		m_bMODE_DETAIL = true;
#endif	// _DEBUG
	}

	lux_triplanar_brush_flashlight_ps30_Static_Index(  )
	{
		m_nTRIPLANAR_BASE = 0;
		m_nMODE_BUMP = 0;
		m_nSSBUMP = 0;
		m_nMODE_DETAIL = 0;
#ifdef _DEBUG
		m_bTRIPLANAR_BASE = false;
		m_bMODE_BUMP = false;
		m_bSSBUMP = false;
		m_bMODE_DETAIL = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTRIPLANAR_BASE && m_bMODE_BUMP && m_bSSBUMP && m_bMODE_DETAIL );
		AssertMsg( !( ( m_nMODE_BUMP == 0 ) && ( m_nSSBUMP != 0 ) ), "Invalid combo combination ( ( MODE_BUMP == 0 ) && ( SSBUMP != 0 ) )" );
		return ( 2 * m_nTRIPLANAR_BASE ) + ( 4 * m_nMODE_BUMP ) + ( 12 * m_nSSBUMP ) + ( 24 * m_nMODE_DETAIL ) + 0;
	}
};

#define shaderStaticTest_lux_triplanar_brush_flashlight_ps30 psh_forgot_to_set_static_TRIPLANAR_BASE + psh_forgot_to_set_static_MODE_BUMP + psh_forgot_to_set_static_SSBUMP + psh_forgot_to_set_static_MODE_DETAIL


class lux_triplanar_brush_flashlight_ps30_Dynamic_Index
{
	unsigned int m_nPROJTEXSHADOWS : 2;
#ifdef _DEBUG
	bool m_bPROJTEXSHADOWS : 1;
#endif	// _DEBUG
public:
	void SetPROJTEXSHADOWS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEXSHADOWS = i;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = true;
#endif	// _DEBUG
	}

	lux_triplanar_brush_flashlight_ps30_Dynamic_Index(  )
	{
		m_nPROJTEXSHADOWS = 0;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEXSHADOWS );
		return ( 1 * m_nPROJTEXSHADOWS ) + 0;
	}
};

#define shaderDynamicTest_lux_triplanar_brush_flashlight_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS

