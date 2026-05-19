// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($TEXCOORDS == 3 && $SPECIALTEXCOORDS != 0)
// ($SPECIALTEXCOORDS != 0 && $VERTEX_SWAY != 0)
// ($SPECIALTEXCOORDS != 0 && $TANGENTS != 0)

#pragma once
#include "shaderlib/cshader.h"
class lux_model_vs30_Static_Index
{
	unsigned int m_nTEXCOORDS : 3;
	unsigned int m_nSPECIALTEXCOORDS : 2;
	unsigned int m_nVERTEX_SWAY : 2;
	unsigned int m_nTANGENTS : 2;
	unsigned int m_nVERTEXCOLORS : 2;
#ifdef _DEBUG
	bool m_bTEXCOORDS : 1;
	bool m_bSPECIALTEXCOORDS : 1;
	bool m_bVERTEX_SWAY : 1;
	bool m_bTANGENTS : 1;
	bool m_bVERTEXCOLORS : 1;
#endif	// _DEBUG
public:
	void SetTEXCOORDS( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nTEXCOORDS = i;
#ifdef _DEBUG
		m_bTEXCOORDS = true;
#endif	// _DEBUG
	}

	void SetSPECIALTEXCOORDS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nSPECIALTEXCOORDS = i;
#ifdef _DEBUG
		m_bSPECIALTEXCOORDS = true;
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

	void SetTANGENTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTANGENTS = i;
#ifdef _DEBUG
		m_bTANGENTS = true;
#endif	// _DEBUG
	}

	void SetVERTEXCOLORS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nVERTEXCOLORS = i;
#ifdef _DEBUG
		m_bVERTEXCOLORS = true;
#endif	// _DEBUG
	}

	lux_model_vs30_Static_Index(  )
	{
		m_nTEXCOORDS = 0;
		m_nSPECIALTEXCOORDS = 0;
		m_nVERTEX_SWAY = 0;
		m_nTANGENTS = 0;
		m_nVERTEXCOLORS = 0;
#ifdef _DEBUG
		m_bTEXCOORDS = false;
		m_bSPECIALTEXCOORDS = false;
		m_bVERTEX_SWAY = false;
		m_bTANGENTS = false;
		m_bVERTEXCOLORS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bTEXCOORDS && m_bSPECIALTEXCOORDS && m_bVERTEX_SWAY && m_bTANGENTS && m_bVERTEXCOLORS );
		AssertMsg( !( ( m_nTEXCOORDS == 3 ) && ( m_nSPECIALTEXCOORDS != 0 ) ), "Invalid combo combination ( ( TEXCOORDS == 3 ) && ( SPECIALTEXCOORDS != 0 ) )" );
		AssertMsg( !( ( m_nSPECIALTEXCOORDS != 0 ) && ( m_nVERTEX_SWAY != 0 ) ), "Invalid combo combination ( ( SPECIALTEXCOORDS != 0 ) && ( VERTEX_SWAY != 0 ) )" );
		AssertMsg( !( ( m_nSPECIALTEXCOORDS != 0 ) && ( m_nTANGENTS != 0 ) ), "Invalid combo combination ( ( SPECIALTEXCOORDS != 0 ) && ( TANGENTS != 0 ) )" );
		return ( 16 * m_nTEXCOORDS ) + ( 64 * m_nSPECIALTEXCOORDS ) + ( 192 * m_nVERTEX_SWAY ) + ( 576 * m_nTANGENTS ) + ( 1152 * m_nVERTEXCOLORS ) + 0;
	}
};

#define shaderStaticTest_lux_model_vs30 vsh_forgot_to_set_static_TEXCOORDS + vsh_forgot_to_set_static_SPECIALTEXCOORDS + vsh_forgot_to_set_static_VERTEX_SWAY + vsh_forgot_to_set_static_TANGENTS + vsh_forgot_to_set_static_VERTEXCOLORS


class lux_model_vs30_Dynamic_Index
{
	unsigned int m_nSTATICPROPLIGHTING : 2;
	unsigned int m_nDYNAMICPROPLIGHTING : 2;
	unsigned int m_nCOMPRESSION : 2;
	unsigned int m_nSKINNING : 2;
#ifdef _DEBUG
	bool m_bSTATICPROPLIGHTING : 1;
	bool m_bDYNAMICPROPLIGHTING : 1;
	bool m_bCOMPRESSION : 1;
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetSTATICPROPLIGHTING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSTATICPROPLIGHTING = i;
#ifdef _DEBUG
		m_bSTATICPROPLIGHTING = true;
#endif	// _DEBUG
	}

	void SetDYNAMICPROPLIGHTING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDYNAMICPROPLIGHTING = i;
#ifdef _DEBUG
		m_bDYNAMICPROPLIGHTING = true;
#endif	// _DEBUG
	}

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

	lux_model_vs30_Dynamic_Index(  )
	{
		m_nSTATICPROPLIGHTING = 0;
		m_nDYNAMICPROPLIGHTING = 0;
		m_nCOMPRESSION = 0;
		m_nSKINNING = 0;
#ifdef _DEBUG
		m_bSTATICPROPLIGHTING = false;
		m_bDYNAMICPROPLIGHTING = false;
		m_bCOMPRESSION = false;
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bSTATICPROPLIGHTING && m_bDYNAMICPROPLIGHTING && m_bCOMPRESSION && m_bSKINNING );
		return ( 1 * m_nSTATICPROPLIGHTING ) + ( 2 * m_nDYNAMICPROPLIGHTING ) + ( 4 * m_nCOMPRESSION ) + ( 8 * m_nSKINNING ) + 0;
	}
};

#define shaderDynamicTest_lux_model_vs30 vsh_forgot_to_set_dynamic_STATICPROPLIGHTING + vsh_forgot_to_set_dynamic_DYNAMICPROPLIGHTING + vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

