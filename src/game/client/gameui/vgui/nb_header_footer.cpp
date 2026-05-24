#include "cbase.h"
#include "nb_header_footer.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/ImagePanel.h"
#include <vgui/ISurface.h>
#include "VGUIMatSurface/IMatSystemSurface.h"
#include "video/ivideoservices.h"
#include "strtools.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;


// ======================================

CNB_Header_Footer::CNB_Header_Footer( vgui::Panel *parent, const char *name ) : BaseClass( parent, name )
{
	// == MANAGED_MEMBER_CREATION_START: Do not edit by hand ==
	m_pBackground = new vgui::Panel( this, "Background" );
	m_pBackgroundImage = new vgui::ImagePanel( this, "BackgroundImage" );	
	m_pTitle = new vgui::Label( this, "Title", "" );
	m_pBottomBar = new vgui::Panel( this, "BottomBar" );
	m_pBottomBarLine = new vgui::Panel( this, "BottomBarLine" );
	m_pTopBar = new vgui::Panel( this, "TopBar" );
	m_pTopBarLine = new vgui::Panel( this, "TopBarLine" );
	// == MANAGED_MEMBER_CREATION_END ==
	m_pGradientBar = new CNB_Gradient_Bar( this, "GradientBar" );
	m_pGradientBar->SetZPos( 2 );

	m_bHeaderEnabled = true;
	m_bFooterEnabled = true;
	m_bMovieEnabled = true;
	m_bGradientBarEnabled = 0;
	m_nTitleStyle = NB_TITLE_MEDIUM;
	m_nBackgroundStyle = NB_BACKGROUND_TRANSPARENT_BLUE;
	m_nGradientBarY = 0;
	m_nGradientBarHeight = 480;
}

CNB_Header_Footer::~CNB_Header_Footer()
{

}

ConVar asw_background_color( "asw_background_color", "16 32 46 128", FCVAR_NONE, "Color of background tinting in briefing screens" );

void CNB_Header_Footer::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	
	LoadControlSettings( "resource/ui/nb_header_footer.res" );

	// TODO: Different image in widescreen to avoid stretching
	// this image is no longer used
	//m_pBackgroundImage->SetImage( "lobby/swarm_background01" );

	switch( m_nTitleStyle )
	{
		case NB_TITLE_BRIGHT: m_pTitle->SetFgColor( Color( 255, 255, 255, 255 ) ); break;
		case NB_TITLE_MEDIUM: m_pTitle->SetFgColor( Color( 47, 79, 111, 255 ) ); break;
	}

	switch( m_nBackgroundStyle )
	{
		case NB_BACKGROUND_DARK:
			{
				m_pBackground->SetVisible( true );
				m_pBackgroundImage->SetVisible( false );
				m_pBackground->SetBgColor( Color( 0, 0, 0, 230 ) );
				break;
			}
		case NB_BACKGROUND_TRANSPARENT_BLUE:
			{
				m_pBackground->SetVisible( true );
				m_pBackgroundImage->SetVisible( false );
				color32 asw_color{0,0,0,0};
				UTIL_StringToColor32(&asw_color, asw_background_color.GetString());
				m_pBackground->SetBgColor( Color(asw_color.r, asw_color.g, asw_color.b, asw_color.a) );
				break;
			}
		case NB_BACKGROUND_TRANSPARENT_RED:
			{
				m_pBackground->SetVisible( true );
				m_pBackgroundImage->SetVisible( false );
				m_pBackground->SetBgColor( Color( 128, 0, 0, 128 ) );
				break;
			}
		case NB_BACKGROUND_BLUE:
			{
				m_pBackground->SetVisible( true );
				m_pBackgroundImage->SetVisible( false );
				m_pBackground->SetBgColor( Color( 16, 32, 46, 230 ) );
				break;
			}
		case NB_BACKGROUND_IMAGE:
			{
				m_pBackground->SetVisible( false );
				m_pBackgroundImage->SetVisible( true );
				break;
			}

		case NB_BACKGROUND_NONE:
			{
				m_pBackground->SetVisible( false );
				m_pBackgroundImage->SetVisible( false );
			}
	}

	m_pTopBar->SetVisible( m_bHeaderEnabled );
	m_pTopBarLine->SetVisible( m_bHeaderEnabled );
	m_pBottomBar->SetVisible( m_bFooterEnabled );
	m_pBottomBarLine->SetVisible( m_bFooterEnabled );
	m_pGradientBar->SetVisible( m_bGradientBarEnabled );
}

