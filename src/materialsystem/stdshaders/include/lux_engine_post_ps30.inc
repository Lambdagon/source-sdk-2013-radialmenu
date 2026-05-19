#pragma once
#include "shaderlib/cshader.h"
class lux_engine_post_ps30_Static_Index
{
public:
	lux_engine_post_ps30_Static_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_lux_engine_post_ps30 1


class lux_engine_post_ps30_Dynamic_Index
{
	unsigned int m_nL4D_VOMIT : 2;
	unsigned int m_nCONTRAST : 2;
	unsigned int m_nDEPTH_BLUR : 2;
	unsigned int m_nCOLORCORRECT_LOOKUPS : 3;
	unsigned int m_nDESATURATE : 2;
	unsigned int m_nVIGNETTE : 2;
	unsigned int m_nNOISE : 2;
#ifdef _DEBUG
	bool m_bL4D_VOMIT : 1;
	bool m_bCONTRAST : 1;
	bool m_bDEPTH_BLUR : 1;
	bool m_bCOLORCORRECT_LOOKUPS : 1;
	bool m_bDESATURATE : 1;
	bool m_bVIGNETTE : 1;
	bool m_bNOISE : 1;
#endif	// _DEBUG
public:
	void SetL4D_VOMIT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nL4D_VOMIT = i;
#ifdef _DEBUG
		m_bL4D_VOMIT = true;
#endif	// _DEBUG
	}

	void SetCONTRAST( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nCONTRAST = i;
#ifdef _DEBUG
		m_bCONTRAST = true;
#endif	// _DEBUG
	}

	void SetDEPTH_BLUR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDEPTH_BLUR = i;
#ifdef _DEBUG
		m_bDEPTH_BLUR = true;
#endif	// _DEBUG
	}

	void SetCOLORCORRECT_LOOKUPS( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nCOLORCORRECT_LOOKUPS = i;
#ifdef _DEBUG
		m_bCOLORCORRECT_LOOKUPS = true;
#endif	// _DEBUG
	}

	void SetDESATURATE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDESATURATE = i;
#ifdef _DEBUG
		m_bDESATURATE = true;
#endif	// _DEBUG
	}

	void SetVIGNETTE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVIGNETTE = i;
#ifdef _DEBUG
		m_bVIGNETTE = true;
#endif	// _DEBUG
	}

	void SetNOISE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNOISE = i;
#ifdef _DEBUG
		m_bNOISE = true;
#endif	// _DEBUG
	}

	lux_engine_post_ps30_Dynamic_Index(  )
	{
		m_nL4D_VOMIT = 0;
		m_nCONTRAST = 0;
		m_nDEPTH_BLUR = 0;
		m_nCOLORCORRECT_LOOKUPS = 0;
		m_nDESATURATE = 0;
		m_nVIGNETTE = 0;
		m_nNOISE = 0;
#ifdef _DEBUG
		m_bL4D_VOMIT = false;
		m_bCONTRAST = false;
		m_bDEPTH_BLUR = false;
		m_bCOLORCORRECT_LOOKUPS = false;
		m_bDESATURATE = false;
		m_bVIGNETTE = false;
		m_bNOISE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bL4D_VOMIT && m_bCONTRAST && m_bDEPTH_BLUR && m_bCOLORCORRECT_LOOKUPS && m_bDESATURATE && m_bVIGNETTE && m_bNOISE );
		return ( 1 * m_nL4D_VOMIT ) + ( 2 * m_nCONTRAST ) + ( 6 * m_nDEPTH_BLUR ) + ( 12 * m_nCOLORCORRECT_LOOKUPS ) + ( 48 * m_nDESATURATE ) + ( 96 * m_nVIGNETTE ) + ( 192 * m_nNOISE ) + 0;
	}
};

#define shaderDynamicTest_lux_engine_post_ps30 psh_forgot_to_set_dynamic_L4D_VOMIT + psh_forgot_to_set_dynamic_CONTRAST + psh_forgot_to_set_dynamic_DEPTH_BLUR + psh_forgot_to_set_dynamic_COLORCORRECT_LOOKUPS + psh_forgot_to_set_dynamic_DESATURATE + psh_forgot_to_set_dynamic_VIGNETTE + psh_forgot_to_set_dynamic_NOISE

