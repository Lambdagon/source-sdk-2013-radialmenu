#pragma once
#include "shaderlib/cshader.h"
class lux_cable_vs30_Static_Index
{
	unsigned int m_nBASETRANSFORM : 2;
	unsigned int m_nBUMPMAPPING : 2;
	unsigned int m_nBUMPTRANSFORM : 2;
	unsigned int m_nTANGENTS : 2;
#ifdef _DEBUG
	bool m_bBASETRANSFORM : 1;
	bool m_bBUMPMAPPING : 1;
	bool m_bBUMPTRANSFORM : 1;
	bool m_bTANGENTS : 1;
#endif	// _DEBUG
public:
	void SetBASETRANSFORM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETRANSFORM = i;
#ifdef _DEBUG
		m_bBASETRANSFORM = true;
#endif	// _DEBUG
	}

	void SetBUMPMAPPING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAPPING = i;
#ifdef _DEBUG
		m_bBUMPMAPPING = true;
#endif	// _DEBUG
	}

	void SetBUMPTRANSFORM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPTRANSFORM = i;
#ifdef _DEBUG
		m_bBUMPTRANSFORM = true;
#endif	// _DEBUG
	}

	void SetTANGENTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTANGENTS = i;
#ifdef _DEBUG
		m_bTANGENTS = true;
#endif	// _DEBUG
	}

	lux_cable_vs30_Static_Index(  )
	{
		m_nBASETRANSFORM = 0;
		m_nBUMPMAPPING = 0;
		m_nBUMPTRANSFORM = 0;
		m_nTANGENTS = 0;
#ifdef _DEBUG
		m_bBASETRANSFORM = false;
		m_bBUMPMAPPING = false;
		m_bBUMPTRANSFORM = false;
		m_bTANGENTS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBASETRANSFORM && m_bBUMPMAPPING && m_bBUMPTRANSFORM && m_bTANGENTS );
		return ( 1 * m_nBASETRANSFORM ) + ( 2 * m_nBUMPMAPPING ) + ( 4 * m_nBUMPTRANSFORM ) + ( 8 * m_nTANGENTS ) + 0;
	}
};

#define shaderStaticTest_lux_cable_vs30 vsh_forgot_to_set_static_BASETRANSFORM + vsh_forgot_to_set_static_BUMPMAPPING + vsh_forgot_to_set_static_BUMPTRANSFORM + vsh_forgot_to_set_static_TANGENTS


class lux_cable_vs30_Dynamic_Index
{
public:
	lux_cable_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_cable_vs30 1

