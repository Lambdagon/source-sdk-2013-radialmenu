#pragma once
#include "shaderlib/cshader.h"
class lux_blurfilters_ps30_Static_Index
{
	unsigned int m_nSRGB_ADAPTER : 2;
#ifdef _DEBUG
	bool m_bSRGB_ADAPTER : 1;
#endif	// _DEBUG
public:
	void SetSRGB_ADAPTER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSRGB_ADAPTER = i;
#ifdef _DEBUG
		m_bSRGB_ADAPTER = true;
#endif	// _DEBUG
	}

	lux_blurfilters_ps30_Static_Index(  )
	{
		m_nSRGB_ADAPTER = 0;
#ifdef _DEBUG
		m_bSRGB_ADAPTER = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSRGB_ADAPTER );
		return ( 1 * m_nSRGB_ADAPTER ) + 0;
	}
};

#define shaderStaticTest_lux_blurfilters_ps30 psh_forgot_to_set_static_SRGB_ADAPTER


class lux_blurfilters_ps30_Dynamic_Index
{
public:
	lux_blurfilters_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_blurfilters_ps30 1

