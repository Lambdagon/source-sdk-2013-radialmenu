#pragma once
#include "shaderlib/cshader.h"
class lux_custom_brush_vs30_Static_Index
{
	unsigned int m_nNORMALS : 2;
	unsigned int m_nVERTEXCOLORS : 2;
	unsigned int m_nBUMPMAPPED : 2;
	unsigned int m_nPROJTEX : 2;
	unsigned int m_nDECALMODE : 2;
#ifdef _DEBUG
	bool m_bNORMALS : 1;
	bool m_bVERTEXCOLORS : 1;
	bool m_bBUMPMAPPED : 1;
	bool m_bPROJTEX : 1;
	bool m_bDECALMODE : 1;
#endif	// _DEBUG
public:
	void SetNORMALS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nNORMALS = i;
#ifdef _DEBUG
		m_bNORMALS = true;
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

	void SetBUMPMAPPED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAPPED = i;
#ifdef _DEBUG
		m_bBUMPMAPPED = true;
#endif	// _DEBUG
	}

	void SetPROJTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEX = i;
#ifdef _DEBUG
		m_bPROJTEX = true;
#endif	// _DEBUG
	}

	void SetDECALMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nDECALMODE = i;
#ifdef _DEBUG
		m_bDECALMODE = true;
#endif	// _DEBUG
	}

	lux_custom_brush_vs30_Static_Index(  )
	{
		m_nNORMALS = 0;
		m_nVERTEXCOLORS = 0;
		m_nBUMPMAPPED = 0;
		m_nPROJTEX = 0;
		m_nDECALMODE = 0;
#ifdef _DEBUG
		m_bNORMALS = false;
		m_bVERTEXCOLORS = false;
		m_bBUMPMAPPED = false;
		m_bPROJTEX = false;
		m_bDECALMODE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNORMALS && m_bVERTEXCOLORS && m_bBUMPMAPPED && m_bPROJTEX && m_bDECALMODE );
		return ( 1 * m_nNORMALS ) + ( 3 * m_nVERTEXCOLORS ) + ( 6 * m_nBUMPMAPPED ) + ( 12 * m_nPROJTEX ) + ( 24 * m_nDECALMODE ) + 0;
	}
};

#define shaderStaticTest_lux_custom_brush_vs30 vsh_forgot_to_set_static_NORMALS + vsh_forgot_to_set_static_VERTEXCOLORS + vsh_forgot_to_set_static_BUMPMAPPED + vsh_forgot_to_set_static_PROJTEX + vsh_forgot_to_set_static_DECALMODE


class lux_custom_brush_vs30_Dynamic_Index
{
public:
	lux_custom_brush_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_custom_brush_vs30 1

