#pragma once
#include "shaderlib/cshader.h"
class lux_sprite_ps30_Static_Index
{
	unsigned int m_nVERTEXRGBA : 2;
	unsigned int m_nCONSTANTCOLOR : 2;
	unsigned int m_nSRGB : 2;
#ifdef _DEBUG
	bool m_bVERTEXRGBA : 1;
	bool m_bCONSTANTCOLOR : 1;
	bool m_bSRGB : 1;
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

	void SetCONSTANTCOLOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCONSTANTCOLOR = i;
#ifdef _DEBUG
		m_bCONSTANTCOLOR = true;
#endif	// _DEBUG
	}

	void SetSRGB( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSRGB = i;
#ifdef _DEBUG
		m_bSRGB = true;
#endif	// _DEBUG
	}

	lux_sprite_ps30_Static_Index(  )
	{
		m_nVERTEXRGBA = 0;
		m_nCONSTANTCOLOR = 0;
		m_nSRGB = 0;
#ifdef _DEBUG
		m_bVERTEXRGBA = false;
		m_bCONSTANTCOLOR = false;
		m_bSRGB = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bVERTEXRGBA && m_bCONSTANTCOLOR && m_bSRGB );
		return ( 1 * m_nVERTEXRGBA ) + ( 2 * m_nCONSTANTCOLOR ) + ( 4 * m_nSRGB ) + 0;
	}
};

#define shaderStaticTest_lux_sprite_ps30 psh_forgot_to_set_static_VERTEXRGBA + psh_forgot_to_set_static_CONSTANTCOLOR + psh_forgot_to_set_static_SRGB


class lux_sprite_ps30_Dynamic_Index
{
public:
	lux_sprite_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_sprite_ps30 1

