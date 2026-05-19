#pragma once
#include "shaderlib/cshader.h"
class lux_emissive_vs30_Static_Index
{
	unsigned int m_nEMISSIVEBLENDMODE : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nSELFILLUMTEXTURE : 2;
	unsigned int m_nNEEDSBASETEXTURE : 2;
#ifdef _DEBUG
	bool m_bEMISSIVEBLENDMODE : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bSELFILLUMTEXTURE : 1;
	bool m_bNEEDSBASETEXTURE : 1;
#endif	// _DEBUG
public:
	void SetEMISSIVEBLENDMODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nEMISSIVEBLENDMODE = i;
#ifdef _DEBUG
		m_bEMISSIVEBLENDMODE = true;
#endif	// _DEBUG
	}

	void SetDETAILTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
#endif	// _DEBUG
	}

	void SetSELFILLUMTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSELFILLUMTEXTURE = i;
#ifdef _DEBUG
		m_bSELFILLUMTEXTURE = true;
#endif	// _DEBUG
	}

	void SetNEEDSBASETEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNEEDSBASETEXTURE = i;
#ifdef _DEBUG
		m_bNEEDSBASETEXTURE = true;
#endif	// _DEBUG
	}

	lux_emissive_vs30_Static_Index(  )
	{
		m_nEMISSIVEBLENDMODE = 0;
		m_nDETAILTEXTURE = 0;
		m_nSELFILLUMTEXTURE = 0;
		m_nNEEDSBASETEXTURE = 0;
#ifdef _DEBUG
		m_bEMISSIVEBLENDMODE = false;
		m_bDETAILTEXTURE = false;
		m_bSELFILLUMTEXTURE = false;
		m_bNEEDSBASETEXTURE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bEMISSIVEBLENDMODE && m_bDETAILTEXTURE && m_bSELFILLUMTEXTURE && m_bNEEDSBASETEXTURE );
		return ( 4 * m_nEMISSIVEBLENDMODE ) + ( 12 * m_nDETAILTEXTURE ) + ( 24 * m_nSELFILLUMTEXTURE ) + ( 48 * m_nNEEDSBASETEXTURE ) + 0;
	}
};

#define shaderStaticTest_lux_emissive_vs30 vsh_forgot_to_set_static_EMISSIVEBLENDMODE + vsh_forgot_to_set_static_DETAILTEXTURE + vsh_forgot_to_set_static_SELFILLUMTEXTURE + vsh_forgot_to_set_static_NEEDSBASETEXTURE


class lux_emissive_vs30_Dynamic_Index
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

	lux_emissive_vs30_Dynamic_Index(  )
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

#define shaderDynamicTest_lux_emissive_vs30 vsh_forgot_to_set_dynamic_COMPRESSION + vsh_forgot_to_set_dynamic_SKINNING

