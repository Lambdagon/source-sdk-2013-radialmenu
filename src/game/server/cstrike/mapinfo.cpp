//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "mapinfo.h"
#include "cs_gamerules.h"

LINK_ENTITY_TO_CLASS( info_map_parameters, CMapInfo );

ConVar DirectorScavengeItemOverride( "director_scavenge_item_override", "0", FCVAR_GAMEDLL );
ConVar DirectorPainPillDensity( "director_pain_pill_density", "1.0", FCVAR_GAMEDLL );
ConVar DirectorMolotovDensity( "director_molotov_density", "1.0", FCVAR_GAMEDLL );
ConVar DirectorPipeBombDensity( "director_pipe_bomb_density", "1.0", FCVAR_GAMEDLL );
ConVar DirectorPistolDensity( "director_pistol_density", "1.0", FCVAR_GAMEDLL );
ConVar DirectorGasCanDensity( "director_gascan_density", "1.0", FCVAR_GAMEDLL );
ConVar DirectorPropaneTankDensity( "director_propane_tank_density", "1.0", FCVAR_GAMEDLL );
ConVar DirectorOxygenTankDensity( "director_oxygen_tank_density", "1.0", FCVAR_GAMEDLL );
ConVar DirectorItemClusterRange( "director_item_cluster_range", "1.0", FCVAR_GAMEDLL );
ConVar DirectorFinaleItemClusterCount( "director_finale_item_cluster_count", "1", FCVAR_GAMEDLL );

BEGIN_DATADESC( CMapInfo )

	DEFINE_INPUTFUNC( FIELD_INTEGER, "FireWinCondition", InputFireWinCondition ),
	DEFINE_INPUTFUNC( FIELD_VOID, "UpdateCvars", InputUpdateCvars ),

END_DATADESC()

CMapInfo *g_pMapInfo = NULL;

CMapInfo::CMapInfo()
{
	m_flBombRadius = 500.0f;
	m_iBuyingStatus = 0;

	m_gasCanDensity = 1.0f;
	m_molotovDensity = 1.0f;
	m_oxygenTankDensity = 1.0f;
	m_painPillDensity = 1.0f;
	m_pipeBombDensity = 1.0f;
	m_pistolDensity = 1.0f;
	m_propaneTankDensity = 1.0f;
	m_itemClusterRange = 1.0f;

	m_finaleItemClusterCount = 1;

	if ( g_pMapInfo )
	{
		Warning( "Warning: Multiple info_map_parameters entities in map!\n" );
	}
	else
	{
		g_pMapInfo = this;
	}
}

CMapInfo::~CMapInfo()
{
	if ( g_pMapInfo == this )
	{
		g_pMapInfo = NULL;
	}
}

bool CMapInfo::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "buying" ) )
	{
		m_iBuyingStatus = atoi( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "bombradius" ) )
	{
		m_flBombRadius = (float)atoi( szValue );

		if ( m_flBombRadius > 2048 )
		{
			m_flBombRadius = 2048;
		}

		return true;
	}

	if ( FStrEq( szKeyName, "PainPillDensity" ) )
	{
		m_painPillDensity = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "MolotovDensity" ) )
	{
		m_molotovDensity = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "PipeBombDensity" ) )
	{
		m_pipeBombDensity = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "PistolDensity" ) )
	{
		m_pistolDensity = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "GasCanDensity" ) )
	{
		m_gasCanDensity = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "PropaneTankDensity" ) )
	{
		m_propaneTankDensity = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "OxygenTankDensity" ) )
	{
		m_oxygenTankDensity = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "ItemClusterRange" ) )
	{
		m_itemClusterRange = atof( szValue );
		return true;
	}

	if ( FStrEq( szKeyName, "FinaleItemClusterCount" ) )
	{
		m_finaleItemClusterCount = atoi( szValue );
		return true;
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}

void CMapInfo::Spawn()
{
	SetMoveType( MOVETYPE_NONE );
	SetSolid( SOLID_NONE );
	AddEffects( EF_NODRAW );
}

void CMapInfo::InputFireWinCondition( inputdata_t &inputdata )
{
	CSGameRules()->TerminateRound( 5, inputdata.value.Int() );
}

void CMapInfo::InputUpdateCvars( inputdata_t &inputdata )
{
	DirectorScavengeItemOverride.SetValue( 1 );

	DirectorPainPillDensity.SetValue( m_painPillDensity );
	DirectorMolotovDensity.SetValue( m_molotovDensity );
	DirectorPipeBombDensity.SetValue( m_pipeBombDensity );
	DirectorPistolDensity.SetValue( m_pistolDensity );
	DirectorGasCanDensity.SetValue( m_gasCanDensity );
	DirectorPropaneTankDensity.SetValue( m_propaneTankDensity );
	DirectorOxygenTankDensity.SetValue( m_oxygenTankDensity );
	DirectorItemClusterRange.SetValue( m_itemClusterRange );
	DirectorFinaleItemClusterCount.SetValue( m_finaleItemClusterCount );
}

float CMapInfo::GetGasCanDensity()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_gasCanDensity;
	}

	return DirectorGasCanDensity.GetFloat();
}

int CMapInfo::GetFinaleItemClusterCount()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_finaleItemClusterCount;
	}

	return DirectorFinaleItemClusterCount.GetInt();
}

float CMapInfo::GetItemClusterRange()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_itemClusterRange;
	}

	return DirectorItemClusterRange.GetFloat();
}

float CMapInfo::GetMolotovDensity()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_molotovDensity;
	}

	return DirectorMolotovDensity.GetFloat();
}

float CMapInfo::GetOxygenTankDensity()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_oxygenTankDensity;
	}

	return DirectorOxygenTankDensity.GetFloat();
}

float CMapInfo::GetPainPillDensity()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_painPillDensity;
	}

	return DirectorPainPillDensity.GetFloat();
}

float CMapInfo::GetPipeBombDensity()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_pipeBombDensity;
	}

	return DirectorPipeBombDensity.GetFloat();
}

float CMapInfo::GetPistolDensity()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_pistolDensity;
	}

	return DirectorPistolDensity.GetFloat();
}

float CMapInfo::GetPropaneTankDensity()
{
	if ( DirectorScavengeItemOverride.GetInt() == 0 )
	{
		return m_propaneTankDensity;
	}

	return DirectorPropaneTankDensity.GetFloat();
}