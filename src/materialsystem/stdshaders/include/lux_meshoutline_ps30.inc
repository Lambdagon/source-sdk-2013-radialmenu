#pragma once
#include "shaderlib/cshader.h"
class lux_meshoutline_ps30_Static_Index
{
	unsigned int m_nENABLEFOG : 2;
	unsigned int m_nWRITETOALPHA : 2;
#ifdef _DEBUG
	bool m_bENABLEFOG : 1;
	bool m_bWRITETOALPHA : 1;
#endif	// _DEBUG
public:
	void SetENABLEFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENABLEFOG = i;
#ifdef _DEBUG
		m_bENABLEFOG = true;
#endif	// _DEBUG
	}

	void SetWRITETOALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRITETOALPHA = i;
#ifdef _DEBUG
		m_bWRITETOALPHA = true;
#endif	// _DEBUG
	}

	lux_meshoutline_ps30_Static_Index(  )
	{
		m_nENABLEFOG = 0;
		m_nWRITETOALPHA = 0;
#ifdef _DEBUG
		m_bENABLEFOG = false;
		m_bWRITETOALPHA = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bENABLEFOG && m_bWRITETOALPHA );
		return ( 1 * m_nENABLEFOG ) + ( 2 * m_nWRITETOALPHA ) + 0;
	}
};

#define shaderStaticTest_lux_meshoutline_ps30 psh_forgot_to_set_static_ENABLEFOG + psh_forgot_to_set_static_WRITETOALPHA


class lux_meshoutline_ps30_Dynamic_Index
{
public:
	lux_meshoutline_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_meshoutline_ps30 1

