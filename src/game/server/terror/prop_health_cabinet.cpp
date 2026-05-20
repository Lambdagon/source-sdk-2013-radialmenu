//=============================================================================
// Basic recreation of the Left 4 Dead prop_health_cabinet for FC - Vvis :3
//=============================================================================

#include "cbase.h"
#include "props.h"
#include "eventqueue.h"
#include "cs_player.h"
#include "weapon_knife.h"
#define HEALTH_CABINET_MODEL "models/props_interiors/medicalcabinet02.mdl"

static int ACT_DOOR_OPEN = 1;

class CPropHealthCabinet : public CDynamicProp
{
public:
	DECLARE_CLASS(CPropHealthCabinet, CDynamicProp);
	DECLARE_DATADESC();

	CPropHealthCabinet();

	void Spawn();
	void Precache();

	void	Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	virtual int ObjectCaps(void) {
		return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE;
	}

	bool	m_bIsOpen;
};

LINK_ENTITY_TO_CLASS(prop_health_cabinet, CPropHealthCabinet);

BEGIN_DATADESC(CPropHealthCabinet)
	DEFINE_FIELD(m_bIsOpen, FIELD_BOOLEAN),
END_DATADESC()


CPropHealthCabinet::CPropHealthCabinet()
{
	m_bIsOpen = false;
}
void CPropHealthCabinet::Precache()
{
	PrecacheModel(HEALTH_CABINET_MODEL);
	BaseClass::Precache();
}

void CPropHealthCabinet::Spawn()
{
	Precache();
	SetModel(HEALTH_CABINET_MODEL);
	BaseClass::Spawn();
	SetSolid(SOLID_BBOX);

	// The code below is super fucking evil and i wanna die - Vvis :3 
	CBaseEntity* pEntity = CreateEntityByName("weapon_first_aid_kit_spawn");
	CBaseEntity* pEntity1 = CreateEntityByName("weapon_first_aid_kit_spawn");
	CBaseEntity* pEntity2 = CreateEntityByName("weapon_first_aid_kit_spawn");
	CBaseEntity* pEntity3 = CreateEntityByName("weapon_first_aid_kit_spawn");

	if (pEntity)
	{
		DispatchSpawn(pEntity);

		int nAttachment = LookupAttachment("item1");

		if (nAttachment > 0)
		{
			pEntity->SetParent(this);
			pEntity->SetParentAttachment("SetParentAttachment", "item1",true);
			pEntity->SetLocalOrigin(vec3_origin);
			pEntity->SetLocalAngles(vec3_angle);
		}
	}

	if (pEntity1)
	{
		DispatchSpawn(pEntity1);

		int nAttachment = LookupAttachment("item2");

		if (nAttachment > 0)
		{
			pEntity1->SetParent(this);
			pEntity1->SetParentAttachment("SetParentAttachment", "item2", true);
			pEntity1->SetLocalOrigin(vec3_origin);
			pEntity1->SetLocalAngles(vec3_angle);
		}
	}

	if (pEntity2)
	{
		DispatchSpawn(pEntity2);

		int nAttachment = LookupAttachment("item3");

		if (nAttachment > 0)
		{
			pEntity2->SetParent(this);
			pEntity2->SetParentAttachment("SetParentAttachment", "item3", true);
			pEntity2->SetLocalOrigin(vec3_origin);
			pEntity2->SetLocalAngles(vec3_angle);
		}
	}
	if (pEntity3)
	{
		DispatchSpawn(pEntity3);

		int nAttachment = LookupAttachment("item4");

		if (nAttachment > 0)
		{
			pEntity3->SetParent(this);
			pEntity3->SetParentAttachment("SetParentAttachment", "item4", true);
			pEntity3->SetLocalOrigin(vec3_origin);
			pEntity3->SetLocalAngles(vec3_angle);
		}
	}
}

void CPropHealthCabinet::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	CCSPlayer* pCSPlayer = ToCSPlayer(pActivator);

	if (!pCSPlayer)
		return;

	if (m_bIsOpen)
		return;

	if(!pCSPlayer->IsAlive())
		return;
	
	if (!m_bIsOpen)
	{
		CDynamicProp* pDynamicProp = dynamic_cast<CDynamicProp*>(this);
		if (pDynamicProp)
			pDynamicProp->PropSetSequence(1);
		m_bIsOpen = true;
	}

	ConColorMsg(Color(255, 0, 255, 255), "Player %s used health cabinet\n", pCSPlayer->GetPlayerName());

	BaseClass::Use(pActivator, pCaller, useType, value);
}

