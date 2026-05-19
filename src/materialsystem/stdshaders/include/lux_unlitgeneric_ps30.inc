#pragma once
#include "shaderlib/cshader.h"
class lux_unlitgeneric_ps30_Static_Index
{
	unsigned int m_nENVMAPMODE : 3;
	unsigned int m_nXBYBASEALPHA : 2;
	unsigned int m_nDETAILTEXTURE : 3;
	unsigned int m_nDISTANCEALPHAMODE : 4;
#ifdef _DEBUG
	bool m_bENVMAPMODE : 1;
	bool m_bXBYBASEALPHA : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bDISTANCEALPHAMODE : 1;
#endif	// _DEBUG
public:
	void SetENVMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nENVMAPMODE = i;
#ifdef _DEBUG
		m_bENVMAPMODE = true;
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
		Assert( i >= 0 && i <= 4 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
#endif	// _DEBUG
	}

	void SetDISTANCEALPHAMODE( int i )
	{
		Assert( i >= 0 && i <= 7 );
		m_nDISTANCEALPHAMODE = i;
#ifdef _DEBUG
		m_bDISTANCEALPHAMODE = true;
#endif	// _DEBUG
	}

	lux_unlitgeneric_ps30_Static_Index(  )
	{
		m_nENVMAPMODE = 0;
		m_nXBYBASEALPHA = 0;
		m_nDETAILTEXTURE = 0;
		m_nDISTANCEALPHAMODE = 0;
#ifdef _DEBUG
		m_bENVMAPMODE = false;
		m_bXBYBASEALPHA = false;
		m_bDETAILTEXTURE = false;
		m_bDISTANCEALPHAMODE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bENVMAPMODE && m_bXBYBASEALPHA && m_bDETAILTEXTURE && m_bDISTANCEALPHAMODE );
		return ( 1 * m_nENVMAPMODE ) + ( 5 * m_nXBYBASEALPHA ) + ( 15 * m_nDETAILTEXTURE ) + ( 75 * m_nDISTANCEALPHAMODE ) + 0;
	}
};

#define shaderStaticTest_lux_unlitgeneric_ps30 psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_XBYBASEALPHA + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_DISTANCEALPHAMODE


class lux_unlitgeneric_ps30_Dynamic_Index
{
public:
	lux_unlitgeneric_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_unlitgeneric_ps30 1

