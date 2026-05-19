#pragma once
#include "shaderlib/cshader.h"
class lux_sheenpass_vs30_Static_Index
{
	unsigned int m_nBUMPMAPPED : 2;
#ifdef _DEBUG
	bool m_bBUMPMAPPED : 1;
#endif	// _DEBUG
public:
	void SetBUMPMAPPED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAPPED = i;
#ifdef _DEBUG
		m_bBUMPMAPPED = true;
#endif	// _DEBUG
	}

	lux_sheenpass_vs30_Static_Index(  )
	{
		m_nBUMPMAPPED = 0;
#ifdef _DEBUG
		m_bBUMPMAPPED = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBUMPMAPPED );
		return ( 4 * m_nBUMPMAPPED ) + 0;
	}
};

#define shaderStaticTest_lux_sheenpass_vs30 vsh_forgot_to_set_static_BUMPMAPPED


class lux_sheenpass_vs30_Dynamic_Index
{
	unsigned int m_nCOMPRESSION : 2;
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSION : 1;
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetCOMPRESSION( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOMPRESSION = i;
#ifdef _DEBUG
		m_bCOMPRESSION = true;
#endif	// _DEBUG
	}

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	lux_sheenpass_vs30_Dynamic_Index(  )
	{
		m_nCOMPRESSION = 0;
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bCOMPRESSION = false;
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSION && m_bSKINNING );
		return ( 1 * m_nCOMPRESSION ) + ( 2 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_lux_sheenpass_vs30 vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

