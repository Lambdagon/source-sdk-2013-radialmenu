//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_KNIFE_H
#define WEAPON_KNIFE_H
#ifdef _WIN32
#pragma once
#endif


#include "weapon_csbase.h"

class CBaseEntity;

#if defined( CLIENT_DLL )

	#define CKnife C_Knife
	#define CBoomerClaw C_BoomerClaw
	#define CSmokerClaw C_SmokerClaw
	#define CHunterClaw C_HunterClaw
	#define CTankClaw C_TankClaw
	#define CChargerClaw C_ChargerClaw
	#define CJockeyClaw C_JockeyClaw
	#define CSpitterClaw C_SpitterClaw
	#define CGasCan C_GasCan
	#define CColaBottles C_ColaBottles
	#define CPills C_Pills
#endif


// ----------------------------------------------------------------------------- //
// CKnife class definition.
// ----------------------------------------------------------------------------- //

class CKnife : public CWeaponCSBase
{
public:
	DECLARE_CLASS( CKnife, CWeaponCSBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	#ifndef CLIENT_DLL
		DECLARE_DATADESC();
	#endif

	
	CKnife();

	// We say yes to this so the weapon system lets us switch to it.
	virtual bool HasPrimaryAmmo();
	virtual bool CanBeSelected();
	
	virtual void Precache();

	void Spawn();
	void Smack();
	//void Smack( trace_t *pTr, float delay );
	bool SwingOrStab( bool bStab );
	void PrimaryAttack();
	void SecondaryAttack();
	void WeaponAnimation( int iAnimation );

	virtual void ItemPostFrame( void );

// 	virtual float GetSpread() const;

	bool Deploy();
	void Holster( int skiplocal = 0 );
	bool CanDrop();

	void WeaponIdle();

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_KNIFE; }

public:
	
	trace_t m_trHit;
	EHANDLE m_pTraceHitEnt;

	CNetworkVar( float, m_flSmackTime );
	bool	m_bStab;

private:
	CKnife( const CKnife & ) {}
};

class CHunterClaw : public CKnife
{
public:
	DECLARE_CLASS(CHunterClaw, CKnife);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CHunterClaw();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_KNIFE; }

	virtual void PrimaryAttack() OVERRIDE;
	virtual void ItemPostFrame() OVERRIDE;

#ifndef CLIENT_DLL
	bool IsLunging() const { return m_bIsLunging; }
	void SetBotLungeTarget( CBaseEntity *target );
	CBaseEntity *GetBotLungeTarget() const { return m_hBotLungeTarget.Get(); }
#endif
private:
	CHunterClaw(const CHunterClaw&) {}

#ifndef CLIENT_DLL
	EHANDLE m_hBotLungeTarget;
	float m_flBotLungeTargetSetTime;
	float m_flNextLungeAllowedTime;
	float m_flLungeStartTime;
	float m_flPounceStartZ;
	float m_flNextWallKickTime;
	bool m_bIsLunging;
	bool m_bIsPouncing;
	bool m_bDidPounceHit;
	bool m_bPendingLandingDelay;
#endif
};

class CTankClaw : public CKnife
{
public:
	DECLARE_CLASS(CTankClaw, CKnife);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTankClaw();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_KNIFE; }
	virtual void SecondaryAttack() OVERRIDE;
private:
	CTankClaw(const CTankClaw&) {}
};

class CSmokerClaw : public CKnife
{
public:
	DECLARE_CLASS(CSmokerClaw, CKnife);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CSmokerClaw();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_KNIFE; }
private:
	CSmokerClaw(const CSmokerClaw&) {}
};

class CBoomerClaw : public CKnife
{
public:
	DECLARE_CLASS(CBoomerClaw, CKnife);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CBoomerClaw();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_KNIFE; }

	virtual void PrimaryAttack() OVERRIDE;
	virtual void ItemPostFrame() OVERRIDE;

#ifndef CLIENT_DLL
	bool CanStartVomit() const;
	bool IsVomiting() const { return m_flVomitEndTime > 0.0f; }
#endif
private:
	CBoomerClaw(const CBoomerClaw&) {}

#ifndef CLIENT_DLL
	float m_flNextVomitAllowedTime;
	float m_flVomitEndTime;
	float m_flNextVomitBlobTime;
#endif
};

class CChargerClaw : public CKnife
{
public:
	DECLARE_CLASS(CChargerClaw, CKnife);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CChargerClaw();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_KNIFE; }
	virtual void PrimaryAttack() OVERRIDE;
private:
	CChargerClaw(const CChargerClaw&) {}
};

class CJockeyClaw : public CKnife
{
public:
	DECLARE_CLASS(CJockeyClaw, CKnife);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CJockeyClaw();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_KNIFE; }
private:
	CJockeyClaw(const CJockeyClaw&) {}
};

class CSpitterClaw : public CKnife
{
public:
	DECLARE_CLASS(CSpitterClaw, CKnife);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CSpitterClaw();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_KNIFE; }

	virtual void PrimaryAttack() OVERRIDE;

#ifndef CLIENT_DLL
	bool CanStartSpit() const;
#endif
private:
	CSpitterClaw(const CSpitterClaw&) {}

#ifndef CLIENT_DLL
	float m_flNextSpitAllowedTime;
#endif
};

class CGasCan : public CWeaponCSBase
{
public:
	DECLARE_CLASS(CGasCan, CWeaponCSBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CGasCan();
	bool MyTouch(CCSPlayer* pPlayer);
	bool Holster(CBaseCombatWeapon* pSwitchingTo);
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_GASCAN; }

	virtual void PrimaryAttack() OVERRIDE;
	virtual void SecondaryAttack() OVERRIDE;
};

class CColaBottles : public CWeaponCSBase
{
public:
	DECLARE_CLASS(CColaBottles, CWeaponCSBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CColaBottles();
	bool MyTouch(CCSPlayer* pPlayer);
	bool Holster(CBaseCombatWeapon* pSwitchingTo);
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_COLA_BOTTLES; }

	virtual void PrimaryAttack() OVERRIDE;
	virtual void SecondaryAttack() OVERRIDE;
};

class CPills : public CWeaponCSBase
{
public:
	DECLARE_CLASS(CPills, CWeaponCSBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CPills();
	virtual CSWeaponID GetWeaponID(void) const { return WEAPON_PAIN_PILLS; }

	virtual void PrimaryAttack() OVERRIDE;
	virtual void SecondaryAttack() OVERRIDE;
};

#endif // WEAPON_KNIFE_H
