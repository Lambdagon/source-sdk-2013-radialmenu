#pragma once
#include "shaderlib/cshader.h"
class lux_refract_vs30_Static_Index
{
	unsigned int m_nBRUSH : 2;
	unsigned int m_nVERTEXCOLORS : 2;
	unsigned int m_nSECONDTEXCOORD : 2;
#ifdef _DEBUG
	bool m_bBRUSH : 1;
	bool m_bVERTEXCOLORS : 1;
	bool m_bSECONDTEXCOORD : 1;
#endif	// _DEBUG
public:
	void SetBRUSH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBRUSH = i;
#ifdef _DEBUG
		m_bBRUSH = true;
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

	void SetSECONDTEXCOORD( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nSECONDTEXCOORD = i;
#ifdef _DEBUG
		m_bSECONDTEXCOORD = true;
#endif	// _DEBUG
	}

	lux_refract_vs30_Static_Index(  )
	{
		m_nBRUSH = 0;
		m_nVERTEXCOLORS = 0;
		m_nSECONDTEXCOORD = 0;
#ifdef _DEBUG
		m_bBRUSH = false;
		m_bVERTEXCOLORS = false;
		m_bSECONDTEXCOORD = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBRUSH && m_bVERTEXCOLORS && m_bSECONDTEXCOORD );
		return ( 4 * m_nBRUSH ) + ( 8 * m_nVERTEXCOLORS ) + ( 16 * m_nSECONDTEXCOORD ) + 0;
	}
};

#define shaderStaticTest_lux_refract_vs30 vsh_forgot_to_set_static_BRUSH + vsh_forgot_to_set_static_VERTEXCOLORS + vsh_forgot_to_set_static_SECONDTEXCOORD


class lux_refract_vs30_Dynamic_Index
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

	lux_refract_vs30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_refract_vs30 vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

