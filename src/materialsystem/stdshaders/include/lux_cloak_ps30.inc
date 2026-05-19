#pragma once
#include "shaderlib/cshader.h"
class lux_cloak_ps30_Static_Index
{
	unsigned int m_nLINEARTOGAMMA : 2;
	unsigned int m_nBUMPMAPPED : 2;
#ifdef _DEBUG
	bool m_bLINEARTOGAMMA : 1;
	bool m_bBUMPMAPPED : 1;
#endif	// _DEBUG
public:
	void SetLINEARTOGAMMA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLINEARTOGAMMA = i;
#ifdef _DEBUG
		m_bLINEARTOGAMMA = true;
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

	lux_cloak_ps30_Static_Index(  )
	{
		m_nLINEARTOGAMMA = 0;
		m_nBUMPMAPPED = 0;
#ifdef _DEBUG
		m_bLINEARTOGAMMA = false;
		m_bBUMPMAPPED = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bLINEARTOGAMMA && m_bBUMPMAPPED );
		return ( 1 * m_nLINEARTOGAMMA ) + ( 2 * m_nBUMPMAPPED ) + 0;
	}
};

#define shaderStaticTest_lux_cloak_ps30 psh_forgot_to_set_static_LINEARTOGAMMA + psh_forgot_to_set_static_BUMPMAPPED


class lux_cloak_ps30_Dynamic_Index
{
public:
	lux_cloak_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_cloak_ps30 1