void CNB_Header_Footer::PerformLayout()
{
	BaseClass::PerformLayout();

	m_pGradientBar->SetBounds( 0, YRES( m_nGradientBarY ), ScreenWidth(), YRES( m_nGradientBarHeight ) );
}

void CNB_Header_Footer::OnThink()
{
	BaseClass::OnThink();
}

void CNB_Header_Footer::SetTitle( const char *pszTitle )
{
	m_pTitle->SetText( pszTitle );
}

void CNB_Header_Footer::SetTitle( wchar_t *pwszTitle )
{
	m_pTitle->SetText( pwszTitle );
}

void CNB_Header_Footer::SetHeaderEnabled( bool bEnabled )
{
	m_pTopBar->SetVisible( bEnabled );
	m_pTopBarLine->SetVisible( bEnabled );
	m_bHeaderEnabled = bEnabled;
}

void CNB_Header_Footer::SetFooterEnabled( bool bEnabled )
{
	m_pBottomBar->SetVisible( bEnabled );
	m_pBottomBarLine->SetVisible( bEnabled );
	m_bFooterEnabled = bEnabled;
}

void CNB_Header_Footer::SetGradientBarEnabled( bool bEnabled )
{
	m_pGradientBar->SetVisible( bEnabled );
	m_bGradientBarEnabled = bEnabled;
}

void CNB_Header_Footer::SetGradientBarPos( int nY, int nHeight )
{
	m_nGradientBarY = nY;
	m_nGradientBarHeight = nHeight;
	m_pGradientBar->SetBounds( 0, YRES( m_nGradientBarY ), ScreenWidth(), YRES( m_nGradientBarHeight ) );
}

void CNB_Header_Footer::SetTitleStyle( NB_Title_Style nTitleStyle )
{
	m_nTitleStyle = nTitleStyle;
	InvalidateLayout( false, true );
}

void CNB_Header_Footer::SetBackgroundStyle( NB_Background_Style nBackgroundStyle )
{
	m_nBackgroundStyle = nBackgroundStyle;
	InvalidateLayout( false, true );
}

void CNB_Header_Footer::SetMovieEnabled( bool bMovieEnabled )
{
	m_bMovieEnabled = bMovieEnabled;
	InvalidateLayout( false, true );
}

void CNB_Header_Footer::PaintBackground()
{
    BaseClass::PaintBackground();
}

// =================

CNB_Gradient_Bar::CNB_Gradient_Bar( vgui::Panel *parent, const char *name ) : BaseClass( parent, name )
{
}

void CNB_Gradient_Bar::PaintBackground()
{
	int wide, tall;
	GetSize( wide, tall );

	int y = 0;
	int iHalfWide = wide * 0.5f;

	float flAlpha = 200.0f / 255.0f;

	// fill bar background
	vgui::surface()->DrawSetColor( Color( 0, 0, 0, 255 * flAlpha ) );
	vgui::surface()->DrawFilledRect( 0, y, wide, y + tall );

	vgui::surface()->DrawSetColor( Color( 53, 86, 117, 255 * flAlpha ) );

	int nBarPosY = y + YRES( 4 );
	int nBarHeight = tall - YRES( 8 );
	vgui::surface()->DrawFilledRectFade( iHalfWide, nBarPosY, wide, nBarPosY + nBarHeight, 255, 0, true );
	vgui::surface()->DrawFilledRectFade( 0, nBarPosY, iHalfWide, nBarPosY + nBarHeight, 0, 255, true );
	// draw highlights
	nBarHeight = YRES( 2 );
	nBarPosY = y;
	vgui::surface()->DrawSetColor( Color( 97, 210, 255, 255 * flAlpha ) );
	vgui::surface()->DrawFilledRectFade( iHalfWide, nBarPosY, wide, nBarPosY + nBarHeight, 255, 0, true );
	vgui::surface()->DrawFilledRectFade( 0, nBarPosY, iHalfWide, nBarPosY + nBarHeight, 0, 255, true );

	nBarPosY = y + tall - YRES( 2 );
	vgui::surface()->DrawFilledRectFade( iHalfWide, nBarPosY, wide, nBarPosY + nBarHeight, 255, 0, true );
	vgui::surface()->DrawFilledRectFade( 0, nBarPosY, iHalfWide, nBarPosY + nBarHeight, 0, 255, true );
}
