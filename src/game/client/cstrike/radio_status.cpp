//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include <string.h>
#include <stdio.h>
#include "voice_status.h"
#include "radio_status.h"
#include "c_playerresource.h"
#include "cliententitylist.h"
#include "c_baseplayer.h"
#include "materialsystem/imesh.h"
#include "view.h"
#include "materialsystem/imaterial.h"
#include "tier0/dbg.h"
#include "cdll_int.h"
#include "c_cs_player.h"
#include "menu.h" // for CHudMenu defs

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

// ---------------------------------------------------------------------- //
// The radio feedback manager for the client.
// ---------------------------------------------------------------------- //
static CRadioStatus s_RadioStatus;

//
//-----------------------------------------------------
//

// Stuff for the Radio Menus
static void radio1_f( void );
static void radio2_f( void );
static void radio3_f( void );

//static ConCommand radio1( "radio1", radio1_f, "Opens a radio menu" );
//static ConCommand radio2( "radio2", radio2_f, "Opens a radio menu" );
//static ConCommand radio3( "radio3", radio3_f, "Opens a radio menu" );
static int g_whichMenu = 0;

//
//--------------------------------------------------------------
//
// These methods will bring up the radio menus from the client side.
// They mimic the old server commands of the same name, which used
// to require a round-trip causing latency and unreliability in 
// menu responses.  Only 1 message is sent to the server now which
// includes both the menu name and the selected item.  The server
// is never informed that the menu has been displayed.
//
//--------------------------------------------------------------
//
void OpenRadioMenu( int index )
{
	// do not show the menu if the player is dead or is an observer
	C_CSPlayer *pPlayer = C_CSPlayer::GetLocalCSPlayer();
	if ( !pPlayer )
		return;

	if ( !pPlayer->IsAlive() || pPlayer->IsObserver() )
		return;

	CHudMenu *pMenu = (CHudMenu *) gHUD.FindElement( "CHudMenu" );
	if ( !pMenu )
		return;

	g_whichMenu = index;

	//
	// the 0x23f and 0x3ff are masks that describes the keys that
	// are valid.  This will have to be changed if the menus change.
	//
	switch ( index )
	{
	case 1:
		pMenu->ShowMenu( "#RadioA", 0x23f );
		break;
	case 2:
		pMenu->ShowMenu( "#RadioB", 0x23f );
		break;
	case 3:
		pMenu->ShowMenu( "#RadioC", 0x3ff );
		break;
	default:
		g_whichMenu = 0;
	}
}

static void radio1_f( void )
{
	OpenRadioMenu( 1 );
}

static void radio2_f( void )
{
	OpenRadioMenu( 2 );
}

static void radio3_f( void )
{
	OpenRadioMenu( 3 );
}

//
//-----------------------------------------------------
//

CRadioStatus* RadioManager()
{
	return &s_RadioStatus;
}


// ---------------------------------------------------------------------- //
// CRadioStatus.
// ---------------------------------------------------------------------- //

CRadioStatus::CRadioStatus()
{
	m_pHeadLabelMaterial = NULL;
	Q_memset(m_radioUntil, 0, sizeof(m_radioUntil));
	Q_memset(m_voiceUntil, 0, sizeof(m_voiceUntil));
}

bool CRadioStatus::Init()
{
	if ( !m_pHeadLabelMaterial )
	{
		m_pHeadLabelMaterial = materials->FindMaterial( "sprites/radio", TEXTURE_GROUP_VGUI );
	}

	if ( IsErrorMaterial( m_pHeadLabelMaterial ) )
		return false;

	m_pHeadLabelMaterial->IncrementReferenceCount();

	return true;
}

void CRadioStatus::Shutdown()
{
	if ( m_pHeadLabelMaterial )
		m_pHeadLabelMaterial->DecrementReferenceCount();

	m_pHeadLabelMaterial = NULL;		
}

void CRadioStatus::LevelInitPostEntity()
{
	ExpireBotVoice( true );
	Q_memset(m_radioUntil, 0, sizeof(m_radioUntil));
	Q_memset(m_voiceUntil, 0, sizeof(m_voiceUntil));
}

void CRadioStatus::LevelShutdownPreEntity()
{
	ExpireBotVoice( true );
	Q_memset(m_radioUntil, 0, sizeof(m_radioUntil));
	Q_memset(m_voiceUntil, 0, sizeof(m_voiceUntil));
}

extern float g_flHeadIconSize;

static float s_flHeadOffset = 3;
static float s_flHeadIconSize = 7;

