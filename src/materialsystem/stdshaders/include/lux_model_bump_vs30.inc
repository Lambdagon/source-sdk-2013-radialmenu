#pragma once
#include "shaderlib/cshader.h"
class lux_model_bump_vs30_Static_Index
{
	unsigned int m_nTEXCOORDS : 2;
	unsigned int m_nVERTEX_SWAY : 2;
	unsigned int m_nWRINKLEMAPS : 2;
#ifdef _DEBUG
	bool m_bTEXCOORDS : 1;
	bool m_bVERTEX_SWAY : 1;
	bool m_bWRINKLEMAPS : 1;
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

	void SetVERTEX_SWAY( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nVERTEX_SWAY = i;
#ifdef _DEBUG
		m_bVERTEX_SWAY = true;
#endif	// _DEBUG
	}

	void SetWRINKLEMAPS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRINKLEMAPS = i;
#ifdef _DEBUG
		m_bWRINKLEMAPS = true;
#endif	// _DEBUG
	}

	lux_model_bump_vs30_Static_Index(  )
	{
		m_nTEXCOORDS = 0;
		m_nVERTEX_SWAY = 0;
		m_nWRINKLEMAPS = 0;
#ifdef _DEBUG
		m_bTEXCOORDS = false;
		m_bVERTEX_SWAY = false;
		m_bWRINKLEMAPS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXCOORDS && m_bVERTEX_SWAY && m_bWRINKLEMAPS );
		return ( 4 * m_nTEXCOORDS ) + ( 12 * m_nVERTEX_SWAY ) + ( 36 * m_nWRINKLEMAPS ) + 0;
	}
};

#define shaderStaticTest_lux_model_bump_vs30 vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_VERTEX_SWAY + vsh_forgot_to_set_static_WRINKLEMAPS


class lux_model_bump_vs30_Dynamic_Index
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

	lux_model_bump_vs30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_model_bump_vs30 vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

