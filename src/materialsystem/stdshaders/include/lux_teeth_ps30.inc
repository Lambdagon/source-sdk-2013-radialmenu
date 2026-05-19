// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($PROJTEX == 1 && $ENVMAPMODE != 0)
// ($PROJTEX == 1 && $LIGHTING_MODE != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_teeth_ps30_Static_Index
{
	unsigned int m_nPROJTEX : 2;
	unsigned int m_nENVMAPMODE : 2;
	unsigned int m_nLIGHTING_MODE : 3;
	unsigned int m_nDETAILTEXTURE : 2;
#ifdef _DEBUG
	bool m_bPROJTEX : 1;
	bool m_bENVMAPMODE : 1;
	bool m_bLIGHTING_MODE : 1;
	bool m_bDETAILTEXTURE : 1;
#endif	// _DEBUG
public:
	void SetPROJTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEX = i;
#ifdef _DEBUG
		m_bPROJTEX = true;
#endif	// _DEBUG
	}

	void SetENVMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nENVMAPMODE = i;
#ifdef _DEBUG
		m_bENVMAPMODE = true;
#endif	// _DEBUG
	}

	void SetLIGHTING_MODE( int i )
	{
		Assert( i >= 0 && i <= 3 );
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

	lux_teeth_ps30_Static_Index(  )
	{
		m_nPROJTEX = 0;
		m_nENVMAPMODE = 0;
		m_nLIGHTING_MODE = 0;
		m_nDETAILTEXTURE = 0;
#ifdef _DEBUG
		m_bPROJTEX = false;
		m_bENVMAPMODE = false;
		m_bLIGHTING_MODE = false;
		m_bDETAILTEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEX && m_bENVMAPMODE && m_bLIGHTING_MODE && m_bDETAILTEXTURE );
		AssertMsg( !( ( m_nPROJTEX == 1 ) && ( m_nENVMAPMODE != 0 ) ), "Invalid combo combination ( ( PROJTEX == 1 ) && ( ENVMAPMODE != 0 ) )" );
		AssertMsg( !( ( m_nPROJTEX == 1 ) && ( m_nLIGHTING_MODE != 0 ) ), "Invalid combo combination ( ( PROJTEX == 1 ) && ( LIGHTING_MODE != 0 ) )" );
		return ( 10 * m_nPROJTEX ) + ( 20 * m_nENVMAPMODE ) + ( 60 * m_nLIGHTING_MODE ) + ( 240 * m_nDETAILTEXTURE ) + 0;
	}
};

#define shaderStaticTest_lux_teeth_ps30 psh_forgot_to_set_static_PROJTEX + psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_LIGHTING_MODE + psh_forgot_to_set_static_DETAILTEXTURE


class lux_teeth_ps30_Dynamic_Index
{
	unsigned int m_nPROJTEXSHADOWS : 2;
	unsigned int m_nNUM_LIGHTS_COMBO : 3;
#ifdef _DEBUG
	bool m_bPROJTEXSHADOWS : 1;
	bool m_bNUM_LIGHTS_COMBO : 1;
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

	void SetNUM_LIGHTS_COMBO( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nNUM_LIGHTS_COMBO = i;
#ifdef _DEBUG
		m_bNUM_LIGHTS_COMBO = true;
#endif	// _DEBUG
	}

	lux_teeth_ps30_Dynamic_Index(  )
	{
		m_nPROJTEXSHADOWS = 0;
		m_nNUM_LIGHTS_COMBO = 0;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = false;
		m_bNUM_LIGHTS_COMBO = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEXSHADOWS && m_bNUM_LIGHTS_COMBO );
		return ( 1 * m_nPROJTEXSHADOWS ) + ( 2 * m_nNUM_LIGHTS_COMBO ) + 0;
	}
};

#define shaderDynamicTest_lux_teeth_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS + psh_forgot_to_set_dynamic_NUM_LIGHTS_COMBO

