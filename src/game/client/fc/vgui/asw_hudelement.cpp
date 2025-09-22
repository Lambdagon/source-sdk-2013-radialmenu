#include "cbase.h"
#include "asw_hudelement.h"
#include "tf_gamerules.h"
#include "c_tf_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar asw_draw_hud("asw_draw_hud","1",0,"Draw the HUD or not");
ConVar asw_hud_alpha("asw_hud_alpha","192",0,"Alpha of the black parts of the HUD (0->255)");

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CASW_HudElement::CASW_HudElement( const char *pElementName ) : CHudElement( pElementName )
{
}

//-----------------------------------------------------------------------------
// Purpose: Hide all the ASW hud in certain cases
//-----------------------------------------------------------------------------
bool CASW_HudElement::ShouldDraw( void )
{
	if (!CHudElement::ShouldDraw())
		return false;

	if (engine->IsLevelMainMenuBackground())
		return false;

	/*if (TFGameRules())
	{
		if (TFGameRules()->IsIntroMap() || TFGameRules()->IsOutroMap())
			return false;
	}*/

	C_TFPlayer *pTFPlayer = C_TFPlayer::GetLocalTFPlayer();
	// hide things due to turret control
	//if ( ( m_iHiddenBits & HIDEHUD_REMOTE_TURRET ) && (pASWPlayer && pASWPlayer->GetMarine() && pASWPlayer->GetMarine()->IsControllingTurret()) )
	//	return false;
	if ( ( m_iHiddenBits & HIDEHUD_PLAYERDEAD ) && ( pTFPlayer->GetHealth()<= 0) )
		return false;

	if ( !asw_draw_hud.GetBool() )
		return false;

	return true;
}