void CRadioStatus::DrawHeadLabels()
{
	ExpireBotVoice();

	if( !m_pHeadLabelMaterial )
		return;

	for(int i=0; i < VOICE_MAX_PLAYERS; i++)
	{
		if ( m_radioUntil[i] < gpGlobals->curtime )
			continue;

		IClientNetworkable *pClient = cl_entitylist->GetClientEntity( i+1 );
		
		// Don't show an icon if the player is not in our PVS.
		if ( !pClient || pClient->IsDormant() )
			continue;

		C_BasePlayer *pPlayer = dynamic_cast<C_BasePlayer*>(pClient);
		if( !pPlayer )
			continue;

		// Don't show an icon for dead or spectating players (ie: invisible entities).
		if( pPlayer->IsPlayerDead() )
			continue;

		// Place it above his head.
		Vector vOrigin = pPlayer->WorldSpaceCenter();
		vOrigin.z += GetClientVoiceMgr()->GetHeadLabelOffset() + s_flHeadOffset;

		if ( GetClientVoiceMgr()->IsPlayerSpeaking( i+1 ) )
		{
			vOrigin.z += g_flHeadIconSize;
		}
		
		// Align it so it never points up or down.
		Vector vUp( 0, 0, 1 );
		Vector vRight = CurrentViewRight();
		if ( fabs( vRight.z ) > 0.95 )	// don't draw it edge-on
			continue;

		vRight.z = 0;
		VectorNormalize( vRight );


		float flSize = s_flHeadIconSize;

		CMatRenderContextPtr pRenderContext( materials );

		pRenderContext->Bind( m_pHeadLabelMaterial );
		IMesh *pMesh = pRenderContext->GetDynamicMesh();
		CMeshBuilder meshBuilder;
		meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

		meshBuilder.Color3f( 1.0, 1.0, 1.0 );
		meshBuilder.TexCoord2f( 0,0,0 );
		meshBuilder.Position3fv( (vOrigin + (vRight * -flSize) + (vUp * flSize)).Base() );
		meshBuilder.AdvanceVertex();

		meshBuilder.Color3f( 1.0, 1.0, 1.0 );
		meshBuilder.TexCoord2f( 0,1,0 );
		meshBuilder.Position3fv( (vOrigin + (vRight * flSize) + (vUp * flSize)).Base() );
		meshBuilder.AdvanceVertex();

		meshBuilder.Color3f( 1.0, 1.0, 1.0 );
		meshBuilder.TexCoord2f( 0,1,1 );
		meshBuilder.Position3fv( (vOrigin + (vRight * flSize) + (vUp * -flSize)).Base() );
		meshBuilder.AdvanceVertex();

		meshBuilder.Color3f( 1.0, 1.0, 1.0 );
		meshBuilder.TexCoord2f( 0,0,1 );
		meshBuilder.Position3fv( (vOrigin + (vRight * -flSize) + (vUp * -flSize)).Base() );
		meshBuilder.AdvanceVertex();
		meshBuilder.End();
		pMesh->Draw();
	}
}


void CRadioStatus::UpdateRadioStatus(int entindex, float duration)
{
	if(entindex > 0 && entindex <= VOICE_MAX_PLAYERS)
	{
		int iClient = entindex - 1;
		if(iClient < 0)
			return;

		m_radioUntil[iClient] = gpGlobals->curtime + duration;
	}
}


void CRadioStatus::UpdateVoiceStatus(int entindex, float duration)
{
	if(entindex > 0 && entindex <= VOICE_MAX_PLAYERS)
	{
		int iClient = entindex - 1;
		if(iClient < 0)
			return;

		m_voiceUntil[iClient] = gpGlobals->curtime + duration;
		GetClientVoiceMgr()->UpdateSpeakerStatus( entindex, true );
	}
}

void CRadioStatus::ExpireBotVoice( bool force )
{
	for(int i=0; i < VOICE_MAX_PLAYERS; i++)
	{
		if ( m_voiceUntil[i] > 0.0f )
		{
			bool expire = force;

			C_CSPlayer *player = static_cast<C_CSPlayer*>( cl_entitylist->GetEnt(i+1) );
			if ( !player )
			{
				// player left the game
				expire = true;
			}
			else if ( m_voiceUntil[i] < gpGlobals->curtime )
			{
				// player is done speaking
				expire = true;
			}

			if ( expire )
			{
				m_voiceUntil[i] = 0.0f;
				GetClientVoiceMgr()->UpdateSpeakerStatus( i+1, false );
			}
		}
	}
}

