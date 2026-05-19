#pragma once
#include "shaderlib/cshader.h"
class lux_brush_vs30_Static_Index
{
	unsigned int m_nTEXCOORDS : 3;
	unsigned int m_nTANGENTS : 2;
	unsigned int m_nBUMPED_LIGHTMAP : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bTEXCOORDS : 1;
	bool m_bTANGENTS : 1;
	bool m_bBUMPED_LIGHTMAP : 1;
	bool m_bVERTEXCOLORS : 1;
#endif	// _DEBUG
public:
	void SetTEXCOORDS( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nTEXCOORDS = i;
#ifdef _DEBUG
		m_bTEXCOORDS = true;
#endif	// _DEBUG
	}

	void SetTANGENTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTANGENTS = i;
#ifdef _DEBUG
		m_bTANGENTS = true;
#endif	// _DEBUG
	}

	void SetBUMPED_LIGHTMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPED_LIGHTMAP = i;
#ifdef _DEBUG
		m_bBUMPED_LIGHTMAP = true;
#endif	// _DEBUG
	}

	void SetVERTEXCOLORS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLORS = i;
#ifdef _DEBUG
		m_bVERTEXCOLORS = true;
#endif	// _DEBUG
	}

	lux_brush_vs30_Static_Index(  )
	{
		m_nTEXCOORDS = 0;
		m_nTANGENTS = 0;
		m_nBUMPED_LIGHTMAP = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bTEXCOORDS = false;
		m_bTANGENTS = false;
		m_bBUMPED_LIGHTMAP = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXCOORDS && m_bTANGENTS && m_bBUMPED_LIGHTMAP && m_bVERTEXCOLORS );
		return ( 1 * m_nTEXCOORDS ) + ( 4 * m_nTANGENTS ) + ( 8 * m_nBUMPED_LIGHTMAP ) + ( 16 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_brush_vs30 vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_TANGENTS + vsh_forgot_to_set_static_BUMPED_LIGHTMAP + vsh_forgot_to_set_static_VERTEXCOLORS


class lux_brush_vs30_Dynamic_Index
{
public:
	lux_brush_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_brush_vs30 1

