#pragma once
#include "shaderlib/cshader.h"
class lux_custom_projtex_ps30_Static_Index
{
	unsigned int m_nPROJTEXFILTER : 2;
	unsigned int m_nBRUSH : 2;
	unsigned int m_nBUMPMAPPED : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bPROJTEXFILTER : 1;
	bool m_bBRUSH : 1;
	bool m_bBUMPMAPPED : 1;
	bool m_bVERTEXCOLORS : 1;
#endif	// _DEBUG
public:
	void SetPROJTEXFILTER( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nPROJTEXFILTER = i;
#ifdef _DEBUG
		m_bPROJTEXFILTER = true;
#endif	// _DEBUG
	}

	void SetBRUSH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBRUSH = i;
#ifdef _DEBUG
		m_bBRUSH = true;
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

	void SetVERTEXCOLORS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLORS = i;
#ifdef _DEBUG
		m_bVERTEXCOLORS = true;
#endif	// _DEBUG
	}

	lux_custom_projtex_ps30_Static_Index(  )
	{
		m_nPROJTEXFILTER = 0;
		m_nBRUSH = 0;
		m_nBUMPMAPPED = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bPROJTEXFILTER = false;
		m_bBRUSH = false;
		m_bBUMPMAPPED = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEXFILTER && m_bBRUSH && m_bBUMPMAPPED && m_bVERTEXCOLORS );
		return ( 2 * m_nPROJTEXFILTER ) + ( 6 * m_nBRUSH ) + ( 12 * m_nBUMPMAPPED ) + ( 24 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_custom_projtex_ps30 psh_forgot_to_set_static_PROJTEXFILTER + psh_forgot_to_set_static_BRUSH + psh_forgot_to_set_static_BUMPMAPPED + psh_forgot_to_set_static_VERTEXCOLORS


class lux_custom_projtex_ps30_Dynamic_Index
{
	unsigned int m_nPROJTEXSHADOWS : 2;
#ifdef _DEBUG
	bool m_bPROJTEXSHADOWS : 1;
#endif	// _DEBUG
public:
	void SetPROJTEXSHADOWS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEXSHADOWS = i;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = true;
#endif	// _DEBUG
	}

	lux_custom_projtex_ps30_Dynamic_Index(  )
	{
		m_nPROJTEXSHADOWS = 0;
#ifdef _DEBUG
		m_bPROJTEXSHADOWS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEXSHADOWS );
		return ( 1 * m_nPROJTEXSHADOWS ) + 0;
	}
};

#define shaderDynamicTest_lux_custom_projtex_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS

