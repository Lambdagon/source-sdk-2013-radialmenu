//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef __VMAINMENU_H__
#define __VMAINMENU_H__

#include "basemodui.h"
#include "VFlyoutMenu.h"
#include "menu_background.h"
#ifdef GAMPADUI
#include "../../../gamepadui/gamepadui_frame.h"
#include "../../../gamepadui/gamepadui_button.h"
#include "../../../gamepadui/gamepadui_image.h"
#endif

namespace BaseModUI {

class MainMenu : public CBaseModFrame, public IBaseModFrameListener, public FlyoutMenuListener
{
	DECLARE_CLASS_SIMPLE( MainMenu, CBaseModFrame );

public:
	MainMenu(vgui::Panel *parent, const char *panelName);
	virtual void MainMenuLogo(vgui::EditablePanel *parent);
	~MainMenu();

	void UpdateVisibility();

	MESSAGE_FUNC_CHARPTR( OpenMainMenuJoinFailed, "OpenMainMenuJoinFailed", msg );
	
	//flyout menu listener
	virtual void OnNotifyChildFocus( vgui::Panel* child );
	virtual void OnFlyoutMenuClose( vgui::Panel* flyTo );
	virtual void OnFlyoutMenuCancelled();

	void StartMainMenuVideo();
	void StopMainMenuVideo();

	class CMainMenu* m_pMainMenuPanel;

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnCommand(const char *command);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void OnThink();
	virtual void OnOpen();
	virtual void RunFrame();
	virtual void PaintBackground();

private:
	static void AcceptCommentaryRulesCallback();
	static void AcceptVersusSoftLockCallback();
	static void AcceptQuitGameCallback();
	void SetFooterState();
	//ImagePanel* m_pLogo;
	wchar_t GameTitle[ 2 ];
	vgui::HFont m_hLogoFont;
#ifdef GAMEPADUI
	GamepadUIString m_LogoText[2];
	GamepadUIImage  m_LogoImage;
#endif
	KeyValues* m_pModData;
	bool				m_bFullscreenPoster;
	bool				m_bDrawBackground;
	vgui::ImagePanel* m_pBGImage;

	int m_DrawLogoX1, m_DrawLogoY1;
	int m_DrawLogoX2, m_DrawLogoY2;
	color32 m_DrawLogoColor1;
	color32 m_DrawLogoColor2;
	wchar_t* m_DrawLogoText1;
	wchar_t* m_DrawLogoText2;
	vgui::HFont m_DrawLogoFont;

	enum MainMenuQuickJoinHelpText
	{
		MMQJHT_NONE,
		MMQJHT_QUICKMATCH,
		MMQJHT_QUICKSTART,
	};
	
	int					m_iQuickJoinHelpText;
};

}

#endif // __VMAINMENU_H__
