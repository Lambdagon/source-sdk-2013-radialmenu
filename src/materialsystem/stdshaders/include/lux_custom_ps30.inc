// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($BRUSH != 0 && $AMBIENTCUBES != 0)
// ($BRUSH != 0 && $LIGHTDATA != 0)
// ($BRUSH == 0 && $ENVMAPCOMBO == 2)
// ($BRUSH != 0 && $NUM_LIGHTS != 0)
// ($BRUSH != 0 && $LIGHTMAPPED_MODEL != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_custom_ps30_Static_Index
{
	unsigned int m_nBRUSH : 2;
	unsigned int m_nAMBIENTCUBES : 2;
	unsigned int m_nLIGHTDATA : 2;
	unsigned int m_nENVMAPCOMBO : 2;
	unsigned int m_nBUMPMAPPED : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bBRUSH : 1;
	bool m_bAMBIENTCUBES : 1;
	bool m_bLIGHTDATA : 1;
	bool m_bENVMAPCOMBO : 1;
	bool m_bBUMPMAPPED : 1;
	bool m_bVERTEXCOLORS : 1;
#endif	// _DEBUG
public:
	void SetBRUSH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBRUSH = i;
#ifdef _DEBUG
		m_bBRUSH = true;
#endif	// _DEBUG
	}

	void SetAMBIENTCUBES( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nAMBIENTCUBES = i;
#ifdef _DEBUG
		m_bAMBIENTCUBES = true;
#endif	// _DEBUG
	}

	void SetLIGHTDATA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTDATA = i;
#ifdef _DEBUG
		m_bLIGHTDATA = true;
#endif	// _DEBUG
	}

	void SetENVMAPCOMBO( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nENVMAPCOMBO = i;
#ifdef _DEBUG
		m_bENVMAPCOMBO = true;
#endif	// _DEBUG
	}

	void SetBUMPMAPPED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAPPED = i;
#ifdef _DEBUG
		m_bBUMPMAPPED = true;
#endif	// _DEBUG
	}

	void SetVERTEXCOLORS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLORS = i;
#ifdef _DEBUG
		m_bVERTEXCOLORS = true;
#endif	// _DEBUG
	}

	lux_custom_ps30_Static_Index(  )
	{
		m_nBRUSH = 0;
		m_nAMBIENTCUBES = 0;
		m_nLIGHTDATA = 0;
		m_nENVMAPCOMBO = 0;
		m_nBUMPMAPPED = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bBRUSH = false;
		m_bAMBIENTCUBES = false;
		m_bLIGHTDATA = false;
		m_bENVMAPCOMBO = false;
		m_bBUMPMAPPED = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBRUSH && m_bAMBIENTCUBES && m_bLIGHTDATA && m_bENVMAPCOMBO && m_bBUMPMAPPED && m_bVERTEXCOLORS );
		AssertMsg( !( ( m_nBRUSH != 0 ) && ( m_nAMBIENTCUBES != 0 ) ), "Invalid combo combination ( ( BRUSH != 0 ) && ( AMBIENTCUBES != 0 ) )" );
		AssertMsg( !( ( m_nBRUSH != 0 ) && ( m_nLIGHTDATA != 0 ) ), "Invalid combo combination ( ( BRUSH != 0 ) && ( LIGHTDATA != 0 ) )" );
		AssertMsg( !( ( m_nBRUSH == 0 ) && ( m_nENVMAPCOMBO == 2 ) ), "Invalid combo combination ( ( BRUSH == 0 ) && ( ENVMAPCOMBO == 2 ) )" );
		return ( 10 * m_nBRUSH ) + ( 20 * m_nAMBIENTCUBES ) + ( 40 * m_nLIGHTDATA ) + ( 80 * m_nENVMAPCOMBO ) + ( 240 * m_nBUMPMAPPED ) + ( 480 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_custom_ps30 psh_forgot_to_set_static_BRUSH + psh_forgot_to_set_static_AMBIENTCUBES + psh_forgot_to_set_static_LIGHTDATA + psh_forgot_to_set_static_ENVMAPCOMBO + psh_forgot_to_set_static_BUMPMAPPED + psh_forgot_to_set_static_VERTEXCOLORS


class lux_custom_ps30_Dynamic_Index
{
	unsigned int m_nNUM_LIGHTS : 3;
	unsigned int m_nLIGHTMAPPED_MODEL : 2;
#ifdef _DEBUG
	bool m_bNUM_LIGHTS : 1;
	bool m_bLIGHTMAPPED_MODEL : 1;
#endif	// _DEBUG
public:
	void SetNUM_LIGHTS( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nNUM_LIGHTS = i;
#ifdef _DEBUG
		m_bNUM_LIGHTS = true;
#endif	// _DEBUG
	}

	void SetLIGHTMAPPED_MODEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPPED_MODEL = i;
#ifdef _DEBUG
		m_bLIGHTMAPPED_MODEL = true;
#endif	// _DEBUG
	}

	lux_custom_ps30_Dynamic_Index(  )
	{
		m_nNUM_LIGHTS = 0;
		m_nLIGHTMAPPED_MODEL = 0;
#ifdef _DEBUG
		m_bNUM_LIGHTS = false;
		m_bLIGHTMAPPED_MODEL = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNUM_LIGHTS && m_bLIGHTMAPPED_MODEL );
		return ( 1 * m_nNUM_LIGHTS ) + ( 5 * m_nLIGHTMAPPED_MODEL ) + 0;
	}
};

#define shaderDynamicTest_lux_custom_ps30 psh_forgot_to_set_dynamic_NUM_LIGHTS + psh_forgot_to_set_dynamic_LIGHTMAPPED_MODEL

