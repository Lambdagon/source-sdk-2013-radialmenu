#pragma once
#include "shaderlib/cshader.h"
class lux_sky_rgbs_ps30_Static_Index
{
public:
	lux_sky_rgbs_ps30_Static_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_lux_sky_rgbs_ps30 1


class lux_sky_rgbs_ps30_Dynamic_Index
{
	unsigned int m_nFILTER : 2;
#ifdef _DEBUG
	bool m_bFILTER : 1;
#endif	// _DEBUG
public:
	void SetFILTER( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nFILTER = i;
#ifdef _DEBUG
		m_bFILTER = true;
#endif	// _DEBUG
	}

	lux_sky_rgbs_ps30_Dynamic_Index(  )
	{
		m_nFILTER = 0;
#ifdef _DEBUG
		m_bFILTER = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFILTER );
		return ( 1 * m_nFILTER ) + 0;
	}
};

#define shaderDynamicTest_lux_sky_rgbs_ps30 psh_forgot_to_set_dynamic_FILTER

