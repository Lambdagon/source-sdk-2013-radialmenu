#pragma once
#include "shaderlib/cshader.h"
class lux_brush_decal_vs30_Static_Index
{
	unsigned int m_nTEXCOORDS : 3;
	unsigned int m_nBUMPMAPPED : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bTEXCOORDS : 1;
	bool m_bBUMPMAPPED : 1;
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

	void SetBUMPMAPPED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAPPED = i;
#ifdef _DEBUG
		m_bBUMPMAPPED = true;
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

	lux_brush_decal_vs30_Static_Index(  )
	{
		m_nTEXCOORDS = 0;
		m_nBUMPMAPPED = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bTEXCOORDS = false;
		m_bBUMPMAPPED = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXCOORDS && m_bBUMPMAPPED && m_bVERTEXCOLORS );
		return ( 1 * m_nTEXCOORDS ) + ( 4 * m_nBUMPMAPPED ) + ( 8 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_brush_decal_vs30 vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_BUMPMAPPED + vsh_forgot_to_set_static_VERTEXCOLORS


class lux_brush_decal_vs30_Dynamic_Index
{
public:
	lux_brush_decal_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_brush_decal_vs30 1

