//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: Game Settings menu with campaign selection via scripts/campaigns.txt
//
//=====================================================================================//
#include "cbase.h"
#include "VGameSettings.h"
#include "KeyValues.h"
#include "VDropDownMenu.h"
#include "VHybridButton.h"
#include "VFooterPanel.h"
#include "vgui/ISurface.h"
#include "EngineInterface.h"
#include "vgui_controls/ImagePanel.h"
#include "nb_header_footer.h"
#include "fmtstr.h"
#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace BaseModUI;

static void WriteServerCFG(KeyValues* pSettings)
{
    FileHandle_t f = filesystem->Open("cfg/l4dserver.cfg", "w", "MOD");

    if (!f)
    {
        Msg("Failed to open l4dserver.cfg for writing\n");
        return;
    }

    const char* difficulty = pSettings->GetString("game/difficulty", "normal");
    int ff = pSettings->GetInt("game/hardcoreFF", 0);
    int onslaught = pSettings->GetInt("game/onslaught", 0);

    filesystem->FPrintf(f, "mp_gamemode %s\n", pSettings->GetString("game/mode", "campaign"));
    filesystem->FPrintf(f, "z_difficulty %s\n", difficulty);
    filesystem->FPrintf(f, "ff_damage_reduction_bullets %d\n", ff);
    filesystem->FPrintf(f, "onslaught_enabled %d\n", onslaught);

    filesystem->Close(f);
}
struct CommandCampaignMap_t
{
    const char* command;
    const char* campaign;
};

static CommandCampaignMap_t g_CommandCampaignMap[] =
{
    { "cmd_campaign_L4D2C1",  "Dead Center" },
    { "cmd_campaign_L4D2C2",  "Dark Carnival" },
    { "cmd_campaign_L4D2C3",  "Swamp Fever" },
    { "cmd_campaign_L4D2C4",  "Hard Rain" },
    { "cmd_campaign_L4D2C5",  "The Parish" },
    { "cmd_campaign_L4D2C6",  "The Passing" },
    { "cmd_campaign_L4D2C7",  "The Sacrifice" },
    { "cmd_campaign_L4D2C8",  "No Mercy" },
    { "cmd_campaign_L4D2C9",  "Crash Course" },
    { "cmd_campaign_L4D2C10", "Death Toll" },
    { "cmd_campaign_L4D2C11", "Dead Air" },
    { "cmd_campaign_L4D2C12", "Blood Harvest" },
    { "cmd_campaign_L4D2C13", "Cold Stream" },
};
const char* ResolveCampaignFromCommand(const char* cmd)
{
    for (int i = 0; i < ARRAYSIZE(g_CommandCampaignMap); i++)
    {
        if (!Q_stricmp(g_CommandCampaignMap[i].command, cmd))
            return g_CommandCampaignMap[i].campaign;
    }

    return NULL;
}
//=============================================================================
GameSettings::GameSettings( vgui::Panel *parent, const char *panelName ) :
    BaseClass( parent, panelName, true, false ),
    m_pSettings( NULL ),
    m_autodelete_pSettings( (KeyValues *)NULL ),
    m_drpDifficulty( NULL ),
    m_drpGameType( NULL ),
    m_drpFriendlyFire( NULL ),
    m_drpOnslaught( NULL ),
    m_drpStartingMission( NULL ),
    m_pTitle( NULL ),
    m_pHeaderFooter( NULL )
{
    m_pHeaderFooter = new CNB_Header_Footer( this, "HeaderFooter" );
    m_pHeaderFooter->SetTitle( "" );
    //m_pHeaderFooter->SetHeaderEnabled( false );
    //m_pHeaderFooter->SetGradientBarEnabled( true );
    //m_pHeaderFooter->SetGradientBarPos( 140, 190 );

    m_pTitle = new vgui::Label( this, "Title", "" );

    SetDeleteSelfOnClose(true);
    SetProportional( true );
    SetLowerGarnishEnabled( true );
    SetCancelButtonEnabled( true );

    m_pCampaignsKV = NULL;

    m_pCampaignsKV = new KeyValues("Campaigns");
    if (!m_pCampaignsKV->LoadFromFile(filesystem, "scripts/campaigns.txt", "MOD"))
    {
        Msg("ERROR: Failed to load scripts/campaigns.txt\n");
        m_pCampaignsKV->deleteThis();
        m_pCampaignsKV = NULL;
    }
}

