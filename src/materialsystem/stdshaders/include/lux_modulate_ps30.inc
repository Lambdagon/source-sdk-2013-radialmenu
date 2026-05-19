#pragma once
#include "shaderlib/cshader.h"
class lux_modulate_ps30_Static_Index
{
	unsigned int m_nVERTEXLIT : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bVERTEXLIT : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bVERTEXCOLORS : 1;
#endif	// _DEBUG
public:
	void SetVERTEXLIT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXLIT = i;
#ifdef _DEBUG
		m_bVERTEXLIT = true;
#endif	// _DEBUG
	}

	void SetDETAILTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
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

	lux_modulate_ps30_Static_Index(  )
	{
		m_nVERTEXLIT = 0;
		m_nDETAILTEXTURE = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bVERTEXLIT = false;
		m_bDETAILTEXTURE = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bVERTEXLIT && m_bDETAILTEXTURE && m_bVERTEXCOLORS );
		return ( 2 * m_nVERTEXLIT ) + ( 4 * m_nDETAILTEXTURE ) + ( 8 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_modulate_ps30 psh_forgot_to_set_static_VERTEXLIT + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_VERTEXCOLORS


class lux_modulate_ps30_Dynamic_Index
{
	unsigned int m_nLIGHTMAPPED_MODEL : 2;
#ifdef _DEBUG
	bool m_bLIGHTMAPPED_MODEL : 1;
#endif	// _DEBUG
public:
	void SetLIGHTMAPPED_MODEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPPED_MODEL = i;
#ifdef _DEBUG
		m_bLIGHTMAPPED_MODEL = true;
#endif	// _DEBUG
	}

	lux_modulate_ps30_Dynamic_Index(  )
	{
		m_nLIGHTMAPPED_MODEL = 0;
#ifdef _DEBUG
		m_bLIGHTMAPPED_MODEL = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bLIGHTMAPPED_MODEL );
		return ( 1 * m_nLIGHTMAPPED_MODEL ) + 0;
	}
};

#define shaderDynamicTest_lux_modulate_ps30 psh_forgot_to_set_dynamic_LIGHTMAPPED_MODEL

