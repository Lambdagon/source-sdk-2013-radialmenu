// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($TRIPLANAR_BASE == 0 && $MODE_BUMP < 2 && $MODE_DETAIL < 2 && $TRIPLANAR_ENVMAPMASK == 0)
// ($ENVMAPMODE != 2 && $TRIPLANAR_ENVMAPMASK != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_triplanar_model_ps30_Static_Index
{
	unsigned int m_nTRIPLANAR_BASE : 2;
	unsigned int m_nMODE_BUMP : 2;
	unsigned int m_nMODE_DETAIL : 2;
	unsigned int m_nENVMAPMODE : 2;
	unsigned int m_nTRIPLANAR_ENVMAPMASK : 2;
#ifdef _DEBUG
	bool m_bTRIPLANAR_BASE : 1;
	bool m_bMODE_BUMP : 1;
	bool m_bMODE_DETAIL : 1;
	bool m_bENVMAPMODE : 1;
	bool m_bTRIPLANAR_ENVMAPMASK : 1;
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

	void SetMODE_DETAIL( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nMODE_DETAIL = i;
#ifdef _DEBUG
		m_bMODE_DETAIL = true;
#endif	// _DEBUG
	}

	void SetENVMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nENVMAPMODE = i;
#ifdef _DEBUG
		m_bENVMAPMODE = true;
#endif	// _DEBUG
	}

	void SetTRIPLANAR_ENVMAPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRIPLANAR_ENVMAPMASK = i;
#ifdef _DEBUG
		m_bTRIPLANAR_ENVMAPMASK = true;
#endif	// _DEBUG
	}

	lux_triplanar_model_ps30_Static_Index(  )
	{
		m_nTRIPLANAR_BASE = 0;
		m_nMODE_BUMP = 0;
		m_nMODE_DETAIL = 0;
		m_nENVMAPMODE = 0;
		m_nTRIPLANAR_ENVMAPMASK = 0;
#ifdef _DEBUG
		m_bTRIPLANAR_BASE = false;
		m_bMODE_BUMP = false;
		m_bMODE_DETAIL = false;
		m_bENVMAPMODE = false;
		m_bTRIPLANAR_ENVMAPMASK = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTRIPLANAR_BASE && m_bMODE_BUMP && m_bMODE_DETAIL && m_bENVMAPMODE && m_bTRIPLANAR_ENVMAPMASK );
		AssertMsg( !( ( m_nTRIPLANAR_BASE == 0 ) && ( ( m_nMODE_BUMP < 2 ) && ( ( m_nMODE_DETAIL < 2 ) && ( m_nTRIPLANAR_ENVMAPMASK == 0 ) ) ) ), "Invalid combo combination ( ( TRIPLANAR_BASE == 0 ) && ( ( MODE_BUMP < 2 ) && ( ( MODE_DETAIL < 2 ) && ( TRIPLANAR_ENVMAPMASK == 0 ) ) ) )" );
		AssertMsg( !( ( m_nENVMAPMODE != 2 ) && ( m_nTRIPLANAR_ENVMAPMASK != 0 ) ), "Invalid combo combination ( ( ENVMAPMODE != 2 ) && ( TRIPLANAR_ENVMAPMASK != 0 ) )" );
		return ( 5 * m_nTRIPLANAR_BASE ) + ( 10 * m_nMODE_BUMP ) + ( 30 * m_nMODE_DETAIL ) + ( 90 * m_nENVMAPMODE ) + ( 270 * m_nTRIPLANAR_ENVMAPMASK ) + 0;
	}
};

#define shaderStaticTest_lux_triplanar_model_ps30 psh_forgot_to_set_static_TRIPLANAR_BASE + psh_forgot_to_set_static_MODE_BUMP + psh_forgot_to_set_static_MODE_DETAIL + psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_TRIPLANAR_ENVMAPMASK


class lux_triplanar_model_ps30_Dynamic_Index
{
	unsigned int m_nNUM_LIGHTS_COMBO : 3;
#ifdef _DEBUG
	bool m_bNUM_LIGHTS_COMBO : 1;
#endif	// _DEBUG
public:
	void SetNUM_LIGHTS_COMBO( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nNUM_LIGHTS_COMBO = i;
#ifdef _DEBUG
		m_bNUM_LIGHTS_COMBO = true;
#endif	// _DEBUG
	}

	lux_triplanar_model_ps30_Dynamic_Index(  )
	{
		m_nNUM_LIGHTS_COMBO = 0;
#ifdef _DEBUG
		m_bNUM_LIGHTS_COMBO = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNUM_LIGHTS_COMBO );
		return ( 1 * m_nNUM_LIGHTS_COMBO ) + 0;
	}
};

#define shaderDynamicTest_lux_triplanar_model_ps30 psh_forgot_to_set_dynamic_NUM_LIGHTS_COMBO

