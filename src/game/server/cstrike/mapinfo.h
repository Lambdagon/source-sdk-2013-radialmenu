//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef MAPINFO_H
#define MAPINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "baseentity.h"

class CMapInfo : public CPointEntity
{
public:

	DECLARE_CLASS( CMapInfo, CPointEntity );
	DECLARE_DATADESC();

	CMapInfo();
	virtual ~CMapInfo();

	virtual bool KeyValue( const char *szKeyName, const char *szValue );
	virtual void Spawn();

	void InputFireWinCondition( inputdata_t &inputdata );
	void InputUpdateCvars( inputdata_t &inputdata );

	float GetGasCanDensity();
	int GetFinaleItemClusterCount();
	float GetItemClusterRange();
	float GetMolotovDensity();
	float GetOxygenTankDensity();
	float GetPainPillDensity();
	float GetPipeBombDensity();
	float GetPistolDensity();
	float GetPropaneTankDensity();

public:

	int m_iBuyingStatus;
	float m_flBombRadius;

	float m_gasCanDensity;
	float m_molotovDensity;
	float m_oxygenTankDensity;
	float m_painPillDensity;
	float m_pipeBombDensity;
	float m_pistolDensity;
	float m_propaneTankDensity;
	float m_itemClusterRange;

	int m_finaleItemClusterCount;
};

extern CMapInfo *g_pMapInfo;

#endif // MAPINFO_H