//========================================================================//
// prop_car_alarm from left 4 dead 1
// It is used to trigger events when the car gets touched or damaged.
// this is an attempted recreation of it based on the info given on the
// Valve Developer Wiki https://developer.valvesoftware.com/wiki/Prop_car_alarm
// and the FGD file from Left 4 Dead 1. Tested workng using the no mercy car logic
// It spawns a horde and fires the outputs when triggered - Vvis :3
// 
// TODO: Chirps? 
//========================================================================//
#include "cbase.h"
#include "props.h"
#include "info_director.h"
#include "cs_player.h"

class CPropCarAlarm : public CPhysicsProp
{
public:
	DECLARE_CLASS( CPropCarAlarm, CPhysicsProp );
	DECLARE_DATADESC();

	CPropCarAlarm() : m_bAlarmed(false) {}

    int OnTakeDamage(const CTakeDamageInfo &info) override  
    {  
       int result = BaseClass::OnTakeDamage(info);  
       if (info.GetAttacker() && info.GetAttacker()->IsPlayer())  
       {  
           inputdata_t input;
           input.pActivator = info.GetAttacker();  
           input.pCaller = this;  
           InputTrigger(input);  
       }  
       return result;  
    }  

    void Touch(CBaseEntity *pOther) override  
    {  
       if (pOther && pOther->IsPlayer())  
       {  
           inputdata_t input;
           input.pActivator = pOther;  
           input.pCaller = this;  
           InputTrigger(input);  
       }  
    }

	void InputTrigger( inputdata_t &input );
	void AlarmEnd();

private:
	bool m_bAlarmed;
	COutputEvent m_OnCarAlarmStart;
	COutputEvent m_OnCarAlarmEnd;
	COutputEvent m_OnCarAlarmChirpStart;
	COutputEvent m_OnCarAlarmChirpEnd;
};

LINK_ENTITY_TO_CLASS( prop_car_alarm, CPropCarAlarm );

BEGIN_DATADESC( CPropCarAlarm )
	DEFINE_KEYFIELD( m_bAlarmed, FIELD_BOOLEAN, "m_bAlarmed" ),
	DEFINE_OUTPUT( m_OnCarAlarmStart, "OnCarAlarmStart" ),
	DEFINE_OUTPUT( m_OnCarAlarmEnd, "OnCarAlarmEnd" ),
	DEFINE_OUTPUT( m_OnCarAlarmChirpStart, "OnCarAlarmChirpStart" ),
	DEFINE_OUTPUT( m_OnCarAlarmChirpEnd, "OnCarAlarmChirpEnd" ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Trigger", InputTrigger ),

	DEFINE_THINKFUNC( AlarmEnd ),
END_DATADESC()

void CPropCarAlarm::InputTrigger( inputdata_t &input )
{
	if ( m_bAlarmed )
		return;

	m_bAlarmed = true;
	m_OnCarAlarmStart.FireOutput( this, this );

	if (g_pDirector)
	{
		g_pDirector->InputPanicEvent( input );
	}

	CCSPlayer* pCSPlayer = ToCSPlayer(input.pActivator);

	pCSPlayer->HintMessage("You Have Alerted The Horde!", true);

	ConColorMsg(Color(255,0,0,255),"Car Alarm Triggered!\n");

	SetContextThink( &CPropCarAlarm::AlarmEnd, gpGlobals->curtime + 30.0f, "CarAlarmThink" );
}

void CPropCarAlarm::AlarmEnd()
{
	m_OnCarAlarmEnd.FireOutput( this, this );
}

//------------------------------------------------------------------------

class CPropCarGlass : public CBreakableProp
{
public:
	DECLARE_CLASS( CPropCarGlass, CBreakableProp );
	DECLARE_DATADESC();

	int OnTakeDamage( const CTakeDamageInfo &info ) override
	{
		int result = BaseClass::OnTakeDamage( info );
		ForwardToAlarm();
		return result;
	}

	void VPhysicsCollision( int index, gamevcollisionevent_t *pEvent ) override
	{
		BaseClass::VPhysicsCollision( index, pEvent );
		if ( pEvent->collisionSpeed > 100.0f )
			ForwardToAlarm();
	}

private:
	void ForwardToAlarm()
	{
		CBaseEntity *pParent = GetMoveParent();
		if ( pParent && FClassnameIs( pParent, "prop_car_alarm" ) )
		{
			variant_t emptyVar;
			pParent->AcceptInput("Trigger", this, this, emptyVar, 0.0f);
			ConColorMsg(Color(255,0,0,255),"Car alarm triggered by glass break!\n");
		}
	}
};

LINK_ENTITY_TO_CLASS( prop_car_glass, CPropCarGlass );

BEGIN_DATADESC( CPropCarGlass )
END_DATADESC()
