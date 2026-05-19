#pragma once
#include "shaderlib/cshader.h"
class lux_combinetextures_ps30_Static_Index
{
	unsigned int m_nTEXTURES : 4;
#ifdef _DEBUG
	bool m_bTEXTURES : 1;
#endif	// _DEBUG
public:
	void SetTEXTURES( int i )
	{
		Assert( i >= 0 && i <= 14 );
		m_nTEXTURES = i;
#ifdef _DEBUG
		m_bTEXTURES = true;
#endif	// _DEBUG
	}

	lux_combinetextures_ps30_Static_Index(  )
	{
		m_nTEXTURES = 0;
#ifdef _DEBUG
		m_bTEXTURES = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXTURES );
		return ( 1 * m_nTEXTURES ) + 0;
	}
};

#define shaderStaticTest_lux_combinetextures_ps30 psh_forgot_to_set_static_TEXTURES


class lux_combinetextures_ps30_Dynamic_Index
{
public:
	lux_combinetextures_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_combinetextures_ps30 1

