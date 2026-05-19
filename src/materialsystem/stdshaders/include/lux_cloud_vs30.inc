#pragma once
#include "shaderlib/cshader.h"
class lux_cloud_vs30_Static_Index
{
	unsigned int m_nALPHATEX_USE_TEX0 : 2;
#ifdef _DEBUG
	bool m_bALPHATEX_USE_TEX0 : 1;
#endif	// _DEBUG
public:
	void SetALPHATEX_USE_TEX0( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nALPHATEX_USE_TEX0 = i;
#ifdef _DEBUG
		m_bALPHATEX_USE_TEX0 = true;
#endif	// _DEBUG
	}

	lux_cloud_vs30_Static_Index(  )
	{
		m_nALPHATEX_USE_TEX0 = 0;
#ifdef _DEBUG
		m_bALPHATEX_USE_TEX0 = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bALPHATEX_USE_TEX0 );
		return ( 1 * m_nALPHATEX_USE_TEX0 ) + 0;
	}
};

#define shaderStaticTest_lux_cloud_vs30 vsh_forgot_to_set_static_ALPHATEX_USE_TEX0


class lux_cloud_vs30_Dynamic_Index
{
public:
	lux_cloud_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_cloud_vs30 1

