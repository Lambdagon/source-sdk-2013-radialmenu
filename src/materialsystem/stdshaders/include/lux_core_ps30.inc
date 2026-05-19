// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($ENVMAPMODE == 1 && ($CORECOLOR == 1 || $REFRACTION == 1))

#pragma once
#include "shaderlib/cshader.h"
class lux_core_ps30_Static_Index
{
	unsigned int m_nENVMAPMODE : 2;
	unsigned int m_nCORECOLOR : 2;
	unsigned int m_nREFRACTION : 2;
	unsigned int m_nFLOWMAPPING : 2;
#ifdef _DEBUG
	bool m_bENVMAPMODE : 1;
	bool m_bCORECOLOR : 1;
	bool m_bREFRACTION : 1;
	bool m_bFLOWMAPPING : 1;
#endif	// _DEBUG
public:
	void SetENVMAPMODE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAPMODE = i;
#ifdef _DEBUG
		m_bENVMAPMODE = true;
#endif	// _DEBUG
	}

	void SetCORECOLOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCORECOLOR = i;
#ifdef _DEBUG
		m_bCORECOLOR = true;
#endif	// _DEBUG
	}

	void SetREFRACTION( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFRACTION = i;
#ifdef _DEBUG
		m_bREFRACTION = true;
#endif	// _DEBUG
	}

	void SetFLOWMAPPING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLOWMAPPING = i;
#ifdef _DEBUG
		m_bFLOWMAPPING = true;
#endif	// _DEBUG
	}

	lux_core_ps30_Static_Index(  )
	{
		m_nENVMAPMODE = 0;
		m_nCORECOLOR = 0;
		m_nREFRACTION = 0;
		m_nFLOWMAPPING = 0;
#ifdef _DEBUG
		m_bENVMAPMODE = false;
		m_bCORECOLOR = false;
		m_bREFRACTION = false;
		m_bFLOWMAPPING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bENVMAPMODE && m_bCORECOLOR && m_bREFRACTION && m_bFLOWMAPPING );
		AssertMsg( !( ( m_nENVMAPMODE == 1 ) && ( ( m_nCORECOLOR == 1 ) || ( m_nREFRACTION == 1 ) ) ), "Invalid combo combination ( ( ENVMAPMODE == 1 ) && ( ( CORECOLOR == 1 ) || ( REFRACTION == 1 ) ) )" );
		return ( 1 * m_nENVMAPMODE ) + ( 2 * m_nCORECOLOR ) + ( 4 * m_nREFRACTION ) + ( 8 * m_nFLOWMAPPING ) + 0;
	}
};

#define shaderStaticTest_lux_core_ps30 psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_CORECOLOR + psh_forgot_to_set_static_REFRACTION + psh_forgot_to_set_static_FLOWMAPPING


class lux_core_ps30_Dynamic_Index
{
public:
	lux_core_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_core_ps30 1

