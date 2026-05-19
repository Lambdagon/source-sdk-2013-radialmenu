#pragma once
#include "shaderlib/cshader.h"
class lux_lightmappedgeneric_flashlight_ps30_Static_Index
{
	unsigned int m_nLIGHTING_MODE : 3;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nXBYBASEALPHA : 2;
#ifdef _DEBUG
	bool m_bLIGHTING_MODE : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bXBYBASEALPHA : 1;
#endif	// _DEBUG
public:
	void SetLIGHTING_MODE( int i )
	{
		Assert( i >= 0 && i <= 6 );
		m_nLIGHTING_MODE = i;
#ifdef _DEBUG
		m_bLIGHTING_MODE = true;
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

	void SetXBYBASEALPHA( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nXBYBASEALPHA = i;
#ifdef _DEBUG
		m_bXBYBASEALPHA = true;
#endif	// _DEBUG
	}

	lux_lightmappedgeneric_flashlight_ps30_Static_Index(  )
	{
		m_nLIGHTING_MODE = 0;
		m_nDETAILTEXTURE = 0;
		m_nXBYBASEALPHA = 0;
#ifdef _DEBUG
		m_bLIGHTING_MODE = false;
		m_bDETAILTEXTURE = false;
		m_bXBYBASEALPHA = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bLIGHTING_MODE && m_bDETAILTEXTURE && m_bXBYBASEALPHA );
		return ( 2 * m_nLIGHTING_MODE ) + ( 14 * m_nDETAILTEXTURE ) + ( 28 * m_nXBYBASEALPHA ) + 0;
	}
};

#define shaderStaticTest_lux_lightmappedgeneric_flashlight_ps30 psh_forgot_to_set_static_LIGHTING_MODE + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_XBYBASEALPHA


class lux_lightmappedgeneric_flashlight_ps30_Dynamic_Index
{
	unsigned int m_nPROJTEXSHADOWS : 2;
#ifdef _DEBUG
	bool m_bPROJTEXSHADOWS : 1;
#endif	// _DEBUG
public:
	void SetPROJTEXSHADOWS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEXSHADOWS = i;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = true;
#endif	// _DEBUG
	}

	lux_lightmappedgeneric_flashlight_ps30_Dynamic_Index(  )
	{
		m_nPROJTEXSHADOWS = 0;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEXSHADOWS );
		return ( 1 * m_nPROJTEXSHADOWS ) + 0;
	}
};

#define shaderDynamicTest_lux_lightmappedgeneric_flashlight_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS

