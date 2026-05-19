// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ( $BUMPMAPPED > 1 && $BUMPMASK != 0 )

#pragma once
#include "shaderlib/cshader.h"
class lux_worldvertextransition_flashlight_ps30_Static_Index
{
	unsigned int m_nBLENDMODULATETEXTURE : 2;
	unsigned int m_nBUMPMAPPED : 3;
	unsigned int m_nBUMPMASK : 2;
	unsigned int m_nEXPONENTTEXTURE : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nXBYBASEALPHA : 2;
#ifdef _DEBUG
	bool m_bBLENDMODULATETEXTURE : 1;
	bool m_bBUMPMAPPED : 1;
	bool m_bBUMPMASK : 1;
	bool m_bEXPONENTTEXTURE : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bXBYBASEALPHA : 1;
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

	void SetBUMPMAPPED( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nBUMPMAPPED = i;
#ifdef _DEBUG
		m_bBUMPMAPPED = true;
#endif	// _DEBUG
	}

	void SetBUMPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMASK = i;
#ifdef _DEBUG
		m_bBUMPMASK = true;
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

	void SetDETAILTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
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

	lux_worldvertextransition_flashlight_ps30_Static_Index(  )
	{
		m_nBLENDMODULATETEXTURE = 0;
		m_nBUMPMAPPED = 0;
		m_nBUMPMASK = 0;
		m_nEXPONENTTEXTURE = 0;
		m_nDETAILTEXTURE = 0;
		m_nXBYBASEALPHA = 0;
#ifdef _DEBUG
		m_bBLENDMODULATETEXTURE = false;
		m_bBUMPMAPPED = false;
		m_bBUMPMASK = false;
		m_bEXPONENTTEXTURE = false;
		m_bDETAILTEXTURE = false;
		m_bXBYBASEALPHA = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBLENDMODULATETEXTURE && m_bBUMPMAPPED && m_bBUMPMASK && m_bEXPONENTTEXTURE && m_bDETAILTEXTURE && m_bXBYBASEALPHA );
		AssertMsg( !( ( m_nBUMPMAPPED > 1 ) && ( m_nBUMPMASK != 0 ) ), "Invalid combo combination ( ( BUMPMAPPED > 1 ) && ( BUMPMASK != 0 ) )" );
		return ( 2 * m_nBLENDMODULATETEXTURE ) + ( 4 * m_nBUMPMAPPED ) + ( 20 * m_nBUMPMASK ) + ( 40 * m_nEXPONENTTEXTURE ) + ( 120 * m_nDETAILTEXTURE ) + ( 360 * m_nXBYBASEALPHA ) + 0;
	}
};

#define shaderStaticTest_lux_worldvertextransition_flashlight_ps30 psh_forgot_to_set_static_BLENDMODULATETEXTURE + psh_forgot_to_set_static_BUMPMAPPED + psh_forgot_to_set_static_BUMPMASK + psh_forgot_to_set_static_EXPONENTTEXTURE + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_XBYBASEALPHA


class lux_worldvertextransition_flashlight_ps30_Dynamic_Index
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

	lux_worldvertextransition_flashlight_ps30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_worldvertextransition_flashlight_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS

