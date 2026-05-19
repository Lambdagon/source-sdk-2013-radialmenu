// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $ENVMAPMODE != 2 && $SELFILLUMMODE == 2 )
// ( $ENVMAPMODE != 4 && $SELFILLUMMODE == 2 )
// ( $SELFILLUMMODE == 1 && $XBYBASEALPHA != 0 )

#pragma once
#include "shaderlib/cshader.h"
class lux_lightmappedgeneric_phong_ps30_Static_Index
{
	unsigned int m_nENVMAPMODE : 3;
	unsigned int m_nSELFILLUMMODE : 2;
	unsigned int m_nXBYBASEALPHA : 2;
	unsigned int m_nSSBUMP : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nEXPONENTTEXTURE : 2;
#ifdef _DEBUG
	bool m_bENVMAPMODE : 1;
	bool m_bSELFILLUMMODE : 1;
	bool m_bXBYBASEALPHA : 1;
	bool m_bSSBUMP : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bEXPONENTTEXTURE : 1;
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

	void SetSSBUMP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSSBUMP = i;
#ifdef _DEBUG
		m_bSSBUMP = true;
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

	void SetEXPONENTTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nEXPONENTTEXTURE = i;
#ifdef _DEBUG
		m_bEXPONENTTEXTURE = true;
#endif	// _DEBUG
	}

	lux_lightmappedgeneric_phong_ps30_Static_Index(  )
	{
		m_nENVMAPMODE = 0;
		m_nSELFILLUMMODE = 0;
		m_nXBYBASEALPHA = 0;
		m_nSSBUMP = 0;
		m_nDETAILTEXTURE = 0;
		m_nEXPONENTTEXTURE = 0;
#ifdef _DEBUG
		m_bENVMAPMODE = false;
		m_bSELFILLUMMODE = false;
		m_bXBYBASEALPHA = false;
		m_bSSBUMP = false;
		m_bDETAILTEXTURE = false;
		m_bEXPONENTTEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bENVMAPMODE && m_bSELFILLUMMODE && m_bXBYBASEALPHA && m_bSSBUMP && m_bDETAILTEXTURE && m_bEXPONENTTEXTURE );
		AssertMsg( !( ( m_nENVMAPMODE != 2 ) && ( m_nSELFILLUMMODE == 2 ) ), "Invalid combo combination ( ( ENVMAPMODE != 2 ) && ( SELFILLUMMODE == 2 ) )" );
		AssertMsg( !( ( m_nENVMAPMODE != 4 ) && ( m_nSELFILLUMMODE == 2 ) ), "Invalid combo combination ( ( ENVMAPMODE != 4 ) && ( SELFILLUMMODE == 2 ) )" );
		AssertMsg( !( ( m_nSELFILLUMMODE == 1 ) && ( m_nXBYBASEALPHA != 0 ) ), "Invalid combo combination ( ( SELFILLUMMODE == 1 ) && ( XBYBASEALPHA != 0 ) )" );
		return ( 2 * m_nENVMAPMODE ) + ( 10 * m_nSELFILLUMMODE ) + ( 30 * m_nXBYBASEALPHA ) + ( 90 * m_nSSBUMP ) + ( 180 * m_nDETAILTEXTURE ) + ( 360 * m_nEXPONENTTEXTURE ) + 0;
	}
};

#define shaderStaticTest_lux_lightmappedgeneric_phong_ps30 psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_SELFILLUMMODE + psh_forgot_to_set_static_XBYBASEALPHA + psh_forgot_to_set_static_SSBUMP + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_EXPONENTTEXTURE


class lux_lightmappedgeneric_phong_ps30_Dynamic_Index
{
	unsigned int m_nBICUBIC_FILTERING : 2;
#ifdef _DEBUG
	bool m_bBICUBIC_FILTERING : 1;
#endif	// _DEBUG
public:
	void SetBICUBIC_FILTERING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBICUBIC_FILTERING = i;
#ifdef _DEBUG
		m_bBICUBIC_FILTERING = true;
#endif	// _DEBUG
	}

	lux_lightmappedgeneric_phong_ps30_Dynamic_Index(  )
	{
		m_nBICUBIC_FILTERING = 0;
#ifdef _DEBUG
		m_bBICUBIC_FILTERING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBICUBIC_FILTERING );
		return ( 1 * m_nBICUBIC_FILTERING ) + 0;
	}
};

#define shaderDynamicTest_lux_lightmappedgeneric_phong_ps30 psh_forgot_to_set_dynamic_BICUBIC_FILTERING

