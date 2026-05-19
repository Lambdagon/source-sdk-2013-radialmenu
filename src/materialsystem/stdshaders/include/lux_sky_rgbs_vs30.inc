#pragma once
#include "shaderlib/cshader.h"
class lux_sky_rgbs_vs30_Static_Index
{
	unsigned int m_nUSEMODELMATRIX : 2;
#ifdef _DEBUG
	bool m_bUSEMODELMATRIX : 1;
#endif	// _DEBUG
public:
	void SetUSEMODELMATRIX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nUSEMODELMATRIX = i;
#ifdef _DEBUG
		m_bUSEMODELMATRIX = true;
#endif	// _DEBUG
	}

	lux_sky_rgbs_vs30_Static_Index(  )
	{
		m_nUSEMODELMATRIX = 0;
#ifdef _DEBUG
		m_bUSEMODELMATRIX = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bUSEMODELMATRIX );
		return ( 3 * m_nUSEMODELMATRIX ) + 0;
	}
};

#define shaderStaticTest_lux_sky_rgbs_vs30 vsh_forgot_to_set_static_USEMODELMATRIX


class lux_sky_rgbs_vs30_Dynamic_Index
{
	unsigned int m_nFILTER : 2;
#ifdef _DEBUG
	bool m_bFILTER : 1;
#endif	// _DEBUG
public:
	void SetFILTER( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nFILTER = i;
#ifdef _DEBUG
		m_bFILTER = true;
#endif	// _DEBUG
	}

	lux_sky_rgbs_vs30_Dynamic_Index(  )
	{
		m_nFILTER = 0;
#ifdef _DEBUG
		m_bFILTER = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFILTER );
		return ( 1 * m_nFILTER ) + 0;
	}
};

#define shaderDynamicTest_lux_sky_rgbs_vs30 vsh_forgot_to_set_dynamic_FILTER

