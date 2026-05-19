#pragma once
#include "shaderlib/cshader.h"
class lux_displacement_vs30_Static_Index
{
	unsigned int m_nPROJTEX : 2;
	unsigned int m_nTEXCOORDS : 3;
	unsigned int m_nBLENDMODULATE_UV : 2;
	unsigned int m_nBUMPED_LIGHTMAP : 2;
	unsigned int m_nTANGENTS : 2;
#ifdef _DEBUG
	bool m_bPROJTEX : 1;
	bool m_bTEXCOORDS : 1;
	bool m_bBLENDMODULATE_UV : 1;
	bool m_bBUMPED_LIGHTMAP : 1;
	bool m_bTANGENTS : 1;
#endif	// _DEBUG
public:
	void SetPROJTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEX = i;
#ifdef _DEBUG
		m_bPROJTEX = true;
#endif	// _DEBUG
	}

	void SetTEXCOORDS( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nTEXCOORDS = i;
#ifdef _DEBUG
		m_bTEXCOORDS = true;
#endif	// _DEBUG
	}

	void SetBLENDMODULATE_UV( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLENDMODULATE_UV = i;
#ifdef _DEBUG
		m_bBLENDMODULATE_UV = true;
#endif	// _DEBUG
	}

	void SetBUMPED_LIGHTMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPED_LIGHTMAP = i;
#ifdef _DEBUG
		m_bBUMPED_LIGHTMAP = true;
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

	lux_displacement_vs30_Static_Index(  )
	{
		m_nPROJTEX = 0;
		m_nTEXCOORDS = 0;
		m_nBLENDMODULATE_UV = 0;
		m_nBUMPED_LIGHTMAP = 0;
		m_nTANGENTS = 0;
#ifdef _DEBUG
		m_bPROJTEX = false;
		m_bTEXCOORDS = false;
		m_bBLENDMODULATE_UV = false;
		m_bBUMPED_LIGHTMAP = false;
		m_bTANGENTS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEX && m_bTEXCOORDS && m_bBLENDMODULATE_UV && m_bBUMPED_LIGHTMAP && m_bTANGENTS );
		return ( 1 * m_nPROJTEX ) + ( 2 * m_nTEXCOORDS ) + ( 8 * m_nBLENDMODULATE_UV ) + ( 16 * m_nBUMPED_LIGHTMAP ) + ( 32 * m_nTANGENTS ) + 0;
	}
};

#define shaderStaticTest_lux_displacement_vs30 vsh_forgot_to_set_static_PROJTEX + vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_BLENDMODULATE_UV + vsh_forgot_to_set_static_BUMPED_LIGHTMAP + vsh_forgot_to_set_static_TANGENTS


class lux_displacement_vs30_Dynamic_Index
{
public:
	lux_displacement_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_displacement_vs30 1

