// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $LIGHTCOMBO < 2 && $WRINKLEMAPS != 0 )

#pragma once
#include "shaderlib/cshader.h"
class lux_vertexlitgeneric_asw_flashlight_ps30_Static_Index
{
	unsigned int m_nLIGHTCOMBO : 3;
	unsigned int m_nWRINKLEMAPS : 2;
	unsigned int m_nXBYBASEALPHA : 2;
	unsigned int m_nDETAILTEXTURE : 2;
#ifdef _DEBUG
	bool m_bLIGHTCOMBO : 1;
	bool m_bWRINKLEMAPS : 1;
	bool m_bXBYBASEALPHA : 1;
	bool m_bDETAILTEXTURE : 1;
#endif	// _DEBUG
public:
	void SetLIGHTCOMBO( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nLIGHTCOMBO = i;
#ifdef _DEBUG
		m_bLIGHTCOMBO = true;
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

	void SetXBYBASEALPHA( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nXBYBASEALPHA = i;
#ifdef _DEBUG
		m_bXBYBASEALPHA = true;
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

	lux_vertexlitgeneric_asw_flashlight_ps30_Static_Index(  )
	{
		m_nLIGHTCOMBO = 0;
		m_nWRINKLEMAPS = 0;
		m_nXBYBASEALPHA = 0;
		m_nDETAILTEXTURE = 0;
#ifdef _DEBUG
		m_bLIGHTCOMBO = false;
		m_bWRINKLEMAPS = false;
		m_bXBYBASEALPHA = false;
		m_bDETAILTEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bLIGHTCOMBO && m_bWRINKLEMAPS && m_bXBYBASEALPHA && m_bDETAILTEXTURE );
		AssertMsg( !( ( m_nLIGHTCOMBO < 2 ) && ( m_nWRINKLEMAPS != 0 ) ), "Invalid combo combination ( ( LIGHTCOMBO < 2 ) && ( WRINKLEMAPS != 0 ) )" );
		return ( 4 * m_nLIGHTCOMBO ) + ( 16 * m_nWRINKLEMAPS ) + ( 32 * m_nXBYBASEALPHA ) + ( 96 * m_nDETAILTEXTURE ) + 0;
	}
};

#define shaderStaticTest_lux_vertexlitgeneric_asw_flashlight_ps30 psh_forgot_to_set_static_LIGHTCOMBO + psh_forgot_to_set_static_WRINKLEMAPS + psh_forgot_to_set_static_XBYBASEALPHA + psh_forgot_to_set_static_DETAILTEXTURE


class lux_vertexlitgeneric_asw_flashlight_ps30_Dynamic_Index
{
	unsigned int m_nPROJTEXSHADOWS : 2;
	unsigned int m_nUBERLIGHT : 2;
#ifdef _DEBUG
	bool m_bPROJTEXSHADOWS : 1;
	bool m_bUBERLIGHT : 1;
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

	void SetUBERLIGHT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nUBERLIGHT = i;
#ifdef _DEBUG
		m_bUBERLIGHT = true;
#endif	// _DEBUG
	}

	lux_vertexlitgeneric_asw_flashlight_ps30_Dynamic_Index(  )
	{
		m_nPROJTEXSHADOWS = 0;
		m_nUBERLIGHT = 0;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = false;
		m_bUBERLIGHT = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEXSHADOWS && m_bUBERLIGHT );
		return ( 1 * m_nPROJTEXSHADOWS ) + ( 2 * m_nUBERLIGHT ) + 0;
	}
};

#define shaderDynamicTest_lux_vertexlitgeneric_asw_flashlight_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS + psh_forgot_to_set_dynamic_UBERLIGHT

