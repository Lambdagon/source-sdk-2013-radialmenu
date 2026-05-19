// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($MODE_BUMP == 0 && $TRIPLANAR_BUMP)
// ($DETAILTEXTURES == 0 && $TRIPLANAR_DETAIL)
// (!$BLENDMODULATE && $TRIPLANAR_BLENDMODULATE)

#pragma once
#include "shaderlib/cshader.h"
class lux_triplanar_displacement_flashlight_ps30_Static_Index
{
	unsigned int m_nMODE_BUMP : 2;
	unsigned int m_nTRIPLANAR_BUMP : 2;
	unsigned int m_nDETAILTEXTURES : 2;
	unsigned int m_nTRIPLANAR_DETAIL : 2;
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nTRIPLANAR_BLENDMODULATE : 2;
	unsigned int m_nTRIPLANAR_BASE : 2;
#ifdef _DEBUG
	bool m_bMODE_BUMP : 1;
	bool m_bTRIPLANAR_BUMP : 1;
	bool m_bDETAILTEXTURES : 1;
	bool m_bTRIPLANAR_DETAIL : 1;
	bool m_bBLENDMODULATE : 1;
	bool m_bTRIPLANAR_BLENDMODULATE : 1;
	bool m_bTRIPLANAR_BASE : 1;
#endif	// _DEBUG
public:
	void SetMODE_BUMP( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nMODE_BUMP = i;
#ifdef _DEBUG
		m_bMODE_BUMP = true;
#endif	// _DEBUG
	}

	void SetTRIPLANAR_BUMP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRIPLANAR_BUMP = i;
#ifdef _DEBUG
		m_bTRIPLANAR_BUMP = true;
#endif	// _DEBUG
	}

	void SetDETAILTEXTURES( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nDETAILTEXTURES = i;
#ifdef _DEBUG
		m_bDETAILTEXTURES = true;
#endif	// _DEBUG
	}

	void SetTRIPLANAR_DETAIL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRIPLANAR_DETAIL = i;
#ifdef _DEBUG
		m_bTRIPLANAR_DETAIL = true;
#endif	// _DEBUG
	}

	void SetBLENDMODULATE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLENDMODULATE = i;
#ifdef _DEBUG
		m_bBLENDMODULATE = true;
#endif	// _DEBUG
	}

	void SetTRIPLANAR_BLENDMODULATE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRIPLANAR_BLENDMODULATE = i;
#ifdef _DEBUG
		m_bTRIPLANAR_BLENDMODULATE = true;
#endif	// _DEBUG
	}

	void SetTRIPLANAR_BASE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTRIPLANAR_BASE = i;
#ifdef _DEBUG
		m_bTRIPLANAR_BASE = true;
#endif	// _DEBUG
	}

	lux_triplanar_displacement_flashlight_ps30_Static_Index(  )
	{
		m_nMODE_BUMP = 0;
		m_nTRIPLANAR_BUMP = 0;
		m_nDETAILTEXTURES = 0;
		m_nTRIPLANAR_DETAIL = 0;
		m_nBLENDMODULATE = 0;
		m_nTRIPLANAR_BLENDMODULATE = 0;
		m_nTRIPLANAR_BASE = 0;
#ifdef _DEBUG
		m_bMODE_BUMP = false;
		m_bTRIPLANAR_BUMP = false;
		m_bDETAILTEXTURES = false;
		m_bTRIPLANAR_DETAIL = false;
		m_bBLENDMODULATE = false;
		m_bTRIPLANAR_BLENDMODULATE = false;
		m_bTRIPLANAR_BASE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMODE_BUMP && m_bTRIPLANAR_BUMP && m_bDETAILTEXTURES && m_bTRIPLANAR_DETAIL && m_bBLENDMODULATE && m_bTRIPLANAR_BLENDMODULATE && m_bTRIPLANAR_BASE );
		AssertMsg( !( ( m_nMODE_BUMP == 0 ) && m_nTRIPLANAR_BUMP ), "Invalid combo combination ( ( MODE_BUMP == 0 ) && TRIPLANAR_BUMP )" );
		AssertMsg( !( ( m_nDETAILTEXTURES == 0 ) && m_nTRIPLANAR_DETAIL ), "Invalid combo combination ( ( DETAILTEXTURES == 0 ) && TRIPLANAR_DETAIL )" );
		AssertMsg( !( !m_nBLENDMODULATE && m_nTRIPLANAR_BLENDMODULATE ), "Invalid combo combination ( !BLENDMODULATE && TRIPLANAR_BLENDMODULATE )" );
		return ( 2 * m_nMODE_BUMP ) + ( 6 * m_nTRIPLANAR_BUMP ) + ( 12 * m_nDETAILTEXTURES ) + ( 36 * m_nTRIPLANAR_DETAIL ) + ( 72 * m_nBLENDMODULATE ) + ( 144 * m_nTRIPLANAR_BLENDMODULATE ) + ( 288 * m_nTRIPLANAR_BASE ) + 0;
	}
};

#define shaderStaticTest_lux_triplanar_displacement_flashlight_ps30 psh_forgot_to_set_static_MODE_BUMP + psh_forgot_to_set_static_TRIPLANAR_BUMP + psh_forgot_to_set_static_DETAILTEXTURES + psh_forgot_to_set_static_TRIPLANAR_DETAIL + psh_forgot_to_set_static_BLENDMODULATE + psh_forgot_to_set_static_TRIPLANAR_BLENDMODULATE + psh_forgot_to_set_static_TRIPLANAR_BASE


class lux_triplanar_displacement_flashlight_ps30_Dynamic_Index
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

	lux_triplanar_displacement_flashlight_ps30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_triplanar_displacement_flashlight_ps30 psh_forgot_to_set_dynamic_PROJTEXSHADOWS

