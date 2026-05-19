#pragma once
#include "shaderlib/cshader.h"
class lux_rimlightpass_ps30_Static_Index
{
	unsigned int m_nNORMALMAP : 2;
	unsigned int m_nUSEAMBIENTCUBE : 2;
	unsigned int m_nBASETEXTUREOPACITY : 2;
	unsigned int m_nMASKED : 2;
#ifdef _DEBUG
	bool m_bNORMALMAP : 1;
	bool m_bUSEAMBIENTCUBE : 1;
	bool m_bBASETEXTUREOPACITY : 1;
	bool m_bMASKED : 1;
#endif	// _DEBUG
public:
	void SetNORMALMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNORMALMAP = i;
#ifdef _DEBUG
		m_bNORMALMAP = true;
#endif	// _DEBUG
	}

	void SetUSEAMBIENTCUBE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nUSEAMBIENTCUBE = i;
#ifdef _DEBUG
		m_bUSEAMBIENTCUBE = true;
#endif	// _DEBUG
	}

	void SetBASETEXTUREOPACITY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTUREOPACITY = i;
#ifdef _DEBUG
		m_bBASETEXTUREOPACITY = true;
#endif	// _DEBUG
	}

	void SetMASKED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMASKED = i;
#ifdef _DEBUG
		m_bMASKED = true;
#endif	// _DEBUG
	}

	lux_rimlightpass_ps30_Static_Index(  )
	{
		m_nNORMALMAP = 0;
		m_nUSEAMBIENTCUBE = 0;
		m_nBASETEXTUREOPACITY = 0;
		m_nMASKED = 0;
#ifdef _DEBUG
		m_bNORMALMAP = false;
		m_bUSEAMBIENTCUBE = false;
		m_bBASETEXTUREOPACITY = false;
		m_bMASKED = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNORMALMAP && m_bUSEAMBIENTCUBE && m_bBASETEXTUREOPACITY && m_bMASKED );
		return ( 1 * m_nNORMALMAP ) + ( 2 * m_nUSEAMBIENTCUBE ) + ( 4 * m_nBASETEXTUREOPACITY ) + ( 8 * m_nMASKED ) + 0;
	}
};

#define shaderStaticTest_lux_rimlightpass_ps30 psh_forgot_to_set_static_NORMALMAP + psh_forgot_to_set_static_USEAMBIENTCUBE + psh_forgot_to_set_static_BASETEXTUREOPACITY + psh_forgot_to_set_static_MASKED


class lux_rimlightpass_ps30_Dynamic_Index
{
public:
	lux_rimlightpass_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_rimlightpass_ps30 1

