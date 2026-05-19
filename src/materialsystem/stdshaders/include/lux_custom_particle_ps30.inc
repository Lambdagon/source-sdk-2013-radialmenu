#pragma once
#include "shaderlib/cshader.h"
class lux_custom_particle_ps30_Static_Index
{
	unsigned int m_nEXTRACTGREENALPHA : 2;
	unsigned int m_nADDBASETEXTURE2 : 2;
	unsigned int m_nDUALSEQUENCE : 2;
	unsigned int m_nALTERNATIVE_MODE : 3;
#ifdef _DEBUG
	bool m_bEXTRACTGREENALPHA : 1;
	bool m_bADDBASETEXTURE2 : 1;
	bool m_bDUALSEQUENCE : 1;
	bool m_bALTERNATIVE_MODE : 1;
#endif	// _DEBUG
public:
	void SetEXTRACTGREENALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nEXTRACTGREENALPHA = i;
#ifdef _DEBUG
		m_bEXTRACTGREENALPHA = true;
#endif	// _DEBUG
	}

	void SetADDBASETEXTURE2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nADDBASETEXTURE2 = i;
#ifdef _DEBUG
		m_bADDBASETEXTURE2 = true;
#endif	// _DEBUG
	}

	void SetDUALSEQUENCE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDUALSEQUENCE = i;
#ifdef _DEBUG
		m_bDUALSEQUENCE = true;
#endif	// _DEBUG
	}

	void SetALTERNATIVE_MODE( int i )
	{
		Assert( i >= 0 && i <= 5 );
		m_nALTERNATIVE_MODE = i;
#ifdef _DEBUG
		m_bALTERNATIVE_MODE = true;
#endif	// _DEBUG
	}

	lux_custom_particle_ps30_Static_Index(  )
	{
		m_nEXTRACTGREENALPHA = 0;
		m_nADDBASETEXTURE2 = 0;
		m_nDUALSEQUENCE = 0;
		m_nALTERNATIVE_MODE = 0;
#ifdef _DEBUG
		m_bEXTRACTGREENALPHA = false;
		m_bADDBASETEXTURE2 = false;
		m_bDUALSEQUENCE = false;
		m_bALTERNATIVE_MODE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bEXTRACTGREENALPHA && m_bADDBASETEXTURE2 && m_bDUALSEQUENCE && m_bALTERNATIVE_MODE );
		return ( 1 * m_nEXTRACTGREENALPHA ) + ( 2 * m_nADDBASETEXTURE2 ) + ( 4 * m_nDUALSEQUENCE ) + ( 8 * m_nALTERNATIVE_MODE ) + 0;
	}
};

#define shaderStaticTest_lux_custom_particle_ps30 psh_forgot_to_set_static_EXTRACTGREENALPHA + psh_forgot_to_set_static_ADDBASETEXTURE2 + psh_forgot_to_set_static_DUALSEQUENCE + psh_forgot_to_set_static_ALTERNATIVE_MODE


class lux_custom_particle_ps30_Dynamic_Index
{
public:
	lux_custom_particle_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_custom_particle_ps30 1

