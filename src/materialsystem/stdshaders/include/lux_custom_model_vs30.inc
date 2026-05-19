// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($PROJTEX != 0 && $STATICPROPLIGHTING == 1)
// ($PROJTEX != 0 && $DYNAMICPROPLIGHTING == 1)

#pragma once
#include "shaderlib/cshader.h"
class lux_custom_model_vs30_Static_Index
{
	unsigned int m_nNORMALS : 2;
	unsigned int m_nVERTEXCOLORS : 2;
	unsigned int m_nBUMPMAPPED : 2;
	unsigned int m_nPROJTEX : 2;
	unsigned int m_nDECALMODE : 2;
#ifdef _DEBUG
	bool m_bNORMALS : 1;
	bool m_bVERTEXCOLORS : 1;
	bool m_bBUMPMAPPED : 1;
	bool m_bPROJTEX : 1;
	bool m_bDECALMODE : 1;
#endif	// _DEBUG
public:
	void SetNORMALS( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nNORMALS = i;
#ifdef _DEBUG
		m_bNORMALS = true;
#endif	// _DEBUG
	}

	void SetVERTEXCOLORS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLORS = i;
#ifdef _DEBUG
		m_bVERTEXCOLORS = true;
#endif	// _DEBUG
	}

	void SetBUMPMAPPED( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nBUMPMAPPED = i;
#ifdef _DEBUG
		m_bBUMPMAPPED = true;
#endif	// _DEBUG
	}

	void SetPROJTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPROJTEX = i;
#ifdef _DEBUG
		m_bPROJTEX = true;
#endif	// _DEBUG
	}

	void SetDECALMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nDECALMODE = i;
#ifdef _DEBUG
		m_bDECALMODE = true;
#endif	// _DEBUG
	}

	lux_custom_model_vs30_Static_Index(  )
	{
		m_nNORMALS = 0;
		m_nVERTEXCOLORS = 0;
		m_nBUMPMAPPED = 0;
		m_nPROJTEX = 0;
		m_nDECALMODE = 0;
#ifdef _DEBUG
		m_bNORMALS = false;
		m_bVERTEXCOLORS = false;
		m_bBUMPMAPPED = false;
		m_bPROJTEX = false;
		m_bDECALMODE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bNORMALS && m_bVERTEXCOLORS && m_bBUMPMAPPED && m_bPROJTEX && m_bDECALMODE );
		return ( 16 * m_nNORMALS ) + ( 48 * m_nVERTEXCOLORS ) + ( 96 * m_nBUMPMAPPED ) + ( 288 * m_nPROJTEX ) + ( 576 * m_nDECALMODE ) + 0;
	}
};

#define shaderStaticTest_lux_custom_model_vs30 vsh_forgot_to_set_static_NORMALS + vsh_forgot_to_set_static_VERTEXCOLORS + vsh_forgot_to_set_static_BUMPMAPPED + vsh_forgot_to_set_static_PROJTEX + vsh_forgot_to_set_static_DECALMODE


class lux_custom_model_vs30_Dynamic_Index
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

	lux_custom_model_vs30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_custom_model_vs30 vsh_forgot_to_set_dynamic_STATICPROPLIGHTING + vsh_forgot_to_set_dynamic_DYNAMICPROPLIGHTING + vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

