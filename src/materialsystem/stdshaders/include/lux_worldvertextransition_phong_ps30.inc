#pragma once
#include "shaderlib/cshader.h"
class lux_worldvertextransition_phong_ps30_Static_Index
{
	unsigned int m_nBLENDMODULATETEXTURE : 2;
	unsigned int m_nEXPONENTTEXTURE : 2;
	unsigned int m_nENVMAPMODE : 3;
	unsigned int m_nXBYBASEALPHA : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nSSBUMP : 2;
#ifdef _DEBUG
	bool m_bBLENDMODULATETEXTURE : 1;
	bool m_bEXPONENTTEXTURE : 1;
	bool m_bENVMAPMODE : 1;
	bool m_bXBYBASEALPHA : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bSSBUMP : 1;
#endif	// _DEBUG
public:
	void SetBLENDMODULATETEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLENDMODULATETEXTURE = i;
#ifdef _DEBUG
		m_bBLENDMODULATETEXTURE = true;
#endif	// _DEBUG
	}

	void SetEXPONENTTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nEXPONENTTEXTURE = i;
#ifdef _DEBUG
		m_bEXPONENTTEXTURE = true;
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

	void SetXBYBASEALPHA( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nXBYBASEALPHA = i;
#ifdef _DEBUG
		m_bXBYBASEALPHA = true;
#endif	// _DEBUG
	}

	void SetDETAILTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
#endif	// _DEBUG
	}

	void SetSSBUMP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSSBUMP = i;
#ifdef _DEBUG
		m_bSSBUMP = true;
#endif	// _DEBUG
	}

	lux_worldvertextransition_phong_ps30_Static_Index(  )
	{
		m_nBLENDMODULATETEXTURE = 0;
		m_nEXPONENTTEXTURE = 0;
		m_nENVMAPMODE = 0;
		m_nXBYBASEALPHA = 0;
		m_nDETAILTEXTURE = 0;
		m_nSSBUMP = 0;
#ifdef _DEBUG
		m_bBLENDMODULATETEXTURE = false;
		m_bEXPONENTTEXTURE = false;
		m_bENVMAPMODE = false;
		m_bXBYBASEALPHA = false;
		m_bDETAILTEXTURE = false;
		m_bSSBUMP = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBLENDMODULATETEXTURE && m_bEXPONENTTEXTURE && m_bENVMAPMODE && m_bXBYBASEALPHA && m_bDETAILTEXTURE && m_bSSBUMP );
		return ( 1 * m_nBLENDMODULATETEXTURE ) + ( 2 * m_nEXPONENTTEXTURE ) + ( 6 * m_nENVMAPMODE ) + ( 30 * m_nXBYBASEALPHA ) + ( 90 * m_nDETAILTEXTURE ) + ( 270 * m_nSSBUMP ) + 0;
	}
};

#define shaderStaticTest_lux_worldvertextransition_phong_ps30 psh_forgot_to_set_static_BLENDMODULATETEXTURE + psh_forgot_to_set_static_EXPONENTTEXTURE + psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_XBYBASEALPHA + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_SSBUMP


class lux_worldvertextransition_phong_ps30_Dynamic_Index
{
public:
	lux_worldvertextransition_phong_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_worldvertextransition_phong_ps30 1

