//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Floating combat text for CS/L4D-style damage feedback.
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "c_cs_player.h"
#include "cdll_util.h"
#include "view.h"
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Panel.h>

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar hud_combattext( "hud_combattext", "1", FCVAR_USERINFO | FCVAR_ARCHIVE, "If set to 1, show floating damage numbers over enemies you hurt." );
ConVar hud_combattext_batching( "hud_combattext_batching", "1", FCVAR_USERINFO | FCVAR_ARCHIVE, "If set to 1, nearby combat text events from the same victim are merged together." );
ConVar hud_combattext_batching_window( "hud_combattext_batching_window", "0.2", FCVAR_ARCHIVE, "Maximum time between damage events to merge combat text.", true, 0.0f, true, 2.0f );
ConVar hud_combattext_red( "hud_combattext_red", "255", FCVAR_USERINFO | FCVAR_ARCHIVE );
ConVar hud_combattext_green( "hud_combattext_green", "255", FCVAR_USERINFO | FCVAR_ARCHIVE );
ConVar hud_combattext_blue( "hud_combattext_blue", "255", FCVAR_USERINFO | FCVAR_ARCHIVE );
ConVar hud_combattext_lifetime( "hud_combattext_lifetime", "1.1", FCVAR_ARCHIVE, "Seconds floating combat text stays on screen.", true, 0.1f, true, 5.0f );
ConVar hud_combattext_rise( "hud_combattext_rise", "26", FCVAR_ARCHIVE, "How many units floating combat text rises over its lifetime.", true, 0.0f, true, 128.0f );

class CHudCombatText : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudCombatText, vgui::Panel );

public:
	CHudCombatText( const char *pElementName );

	virtual void Reset( void ) OVERRIDE;
	virtual bool ShouldDraw( void ) OVERRIDE;
	virtual void FireGameEvent( IGameEvent *event ) OVERRIDE;

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void Paint( void ) OVERRIDE;

private:
	struct combattext_t
	{
		int nAmount;
		int nVictimEntIndex;
		float flStartTime;
		float flDieTime;
		Vector vecOrigin;
		bool bLargeFont;
	};

	void AddOrUpdateText( int nVictimEntIndex, int nAmount, const Vector &vecOrigin, bool bLargeFont );

	CUtlVector< combattext_t > m_Items;
	HFont m_hSmallFont;
	HFont m_hLargeFont;
};

DECLARE_HUDELEMENT( CHudCombatText );

CHudCombatText::CHudCombatText( const char *pElementName ) :
	CHudElement( pElementName ),
	BaseClass( NULL, "HudCombatText" ),
	m_hSmallFont( INVALID_FONT ),
	m_hLargeFont( INVALID_FONT )
{
	SetParent( g_pClientMode->GetViewport() );
	SetHiddenBits( HIDEHUD_PLAYERDEAD );
	SetPaintBackgroundEnabled( false );

	ListenForGameEvent( "player_hurt" );
}

void CHudCombatText::Reset( void )
{
	m_Items.RemoveAll();
}

bool CHudCombatText::ShouldDraw( void )
{
	if ( !CHudElement::ShouldDraw() || !hud_combattext.GetBool() )
		return false;

	C_CSPlayer *pLocal = C_CSPlayer::GetLocalCSPlayer();
	if ( !pLocal || !pLocal->IsAlive() )
	{
		m_Items.RemoveAll();
		return false;
	}

	return ( m_Items.Count() > 0 );
}

void CHudCombatText::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_hSmallFont = pScheme->GetFont( "Default", IsProportional() );
	m_hLargeFont = pScheme->GetFont( "DefaultLarge", IsProportional() );
	if ( m_hLargeFont == INVALID_FONT )
	{
		m_hLargeFont = m_hSmallFont;
	}
}

void CHudCombatText::AddOrUpdateText( int nVictimEntIndex, int nAmount, const Vector &vecOrigin, bool bLargeFont )
{
	const float flNow = gpGlobals->curtime;
	const float flBatchWindow = hud_combattext_batching.GetBool() ? hud_combattext_batching_window.GetFloat() : 0.0f;

	if ( flBatchWindow > 0.0f )
	{
		FOR_EACH_VEC_BACK( m_Items, i )
		{
			combattext_t &item = m_Items[i];
			if ( item.nVictimEntIndex != nVictimEntIndex )
				continue;

			if ( ( flNow - item.flStartTime ) > flBatchWindow )
				continue;

			item.nAmount += nAmount;
			item.flStartTime = flNow;
			item.flDieTime = flNow + hud_combattext_lifetime.GetFloat();
			item.vecOrigin = vecOrigin;
			item.bLargeFont = item.bLargeFont || bLargeFont;
			return;
		}
	}

	combattext_t &item = m_Items[ m_Items.AddToTail() ];
	item.nAmount = nAmount;
	item.nVictimEntIndex = nVictimEntIndex;
	item.flStartTime = flNow;
	item.flDieTime = flNow + hud_combattext_lifetime.GetFloat();
	item.vecOrigin = vecOrigin;
	item.bLargeFont = bLargeFont;
}

