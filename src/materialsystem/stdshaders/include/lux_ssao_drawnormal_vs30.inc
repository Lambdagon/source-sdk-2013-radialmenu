// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($BRUSH_OR_DISPLACEMENT == 0 && $NORMALMAPS == 2)
// ($BRUSH_OR_DISPLACEMENT == 0 && $BLENDMODULATE != 0)
// ($BRUSH_OR_DISPLACEMENT != 0 && $VERTEX_SWAY != 0)
// ($BRUSH_OR_DISPLACEMENT != 0 && $NORMALS == 2)

#pragma once
#include "shaderlib/cshader.h"
class lux_ssao_drawnormal_vs30_Static_Index
{
	unsigned int m_nBRUSH_OR_DISPLACEMENT : 2;
	unsigned int m_nNORMALMAPS : 2;
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nNORMALS : 2;
	unsigned int m_nVERTEX_SWAY : 2;
#ifdef _DEBUG
	bool m_bBRUSH_OR_DISPLACEMENT : 1;
	bool m_bNORMALMAPS : 1;
	bool m_bBLENDMODULATE : 1;
	bool m_bNORMALS : 1;
	bool m_bVERTEX_SWAY : 1;
#endif	// _DEBUG
public:
	void SetBRUSH_OR_DISPLACEMENT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBRUSH_OR_DISPLACEMENT = i;
#ifdef _DEBUG
		m_bBRUSH_OR_DISPLACEMENT = true;
#endif	// _DEBUG
	}

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

	void SetNORMALS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nNORMALS = i;
#ifdef _DEBUG
		m_bNORMALS = true;
#endif	// _DEBUG
	}

	void SetVERTEX_SWAY( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nVERTEX_SWAY = i;
#ifdef _DEBUG
		m_bVERTEX_SWAY = true;
#endif	// _DEBUG
	}

	lux_ssao_drawnormal_vs30_Static_Index(  )
	{
		m_nBRUSH_OR_DISPLACEMENT = 0;
		m_nNORMALMAPS = 0;
		m_nBLENDMODULATE = 0;
		m_nNORMALS = 0;
		m_nVERTEX_SWAY = 0;
#ifdef _DEBUG
		m_bBRUSH_OR_DISPLACEMENT = false;
		m_bNORMALMAPS = false;
		m_bBLENDMODULATE = false;
		m_bNORMALS = false;
		m_bVERTEX_SWAY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBRUSH_OR_DISPLACEMENT && m_bNORMALMAPS && m_bBLENDMODULATE && m_bNORMALS && m_bVERTEX_SWAY );
		AssertMsg( !( ( m_nBRUSH_OR_DISPLACEMENT == 0 ) && ( m_nNORMALMAPS == 2 ) ), "Invalid combo combination ( ( BRUSH_OR_DISPLACEMENT == 0 ) && ( NORMALMAPS == 2 ) )" );
		AssertMsg( !( ( m_nBRUSH_OR_DISPLACEMENT == 0 ) && ( m_nBLENDMODULATE != 0 ) ), "Invalid combo combination ( ( BRUSH_OR_DISPLACEMENT == 0 ) && ( BLENDMODULATE != 0 ) )" );
		AssertMsg( !( ( m_nBRUSH_OR_DISPLACEMENT != 0 ) && ( m_nVERTEX_SWAY != 0 ) ), "Invalid combo combination ( ( BRUSH_OR_DISPLACEMENT != 0 ) && ( VERTEX_SWAY != 0 ) )" );
		AssertMsg( !( ( m_nBRUSH_OR_DISPLACEMENT != 0 ) && ( m_nNORMALS == 2 ) ), "Invalid combo combination ( ( BRUSH_OR_DISPLACEMENT != 0 ) && ( NORMALS == 2 ) )" );
		return ( 4 * m_nBRUSH_OR_DISPLACEMENT ) + ( 8 * m_nNORMALMAPS ) + ( 24 * m_nBLENDMODULATE ) + ( 48 * m_nNORMALS ) + ( 144 * m_nVERTEX_SWAY ) + 0;
	}
};

#define shaderStaticTest_lux_ssao_drawnormal_vs30 vsh_forgot_to_set_static_BRUSH_OR_DISPLACEMENT + vsh_forgot_to_set_static_NORMALMAPS + vsh_forgot_to_set_static_BLENDMODULATE + vsh_forgot_to_set_static_NORMALS + vsh_forgot_to_set_static_VERTEX_SWAY


class lux_ssao_drawnormal_vs30_Dynamic_Index
{
	unsigned int m_nCOMPRESSION : 2;
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSION : 1;
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
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

	lux_ssao_drawnormal_vs30_Dynamic_Index(  )
	{
		m_nCOMPRESSION = 0;
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bCOMPRESSION = false;
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSION && m_bSKINNING );
		return ( 1 * m_nCOMPRESSION ) + ( 2 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_lux_ssao_drawnormal_vs30 vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

