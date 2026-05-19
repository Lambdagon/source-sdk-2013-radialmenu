// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($DEPTH_WRITE == 1 && $IGNOREVERTEXALPHA != 0)
// ($DEPTH_WRITE == 1 && $IGNORELIGHTDIR != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_cable_spline_ps30_Static_Index
{
	unsigned int m_nDEPTH_WRITE : 2;
	unsigned int m_nIGNOREVERTEXALPHA : 2;
	unsigned int m_nIGNORELIGHTDIR : 2;
#ifdef _DEBUG
	bool m_bDEPTH_WRITE : 1;
	bool m_bIGNOREVERTEXALPHA : 1;
	bool m_bIGNORELIGHTDIR : 1;
#endif	// _DEBUG
public:
	void SetDEPTH_WRITE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDEPTH_WRITE = i;
#ifdef _DEBUG
		m_bDEPTH_WRITE = true;
#endif	// _DEBUG
	}

	void SetIGNOREVERTEXALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nIGNOREVERTEXALPHA = i;
#ifdef _DEBUG
		m_bIGNOREVERTEXALPHA = true;
#endif	// _DEBUG
	}

	void SetIGNORELIGHTDIR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nIGNORELIGHTDIR = i;
#ifdef _DEBUG
		m_bIGNORELIGHTDIR = true;
#endif	// _DEBUG
	}

	lux_cable_spline_ps30_Static_Index(  )
	{
		m_nDEPTH_WRITE = 0;
		m_nIGNOREVERTEXALPHA = 0;
		m_nIGNORELIGHTDIR = 0;
#ifdef _DEBUG
		m_bDEPTH_WRITE = false;
		m_bIGNOREVERTEXALPHA = false;
		m_bIGNORELIGHTDIR = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bDEPTH_WRITE && m_bIGNOREVERTEXALPHA && m_bIGNORELIGHTDIR );
		AssertMsg( !( ( m_nDEPTH_WRITE == 1 ) && ( m_nIGNOREVERTEXALPHA != 0 ) ), "Invalid combo combination ( ( DEPTH_WRITE == 1 ) && ( IGNOREVERTEXALPHA != 0 ) )" );
		AssertMsg( !( ( m_nDEPTH_WRITE == 1 ) && ( m_nIGNORELIGHTDIR != 0 ) ), "Invalid combo combination ( ( DEPTH_WRITE == 1 ) && ( IGNORELIGHTDIR != 0 ) )" );
		return ( 1 * m_nDEPTH_WRITE ) + ( 2 * m_nIGNOREVERTEXALPHA ) + ( 4 * m_nIGNORELIGHTDIR ) + 0;
	}
};

#define shaderStaticTest_lux_cable_spline_ps30 psh_forgot_to_set_static_DEPTH_WRITE + psh_forgot_to_set_static_IGNOREVERTEXALPHA + psh_forgot_to_set_static_IGNORELIGHTDIR


class lux_cable_spline_ps30_Dynamic_Index
{
public:
	lux_cable_spline_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_cable_spline_ps30 1

