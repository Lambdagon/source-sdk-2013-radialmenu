// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($DUALSEQUENCE == 0 && $ZOOM_ANIMATE_SEQ2 == 1)

#pragma once
#include "shaderlib/cshader.h"
class lux_spritecard_vs30_Static_Index
{
	unsigned int m_nDUALSEQUENCE : 2;
	unsigned int m_nZOOM_ANIMATE_SEQ2 : 2;
	unsigned int m_nEXTRACTGREENALPHA : 2;
	unsigned int m_nADDBASETEXTURE2 : 2;
#ifdef _DEBUG
	bool m_bDUALSEQUENCE : 1;
	bool m_bZOOM_ANIMATE_SEQ2 : 1;
	bool m_bEXTRACTGREENALPHA : 1;
	bool m_bADDBASETEXTURE2 : 1;
#endif	// _DEBUG
public:
	void SetDUALSEQUENCE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDUALSEQUENCE = i;
#ifdef _DEBUG
		m_bDUALSEQUENCE = true;
#endif	// _DEBUG
	}

	void SetZOOM_ANIMATE_SEQ2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nZOOM_ANIMATE_SEQ2 = i;
#ifdef _DEBUG
		m_bZOOM_ANIMATE_SEQ2 = true;
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

	lux_spritecard_vs30_Static_Index(  )
	{
		m_nDUALSEQUENCE = 0;
		m_nZOOM_ANIMATE_SEQ2 = 0;
		m_nEXTRACTGREENALPHA = 0;
		m_nADDBASETEXTURE2 = 0;
#ifdef _DEBUG
		m_bDUALSEQUENCE = false;
		m_bZOOM_ANIMATE_SEQ2 = false;
		m_bEXTRACTGREENALPHA = false;
		m_bADDBASETEXTURE2 = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bDUALSEQUENCE && m_bZOOM_ANIMATE_SEQ2 && m_bEXTRACTGREENALPHA && m_bADDBASETEXTURE2 );
		AssertMsg( !( ( m_nDUALSEQUENCE == 0 ) && ( m_nZOOM_ANIMATE_SEQ2 == 1 ) ), "Invalid combo combination ( ( DUALSEQUENCE == 0 ) && ( ZOOM_ANIMATE_SEQ2 == 1 ) )" );
		return ( 3 * m_nDUALSEQUENCE ) + ( 6 * m_nZOOM_ANIMATE_SEQ2 ) + ( 12 * m_nEXTRACTGREENALPHA ) + ( 24 * m_nADDBASETEXTURE2 ) + 0;
	}
};

#define shaderStaticTest_lux_spritecard_vs30 vsh_forgot_to_set_static_DUALSEQUENCE + vsh_forgot_to_set_static_ZOOM_ANIMATE_SEQ2 + vsh_forgot_to_set_static_EXTRACTGREENALPHA + vsh_forgot_to_set_static_ADDBASETEXTURE2


class lux_spritecard_vs30_Dynamic_Index
{
	unsigned int m_nORIENTATION : 2;
#ifdef _DEBUG
	bool m_bORIENTATION : 1;
#endif	// _DEBUG
public:
	void SetORIENTATION( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nORIENTATION = i;
#ifdef _DEBUG
		m_bORIENTATION = true;
#endif	// _DEBUG
	}

	lux_spritecard_vs30_Dynamic_Index(  )
	{
		m_nORIENTATION = 0;
#ifdef _DEBUG
		m_bORIENTATION = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bORIENTATION );
		return ( 1 * m_nORIENTATION ) + 0;
	}
};

#define shaderDynamicTest_lux_spritecard_vs30 vsh_forgot_to_set_dynamic_ORIENTATION

