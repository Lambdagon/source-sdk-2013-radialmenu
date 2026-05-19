#pragma once
#include "shaderlib/cshader.h"
class lux_decalmodulate_ps30_Static_Index
{
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nVERTEXALPHA : 2;
#ifdef _DEBUG
	bool m_bDETAILTEXTURE : 1;
	bool m_bVERTEXALPHA : 1;
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

	void SetVERTEXALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXALPHA = i;
#ifdef _DEBUG
		m_bVERTEXALPHA = true;
#endif	// _DEBUG
	}

	lux_decalmodulate_ps30_Static_Index(  )
	{
		m_nDETAILTEXTURE = 0;
		m_nVERTEXALPHA = 0;
#ifdef _DEBUG
		m_bDETAILTEXTURE = false;
		m_bVERTEXALPHA = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bDETAILTEXTURE && m_bVERTEXALPHA );
		return ( 1 * m_nDETAILTEXTURE ) + ( 2 * m_nVERTEXALPHA ) + 0;
	}
};

#define shaderStaticTest_lux_decalmodulate_ps30 psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_VERTEXALPHA


class lux_decalmodulate_ps30_Dynamic_Index
{
public:
	lux_decalmodulate_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_decalmodulate_ps30 1