void CHudCombatText::FireGameEvent( IGameEvent *event )
{
	if ( !hud_combattext.GetBool() || !FStrEq( event->GetName(), "player_hurt" ) )
		return;

	C_CSPlayer *pLocal = C_CSPlayer::GetLocalCSPlayer();
	if ( !pLocal || !pLocal->IsAlive() )
		return;

	const int nAttackerIndex = engine->GetPlayerForUserID( event->GetInt( "attacker" ) );
	if ( nAttackerIndex != pLocal->entindex() )
		return;

	const int nVictimIndex = engine->GetPlayerForUserID( event->GetInt( "userid" ) );
	C_CSPlayer *pVictim = ToCSPlayer( UTIL_PlayerByIndex( nVictimIndex ) );
	if ( !pVictim || pVictim == pLocal )
		return;

	if ( pVictim->GetTeamNumber() == pLocal->GetTeamNumber() )
		return;

	const int nDamage = event->GetInt( "damageamount", event->GetInt( "dmg_health" ) );
	if ( nDamage <= 0 )
		return;

	trace_t tr;
	UTIL_TraceLine( pVictim->WorldSpaceCenter(), MainViewOrigin(), MASK_SOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &tr );
	if ( tr.fraction < 1.0f )
		return;

	Vector vecOrigin = pVictim->WorldSpaceCenter();
	vecOrigin.z += 18.0f;

	const bool bLargeFont = ( event->GetInt( "hitgroup" ) == HITGROUP_HEAD ) || ( event->GetInt( "health" ) <= 0 );
	AddOrUpdateText( pVictim->entindex(), nDamage, vecOrigin, bLargeFont );
}

void CHudCombatText::Paint( void )
{
	BaseClass::Paint();

	const Color textColor( hud_combattext_red.GetInt(), hud_combattext_green.GetInt(), hud_combattext_blue.GetInt(), 255 );
	const float flLifetime = MAX( 0.01f, hud_combattext_lifetime.GetFloat() );
	const float flRise = hud_combattext_rise.GetFloat();

	FOR_EACH_VEC_BACK( m_Items, i )
	{
		combattext_t &item = m_Items[i];
		if ( item.flDieTime <= gpGlobals->curtime )
		{
			m_Items.Remove( i );
			continue;
		}

		const float flFrac = clamp( ( gpGlobals->curtime - item.flStartTime ) / flLifetime, 0.0f, 1.0f );
		Color drawColor = textColor;
		drawColor[3] = ( flFrac < 0.65f ) ? 255 : (int)( 255.0f * ( 1.0f - RemapValClamped( flFrac, 0.65f, 1.0f, 0.0f, 1.0f ) ) );

		Vector vecDrawPos = item.vecOrigin;
		vecDrawPos.z += flFrac * flRise;

		int x, y;
		if ( !GetVectorInHudSpace( vecDrawPos, x, y ) )
			continue;

		wchar_t wszDamage[16];
		V_swprintf_safe( wszDamage, L"%d", item.nAmount );

		const HFont font = item.bLargeFont ? m_hLargeFont : m_hSmallFont;
		int wide = 0, tall = 0;
		surface()->GetTextSize( font, wszDamage, wide, tall );

		const int textX = x - ( wide / 2 );
		const int textY = y - ( tall / 2 );

		surface()->DrawSetTextFont( font );
		surface()->DrawSetTextPos( textX + 1, textY + 1 );
		surface()->DrawSetTextColor( Color( 0, 0, 0, drawColor[3] ) );
		surface()->DrawPrintText( wszDamage, wcslen( wszDamage ), FONT_DRAW_NONADDITIVE );

		surface()->DrawSetTextPos( textX, textY );
		surface()->DrawSetTextColor( drawColor );
		surface()->DrawPrintText( wszDamage, wcslen( wszDamage ), FONT_DRAW_NONADDITIVE );
	}
}
