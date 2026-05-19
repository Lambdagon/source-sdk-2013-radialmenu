#pragma once
#include "shaderlib/cshader.h"
class lux_eyes_vs30_Static_Index
{
	unsigned int m_nINTRO : 2;
	unsigned int m_nPROJTEX : 2;
#ifdef _DEBUG
	bool m_bINTRO : 1;
	bool m_bPROJTEX : 1;
#endif	// _DEBUG
public:
	void SetINTRO( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nINTRO = i;
#ifdef _DEBUG
		m_bINTRO = true;
#endif	// _DEBUG
	}

	void SetPROJTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEX = i;
#ifdef _DEBUG
		m_bPROJTEX = true;
#endif	// _DEBUG
	}

	lux_eyes_vs30_Static_Index(  )
	{
		m_nINTRO = 0;
		m_nPROJTEX = 0;
#ifdef _DEBUG
		m_bINTRO = false;
		m_bPROJTEX = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bINTRO && m_bPROJTEX );
		return ( 16 * m_nINTRO ) + ( 32 * m_nPROJTEX ) + 0;
	}
};

#define shaderStaticTest_lux_eyes_vs30 vsh_forgot_to_set_static_INTRO + vsh_forgot_to_set_static_PROJTEX


class lux_eyes_vs30_Dynamic_Index
{
	unsigned int m_nSTATICPROPLIGHTING : 2;
	unsigned int m_nDYNAMICPROPLIGHTING : 2;
	unsigned int m_nCOMPRESSION : 2;
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bSTATICPROPLIGHTING : 1;
	bool m_bDYNAMICPROPLIGHTING : 1;
	bool m_bCOMPRESSION : 1;
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetSTATICPROPLIGHTING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSTATICPROPLIGHTING = i;
#ifdef _DEBUG
		m_bSTATICPROPLIGHTING = true;
#endif	// _DEBUG
	}

	void SetDYNAMICPROPLIGHTING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDYNAMICPROPLIGHTING = i;
#ifdef _DEBUG
		m_bDYNAMICPROPLIGHTING = true;
#endif	// _DEBUG
	}

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

	lux_eyes_vs30_Dynamic_Index(  )
	{
		m_nSTATICPROPLIGHTING = 0;
		m_nDYNAMICPROPLIGHTING = 0;
		m_nCOMPRESSION = 0;
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bSTATICPROPLIGHTING = false;
		m_bDYNAMICPROPLIGHTING = false;
		m_bCOMPRESSION = false;
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSTATICPROPLIGHTING && m_bDYNAMICPROPLIGHTING && m_bCOMPRESSION && m_bSKINNING );
		return ( 1 * m_nSTATICPROPLIGHTING ) + ( 2 * m_nDYNAMICPROPLIGHTING ) + ( 4 * m_nCOMPRESSION ) + ( 8 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_lux_eyes_vs30 vsh_forgot_to_set_dynamic_STATICPROPLIGHTING + vsh_forgot_to_set_dynamic_DYNAMICPROPLIGHTING + vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

