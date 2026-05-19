#pragma once
#include "shaderlib/cshader.h"
class lux_infected_vs30_Static_Index
{
	unsigned int m_nPROJTEX : 2;
	unsigned int m_nDETAILTEXTURE_UV : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bPROJTEX : 1;
	bool m_bDETAILTEXTURE_UV : 1;
	bool m_bVERTEXCOLORS : 1;
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

	void SetDETAILTEXTURE_UV( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAILTEXTURE_UV = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE_UV = true;
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

	lux_infected_vs30_Static_Index(  )
	{
		m_nPROJTEX = 0;
		m_nDETAILTEXTURE_UV = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bPROJTEX = false;
		m_bDETAILTEXTURE_UV = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPROJTEX && m_bDETAILTEXTURE_UV && m_bVERTEXCOLORS );
		return ( 16 * m_nPROJTEX ) + ( 32 * m_nDETAILTEXTURE_UV ) + ( 64 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_infected_vs30 vsh_forgot_to_set_static_PROJTEX + vsh_forgot_to_set_static_DETAILTEXTURE_UV + vsh_forgot_to_set_static_VERTEXCOLORS


class lux_infected_vs30_Dynamic_Index
{
	unsigned int m_nELLIPSOIDCULLING : 3;
	unsigned int m_nCOMPRESSION : 2;
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bELLIPSOIDCULLING : 1;
	bool m_bCOMPRESSION : 1;
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetELLIPSOIDCULLING( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nELLIPSOIDCULLING = i;
#ifdef _DEBUG
		m_bELLIPSOIDCULLING = true;
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

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	lux_infected_vs30_Dynamic_Index(  )
	{
		m_nELLIPSOIDCULLING = 0;
		m_nCOMPRESSION = 0;
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bELLIPSOIDCULLING = false;
		m_bCOMPRESSION = false;
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bELLIPSOIDCULLING && m_bCOMPRESSION && m_bSKINNING );
		return ( 1 * m_nELLIPSOIDCULLING ) + ( 4 * m_nCOMPRESSION ) + ( 8 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_lux_infected_vs30 vsh_forgot_to_set_dynamic_ELLIPSOIDCULLING + vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

