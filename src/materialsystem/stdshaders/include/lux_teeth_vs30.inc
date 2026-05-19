#pragma once
#include "shaderlib/cshader.h"
class lux_teeth_vs30_Static_Index
{
	unsigned int m_nTANGENTS : 2;
	unsigned int m_nHALFLAMBERT : 2;
#ifdef _DEBUG
	bool m_bTANGENTS : 1;
	bool m_bHALFLAMBERT : 1;
#endif	// _DEBUG
public:
	void SetTANGENTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTANGENTS = i;
#ifdef _DEBUG
		m_bTANGENTS = true;
#endif	// _DEBUG
	}

	void SetHALFLAMBERT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHALFLAMBERT = i;
#ifdef _DEBUG
		m_bHALFLAMBERT = true;
#endif	// _DEBUG
	}

	lux_teeth_vs30_Static_Index(  )
	{
		m_nTANGENTS = 0;
		m_nHALFLAMBERT = 0;
#ifdef _DEBUG
		m_bTANGENTS = false;
		m_bHALFLAMBERT = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTANGENTS && m_bHALFLAMBERT );
		return ( 16 * m_nTANGENTS ) + ( 32 * m_nHALFLAMBERT ) + 0;
	}
};

#define shaderStaticTest_lux_teeth_vs30 vsh_forgot_to_set_static_TANGENTS + vsh_forgot_to_set_static_HALFLAMBERT


class lux_teeth_vs30_Dynamic_Index
{
	unsigned int m_nSTATICPROPLIGHTING : 2;
	unsigned int m_nDYNAMICPROPLIGHTING : 2;
	unsigned int m_nSKINNING : 2;
	unsigned int m_nCOMPRESSION : 2;
#ifdef _DEBUG
	bool m_bSTATICPROPLIGHTING : 1;
	bool m_bDYNAMICPROPLIGHTING : 1;
	bool m_bSKINNING : 1;
	bool m_bCOMPRESSION : 1;
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

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
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

	lux_teeth_vs30_Dynamic_Index(  )
	{
		m_nSTATICPROPLIGHTING = 0;
		m_nDYNAMICPROPLIGHTING = 0;
		m_nSKINNING = 0;
		m_nCOMPRESSION = 0;
#ifdef _DEBUG
		m_bSTATICPROPLIGHTING = false;
		m_bDYNAMICPROPLIGHTING = false;
		m_bSKINNING = false;
		m_bCOMPRESSION = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSTATICPROPLIGHTING && m_bDYNAMICPROPLIGHTING && m_bSKINNING && m_bCOMPRESSION );
		return ( 1 * m_nSTATICPROPLIGHTING ) + ( 2 * m_nDYNAMICPROPLIGHTING ) + ( 4 * m_nSKINNING ) + ( 8 * m_nCOMPRESSION ) + 0;
	}
};

#define shaderDynamicTest_lux_teeth_vs30 vsh_forgot_to_set_dynamic_STATICPROPLIGHTING + vsh_forgot_to_set_dynamic_DYNAMICPROPLIGHTING + vsh_forgot_to_set_dynamic_SKINNING + vsh_forgot_to_set_dynamic_COMPRESSION

