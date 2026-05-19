#pragma once
#include "shaderlib/cshader.h"
class lux_model_simplified_vs30_Static_Index
{
	unsigned int m_nTEXCOORDS : 3;
	unsigned int m_nVERTEXCOLORS : 2;
	unsigned int m_nNORMALS : 3;
	unsigned int m_nVERTEX_SWAY : 2;
#ifdef _DEBUG
	bool m_bTEXCOORDS : 1;
	bool m_bVERTEXCOLORS : 1;
	bool m_bNORMALS : 1;
	bool m_bVERTEX_SWAY : 1;
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

	void SetVERTEXCOLORS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLORS = i;
#ifdef _DEBUG
		m_bVERTEXCOLORS = true;
#endif	// _DEBUG
	}

	void SetNORMALS( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nNORMALS = i;
#ifdef _DEBUG
		m_bNORMALS = true;
#endif	// _DEBUG
	}

	void SetVERTEX_SWAY( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nVERTEX_SWAY = i;
#ifdef _DEBUG
		m_bVERTEX_SWAY = true;
#endif	// _DEBUG
	}

	lux_model_simplified_vs30_Static_Index(  )
	{
		m_nTEXCOORDS = 0;
		m_nVERTEXCOLORS = 0;
		m_nNORMALS = 0;
		m_nVERTEX_SWAY = 0;
#ifdef _DEBUG
		m_bTEXCOORDS = false;
		m_bVERTEXCOLORS = false;
		m_bNORMALS = false;
		m_bVERTEX_SWAY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXCOORDS && m_bVERTEXCOLORS && m_bNORMALS && m_bVERTEX_SWAY );
		return ( 4 * m_nTEXCOORDS ) + ( 16 * m_nVERTEXCOLORS ) + ( 32 * m_nNORMALS ) + ( 128 * m_nVERTEX_SWAY ) + 0;
	}
};

#define shaderStaticTest_lux_model_simplified_vs30 vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_VERTEXCOLORS + vsh_forgot_to_set_static_NORMALS + vsh_forgot_to_set_static_VERTEX_SWAY


class lux_model_simplified_vs30_Dynamic_Index
{
	unsigned int m_nCOMPRESSION : 2;
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSION : 1;
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetCOMPRESSION( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOMPRESSION = i;
#ifdef _DEBUG
		m_bCOMPRESSION = true;
#endif	// _DEBUG
	}

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	lux_model_simplified_vs30_Dynamic_Index(  )
	{
		m_nCOMPRESSION = 0;
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bCOMPRESSION = false;
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSION && m_bSKINNING );
		return ( 1 * m_nCOMPRESSION ) + ( 2 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_lux_model_simplified_vs30 vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

