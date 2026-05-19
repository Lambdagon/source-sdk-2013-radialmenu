#pragma once
#include "shaderlib/cshader.h"
class lux_distancealpha_ps30_Static_Index
{
	unsigned int m_nMATERIAL_TYPE : 3;
	unsigned int m_nVERTEX_COLORS : 2;
	unsigned int m_nDETAILMODE : 3;
	unsigned int m_nSOFT_MASK : 2;
	unsigned int m_nOUTLINE : 2;
	unsigned int m_nOUTER_GLOW : 2;
#ifdef _DEBUG
	bool m_bMATERIAL_TYPE : 1;
	bool m_bVERTEX_COLORS : 1;
	bool m_bDETAILMODE : 1;
	bool m_bSOFT_MASK : 1;
	bool m_bOUTLINE : 1;
	bool m_bOUTER_GLOW : 1;
#endif	// _DEBUG
public:
	void SetMATERIAL_TYPE( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nMATERIAL_TYPE = i;
#ifdef _DEBUG
		m_bMATERIAL_TYPE = true;
#endif	// _DEBUG
	}

	void SetVERTEX_COLORS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEX_COLORS = i;
#ifdef _DEBUG
		m_bVERTEX_COLORS = true;
#endif	// _DEBUG
	}

	void SetDETAILMODE( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nDETAILMODE = i;
#ifdef _DEBUG
		m_bDETAILMODE = true;
#endif	// _DEBUG
	}

	void SetSOFT_MASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSOFT_MASK = i;
#ifdef _DEBUG
		m_bSOFT_MASK = true;
#endif	// _DEBUG
	}

	void SetOUTLINE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nOUTLINE = i;
#ifdef _DEBUG
		m_bOUTLINE = true;
#endif	// _DEBUG
	}

	void SetOUTER_GLOW( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nOUTER_GLOW = i;
#ifdef _DEBUG
		m_bOUTER_GLOW = true;
#endif	// _DEBUG
	}

	lux_distancealpha_ps30_Static_Index(  )
	{
		m_nMATERIAL_TYPE = 0;
		m_nVERTEX_COLORS = 0;
		m_nDETAILMODE = 0;
		m_nSOFT_MASK = 0;
		m_nOUTLINE = 0;
		m_nOUTER_GLOW = 0;
#ifdef _DEBUG
		m_bMATERIAL_TYPE = false;
		m_bVERTEX_COLORS = false;
		m_bDETAILMODE = false;
		m_bSOFT_MASK = false;
		m_bOUTLINE = false;
		m_bOUTER_GLOW = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMATERIAL_TYPE && m_bVERTEX_COLORS && m_bDETAILMODE && m_bSOFT_MASK && m_bOUTLINE && m_bOUTER_GLOW );
		return ( 4 * m_nMATERIAL_TYPE ) + ( 16 * m_nVERTEX_COLORS ) + ( 32 * m_nDETAILMODE ) + ( 128 * m_nSOFT_MASK ) + ( 256 * m_nOUTLINE ) + ( 512 * m_nOUTER_GLOW ) + 0;
	}
};

#define shaderStaticTest_lux_distancealpha_ps30 psh_forgot_to_set_static_MATERIAL_TYPE + psh_forgot_to_set_static_VERTEX_COLORS + psh_forgot_to_set_static_DETAILMODE + psh_forgot_to_set_static_SOFT_MASK + psh_forgot_to_set_static_OUTLINE + psh_forgot_to_set_static_OUTER_GLOW


class lux_distancealpha_ps30_Dynamic_Index
{
	unsigned int m_nLIGHTMAPPED_MODEL : 2;
	unsigned int m_nPROJTEXSHADOWS : 2;
#ifdef _DEBUG
	bool m_bLIGHTMAPPED_MODEL : 1;
	bool m_bPROJTEXSHADOWS : 1;
#endif	// _DEBUG
public:
	void SetLIGHTMAPPED_MODEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPPED_MODEL = i;
#ifdef _DEBUG
		m_bLIGHTMAPPED_MODEL = true;
#endif	// _DEBUG
	}

	void SetPROJTEXSHADOWS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEXSHADOWS = i;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = true;
#endif	// _DEBUG
	}

	lux_distancealpha_ps30_Dynamic_Index(  )
	{
		m_nLIGHTMAPPED_MODEL = 0;
		m_nPROJTEXSHADOWS = 0;
#ifdef _DEBUG
		m_bLIGHTMAPPED_MODEL = false;
		m_bPROJTEXSHADOWS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bLIGHTMAPPED_MODEL && m_bPROJTEXSHADOWS );
		return ( 1 * m_nLIGHTMAPPED_MODEL ) + ( 2 * m_nPROJTEXSHADOWS ) + 0;
	}
};

#define shaderDynamicTest_lux_distancealpha_ps30 psh_forgot_to_set_dynamic_LIGHTMAPPED_MODEL + psh_forgot_to_set_dynamic_PROJTEXSHADOWS

