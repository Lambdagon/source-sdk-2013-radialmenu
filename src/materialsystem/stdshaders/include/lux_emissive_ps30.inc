// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($EMISSIVEBLEND == 0 && $NOFLOW == 1)

#pragma once
#include "shaderlib/cshader.h"
class lux_emissive_ps30_Static_Index
{
	unsigned int m_nEMISSIVEBLEND : 3;
	unsigned int m_nNOFLOW : 2;
	unsigned int m_nDETAILMODE : 2;
	unsigned int m_nSELFILLUMTEXTURE : 2;
	unsigned int m_nBASETEXTUREMODES : 3;
#ifdef _DEBUG
	bool m_bEMISSIVEBLEND : 1;
	bool m_bNOFLOW : 1;
	bool m_bDETAILMODE : 1;
	bool m_bSELFILLUMTEXTURE : 1;
	bool m_bBASETEXTUREMODES : 1;
#endif	// _DEBUG
public:
	void SetEMISSIVEBLEND( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nEMISSIVEBLEND = i;
#ifdef _DEBUG
		m_bEMISSIVEBLEND = true;
#endif	// _DEBUG
	}

	void SetNOFLOW( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNOFLOW = i;
#ifdef _DEBUG
		m_bNOFLOW = true;
#endif	// _DEBUG
	}

	void SetDETAILMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nDETAILMODE = i;
#ifdef _DEBUG
		m_bDETAILMODE = true;
#endif	// _DEBUG
	}

	void SetSELFILLUMTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSELFILLUMTEXTURE = i;
#ifdef _DEBUG
		m_bSELFILLUMTEXTURE = true;
#endif	// _DEBUG
	}

	void SetBASETEXTUREMODES( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nBASETEXTUREMODES = i;
#ifdef _DEBUG
		m_bBASETEXTUREMODES = true;
#endif	// _DEBUG
	}

	lux_emissive_ps30_Static_Index(  )
	{
		m_nEMISSIVEBLEND = 0;
		m_nNOFLOW = 0;
		m_nDETAILMODE = 0;
		m_nSELFILLUMTEXTURE = 0;
		m_nBASETEXTUREMODES = 0;
#ifdef _DEBUG
		m_bEMISSIVEBLEND = false;
		m_bNOFLOW = false;
		m_bDETAILMODE = false;
		m_bSELFILLUMTEXTURE = false;
		m_bBASETEXTUREMODES = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bEMISSIVEBLEND && m_bNOFLOW && m_bDETAILMODE && m_bSELFILLUMTEXTURE && m_bBASETEXTUREMODES );
		AssertMsg( !( ( m_nEMISSIVEBLEND == 0 ) && ( m_nNOFLOW == 1 ) ), "Invalid combo combination ( ( EMISSIVEBLEND == 0 ) && ( NOFLOW == 1 ) )" );
		return ( 1 * m_nEMISSIVEBLEND ) + ( 5 * m_nNOFLOW ) + ( 10 * m_nDETAILMODE ) + ( 30 * m_nSELFILLUMTEXTURE ) + ( 60 * m_nBASETEXTUREMODES ) + 0;
	}
};

#define shaderStaticTest_lux_emissive_ps30 psh_forgot_to_set_static_EMISSIVEBLEND + psh_forgot_to_set_static_NOFLOW + psh_forgot_to_set_static_DETAILMODE + psh_forgot_to_set_static_SELFILLUMTEXTURE + psh_forgot_to_set_static_BASETEXTUREMODES


class lux_emissive_ps30_Dynamic_Index
{
public:
	lux_emissive_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_emissive_ps30 1

