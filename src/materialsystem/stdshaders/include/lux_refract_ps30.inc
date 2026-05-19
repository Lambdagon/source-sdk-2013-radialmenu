// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($MASKED != 0 && $SILHOUETTEFADE != 0)
// ($MASKED != 0 && $REFRACTTINTTEXTURE != 0)
// ($MASKED != 0 && $BLUR != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_refract_ps30_Static_Index
{
	unsigned int m_nMASKED : 2;
	unsigned int m_nSILHOUETTEFADE : 2;
	unsigned int m_nREFRACTTINTTEXTURE : 2;
	unsigned int m_nBLUR : 2;
	unsigned int m_nENVMAPMODE : 3;
	unsigned int m_nSECONDARYNORMAL : 2;
	unsigned int m_nMAGNIFY : 2;
	unsigned int m_nLOCALREFRACT : 2;
#ifdef _DEBUG
	bool m_bMASKED : 1;
	bool m_bSILHOUETTEFADE : 1;
	bool m_bREFRACTTINTTEXTURE : 1;
	bool m_bBLUR : 1;
	bool m_bENVMAPMODE : 1;
	bool m_bSECONDARYNORMAL : 1;
	bool m_bMAGNIFY : 1;
	bool m_bLOCALREFRACT : 1;
#endif	// _DEBUG
public:
	void SetMASKED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMASKED = i;
#ifdef _DEBUG
		m_bMASKED = true;
#endif	// _DEBUG
	}

	void SetSILHOUETTEFADE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSILHOUETTEFADE = i;
#ifdef _DEBUG
		m_bSILHOUETTEFADE = true;
#endif	// _DEBUG
	}

	void SetREFRACTTINTTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nREFRACTTINTTEXTURE = i;
#ifdef _DEBUG
		m_bREFRACTTINTTEXTURE = true;
#endif	// _DEBUG
	}

	void SetBLUR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLUR = i;
#ifdef _DEBUG
		m_bBLUR = true;
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

	void SetSECONDARYNORMAL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSECONDARYNORMAL = i;
#ifdef _DEBUG
		m_bSECONDARYNORMAL = true;
#endif	// _DEBUG
	}

	void SetMAGNIFY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMAGNIFY = i;
#ifdef _DEBUG
		m_bMAGNIFY = true;
#endif	// _DEBUG
	}

	void SetLOCALREFRACT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLOCALREFRACT = i;
#ifdef _DEBUG
		m_bLOCALREFRACT = true;
#endif	// _DEBUG
	}

	lux_refract_ps30_Static_Index(  )
	{
		m_nMASKED = 0;
		m_nSILHOUETTEFADE = 0;
		m_nREFRACTTINTTEXTURE = 0;
		m_nBLUR = 0;
		m_nENVMAPMODE = 0;
		m_nSECONDARYNORMAL = 0;
		m_nMAGNIFY = 0;
		m_nLOCALREFRACT = 0;
#ifdef _DEBUG
		m_bMASKED = false;
		m_bSILHOUETTEFADE = false;
		m_bREFRACTTINTTEXTURE = false;
		m_bBLUR = false;
		m_bENVMAPMODE = false;
		m_bSECONDARYNORMAL = false;
		m_bMAGNIFY = false;
		m_bLOCALREFRACT = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMASKED && m_bSILHOUETTEFADE && m_bREFRACTTINTTEXTURE && m_bBLUR && m_bENVMAPMODE && m_bSECONDARYNORMAL && m_bMAGNIFY && m_bLOCALREFRACT );
		AssertMsg( !( ( m_nMASKED != 0 ) && ( m_nSILHOUETTEFADE != 0 ) ), "Invalid combo combination ( ( MASKED != 0 ) && ( SILHOUETTEFADE != 0 ) )" );
		AssertMsg( !( ( m_nMASKED != 0 ) && ( m_nREFRACTTINTTEXTURE != 0 ) ), "Invalid combo combination ( ( MASKED != 0 ) && ( REFRACTTINTTEXTURE != 0 ) )" );
		AssertMsg( !( ( m_nMASKED != 0 ) && ( m_nBLUR != 0 ) ), "Invalid combo combination ( ( MASKED != 0 ) && ( BLUR != 0 ) )" );
		return ( 1 * m_nMASKED ) + ( 2 * m_nSILHOUETTEFADE ) + ( 4 * m_nREFRACTTINTTEXTURE ) + ( 8 * m_nBLUR ) + ( 16 * m_nENVMAPMODE ) + ( 80 * m_nSECONDARYNORMAL ) + ( 160 * m_nMAGNIFY ) + ( 320 * m_nLOCALREFRACT ) + 0;
	}
};

#define shaderStaticTest_lux_refract_ps30 psh_forgot_to_set_static_MASKED + psh_forgot_to_set_static_SILHOUETTEFADE + psh_forgot_to_set_static_REFRACTTINTTEXTURE + psh_forgot_to_set_static_BLUR + psh_forgot_to_set_static_ENVMAPMODE + psh_forgot_to_set_static_SECONDARYNORMAL + psh_forgot_to_set_static_MAGNIFY + psh_forgot_to_set_static_LOCALREFRACT


class lux_refract_ps30_Dynamic_Index
{
public:
	lux_refract_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_refract_ps30 1

