#ifndef TERROR_WEAPON_SCRIPTED_SPAWN_H
#define TERROR_WEAPON_SCRIPTED_SPAWN_H
#ifdef _WIN32
#pragma once
#endif

class CBaseEntity;
class CCSPlayer;

bool Terror_IsScriptedWeaponPickupEntity( CBaseEntity *pEntity );
bool Terror_ScriptedWeaponPickupIsUsefulForPlayer( CBaseEntity *pEntity, CCSPlayer *pPlayer );

#endif
