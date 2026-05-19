// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $EFFECTMODE == 0 && $REFLECTIONLIGHTSCALE != 0)
// ( $EFFECTMODE == 2 && $REFLECTIONLIGHTSCALE != 0)
// ( $REFLECTIONLIGHTSCALE == 2 && $DIFFUSETEXTURE == 2)
// ( $DIFFUSETEXTURE == 0 && $DETAILTEXTURE == 1)

#pragma once
#include "shaderlib/cshader.h"
class lux_lightmappedreflective_ps30_Static_Index
{
	unsigned int m_nEFFECTMODE : 3;
	unsigned int m_nREFLECTIONLIGHTSCALE : 2;
	unsigned int m_nDIFFUSETEXTURE : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nEMM : 2;
#ifdef _DEBUG
	bool m_bEFFECTMODE : 1;
	bool m_bREFLECTIONLIGHTSCALE : 1;
	bool m_bDIFFUSETEXTURE : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bEMM : 1;
#endif	// _DEBUG
public:
	void SetEFFECTMODE( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nEFFECTMODE = i;
#ifdef _DEBUG
		m_bEFFECTMODE = true;
#endif	// _DEBUG
	}

	void SetREFLECTIONLIGHTSCALE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nREFLECTIONLIGHTSCALE = i;
#ifdef _DEBUG
		m_bREFLECTIONLIGHTSCALE = true;
#endif	// _DEBUG
	}

	void SetDIFFUSETEXTURE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nDIFFUSETEXTURE = i;
#ifdef _DEBUG
		m_bDIFFUSETEXTURE = true;
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

	void SetEMM( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nEMM = i;
#ifdef _DEBUG
		m_bEMM = true;
#endif	// _DEBUG
	}

	lux_lightmappedreflective_ps30_Static_Index(  )
	{
		m_nEFFECTMODE = 0;
		m_nREFLECTIONLIGHTSCALE = 0;
		m_nDIFFUSETEXTURE = 0;
		m_nDETAILTEXTURE = 0;
		m_nEMM = 0;
#ifdef _DEBUG
		m_bEFFECTMODE = false;
		m_bREFLECTIONLIGHTSCALE = false;
		m_bDIFFUSETEXTURE = false;
		m_bDETAILTEXTURE = false;
		m_bEMM = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bEFFECTMODE && m_bREFLECTIONLIGHTSCALE && m_bDIFFUSETEXTURE && m_bDETAILTEXTURE && m_bEMM );
		AssertMsg( !( ( m_nEFFECTMODE == 0 ) && ( m_nREFLECTIONLIGHTSCALE != 0 ) ), "Invalid combo combination ( ( EFFECTMODE == 0 ) && ( REFLECTIONLIGHTSCALE != 0 ) )" );
		AssertMsg( !( ( m_nEFFECTMODE == 2 ) && ( m_nREFLECTIONLIGHTSCALE != 0 ) ), "Invalid combo combination ( ( EFFECTMODE == 2 ) && ( REFLECTIONLIGHTSCALE != 0 ) )" );
		AssertMsg( !( ( m_nREFLECTIONLIGHTSCALE == 2 ) && ( m_nDIFFUSETEXTURE == 2 ) ), "Invalid combo combination ( ( REFLECTIONLIGHTSCALE == 2 ) && ( DIFFUSETEXTURE == 2 ) )" );
		AssertMsg( !( ( m_nDIFFUSETEXTURE == 0 ) && ( m_nDETAILTEXTURE == 1 ) ), "Invalid combo combination ( ( DIFFUSETEXTURE == 0 ) && ( DETAILTEXTURE == 1 ) )" );
		return ( 2 * m_nEFFECTMODE ) + ( 8 * m_nREFLECTIONLIGHTSCALE ) + ( 24 * m_nDIFFUSETEXTURE ) + ( 72 * m_nDETAILTEXTURE ) + ( 144 * m_nEMM ) + 0;
	}
};

#define shaderStaticTest_lux_lightmappedreflective_ps30 psh_forgot_to_set_static_EFFECTMODE + psh_forgot_to_set_static_REFLECTIONLIGHTSCALE + psh_forgot_to_set_static_DIFFUSETEXTURE + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_EMM


class lux_lightmappedreflective_ps30_Dynamic_Index
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

	lux_lightmappedreflective_ps30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_lightmappedreflective_ps30 psh_forgot_to_set_dynamic_BICUBIC_FILTERING

