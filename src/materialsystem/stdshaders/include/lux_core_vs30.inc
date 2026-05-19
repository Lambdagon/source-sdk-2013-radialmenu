#pragma once
#include "shaderlib/cshader.h"
class lux_core_vs30_Static_Index
{
public:
	lux_core_vs30_Static_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_lux_core_vs30 1


class lux_core_vs30_Dynamic_Index
{
	unsigned int m_nSKINNING : 2;
	unsigned int m_nCOMPRESSION : 2;
#ifdef _DEBUG
	bool m_bSKINNING : 1;
	bool m_bCOMPRESSION : 1;
#endif	// _DEBUG
public:
	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	void SetCOMPRESSION( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOMPRESSION = i;
#ifdef _DEBUG
		m_bCOMPRESSION = true;
#endif	// _DEBUG
	}

	lux_core_vs30_Dynamic_Index(  )
	{
		m_nSKINNING = 0;
		m_nCOMPRESSION = 0;
#ifdef _DEBUG
		m_bSKINNING = false;
		m_bCOMPRESSION = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSKINNING && m_bCOMPRESSION );
		return ( 1 * m_nSKINNING ) + ( 2 * m_nCOMPRESSION ) + 0;
	}
};

#define shaderDynamicTest_lux_core_vs30 vsh_forgot_to_set_dynamic_SKINNING + vsh_forgot_to_set_dynamic_COMPRESSION

