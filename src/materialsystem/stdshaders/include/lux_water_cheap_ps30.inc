// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $FLOWMAPPING == 0 && $FLOWSTOP == 1)
// ( $FLOWMAPPING > 0 && $MULTITEXTURE == 1 )

#pragma once
#include "shaderlib/cshader.h"
class lux_water_cheap_ps30_Static_Index
{
	unsigned int m_nFLOWMAPPING : 2;
	unsigned int m_nFLOWSTOP : 2;
	unsigned int m_nMULTITEXTURE : 2;
	unsigned int m_nBASETEXTURELAYER : 2;
	unsigned int m_nLIGHTMAPWATERFOG : 2;
	unsigned int m_nBLEND : 2;
	unsigned int m_nPARALLAXCORRECT : 2;
	unsigned int m_nPRETTYHAMMER : 2;
#ifdef _DEBUG
	bool m_bFLOWMAPPING : 1;
	bool m_bFLOWSTOP : 1;
	bool m_bMULTITEXTURE : 1;
	bool m_bBASETEXTURELAYER : 1;
	bool m_bLIGHTMAPWATERFOG : 1;
	bool m_bBLEND : 1;
	bool m_bPARALLAXCORRECT : 1;
	bool m_bPRETTYHAMMER : 1;
#endif	// _DEBUG
public:
	void SetFLOWMAPPING( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nFLOWMAPPING = i;
#ifdef _DEBUG
		m_bFLOWMAPPING = true;
#endif	// _DEBUG
	}

	void SetFLOWSTOP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLOWSTOP = i;
#ifdef _DEBUG
		m_bFLOWSTOP = true;
#endif	// _DEBUG
	}

	void SetMULTITEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMULTITEXTURE = i;
#ifdef _DEBUG
		m_bMULTITEXTURE = true;
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

	void SetLIGHTMAPWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPWATERFOG = i;
#ifdef _DEBUG
		m_bLIGHTMAPWATERFOG = true;
#endif	// _DEBUG
	}

	void SetBLEND( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nBLEND = i;
#ifdef _DEBUG
		m_bBLEND = true;
#endif	// _DEBUG
	}

	void SetPARALLAXCORRECT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPARALLAXCORRECT = i;
#ifdef _DEBUG
		m_bPARALLAXCORRECT = true;
#endif	// _DEBUG
	}

	void SetPRETTYHAMMER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPRETTYHAMMER = i;
#ifdef _DEBUG
		m_bPRETTYHAMMER = true;
#endif	// _DEBUG
	}

	lux_water_cheap_ps30_Static_Index(  )
	{
		m_nFLOWMAPPING = 0;
		m_nFLOWSTOP = 0;
		m_nMULTITEXTURE = 0;
		m_nBASETEXTURELAYER = 0;
		m_nLIGHTMAPWATERFOG = 0;
		m_nBLEND = 0;
		m_nPARALLAXCORRECT = 0;
		m_nPRETTYHAMMER = 0;
#ifdef _DEBUG
		m_bFLOWMAPPING = false;
		m_bFLOWSTOP = false;
		m_bMULTITEXTURE = false;
		m_bBASETEXTURELAYER = false;
		m_bLIGHTMAPWATERFOG = false;
		m_bBLEND = false;
		m_bPARALLAXCORRECT = false;
		m_bPRETTYHAMMER = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFLOWMAPPING && m_bFLOWSTOP && m_bMULTITEXTURE && m_bBASETEXTURELAYER && m_bLIGHTMAPWATERFOG && m_bBLEND && m_bPARALLAXCORRECT && m_bPRETTYHAMMER );
		AssertMsg( !( ( m_nFLOWMAPPING == 0 ) && ( m_nFLOWSTOP == 1 ) ), "Invalid combo combination ( ( FLOWMAPPING == 0 ) && ( FLOWSTOP == 1 ) )" );
		AssertMsg( !( ( m_nFLOWMAPPING > 0 ) && ( m_nMULTITEXTURE == 1 ) ), "Invalid combo combination ( ( FLOWMAPPING > 0 ) && ( MULTITEXTURE == 1 ) )" );
		return ( 1 * m_nFLOWMAPPING ) + ( 3 * m_nFLOWSTOP ) + ( 6 * m_nMULTITEXTURE ) + ( 12 * m_nBASETEXTURELAYER ) + ( 36 * m_nLIGHTMAPWATERFOG ) + ( 72 * m_nBLEND ) + ( 216 * m_nPARALLAXCORRECT ) + ( 432 * m_nPRETTYHAMMER ) + 0;
	}
};

#define shaderStaticTest_lux_water_cheap_ps30 psh_forgot_to_set_static_FLOWMAPPING + psh_forgot_to_set_static_FLOWSTOP + psh_forgot_to_set_static_MULTITEXTURE + psh_forgot_to_set_static_BASETEXTURELAYER + psh_forgot_to_set_static_LIGHTMAPWATERFOG + psh_forgot_to_set_static_BLEND + psh_forgot_to_set_static_PARALLAXCORRECT + psh_forgot_to_set_static_PRETTYHAMMER


class lux_water_cheap_ps30_Dynamic_Index
{
public:
	lux_water_cheap_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_water_cheap_ps30 1

