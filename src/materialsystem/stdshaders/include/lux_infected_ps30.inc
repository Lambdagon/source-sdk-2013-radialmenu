// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $PROJTEX == 0 && $PROJTEXSHADOWS != 0 )
// ( $PROJTEX != 0 && $BURNING != 0 )
// ( $PROJTEX != 0 && $NUM_LIGHTS_COMBO != 0 )
// ( $PROJTEXSHADOWS != 0 && $NUM_LIGHTS_COMBO != 0 )

#pragma once
#include "shaderlib/cshader.h"
class lux_infected_ps30_Static_Index
{
	unsigned int m_nPROJTEX : 2;
	unsigned int m_nBURNING : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nEYEGLOW : 2;
	unsigned int m_nVERTEXCOLORS : 2;
	unsigned int m_nELLIPSOIDDEBUG : 2;
#ifdef _DEBUG
	bool m_bPROJTEX : 1;
	bool m_bBURNING : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bEYEGLOW : 1;
	bool m_bVERTEXCOLORS : 1;
	bool m_bELLIPSOIDDEBUG : 1;
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

	void SetBURNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBURNING = i;
#ifdef _DEBUG
		m_bBURNING = true;
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

	void SetEYEGLOW( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nEYEGLOW = i;
#ifdef _DEBUG
		m_bEYEGLOW = true;
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

	void SetELLIPSOIDDEBUG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nELLIPSOIDDEBUG = i;
#ifdef _DEBUG
		m_bELLIPSOIDDEBUG = true;
#endif	// _DEBUG
	}

	lux_infected_ps30_Static_Index(  )
	{
		m_nPROJTEX = 0;
		m_nBURNING = 0;
		m_nDETAILTEXTURE = 0;
		m_nEYEGLOW = 0;
		m_nVERTEXCOLORS = 0;
		m_nELLIPSOIDDEBUG = 0;
#ifdef _DEBUG
		m_bPROJTEX = false;
		m_bBURNING = false;
		m_bDETAILTEXTURE = false;
		m_bEYEGLOW = false;
		m_bVERTEXCOLORS = false;
		m_bELLIPSOIDDEBUG = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEX && m_bBURNING && m_bDETAILTEXTURE && m_bEYEGLOW && m_bVERTEXCOLORS && m_bELLIPSOIDDEBUG );
		AssertMsg( !( ( m_nPROJTEX != 0 ) && ( m_nBURNING != 0 ) ), "Invalid combo combination ( ( PROJTEX != 0 ) && ( BURNING != 0 ) )" );
		return ( 40 * m_nPROJTEX ) + ( 80 * m_nBURNING ) + ( 160 * m_nDETAILTEXTURE ) + ( 320 * m_nEYEGLOW ) + ( 640 * m_nVERTEXCOLORS ) + ( 1280 * m_nELLIPSOIDDEBUG ) + 0;
	}
};

#define shaderStaticTest_lux_infected_ps30 psh_forgot_to_set_static_PROJTEX + psh_forgot_to_set_static_BURNING + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_EYEGLOW + psh_forgot_to_set_static_VERTEXCOLORS + psh_forgot_to_set_static_ELLIPSOIDDEBUG


class lux_infected_ps30_Dynamic_Index
{
	unsigned int m_nELLIPSOIDCULLING : 3;
	unsigned int m_nPROJTEXSHADOWS : 2;
	unsigned int m_nNUM_LIGHTS_COMBO : 3;
#ifdef _DEBUG
	bool m_bELLIPSOIDCULLING : 1;
	bool m_bPROJTEXSHADOWS : 1;
	bool m_bNUM_LIGHTS_COMBO : 1;
#endif	// _DEBUG
public:
	void SetELLIPSOIDCULLING( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nELLIPSOIDCULLING = i;
#ifdef _DEBUG
		m_bELLIPSOIDCULLING = true;
#endif	// _DEBUG
	}

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

	lux_infected_ps30_Dynamic_Index(  )
	{
		m_nELLIPSOIDCULLING = 0;
		m_nPROJTEXSHADOWS = 0;
		m_nNUM_LIGHTS_COMBO = 0;
#ifdef _DEBUG
		m_bELLIPSOIDCULLING = false;
		m_bPROJTEXSHADOWS = false;
		m_bNUM_LIGHTS_COMBO = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bELLIPSOIDCULLING && m_bPROJTEXSHADOWS && m_bNUM_LIGHTS_COMBO );
		AssertMsg( !( ( m_nPROJTEXSHADOWS != 0 ) && ( m_nNUM_LIGHTS_COMBO != 0 ) ), "Invalid combo combination ( ( PROJTEXSHADOWS != 0 ) && ( NUM_LIGHTS_COMBO != 0 ) )" );
		return ( 1 * m_nELLIPSOIDCULLING ) + ( 4 * m_nPROJTEXSHADOWS ) + ( 8 * m_nNUM_LIGHTS_COMBO ) + 0;
	}
};

#define shaderDynamicTest_lux_infected_ps30 psh_forgot_to_set_dynamic_ELLIPSOIDCULLING + psh_forgot_to_set_dynamic_PROJTEXSHADOWS + psh_forgot_to_set_dynamic_NUM_LIGHTS_COMBO

