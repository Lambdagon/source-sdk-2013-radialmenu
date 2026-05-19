// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($ANIMBLEND == 1 && $MAXLUMFRAMEBLEND1 == 1)
// (($ANIMBLEND == 1 || $MAXLUMFRAMEBLEND1 == 1) && $EXTRACTGREENALPHA == 1 && $ADDBASETEXTURE2 == 0)
// ($ADDBASETEXTURE2 == 1 && $ADDSELF == 1)
// ($DUALSEQUENCE == 0 && ($MAXLUMFRAMEBLEND2 == 1 || $SEQUENCE_BLEND_MODE != 0))

#pragma once
#include "shaderlib/cshader.h"
class lux_spritecard_ps30_Static_Index
{
	unsigned int m_nANIMBLEND : 2;
	unsigned int m_nMAXLUMFRAMEBLEND1 : 2;
	unsigned int m_nEXTRACTGREENALPHA : 2;
	unsigned int m_nADDBASETEXTURE2 : 2;
	unsigned int m_nADDSELF : 2;
	unsigned int m_nDUALSEQUENCE : 2;
	unsigned int m_nMAXLUMFRAMEBLEND2 : 2;
	unsigned int m_nSEQUENCE_BLEND_MODE : 2;
	unsigned int m_nCOLORRAMP : 2;
	unsigned int m_nDEPTHBLEND : 2;
	unsigned int m_nMOD2X : 2;
#ifdef _DEBUG
	bool m_bANIMBLEND : 1;
	bool m_bMAXLUMFRAMEBLEND1 : 1;
	bool m_bEXTRACTGREENALPHA : 1;
	bool m_bADDBASETEXTURE2 : 1;
	bool m_bADDSELF : 1;
	bool m_bDUALSEQUENCE : 1;
	bool m_bMAXLUMFRAMEBLEND2 : 1;
	bool m_bSEQUENCE_BLEND_MODE : 1;
	bool m_bCOLORRAMP : 1;
	bool m_bDEPTHBLEND : 1;
	bool m_bMOD2X : 1;
#endif	// _DEBUG
public:
	void SetANIMBLEND( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nANIMBLEND = i;
#ifdef _DEBUG
		m_bANIMBLEND = true;
#endif	// _DEBUG
	}

	void SetMAXLUMFRAMEBLEND1( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMAXLUMFRAMEBLEND1 = i;
#ifdef _DEBUG
		m_bMAXLUMFRAMEBLEND1 = true;
#endif	// _DEBUG
	}

	void SetEXTRACTGREENALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nEXTRACTGREENALPHA = i;
#ifdef _DEBUG
		m_bEXTRACTGREENALPHA = true;
#endif	// _DEBUG
	}

	void SetADDBASETEXTURE2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nADDBASETEXTURE2 = i;
#ifdef _DEBUG
		m_bADDBASETEXTURE2 = true;
#endif	// _DEBUG
	}

	void SetADDSELF( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nADDSELF = i;
#ifdef _DEBUG
		m_bADDSELF = true;
#endif	// _DEBUG
	}

	void SetDUALSEQUENCE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDUALSEQUENCE = i;
#ifdef _DEBUG
		m_bDUALSEQUENCE = true;
#endif	// _DEBUG
	}

	void SetMAXLUMFRAMEBLEND2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMAXLUMFRAMEBLEND2 = i;
#ifdef _DEBUG
		m_bMAXLUMFRAMEBLEND2 = true;
#endif	// _DEBUG
	}

	void SetSEQUENCE_BLEND_MODE( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nSEQUENCE_BLEND_MODE = i;
#ifdef _DEBUG
		m_bSEQUENCE_BLEND_MODE = true;
#endif	// _DEBUG
	}

	void SetCOLORRAMP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOLORRAMP = i;
#ifdef _DEBUG
		m_bCOLORRAMP = true;
