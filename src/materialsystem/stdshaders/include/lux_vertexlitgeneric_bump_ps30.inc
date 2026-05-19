// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $ENVMAPMODE == 0 && $ENVMAPLERP != 0 )
// ( $ENVMAPMODE != 2 && $SELFILLUMMODE == 2 )

#pragma once
#include "shaderlib/cshader.h"
class lux_vertexlitgeneric_bump_ps30_Static_Index
{
	unsigned int m_nENVMAPMODE : 2;
	unsigned int m_nENVMAPLERP : 2;
	unsigned int m_nSELFILLUMMODE : 2;
	unsigned int m_nXBYBASEALPHA : 2;
	unsigned int m_nDETAILTEXTURE : 2;
#ifdef _DEBUG
	bool m_bENVMAPMODE : 1;
	bool m_bENVMAPLERP : 1;
	bool m_bSELFILLUMMODE : 1;
	bool m_bXBYBASEALPHA : 1;
	bool m_bDETAILTEXTURE : 1;
#endif	// _DEBUG
public:
	void SetENVMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nENVMAPMODE = i;
#ifdef _DEBUG
		m_bENVMAPMODE = true;
#endif	// _DEBUG
	}

	void SetENVMAPLERP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAPLERP = i;
#ifdef _DEBUG
		m_bENVMAPLERP = true;
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
		Assert( i >= 0 && i <= 1 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
#endif	// _DEBUG
	}

	lux_vertexlitgeneric_bump_ps30_Static_Index(  )
	{
		m_nENVMAPMODE = 0;
		m_nENVMAPLERP = 0;
		m_nSELFILLUMMODE = 0;
		m_nXBYBASEALPHA = 0;
		m_nDETAILTEXTURE = 0;
#ifdef _DEBUG
		m_bENVMAPMODE = false;
		m_bENVMAPLERP = false;
		m_bSELFILLUMMODE = false;
		m_bXBYBASEALPHA = false;
		m_bDETAILTEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bENVMAPMODE && m_bENVMAPLERP && m_bSELFILLUMMODE && m_bXBYBASEALPHA && m_bDETAILTEXTURE );
		AssertMsg( !( ( m_nENVMAPMODE == 0 ) && ( m_nENVMAPLERP != 0 ) ), "Invalid combo combination ( ( ENVMAPMODE == 0 ) && ( ENVMAPLERP != 0 ) )" );
		AssertMsg( !( ( m_nENVMAPMODE != 2 ) && ( m_nSELFILLUMMODE == 2 ) ), "Invalid combo combination ( ( ENVMAPMODE != 2 ) && ( SELFILLUMMODE == 2 ) )" );
		return ( 5 * m_nENVMAPMODE ) + ( 15 * m_nENVMAPLERP ) + ( 30 * m_nSELFILLUMMODE ) + ( 90 * m_nXBYBASEALPHA ) + ( 270 * m_nDETAILTEXTURE ) + 0;
	}
};

#define shaderStaticTest_lux_vertexlitgeneric_bump_ps30 psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_ENVMAPLERP + psh_forgot_to_set_static_SELFILLUMMODE + psh_forgot_to_set_static_XBYBASEALPHA + psh_forgot_to_set_static_DETAILTEXTURE


class lux_vertexlitgeneric_bump_ps30_Dynamic_Index
{
	unsigned int m_nNUM_LIGHTS_COMBO : 3;
#ifdef _DEBUG
	bool m_bNUM_LIGHTS_COMBO : 1;
#endif	// _DEBUG
public:
	void SetNUM_LIGHTS_COMBO( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nNUM_LIGHTS_COMBO = i;
#ifdef _DEBUG
		m_bNUM_LIGHTS_COMBO = true;
#endif	// _DEBUG
	}

	lux_vertexlitgeneric_bump_ps30_Dynamic_Index(  )
	{
		m_nNUM_LIGHTS_COMBO = 0;
#ifdef _DEBUG
		m_bNUM_LIGHTS_COMBO = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNUM_LIGHTS_COMBO );
		return ( 1 * m_nNUM_LIGHTS_COMBO ) + 0;
	}
};

#define shaderDynamicTest_lux_vertexlitgeneric_bump_ps30 psh_forgot_to_set_dynamic_NUM_LIGHTS_COMBO