GameSettings::~GameSettings()
{
    if (m_pCampaignsKV)
    {
        m_pCampaignsKV->deleteThis();
        m_pCampaignsKV = NULL;
    }
}

void GameSettings::SetDataSettings( KeyValues *pSettings )
{
    m_pSettings = pSettings;
}

void GameSettings::PaintBackground()
{
    const char *szMode = m_pSettings->GetString( "game/mode", "campaign" );
    const char *pTitle = "#ASUI_GameSettings_Solo";

    if ( !Q_stricmp( szMode, "campaign" ) )
        pTitle = "#ASUI_GameSettings_MP_campaign";
    else if ( !Q_stricmp( szMode, "single_mission" ) )
        pTitle = "#ASUI_GameSettings_MP_single_mission";

    m_pTitle->SetText( pTitle );
}

void GameSettings::Activate()
{
    BaseClass::Activate();

    if ( m_drpGameType )
    {
        const char *szGameMode = m_pSettings->GetString( "game/mode", "campaign" );
        m_drpGameType->SetCurrentSelection( !Q_stricmp( szGameMode, "campaign" ) ?
            "#ASUI_GameType_Campaign" : "#ASUI_GameType_Single_Mission" );

        UpdateSelectMissionButton();
    }

    if ( m_drpDifficulty )
        m_drpDifficulty->SetCurrentSelection( CFmtStr( "#L4D360UI_Difficulty_%s", m_pSettings->GetString( "game/difficulty", "normal" ) ) );

    if ( m_drpFriendlyFire )
        m_drpFriendlyFire->SetCurrentSelection( m_pSettings->GetInt( "game/hardcoreFF", 0 ) ? "#L4D360UI_HardcoreFF" : "#L4D360UI_RegularFF" );

    if ( m_drpOnslaught )
        m_drpOnslaught->SetCurrentSelection( m_pSettings->GetInt( "game/onslaught", 0 ) ? "#L4D360UI_OnslaughtEnabled" : "#L4D360UI_OnslaughtDisabled" );

    UpdateMissionImage();
    UpdateFooter();
}

void GameSettings::OnCommand(const char *command)
{
    const char* campaign = ResolveCampaignFromCommand(command);
    if (campaign)
    {
        m_pSettings->SetString("game/campaign", campaign);

        if (m_pCampaignsKV)
        {
            KeyValues* pCamp = m_pCampaignsKV->FindKey(campaign);
            if (pCamp)
            {
                KeyValues* pMaps = pCamp->FindKey("maps");
                if (pMaps && pMaps->GetFirstSubKey())
                {
                    const char* firstMap = pMaps->GetFirstSubKey()->GetName();
                    m_pSettings->SetString("game/mission", firstMap);

                    Msg("Campaign '%s' selected -> First map: %s\n", campaign, firstMap);
                }
            }
        }

        UpdateSelectMissionButton();
        UpdateMissionImage();
    }

    if ( V_strcmp( command, "cmd_gametype_campaign" ) == 0 )
    {
        m_pSettings->SetString( "game/mode", "campaign" );
        UpdateSelectMissionButton();
        UpdateMissionImage();
    }
    else if ( V_strcmp( command, "cmd_gametype_single_mission" ) == 0 )
    {
        m_pSettings->SetString( "game/mode", "single_mission" );
        UpdateSelectMissionButton();
        UpdateMissionImage();
    }
    else if ( V_strcmp( command, "cmd_change_mission" ) == 0 || V_strcmp( command, "cmd_change_starting_mission" ) == 0 )
    {
        ShowMissionSelect();
    }
    else if ( V_strcmp( command, "StartGame" ) == 0 )
    {
        Navigate();
    }
    else if (V_strcmp(command, "Back") == 0)
    {
        CBaseModPanel::GetSingleton().OpenWindow(WT_MAINMENU, this, false);

        GameSettings* self =
            static_cast<GameSettings*>(CBaseModPanel::GetSingleton().GetWindow(WT_GAMESETTINGS));

        if (self)
        {
            self->Close();
        }
    }
    else if ( const char *szDifficultyValue = StringAfterPrefix( command, "#L4D360UI_Difficulty_" ) )
    {
        m_pSettings->SetString( "game/difficulty", szDifficultyValue );
    }
    else if ( !Q_strcmp( command, "#L4D360UI_RegularFF" ) )
        m_pSettings->SetInt( "game/hardcoreFF", 0 );
    else if ( !Q_strcmp( command, "#L4D360UI_HardcoreFF" ) )
        m_pSettings->SetInt( "game/hardcoreFF", 1 );
    else if ( !Q_strcmp( command, "#L4D360UI_OnslaughtDisabled" ) )
        m_pSettings->SetInt( "game/onslaught", 0 );
    else if ( !Q_strcmp( command, "#L4D360UI_OnslaughtEnabled" ) )
        m_pSettings->SetInt( "game/onslaught", 1 );
    else
    {
        BaseClass::OnCommand( command );
    }
}