#endif	// _DEBUG
	}

	void SetDEPTHBLEND( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDEPTHBLEND = i;
#ifdef _DEBUG
		m_bDEPTHBLEND = true;
#endif	// _DEBUG
	}

	void SetMOD2X( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMOD2X = i;
#ifdef _DEBUG
		m_bMOD2X = true;
#endif	// _DEBUG
	}

	lux_spritecard_ps30_Static_Index(  )
	{
		m_nANIMBLEND = 0;
		m_nMAXLUMFRAMEBLEND1 = 0;
		m_nEXTRACTGREENALPHA = 0;
		m_nADDBASETEXTURE2 = 0;
		m_nADDSELF = 0;
		m_nDUALSEQUENCE = 0;
		m_nMAXLUMFRAMEBLEND2 = 0;
		m_nSEQUENCE_BLEND_MODE = 0;
		m_nCOLORRAMP = 0;
		m_nDEPTHBLEND = 0;
		m_nMOD2X = 0;
#ifdef _DEBUG
		m_bANIMBLEND = false;
		m_bMAXLUMFRAMEBLEND1 = false;
		m_bEXTRACTGREENALPHA = false;
		m_bADDBASETEXTURE2 = false;
		m_bADDSELF = false;
		m_bDUALSEQUENCE = false;
		m_bMAXLUMFRAMEBLEND2 = false;
		m_bSEQUENCE_BLEND_MODE = false;
		m_bCOLORRAMP = false;
		m_bDEPTHBLEND = false;
		m_bMOD2X = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bANIMBLEND && m_bMAXLUMFRAMEBLEND1 && m_bEXTRACTGREENALPHA && m_bADDBASETEXTURE2 && m_bADDSELF && m_bDUALSEQUENCE && m_bMAXLUMFRAMEBLEND2 && m_bSEQUENCE_BLEND_MODE && m_bCOLORRAMP && m_bDEPTHBLEND && m_bMOD2X );
		AssertMsg( !( ( m_nANIMBLEND == 1 ) && ( m_nMAXLUMFRAMEBLEND1 == 1 ) ), "Invalid combo combination ( ( ANIMBLEND == 1 ) && ( MAXLUMFRAMEBLEND1 == 1 ) )" );
		AssertMsg( !( ( ( m_nANIMBLEND == 1 ) || ( m_nMAXLUMFRAMEBLEND1 == 1 ) ) && ( ( m_nEXTRACTGREENALPHA == 1 ) && ( m_nADDBASETEXTURE2 == 0 ) ) ), "Invalid combo combination ( ( ( ANIMBLEND == 1 ) || ( MAXLUMFRAMEBLEND1 == 1 ) ) && ( ( EXTRACTGREENALPHA == 1 ) && ( ADDBASETEXTURE2 == 0 ) ) )" );
		AssertMsg( !( ( m_nADDBASETEXTURE2 == 1 ) && ( m_nADDSELF == 1 ) ), "Invalid combo combination ( ( ADDBASETEXTURE2 == 1 ) && ( ADDSELF == 1 ) )" );
		AssertMsg( !( ( m_nDUALSEQUENCE == 0 ) && ( ( m_nMAXLUMFRAMEBLEND2 == 1 ) || ( m_nSEQUENCE_BLEND_MODE != 0 ) ) ), "Invalid combo combination ( ( DUALSEQUENCE == 0 ) && ( ( MAXLUMFRAMEBLEND2 == 1 ) || ( SEQUENCE_BLEND_MODE != 0 ) ) )" );
		return ( 1 * m_nANIMBLEND ) + ( 2 * m_nMAXLUMFRAMEBLEND1 ) + ( 4 * m_nEXTRACTGREENALPHA ) + ( 8 * m_nADDBASETEXTURE2 ) + ( 16 * m_nADDSELF ) + ( 32 * m_nDUALSEQUENCE ) + ( 64 * m_nMAXLUMFRAMEBLEND2 ) + ( 128 * m_nSEQUENCE_BLEND_MODE ) + ( 384 * m_nCOLORRAMP ) + ( 768 * m_nDEPTHBLEND ) + ( 1536 * m_nMOD2X ) + 0;
	}
};

#define shaderStaticTest_lux_spritecard_ps30 psh_forgot_to_set_static_ANIMBLEND + psh_forgot_to_set_static_MAXLUMFRAMEBLEND1 + psh_forgot_to_set_static_EXTRACTGREENALPHA + psh_forgot_to_set_static_ADDBASETEXTURE2 + psh_forgot_to_set_static_ADDSELF + psh_forgot_to_set_static_DUALSEQUENCE + psh_forgot_to_set_static_MAXLUMFRAMEBLEND2 + psh_forgot_to_set_static_SEQUENCE_BLEND_MODE + psh_forgot_to_set_static_COLORRAMP + psh_forgot_to_set_static_DEPTHBLEND + psh_forgot_to_set_static_MOD2X


class lux_spritecard_ps30_Dynamic_Index
{
public:
	lux_spritecard_ps30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_spritecard_ps30 1

