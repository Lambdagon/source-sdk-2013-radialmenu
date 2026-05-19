#pragma once
#include "shaderlib/cshader.h"
class lux_sprite_vs30_Static_Index
{
	unsigned int m_nVERTEXRGBA : 2;
	unsigned int m_nTRANSFORM : 2;
#ifdef _DEBUG
	bool m_bVERTEXRGBA : 1;
	bool m_bTRANSFORM : 1;
#endif	// _DEBUG
public:
	void SetVERTEXRGBA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXRGBA = i;
#ifdef _DEBUG
		m_bVERTEXRGBA = true;
#endif	// _DEBUG
	}

	void SetTRANSFORM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRANSFORM = i;
#ifdef _DEBUG
		m_bTRANSFORM = true;
#endif	// _DEBUG
	}

	lux_sprite_vs30_Static_Index(  )
	{
		m_nVERTEXRGBA = 0;
		m_nTRANSFORM = 0;
#ifdef _DEBUG
		m_bVERTEXRGBA = false;
		m_bTRANSFORM = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bVERTEXRGBA && m_bTRANSFORM );
		return ( 1 * m_nVERTEXRGBA ) + ( 2 * m_nTRANSFORM ) + 0;
	}
};

#define shaderStaticTest_lux_sprite_vs30 vsh_forgot_to_set_static_VERTEXRGBA + vsh_forgot_to_set_static_TRANSFORM


class lux_sprite_vs30_Dynamic_Index
{
public:
	lux_sprite_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_sprite_vs30 1

