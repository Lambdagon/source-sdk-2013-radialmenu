// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($NORMALMAPS == 0 && $WRINKLEMAPPING != 0)
// ($NORMALMAPS != 2 && $BLENDMODULATE != 0)
// ($NORMALMAPS == 2 && $WRINKLEMAPPING != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_ssao_drawnormal_ps30_Static_Index
{
	unsigned int m_nNORMALMAPS : 2;
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nWRINKLEMAPPING : 2;
#ifdef _DEBUG
	bool m_bNORMALMAPS : 1;
	bool m_bBLENDMODULATE : 1;
	bool m_bWRINKLEMAPPING : 1;
#endif	// _DEBUG
public:
	void SetNORMALMAPS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nNORMALMAPS = i;
#ifdef _DEBUG
		m_bNORMALMAPS = true;
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

	void SetWRINKLEMAPPING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRINKLEMAPPING = i;
#ifdef _DEBUG
		m_bWRINKLEMAPPING = true;
#endif	// _DEBUG
	}

	lux_ssao_drawnormal_ps30_Static_Index(  )
	{
		m_nNORMALMAPS = 0;
		m_nBLENDMODULATE = 0;
		m_nWRINKLEMAPPING = 0;
#ifdef _DEBUG
		m_bNORMALMAPS = false;
		m_bBLENDMODULATE = false;
		m_bWRINKLEMAPPING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNORMALMAPS && m_bBLENDMODULATE && m_bWRINKLEMAPPING );
		AssertMsg( !( ( m_nNORMALMAPS == 0 ) && ( m_nWRINKLEMAPPING != 0 ) ), "Invalid combo combination ( ( NORMALMAPS == 0 ) && ( WRINKLEMAPPING != 0 ) )" );
		AssertMsg( !( ( m_nNORMALMAPS != 2 ) && ( m_nBLENDMODULATE != 0 ) ), "Invalid combo combination ( ( NORMALMAPS != 2 ) && ( BLENDMODULATE != 0 ) )" );
		AssertMsg( !( ( m_nNORMALMAPS == 2 ) && ( m_nWRINKLEMAPPING != 0 ) ), "Invalid combo combination ( ( NORMALMAPS == 2 ) && ( WRINKLEMAPPING != 0 ) )" );
		return ( 1 * m_nNORMALMAPS ) + ( 3 * m_nBLENDMODULATE ) + ( 6 * m_nWRINKLEMAPPING ) + 0;
	}
};

#define shaderStaticTest_lux_ssao_drawnormal_ps30 psh_forgot_to_set_static_NORMALMAPS + psh_forgot_to_set_static_BLENDMODULATE + psh_forgot_to_set_static_WRINKLEMAPPING


class lux_ssao_drawnormal_ps30_Dynamic_Index
{
public:
	lux_ssao_drawnormal_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_ssao_drawnormal_ps30 1

