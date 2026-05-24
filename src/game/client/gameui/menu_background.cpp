//========= Copyright Jorge "BSVino" Rodriguez, All rights reserved. ============//
//
// Purpose: Animated menu background
//
//====================================================================================//

#include "cbase.h"
#include "menu_background.h"
#include <cdll_client_int.h>
#include <ienginevgui.h>
#include <KeyValues.h>
#include "iclientmode.h"

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>

#include "clienteffectprecachesystem.h"
#include "tier0/icommandline.h"
#include "fmtstr.h"
#include "baseviewport.h"
#if TF_CLIENT_DLL
#include "tf/tf_hud_mainmenuoverride.h"
#include "tf/tf_shareddefs.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// ???


static bool bOverrideVideo = false;
static char* bOverrideVideoBuf = NULL;
static char* bOverrideVideoSongBuf = NULL;
static float flVolume = NULL;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMainMenu::CMainMenu(vgui::Panel* parent, const char* pElementName) : vgui::Panel(NULL, "MainMenu")
{
	vgui::VPANEL pParent = enginevgui->GetPanel(PANEL_GAMEUIDLL);
	SetParent(pParent);
	SetBuildModeEditable(false);
	SetVisible(false);
	SetPaintEnabled(false);
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetPaintBorderEnabled(false);
	m_VideoMaterial = NULL;
	m_nPlaybackWidth = 0;
	m_nPlaybackHeight = 0;

	m_bToolsMode = false;
	m_bGamepadUIMode = false;
	m_bLoaded = false;

	m_DrawLogoX1 = 0;
	m_DrawLogoY1 = 0;

	m_DrawLogoX2 = 0;
	m_DrawLogoY2 = 0;

	m_DrawLogoText1 = NULL;
	m_DrawLogoText2 = NULL;

	m_DrawLogoColor1 = color32{ 255, 255, 255, 255 };
	m_DrawLogoColor2 = color32{ 255, 255, 255, 128 };

	m_DrawLogoFont = INVALID_FONT;
	bInit = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CMainMenu::~CMainMenu()
{
	ReleaseVideo();
	MarkForDeletion();

	if (m_DrawLogoText1)
		free(m_DrawLogoText1);

	if (m_DrawLogoText2)
		free(m_DrawLogoText2);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMainMenu::ApplySchemeSettings(IScheme* pScheme)
{
	if (bInit)
		return;

	bInit = true;
	SetPos(-1, -1);
	SetSize(ScreenWidth() + 2, ScreenHeight() + 2);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CMainMenu::IsVideoPlaying()
{
	return m_bPaintVideo;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMainMenu::StartVideo()
{
	m_bToolsMode = (IsPC() && (CommandLine()->CheckParm("-tools") != NULL)) ? true : false;

	SetVisible(true);
	SetPaintEnabled(true);

	ConVarRef sv_unlockedchapters("sv_unlockedchapters");

	// If an override video was specified, play that. Otherwise pick a random .webm from media/menubackgrounds
	if (bOverrideVideo && bOverrideVideoBuf)
	{
		if (BeginPlayback(bOverrideVideoBuf))
			m_bLoaded = true;
	}
	else
	{
		FileFindHandle_t fh;
		const char* p = g_pFullFileSystem->FindFirstEx("media/menubackgrounds/*.webm", "MOD", &fh);
		int total = 0;

		while (p)
		{
			++total;
			p = g_pFullFileSystem->FindNext(fh);
		}
		g_pFullFileSystem->FindClose(fh);

		if (total > 0)
		{
			int idx = (int)(Plat_FloatTime() * 1000.0f) % total;
			int cur = 0;
			char selected[MAX_PATH] = { 0 };

			p = g_pFullFileSystem->FindFirstEx("media/menubackgrounds/*.webm", "MOD", &fh);
			while (p)
			{
				if (cur == idx)
				{
					if (Q_strnicmp(p, "media/menubackgrounds/", 6) == 0)
						Q_strncpy(selected, p, sizeof(selected));
					else
						Q_snprintf(selected, sizeof(selected), "media/menubackgrounds/%s", p);
					break;
				}
				++cur;
				p = g_pFullFileSystem->FindNext(fh);
			}
			g_pFullFileSystem->FindClose(fh);

			if (selected[0] != '\0')
			{
				if (BeginPlayback(selected))
					m_bLoaded = true;
				else
				{
					if (BeginPlayback("media/valve.webm"))
						m_bLoaded = true;
				}
			}
			else
			{
				if (BeginPlayback("media/valve.webm"))
					m_bLoaded = true;
			}
		}
		else
		{
			if (BeginPlayback("media/valve.webm"))
				m_bLoaded = true;
		}
	}

	if (bOverrideVideoBuf)
	{
		free(bOverrideVideoBuf);
		bOverrideVideoBuf = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMainMenu::StopVideo()
{
	// NOTE(Tony): Release the video when stopping
	// NOTE(Tony): Modified, don't hide the panel anymore, because we draw the main menu logo thing for in-game
	//SetVisible( false ); 
	//SetPaintEnabled( false );
	ReleaseVideo();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMainMenu::GetPanelPos(int& xpos, int& ypos)
{
	vgui::ipanel()->GetAbsPos(GetVPanel(), xpos, ypos);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMainMenu::Paint(void)
{
	m_bGamepadUIMode = (IsPC() && (CommandLine()->CheckParm("-gamepadui") != NULL)) ? true : false;
	if (m_bToolsMode)
		return;

	if (engine->IsConnected())
		return;

	if (!m_bLoaded)
		return;

	if (m_bPaintVideo)
	{
		m_nPlaybackHeight = ScreenHeight() + 2;
		m_nPlaybackWidth = ScreenWidth() + 2;

		// No video to play, so do nothing
		if (m_VideoMaterial == NULL)
			return;

		// Update our frame
		if (m_VideoMaterial->Update() == false)
			return;

		float cur_vidtime = m_VideoMaterial->GetCurrentVideoTime();
		float dur_vidtime = m_VideoMaterial->GetVideoDuration();
		if ((cur_vidtime + 0.1) >= dur_vidtime)
			m_VideoMaterial->SetTime(0);

		// Sit in the "center"
		int xpos, ypos;
		GetPanelPos(xpos, ypos);

		// Draw the polys to draw this out
		CMatRenderContextPtr pRenderContext(materials);

		pRenderContext->MatrixMode(MATERIAL_VIEW);
		pRenderContext->PushMatrix();
		pRenderContext->LoadIdentity();

		pRenderContext->MatrixMode(MATERIAL_PROJECTION);
		pRenderContext->PushMatrix();
		pRenderContext->LoadIdentity();

		pRenderContext->Bind(m_pMaterial, NULL);

		CMeshBuilder meshBuilder;
		IMesh* pMesh = pRenderContext->GetDynamicMesh(true);
		meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

		float flLeftX = xpos;
		float flRightX = xpos + (m_nPlaybackWidth - 1);

		float flTopY = ypos;
		float flBottomY = ypos + (m_nPlaybackHeight - 1);

		// Map our UVs to cut out just the portion of the video we're interested in
		float flLeftU = 0.0f;
		float flTopV = 0.0f;

		// We need to subtract off a pixel to make sure we don't bleed
		float flRightU = m_flU - (1.0f / (float)m_nPlaybackWidth);
		float flBottomV = m_flV - (1.0f / (float)m_nPlaybackHeight);

		// Get the current viewport size
		int vx, vy, vw, vh;
		pRenderContext->GetViewport(vx, vy, vw, vh);

		// Map from screen pixel coords to -1..1
		flRightX = FLerp(-1, 1, 0, vw, flRightX);
		flLeftX = FLerp(-1, 1, 0, vw, flLeftX);
		flTopY = FLerp(1, -1, 0, vh, flTopY);
		flBottomY = FLerp(1, -1, 0, vh, flBottomY);

		float alpha = ((float)GetFgColor()[3] / 255.0f);

		for (int corner = 0; corner < 4; corner++)
		{
			bool bLeft = (corner == 0) || (corner == 3);
			meshBuilder.Position3f((bLeft) ? flLeftX : flRightX, (corner & 2) ? flBottomY : flTopY, 0.0f);
			meshBuilder.Normal3f(0.0f, 0.0f, 1.0f);
			meshBuilder.TexCoord2f(0, (bLeft) ? flLeftU : flRightU, (corner & 2) ? flBottomV : flTopV);
			meshBuilder.TangentS3f(0.0f, 1.0f, 0.0f);
			meshBuilder.TangentT3f(1.0f, 0.0f, 0.0f);
			meshBuilder.Color4f(1.0f, 1.0f, 1.0f, alpha);
			meshBuilder.AdvanceVertex();
		}

		meshBuilder.End();
		pMesh->Draw();

		pRenderContext->MatrixMode(MATERIAL_VIEW);
		pRenderContext->PopMatrix();

		pRenderContext->MatrixMode(MATERIAL_PROJECTION);
		pRenderContext->PopMatrix();

		if (engine->IsDrawingLoadingImage())
			return;

		if (m_bGamepadUIMode)
			return;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CMainMenu::BeginPlayback(const char* pFilename)
{
	// Need working video services
//	if (g_pVideo == NULL)
//		return false;

	// Destroy any previously allocated video
	if (m_VideoMaterial != NULL)
	{
		g_pVideo->DestroyVideoMaterial(m_VideoMaterial);
		m_VideoMaterial = NULL;
	}

	// Create new Video material
	m_VideoMaterial = g_pVideo->CreateVideoMaterial("VideoMaterial", pFilename, "GAME",
		VideoPlaybackFlags::DEFAULT_MATERIAL_OPTIONS,
		VideoSystem::DETERMINE_FROM_FILE_EXTENSION, m_bAllowAlternateMedia);
	if (m_VideoMaterial == NULL)
		return false;

	m_bPaintVideo = true;

	m_VideoMaterial->SetLooping(true);

	int nWidth, nHeight;
	m_VideoMaterial->GetVideoImageSize(&nWidth, &nHeight);
	m_VideoMaterial->GetVideoTexCoordRange(&m_flU, &m_flV);
	m_pMaterial = m_VideoMaterial->GetMaterial();

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMainMenu::ReleaseVideo()
{
	m_bPaintVideo = false;

	// NOTE(Tony): Not touching the sound!!
	//enginesound->NotifyEndMoviePlayback();

	// Destroy any previously allocated video
	// Shut down this video, destroy the video material
	if (g_pVideo != NULL && m_VideoMaterial != NULL)
	{
		g_pVideo->DestroyVideoMaterial(m_VideoMaterial);
		m_VideoMaterial = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMainMenu::DoModal()
{
	vgui::surface()->RestrictPaintToSinglePanel(GetVPanel());
}

//-----------------------------------------------------------------------------
// Purpose: If we get disconnected, load the menu
//-----------------------------------------------------------------------------
void CMainMenu::OnDisconnectFromGame()
{
	StartVideo();
}

//-----------------------------------------------------------------------------
// Purpose: Find the disconnect command, and rename it...
//-----------------------------------------------------------------------------
CON_COMMAND(__disconnect, "Disconnect game from server.")
{
#if TF_CLIENT_DLL
	IViewPortPanel* pMMOverride = (gViewPortInterface->FindPanelByName(PANEL_MAINMENUOVERRIDE));
	if (pMMOverride)
		((CHudMainMenuOverride*)pMMOverride)->StartMainMenuVideo();
#endif

	engine->ClientCmd_Unrestricted("__real_disconnect");

	if (bOverrideVideoSongBuf)
	{
		free(bOverrideVideoSongBuf);
		bOverrideVideoSongBuf = NULL;
	}
	bOverrideVideo = false;
}

void CC_MapBackground(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		ConMsg("Command Usage: map_background <background video> <override song> <override volume> <override>. If the <override> parameter is 1 then it will override the background video to play to the inputted background video at the 2nd argument, it will also play the song of the 3rd parameter with the volume of the 4th parameter. else it will just play the normal background video (the one that would play if you disconnected)\n");
		return;
	}

	if (atoi(args.Arg(4)) != 0)
	{
		bOverrideVideo = true;
		bOverrideVideoBuf = _strdup(args.Arg(1));
		bOverrideVideoSongBuf = _strdup(args.Arg(2));
		flVolume = atof(args.Arg(3));
	}

	__disconnect(CCommand{});
}

void SwapDisconnectCommand()
{
	//DevMsg("SwapDisconnectCommand\n");
	ConCommand* _realDisconnectCommand = dynamic_cast<ConCommand*>(g_pCVar->FindCommand("disconnect"));
	ConCommand* _DisconnectCommand = dynamic_cast<ConCommand*>(g_pCVar->FindCommand("__disconnect"));
	ConCommand* _Startupmenu = dynamic_cast<ConCommand*>(g_pCVar->FindCommand("startupmenu"));
	ConCommand* map_background = dynamic_cast<ConCommand*>(g_pCVar->FindCommand("map_background"));

	if (!_realDisconnectCommand)
		return;

	if (!_DisconnectCommand)
		return;

	_realDisconnectCommand->Shutdown();
	_realDisconnectCommand->CreateBase("__real_disconnect", "");
	_realDisconnectCommand->Init();

	_DisconnectCommand->Shutdown();
	_DisconnectCommand->CreateBase("disconnect", "Disconnect game from server.");
	_DisconnectCommand->Init();

	if (_Startupmenu)
	{
		_Startupmenu->m_bHasCompletionCallback = false;
		_Startupmenu->m_fnCommandCallback = __disconnect;
	}

	if (map_background)
	{
		map_background->Shutdown();
		map_background->CreateBase("map_background", "Plays the background video that would play when you disconnect/goto the main menu. execute this command with no args for command explination\n");
		map_background->Init();
		map_background->m_bHasCompletionCallback = false;
		map_background->m_fnCommandCallback = CC_MapBackground;
	}
}