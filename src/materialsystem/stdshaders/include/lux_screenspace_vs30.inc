#pragma once
#include "shaderlib/cshader.h"
class lux_screenspace_vs30_Static_Index
{
	unsigned int m_nX360APPCHOOSER : 2;
#ifdef _DEBUG
	bool m_bX360APPCHOOSER : 1;
#endif	// _DEBUG
public:
	void SetX360APPCHOOSER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nX360APPCHOOSER = i;
#ifdef _DEBUG
		m_bX360APPCHOOSER = true;
#endif	// _DEBUG
	}

	lux_screenspace_vs30_Static_Index(  )
	{
		m_nX360APPCHOOSER = 0;
#ifdef _DEBUG
		m_bX360APPCHOOSER = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bX360APPCHOOSER );
		return ( 1 * m_nX360APPCHOOSER ) + 0;
	}
};

#define shaderStaticTest_lux_screenspace_vs30 vsh_forgot_to_set_static_X360APPCHOOSER


class lux_screenspace_vs30_Dynamic_Index
{
public:
	lux_screenspace_vs30_Dynamic_Index(  )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_lux_screenspace_vs30 1

