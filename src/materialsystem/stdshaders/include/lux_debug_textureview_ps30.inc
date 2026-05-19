#pragma once
#include "shaderlib/cshader.h"
class lux_debug_textureview_ps30_Static_Index
{
	unsigned int m_nSHOWALPHA : 2;
#ifdef _DEBUG
	bool m_bSHOWALPHA : 1;
#endif	// _DEBUG
public:
	void SetSHOWALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSHOWALPHA = i;
#ifdef _DEBUG
		m_bSHOWALPHA = true;
#endif	// _DEBUG
	}

	lux_debug_textureview_ps30_Static_Index(  )
	{
		m_nSHOWALPHA = 0;
#ifdef _DEBUG
		m_bSHOWALPHA = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSHOWALPHA );
		return ( 9 * m_nSHOWALPHA ) + 0;
	}
};

#define shaderStaticTest_lux_debug_textureview_ps30 psh_forgot_to_set_static_SHOWALPHA


class lux_debug_textureview_ps30_Dynamic_Index
{
	unsigned int m_nCUBEMAPMODE : 2;
	unsigned int m_nHDRTEXTUREMODE : 2;
#ifdef _DEBUG
	bool m_bCUBEMAPMODE : 1;
	bool m_bHDRTEXTUREMODE : 1;
#endif	// _DEBUG
public:
	void SetCUBEMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nCUBEMAPMODE = i;
#ifdef _DEBUG
		m_bCUBEMAPMODE = true;
#endif	// _DEBUG
	}

	void SetHDRTEXTUREMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nHDRTEXTUREMODE = i;
#ifdef _DEBUG
		m_bHDRTEXTUREMODE = true;
#endif	// _DEBUG
	}

	lux_debug_textureview_ps30_Dynamic_Index(  )
	{
		m_nCUBEMAPMODE = 0;
		m_nHDRTEXTUREMODE = 0;
#ifdef _DEBUG
		m_bCUBEMAPMODE = false;
		m_bHDRTEXTUREMODE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCUBEMAPMODE && m_bHDRTEXTUREMODE );
		return ( 1 * m_nCUBEMAPMODE ) + ( 3 * m_nHDRTEXTUREMODE ) + 0;
	}
};

#define shaderDynamicTest_lux_debug_textureview_ps30 psh_forgot_to_set_dynamic_CUBEMAPMODE + psh_forgot_to_set_dynamic_HDRTEXTUREMODE

