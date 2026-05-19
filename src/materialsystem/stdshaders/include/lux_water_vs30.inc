#pragma once
#include "shaderlib/cshader.h"
class lux_water_vs30_Static_Index
{
	unsigned int m_nMULTITEXTURE : 2;
	unsigned int m_nLIGHTMAPPING : 2;
	unsigned int m_nSURFACELAYER : 2;
	unsigned int m_nREFLECTIONS : 2;
	unsigned int m_nREFRACTIONS : 2;
#ifdef _DEBUG
	bool m_bMULTITEXTURE : 1;
	bool m_bLIGHTMAPPING : 1;
	bool m_bSURFACELAYER : 1;
	bool m_bREFLECTIONS : 1;
	bool m_bREFRACTIONS : 1;
#endif	// _DEBUG
public:
	void SetMULTITEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMULTITEXTURE = i;
#ifdef _DEBUG
		m_bMULTITEXTURE = true;
#endif	// _DEBUG
	}

	void SetLIGHTMAPPING( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nLIGHTMAPPING = i;
#ifdef _DEBUG
		m_bLIGHTMAPPING = true;
#endif	// _DEBUG
	}

	void SetSURFACELAYER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSURFACELAYER = i;
#ifdef _DEBUG
		m_bSURFACELAYER = true;
#endif	// _DEBUG
	}

	void SetREFLECTIONS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFLECTIONS = i;
#ifdef _DEBUG
		m_bREFLECTIONS = true;
#endif	// _DEBUG
	}

	void SetREFRACTIONS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFRACTIONS = i;
#ifdef _DEBUG
		m_bREFRACTIONS = true;
#endif	// _DEBUG
	}

	lux_water_vs30_Static_Index(  )
	{
		m_nMULTITEXTURE = 0;
		m_nLIGHTMAPPING = 0;
		m_nSURFACELAYER = 0;
		m_nREFLECTIONS = 0;
		m_nREFRACTIONS = 0;
#ifdef _DEBUG
		m_bMULTITEXTURE = false;
		m_bLIGHTMAPPING = false;
		m_bSURFACELAYER = false;
		m_bREFLECTIONS = false;
		m_bREFRACTIONS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMULTITEXTURE && m_bLIGHTMAPPING && m_bSURFACELAYER && m_bREFLECTIONS && m_bREFRACTIONS );
		return ( 1 * m_nMULTITEXTURE ) + ( 2 * m_nLIGHTMAPPING ) + ( 6 * m_nSURFACELAYER ) + ( 12 * m_nREFLECTIONS ) + ( 24 * m_nREFRACTIONS ) + 0;
	}
};

#define shaderStaticTest_lux_water_vs30 vsh_forgot_to_set_static_MULTITEXTURE + vsh_forgot_to_set_static_LIGHTMAPPING + vsh_forgot_to_set_static_SURFACELAYER + vsh_forgot_to_set_static_REFLECTIONS + vsh_forgot_to_set_static_REFRACTIONS


class lux_water_vs30_Dynamic_Index
{
public:
	lux_water_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_water_vs30 1

