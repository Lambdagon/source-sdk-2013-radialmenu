#pragma once
#include "shaderlib/cshader.h"
class lux_brush_simplified_vs30_Static_Index
{
	unsigned int m_nTEXCOORDS : 3;
	unsigned int m_nNOMODELMATRIX : 2;
	unsigned int m_nNORMALS : 2;
	unsigned int m_nLIGHTMAP_UV : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bTEXCOORDS : 1;
	bool m_bNOMODELMATRIX : 1;
	bool m_bNORMALS : 1;
	bool m_bLIGHTMAP_UV : 1;
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

	void SetNOMODELMATRIX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNOMODELMATRIX = i;
#ifdef _DEBUG
		m_bNOMODELMATRIX = true;
#endif	// _DEBUG
	}

	void SetNORMALS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nNORMALS = i;
#ifdef _DEBUG
		m_bNORMALS = true;
#endif	// _DEBUG
	}

	void SetLIGHTMAP_UV( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAP_UV = i;
#ifdef _DEBUG
		m_bLIGHTMAP_UV = true;
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

	lux_brush_simplified_vs30_Static_Index(  )
	{
		m_nTEXCOORDS = 0;
		m_nNOMODELMATRIX = 0;
		m_nNORMALS = 0;
		m_nLIGHTMAP_UV = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bTEXCOORDS = false;
		m_bNOMODELMATRIX = false;
		m_bNORMALS = false;
		m_bLIGHTMAP_UV = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXCOORDS && m_bNOMODELMATRIX && m_bNORMALS && m_bLIGHTMAP_UV && m_bVERTEXCOLORS );
		return ( 1 * m_nTEXCOORDS ) + ( 4 * m_nNOMODELMATRIX ) + ( 8 * m_nNORMALS ) + ( 24 * m_nLIGHTMAP_UV ) + ( 48 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_brush_simplified_vs30 vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_NOMODELMATRIX + vsh_forgot_to_set_static_NORMALS + vsh_forgot_to_set_static_LIGHTMAP_UV + vsh_forgot_to_set_static_VERTEXCOLORS


class lux_brush_simplified_vs30_Dynamic_Index
{
public:
	lux_brush_simplified_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_brush_simplified_vs30 1