void GameSettings::Navigate()
{
	if ( !m_pSettings )
	{
		Msg( "GameSettings: No settings, aborting start.\n" );
		return;
	}

	CBaseModPanel::GetSingleton().PlayUISound( UISOUND_ACCEPT );

	Msg( "GameSettings: Starting game...\n" );

	KeyValues *pLaunch = new KeyValues( "LaunchGame" );

	KeyValues *pGame = pLaunch->FindKey( "game", true );
	KeyValues *pSystem = pLaunch->FindKey( "system", true );

	pGame->SetString( "mode", m_pSettings->GetString( "game/mode", "campaign" ) );
	pGame->SetString( "campaign", m_pSettings->GetString( "game/campaign", "jacob" ) );
	pGame->SetString( "mission", m_pSettings->GetString( "game/mission", "c1m1_hotel" ) );
	pGame->SetString( "difficulty", m_pSettings->GetString( "game/difficulty", "normal" ) );

	pGame->SetInt( "hardcoreFF", m_pSettings->GetInt( "game/hardcoreFF", 0 ) );
	pGame->SetInt( "onslaught", m_pSettings->GetInt( "game/onslaught", 0 ) );

	pSystem->SetString( "network", m_pSettings->GetString( "system/network", "LIVE" ) );
	pSystem->SetString( "access", m_pSettings->GetString( "system/access", "public" ) );

	FileHandle_t f = filesystem->Open("cfg/l4dserver.cfg", "w", "MOD");
	if ( f )
	{
		filesystem->FPrintf(f, "sv_cheats 0\n");
		filesystem->FPrintf(f, "mp_gamemode %s\n", pGame->GetString("mode", "campaign"));
		filesystem->FPrintf(f, "z_difficulty %s\n", pGame->GetString("difficulty", "normal"));
		filesystem->FPrintf(f, "ff_damage_reduction_bullets %d\n", pGame->GetInt("hardcoreFF", 0));
		filesystem->FPrintf(f, "onslaught_enabled %d\n", pGame->GetInt("onslaught", 0));
		filesystem->Close(f);
	}
	else
	{
		Msg("Failed to write cfg/l4dserver.cfg\n");
	}

	const char *mode = pGame->GetString("mode", "campaign");
	const char *map = pGame->GetString("mission", "");


	if ( !Q_stricmp(mode, "campaign") && (!map || !*map) )
	{
		map = "";
	}

	Msg("Launching map: %s (mode: %s)\n", map, mode);

	engine->ClientCmd(VarArgs("map %s; exec l4dserver\n", map));

	CBaseModPanel::GetSingleton().CloseAllWindows();

	pLaunch->deleteThis();
}

