//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side receive stub for func_elevator.
//
//=============================================================================//

#include "cbase.h"
#include "c_baseentity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_FuncElevator : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_FuncElevator, C_BaseEntity );
	DECLARE_CLIENTCLASS();

	C_FuncElevator();

private:
	float m_acceleration;
	float m_currentSpeed;
	float m_movementStartTime;
	float m_movementStartSpeed;
	float m_movementStartZ;
	float m_destinationFloorPosition;
	float m_maxSpeed;
	bool m_isMoving;
};

IMPLEMENT_CLIENTCLASS_DT( C_FuncElevator, DT_FuncElevator, CFuncElevator )
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
	RecvPropFloat( RECVINFO( m_acceleration ) ),
	RecvPropFloat( RECVINFO( m_currentSpeed ) ),
	RecvPropFloat( RECVINFO( m_movementStartTime ) ),
	RecvPropFloat( RECVINFO( m_movementStartSpeed ) ),
	RecvPropFloat( RECVINFO( m_movementStartZ ) ),
	RecvPropFloat( RECVINFO( m_destinationFloorPosition ) ),
	RecvPropFloat( RECVINFO( m_maxSpeed ) ),
	RecvPropBool( RECVINFO( m_isMoving ) ),
END_RECV_TABLE()

C_FuncElevator::C_FuncElevator()
{
	m_acceleration = 0.0f;
	m_currentSpeed = 0.0f;
	m_movementStartTime = 0.0f;
	m_movementStartSpeed = 0.0f;
	m_movementStartZ = 0.0f;
	m_destinationFloorPosition = 0.0f;
	m_maxSpeed = 0.0f;
	m_isMoving = false;
}
