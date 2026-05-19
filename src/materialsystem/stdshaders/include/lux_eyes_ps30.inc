#pragma once
#include "shaderlib/cshader.h"
class lux_eyes_ps30_Static_Index
{
	unsigned int m_nPROJTEX : 2;
#ifdef _DEBUG
	bool m_bPROJTEX : 1;
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

	lux_eyes_ps30_Static_Index(  )
	{
		m_nPROJTEX = 0;
#ifdef _DEBUG
		m_bPROJTEX = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEX );
		return ( 2 * m_nPROJTEX ) + 0;
	}
};

#define shaderStaticTest_lux_eyes_ps30 psh_forgot_to_set_static_PROJTEX


class lux_eyes_ps30_Dynamic_Index
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

	lux_eyes_ps30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_eyes_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS

