// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( ($ENVMAPMODE != 2 && $ENVMAPMODE != 4) && $SELFILLUMMODE == 2 )
// ( $SELFILLUMMODE == 1 && $XBYBASEALPHA != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_worldvertextransition_simple_ps30_Static_Index
{
	unsigned int m_nBLENDMODULATETEXTURE : 2;
	unsigned int m_nENVMAPMODE : 3;
	unsigned int m_nSELFILLUMMODE : 2;
	unsigned int m_nXBYBASEALPHA : 2;
	unsigned int m_nDETAILTEXTURE : 2;
#ifdef _DEBUG
	bool m_bBLENDMODULATETEXTURE : 1;
	bool m_bENVMAPMODE : 1;
	bool m_bSELFILLUMMODE : 1;
	bool m_bXBYBASEALPHA : 1;
	bool m_bDETAILTEXTURE : 1;
#endif	// _DEBUG
public:
	void SetBLENDMODULATETEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLENDMODULATETEXTURE = i;
#ifdef _DEBUG
		m_bBLENDMODULATETEXTURE = true;
#endif	// _DEBUG
	}

	void SetENVMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nENVMAPMODE = i;
#ifdef _DEBUG
		m_bENVMAPMODE = true;
#endif	// _DEBUG
	}

	void SetSELFILLUMMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nSELFILLUMMODE = i;
#ifdef _DEBUG
		m_bSELFILLUMMODE = true;
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
		Assert( i >= 0 && i <= 2 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
#endif	// _DEBUG
	}

	lux_worldvertextransition_simple_ps30_Static_Index(  )
	{
		m_nBLENDMODULATETEXTURE = 0;
		m_nENVMAPMODE = 0;
		m_nSELFILLUMMODE = 0;
		m_nXBYBASEALPHA = 0;
		m_nDETAILTEXTURE = 0;
#ifdef _DEBUG
		m_bBLENDMODULATETEXTURE = false;
		m_bENVMAPMODE = false;
		m_bSELFILLUMMODE = false;
		m_bXBYBASEALPHA = false;
		m_bDETAILTEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBLENDMODULATETEXTURE && m_bENVMAPMODE && m_bSELFILLUMMODE && m_bXBYBASEALPHA && m_bDETAILTEXTURE );
		AssertMsg( !( ( ( m_nENVMAPMODE != 2 ) && ( m_nENVMAPMODE != 4 ) ) && ( m_nSELFILLUMMODE == 2 ) ), "Invalid combo combination ( ( ( ENVMAPMODE != 2 ) && ( ENVMAPMODE != 4 ) ) && ( SELFILLUMMODE == 2 ) )" );
		AssertMsg( !( ( m_nSELFILLUMMODE == 1 ) && ( m_nXBYBASEALPHA != 0 ) ), "Invalid combo combination ( ( SELFILLUMMODE == 1 ) && ( XBYBASEALPHA != 0 ) )" );
		return ( 1 * m_nBLENDMODULATETEXTURE ) + ( 2 * m_nENVMAPMODE ) + ( 10 * m_nSELFILLUMMODE ) + ( 30 * m_nXBYBASEALPHA ) + ( 90 * m_nDETAILTEXTURE ) + 0;
	}
};

#define shaderStaticTest_lux_worldvertextransition_simple_ps30 psh_forgot_to_set_static_BLENDMODULATETEXTURE + psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_SELFILLUMMODE + psh_forgot_to_set_static_XBYBASEALPHA + psh_forgot_to_set_static_DETAILTEXTURE


class lux_worldvertextransition_simple_ps30_Dynamic_Index
{
public:
	lux_worldvertextransition_simple_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_worldvertextransition_simple_ps30 1

