// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $PROJTEX == 0 && $PROJTEXSHADOWS == 1 )

#pragma once
#include "shaderlib/cshader.h"
class lux_water_expensive_ps30_Static_Index
{
	unsigned int m_nPROJTEX : 2;
	unsigned int m_nFLOWMAPPING_MULTITEXTURE : 3;
	unsigned int m_nBASETEXTURELAYER : 2;
	unsigned int m_nREFRACT : 2;
	unsigned int m_nLIGHTMAPWATERFOG : 2;
	unsigned int m_nREFLECT : 2;
	unsigned int m_nABOVEWATER : 2;
#ifdef _DEBUG
	bool m_bPROJTEX : 1;
	bool m_bFLOWMAPPING_MULTITEXTURE : 1;
	bool m_bBASETEXTURELAYER : 1;
	bool m_bREFRACT : 1;
	bool m_bLIGHTMAPWATERFOG : 1;
	bool m_bREFLECT : 1;
	bool m_bABOVEWATER : 1;
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

	void SetFLOWMAPPING_MULTITEXTURE( int i )
	{
		Assert( i >= 0 && i <= 5 );
		m_nFLOWMAPPING_MULTITEXTURE = i;
#ifdef _DEBUG
		m_bFLOWMAPPING_MULTITEXTURE = true;
#endif	// _DEBUG
	}

	void SetBASETEXTURELAYER( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nBASETEXTURELAYER = i;
#ifdef _DEBUG
		m_bBASETEXTURELAYER = true;
#endif	// _DEBUG
	}

	void SetREFRACT( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nREFRACT = i;
#ifdef _DEBUG
		m_bREFRACT = true;
#endif	// _DEBUG
	}

	void SetLIGHTMAPWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPWATERFOG = i;
#ifdef _DEBUG
		m_bLIGHTMAPWATERFOG = true;
#endif	// _DEBUG
	}

	void SetREFLECT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFLECT = i;
#ifdef _DEBUG
		m_bREFLECT = true;
#endif	// _DEBUG
	}

	void SetABOVEWATER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nABOVEWATER = i;
#ifdef _DEBUG
		m_bABOVEWATER = true;
#endif	// _DEBUG
	}

	lux_water_expensive_ps30_Static_Index(  )
	{
		m_nPROJTEX = 0;
		m_nFLOWMAPPING_MULTITEXTURE = 0;
		m_nBASETEXTURELAYER = 0;
		m_nREFRACT = 0;
		m_nLIGHTMAPWATERFOG = 0;
		m_nREFLECT = 0;
		m_nABOVEWATER = 0;
#ifdef _DEBUG
		m_bPROJTEX = false;
		m_bFLOWMAPPING_MULTITEXTURE = false;
		m_bBASETEXTURELAYER = false;
		m_bREFRACT = false;
		m_bLIGHTMAPWATERFOG = false;
		m_bREFLECT = false;
		m_bABOVEWATER = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEX && m_bFLOWMAPPING_MULTITEXTURE && m_bBASETEXTURELAYER && m_bREFRACT && m_bLIGHTMAPWATERFOG && m_bREFLECT && m_bABOVEWATER );
		return ( 2 * m_nPROJTEX ) + ( 4 * m_nFLOWMAPPING_MULTITEXTURE ) + ( 24 * m_nBASETEXTURELAYER ) + ( 72 * m_nREFRACT ) + ( 216 * m_nLIGHTMAPWATERFOG ) + ( 432 * m_nREFLECT ) + ( 864 * m_nABOVEWATER ) + 0;
	}
};

#define shaderStaticTest_lux_water_expensive_ps30 psh_forgot_to_set_static_PROJTEX + psh_forgot_to_set_static_FLOWMAPPING_MULTITEXTURE + psh_forgot_to_set_static_BASETEXTURELAYER + psh_forgot_to_set_static_REFRACT + psh_forgot_to_set_static_LIGHTMAPWATERFOG + psh_forgot_to_set_static_REFLECT + psh_forgot_to_set_static_ABOVEWATER


class lux_water_expensive_ps30_Dynamic_Index
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

	lux_water_expensive_ps30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_water_expensive_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS

