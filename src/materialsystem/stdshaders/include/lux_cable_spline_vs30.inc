#pragma once
#include "shaderlib/cshader.h"
class lux_cable_spline_vs30_Static_Index
{
	unsigned int m_nDEPTH_WRITE : 2;
#ifdef _DEBUG
	bool m_bDEPTH_WRITE : 1;
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

	lux_cable_spline_vs30_Static_Index(  )
	{
		m_nDEPTH_WRITE = 0;
#ifdef _DEBUG
		m_bDEPTH_WRITE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bDEPTH_WRITE );
		return ( 1 * m_nDEPTH_WRITE ) + 0;
	}
};

#define shaderStaticTest_lux_cable_spline_vs30 vsh_forgot_to_set_static_DEPTH_WRITE


class lux_cable_spline_vs30_Dynamic_Index
{
public:
	lux_cable_spline_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_cable_spline_vs30 1