void GameSettings::ApplySchemeSettings( vgui::IScheme *pScheme )
{
    BaseClass::ApplySchemeSettings( pScheme );

    LoadControlSettings( "Resource/UI/l4d360ui/GameSettings.res" );

    SetPaintBackgroundEnabled( true );
    SetupAsDialogStyle();

    m_drpDifficulty      = dynamic_cast<DropDownMenu*>( FindChildByName( "DrpDifficulty" ) );
    m_drpGameType        = dynamic_cast<DropDownMenu*>( FindChildByName( "DrpGameType" ) );
    m_drpFriendlyFire    = dynamic_cast<DropDownMenu*>( FindChildByName( "DrpFriendlyFire" ) );
    m_drpOnslaught       = dynamic_cast<DropDownMenu*>( FindChildByName( "DrpOnslaught" ) );
    m_drpStartingMission = dynamic_cast<DropDownMenu*>( FindChildByName( "DrpStartingMission" ) );

   Activate();
}

void GameSettings::OnKeyCodePressed( KeyCode code )
{
    if ( GetBaseButtonCode( code ) == KEY_XBUTTON_B )
    {
        CBaseModPanel::GetSingleton().PlayUISound( UISOUND_BACK );
        NavigateBack();
    }
    else
        BaseClass::OnKeyCodePressed( code );
}

void GameSettings::UpdateFooter()
{
    CBaseModFooterPanel *footer = BaseModUI::CBaseModPanel::GetSingleton().GetFooterPanel();
   /* if ( footer )
    {
        footer->SetButtons( FB_ABUTTON | FB_BBUTTON, FF_AB_ONLY, false );
        footer->SetButtonText( FB_ABUTTON, "#L4D360UI_Select" );
        footer->SetButtonText( FB_BBUTTON, "#L4D360UI_Cancel" );
    }*/
}

// Stubs
void GameSettings::ShowMissionSelect() { Msg( "[GameSettings] Mission select stub\n" ); }
void GameSettings::ShowStartingMissionSelect() { Msg( "[GameSettings] Starting mission stub\n" ); }

void GameSettings::UpdateMissionImage()
{
    ImagePanel* img = dynamic_cast<ImagePanel*>( FindChildByName( "ImgLevelImage" ) );
    if ( img )
    {
        const char *szMission = m_pSettings->GetString( "game/mission", "c1m1_hotel" );
		ConColorMsg(Color(255, 0, 0, 255), "Updating mission image: %s\n", szMission);
        img->SetImage( VarArgs( "maps/%s", szMission ) );
    }
}

void GameSettings::UpdateSelectMissionButton()
{
    DropDownMenu *menu = dynamic_cast<DropDownMenu*>( FindChildByName( "DrpSelectMission", true ) );
    if ( !menu ) return;
    BaseModHybridButton *btn = menu->GetButton();
    if ( !btn ) return;

    const char *mode = m_pSettings->GetString( "game/mode", "campaign" );
	ConColorMsg(Color(255, 0, 0, 255), "Updating select mission button text for mode: %s\n", mode);
    btn->SetText( !Q_stricmp( mode, "campaign" ) ? "#ASUI_Select_Campaign" : "#ASUI_Select_Mission" );
}

void GameSettings::OnClose()
{
    BaseClass::OnClose();
    if ( m_drpDifficulty ) m_drpDifficulty->CloseDropDown();
    if ( m_drpGameType ) m_drpGameType->CloseDropDown();
    if ( m_drpFriendlyFire ) m_drpFriendlyFire->CloseDropDown();
    if ( m_drpOnslaught ) m_drpOnslaught->CloseDropDown();
    m_pSettings = NULL;
}

void GameSettings::OnFlyoutMenuClose( vgui::Panel* flyTo )
{
    //UpdateFooter();
    UpdateMissionImage();
    UpdateSelectMissionButton();
}

void GameSettings::OnFlyoutMenuCancelled() {}
void GameSettings::OnNotifyChildFocus( vgui::Panel* child ) {}
void GameSettings::OnThink() { BaseClass::OnThink(); }

static void ShowGameSettings()
{
    CBaseModFrame* mainMenu = CBaseModPanel::GetSingleton().GetWindow(WT_MAINMENU);
    CBaseModPanel::GetSingleton().OpenWindow(WT_GAMESETTINGS, mainMenu);
}

ConCommand showGameSettings("showGameSettings", ShowGameSettings);
