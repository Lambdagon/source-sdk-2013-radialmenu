#pragma once
#include "shaderlib/cshader.h"
class lux_shatteredglass_ps30_Static_Index
{
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nENVMAPMODE : 3;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bDETAILTEXTURE : 1;
	bool m_bENVMAPMODE : 1;
	bool m_bVERTEXCOLORS : 1;
#endif	// _DEBUG
public:
	void SetDETAILTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
#endif	// _DEBUG
	}

	void SetENVMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nENVMAPMODE = i;
#ifdef _DEBUG
		m_bENVMAPMODE = true;
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

	lux_shatteredglass_ps30_Static_Index(  )
	{
		m_nDETAILTEXTURE = 0;
		m_nENVMAPMODE = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bDETAILTEXTURE = false;
		m_bENVMAPMODE = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bDETAILTEXTURE && m_bENVMAPMODE && m_bVERTEXCOLORS );
		return ( 1 * m_nDETAILTEXTURE ) + ( 2 * m_nENVMAPMODE ) + ( 10 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_shatteredglass_ps30 psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_VERTEXCOLORS


class lux_shatteredglass_ps30_Dynamic_Index
{
public:
	lux_shatteredglass_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_shatteredglass_ps30 1

