#pragma once
#include "shaderlib/cshader.h"
class lux_sky_hdri_ps30_Static_Index
{
	unsigned int m_nMODE : 2;
	unsigned int m_nHALF : 2;
	unsigned int m_nSTRETCH : 2;
	unsigned int m_nTOSRGB : 2;
	unsigned int m_nINVERTX : 2;
#ifdef _DEBUG
	bool m_bMODE : 1;
	bool m_bHALF : 1;
	bool m_bSTRETCH : 1;
	bool m_bTOSRGB : 1;
	bool m_bINVERTX : 1;
#endif	// _DEBUG
public:
	void SetMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nMODE = i;
#ifdef _DEBUG
		m_bMODE = true;
#endif	// _DEBUG
	}

	void SetHALF( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHALF = i;
#ifdef _DEBUG
		m_bHALF = true;
#endif	// _DEBUG
	}

	void SetSTRETCH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSTRETCH = i;
#ifdef _DEBUG
		m_bSTRETCH = true;
#endif	// _DEBUG
	}

	void SetTOSRGB( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTOSRGB = i;
#ifdef _DEBUG
		m_bTOSRGB = true;
#endif	// _DEBUG
	}

	void SetINVERTX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nINVERTX = i;
#ifdef _DEBUG
		m_bINVERTX = true;
#endif	// _DEBUG
	}

	lux_sky_hdri_ps30_Static_Index(  )
	{
		m_nMODE = 0;
		m_nHALF = 0;
		m_nSTRETCH = 0;
		m_nTOSRGB = 0;
		m_nINVERTX = 0;
#ifdef _DEBUG
		m_bMODE = false;
		m_bHALF = false;
		m_bSTRETCH = false;
		m_bTOSRGB = false;
		m_bINVERTX = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMODE && m_bHALF && m_bSTRETCH && m_bTOSRGB && m_bINVERTX );
		return ( 1 * m_nMODE ) + ( 3 * m_nHALF ) + ( 6 * m_nSTRETCH ) + ( 12 * m_nTOSRGB ) + ( 24 * m_nINVERTX ) + 0;
	}
};

#define shaderStaticTest_lux_sky_hdri_ps30 psh_forgot_to_set_static_MODE + psh_forgot_to_set_static_HALF + psh_forgot_to_set_static_STRETCH + psh_forgot_to_set_static_TOSRGB + psh_forgot_to_set_static_INVERTX


class lux_sky_hdri_ps30_Dynamic_Index
{
public:
	lux_sky_hdri_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_sky_hdri_ps30 1

