#pragma once
#include "shaderlib/cshader.h"
class lux_shatteredglass_vs30_Static_Index
{
	unsigned int m_nTEXCOORDS : 2;
	unsigned int m_nENVMAP : 2;
#ifdef _DEBUG
	bool m_bTEXCOORDS : 1;
	bool m_bENVMAP : 1;
#endif	// _DEBUG
public:
	void SetTEXCOORDS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nTEXCOORDS = i;
#ifdef _DEBUG
		m_bTEXCOORDS = true;
#endif	// _DEBUG
	}

	void SetENVMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAP = i;
#ifdef _DEBUG
		m_bENVMAP = true;
#endif	// _DEBUG
	}

	lux_shatteredglass_vs30_Static_Index(  )
	{
		m_nTEXCOORDS = 0;
		m_nENVMAP = 0;
#ifdef _DEBUG
		m_bTEXCOORDS = false;
		m_bENVMAP = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXCOORDS && m_bENVMAP );
		return ( 1 * m_nTEXCOORDS ) + ( 3 * m_nENVMAP ) + 0;
	}
};

#define shaderStaticTest_lux_shatteredglass_vs30 vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_ENVMAP


class lux_shatteredglass_vs30_Dynamic_Index
{
public:
	lux_shatteredglass_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_shatteredglass_vs30 1

