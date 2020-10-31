#include "main.h"
#include "game.h"
#include "net/netgame.h"
#include "chatwindow.h"
#include <cmath>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

CObject::CObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance)
{
	uint32_t dwRetID 	= 0;
	m_pEntity 			= 0;
	m_dwGTAId 			= 0;

	ScriptCommand(&create_object, iModel, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&put_object_at, dwRetID, fPosX, fPosY, fPosZ);

	m_vecRot = vecRot;
	m_vecTargetRotTar = vecRot;

	m_pEntity = GamePool_Object_GetAt(dwRetID);
	m_dwGTAId = dwRetID;
	m_byteMoving = 0;
	m_fMoveSpeed = 0.0;

	m_bIsPlayerSurfing = false;

	InstantRotate(vecRot.X, vecRot.Y, vecRot.Z);
}

CObject::~CObject()
{
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if(m_pEntity)
	{
		ScriptCommand(&destroy_object, m_dwGTAId);
	}
}

void CObject::UpdateRwMatrixAndFrame()
{
	if(m_pEntity && m_pEntity->vtable != g_libGTASA+0x5C7358)
	{
		if(m_pEntity->pdwRenderWare)
		{
			if(m_pEntity->mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)(m_pEntity->pdwRenderWare + 4) + 0x10;
				((void (*) (MATRIX4X4*, uintptr_t))(g_libGTASA+0x3E862C+1))(m_pEntity->mat, pRwMatrix);
				((void (*) (ENTITY_TYPE*))(g_libGTASA+0x39194C+1))(m_pEntity);
			}
		}
	}
}

void CObject::Process(float fElapsedTime)
{
	if(!m_bAttached && m_iAttachVehicle && m_iAttachVehicle != 65535)
	{
		CVehiclePool *pool = pNetGame->GetVehiclePool(); 
		CVehicle *vehicleObj = pool->GetAt(m_iAttachVehicle);
		if(vehicleObj)
		{
			m_bAttached = true;
			ScriptCommand(&attach_object_to_car, 
			m_dwGTAId,
			vehicleObj->m_dwGTAId,
			m_vecAttachOffset.X, m_vecAttachOffset.Y, m_vecAttachOffset.Z, //offsets
			m_vecAttachRot.X, m_vecAttachRot.Y, m_vecAttachRot.Z); //rotations
		}
	}
	if (m_byteMoving & 1)
	{
		MATRIX4X4 matPos;
		GetMatrix(&matPos);


		VECTOR matRot = m_vecRot;

		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matPos);
		float remainingRot = RotaionRemaining(m_vecTargetRotTar);
		/*TeleportTo(m_matTarget.pos.X, m_matTarget.pos.Y, m_matTarget.pos.Z);
		InstantRotate(m_vecTargetRot.X, m_vecTargetRot.Y, m_vecTargetRot.Z);*/

		if (distance >= remaining)
		{
			m_byteMoving &= ~1; // Stop it moving
			// Force the final location so we don't overshoot slightly

			m_vecTargetRotTar = m_vecTargetRot;

			if(m_vecTargetRot.X < -1000.0)
			{
				m_vecRot.X = m_vecTargetRot.X;
				m_vecTargetRotTar.X = m_vecTargetRot.X;
			}
			if(m_vecTargetRot.Y < -1000.0)
			{
				m_vecRot.X = m_vecTargetRot.X;
				m_vecTargetRotTar.Y = m_vecTargetRot.Y;
			}
			if(m_vecTargetRot.Z < -1000.0)
			{
				m_vecRot.X = m_vecTargetRot.X;
				m_vecTargetRotTar.Z = m_vecTargetRot.Z;
			}
			TeleportTo(m_matTarget.pos.X, m_matTarget.pos.Y, m_matTarget.pos.Z);
			InstantRotate(m_vecRot.X, m_vecRot.Y, m_vecRot.Z);
		}
		else
		{
			// Else interpolate the new position between the current and final positions
			remaining /= distance; // Calculate ratio
			remainingRot /= distance;

			matPos.pos.X += (m_matTarget.pos.X - matPos.pos.X) / remaining;
			matPos.pos.Y += (m_matTarget.pos.Y - matPos.pos.Y) / remaining;
			matPos.pos.Z += (m_matTarget.pos.Z - matPos.pos.Z) / remaining;

			if(m_vecTargetRot.X < -1000.0)
				m_vecRot.X += (m_vecTargetRot.X - m_vecTargetRotTar.X) / remainingRot*10.0;
			
			if(m_vecTargetRot.Y < -1000.0)
				m_vecRot.Y += (m_vecTargetRot.Y - m_vecTargetRotTar.Y) / remainingRot*10.0;

			if(m_vecTargetRot.Z < -1000.0)
				m_vecRot.Z += (m_vecTargetRot.Z - m_vecTargetRotTar.Z) / remainingRot*10.0;

			/*Log("move object > {%f} (%f %f %f) %f ; %f ; %f / %f %f %f // %f %f %f",
			remainingRot,
			m_vecRot.X, m_vecRot.Y, m_vecRot.Z,
			matPos.pos.X, matPos.pos.Y, matPos.pos.Z,
			matRot.X, matRot.Y, matRot.Z,
			m_vecTargetRot.X, m_vecTargetRot.Y, m_vecTargetRot.Z);*/
			InstantRotate(m_vecRot.X, m_vecRot.Y, m_vecRot.Z);
			TeleportTo(matPos.pos.X, matPos.pos.Y, matPos.pos.Z);
		}
	}
}

float CObject::DistanceRemaining(MATRIX4X4 *matPos)
{

	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.X - m_matTarget.pos.X) * (matPos->pos.X - m_matTarget.pos.X);
	fSY = (matPos->pos.Y - m_matTarget.pos.Y) * (matPos->pos.Y - m_matTarget.pos.Y);
	fSZ = (matPos->pos.Z - m_matTarget.pos.Z) * (matPos->pos.Z - m_matTarget.pos.Z);
	return (float)sqrt(fSX + fSY + fSZ);
}

float CObject::RotaionRemaining(VECTOR matPos)
{
	float fSX,fSY,fSZ;
	fSX = (matPos.X - m_vecTargetRot.X) * (matPos.X - m_vecTargetRot.X);
	fSY = (matPos.Y - m_vecTargetRot.Y) * (matPos.Y - m_vecTargetRot.Y);
	fSZ = (matPos.Z - m_vecTargetRot.Z) * (matPos.Z - m_vecTargetRot.Z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObject::SetPos(float x, float y, float z)
{
	ScriptCommand(&put_object_at, m_dwGTAId, x, y, z);
}

void CObject::MoveTo(float X, float Y, float Z, float speed, float rX, float rY, float rZ)
{
	m_matTarget.pos.X = X;
	m_matTarget.pos.Y = Y;
	m_matTarget.pos.Z = Z;

	m_vecTargetRot.X = rX;
	m_vecTargetRot.Y = rY;
	m_vecTargetRot.Z = rZ;
	
	m_fMoveSpeed = speed;
	m_byteMoving |= 1;
}

void CObject::InstantRotate(float x, float y, float z)
{
	ScriptCommand(&set_object_rotation, m_dwGTAId, x, y, z);
}

void CObject::AttachProccess(uint16_t iAttachedVehicle, VECTOR AttachOffset, VECTOR AttachRot)
{
	m_iAttachVehicle = iAttachedVehicle;
	m_bAttached = false;
	m_vecAttachOffset = AttachOffset;
	m_vecAttachRot = AttachRot;
}