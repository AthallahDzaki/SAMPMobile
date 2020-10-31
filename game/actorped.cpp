#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "util/armhook.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

CActorPed::CActorPed(uint16_t usModel, VECTOR vecPosition, float fRotation, float fHealth, bool bInvulnerable) {
	Destroy();

	if(!pGame->IsModelLoaded(usModel)) {
		pGame->RequestModel(usModel);
		pGame->LoadRequestedModels();
	}

	if(!IsPedModel(usModel)) {
		usModel = 0;
	}

	uint32_t actorGTAId = 0;
	ScriptCommand(&create_actor, 22, usModel, vecPosition.X, vecPosition.Y, vecPosition.Z, &actorGTAId);

	m_dwGTAId = actorGTAId;
	m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
	m_pEntity = (ENTITY_TYPE *)m_pPed;

	ForceTargetRotation(fRotation);
	TeleportTo(vecPosition.X, vecPosition.Y, vecPosition.Z);

	if(fHealth < 1.0f) {
		SetDead();
	} else {
		SetHealth(fHealth);
	}

	ScriptCommand(&lock_actor, m_dwGTAId, 1);
	m_pEntity->nEntityFlags.m_bUsesCollision = 0;

	if(bInvulnerable) {
		ScriptCommand(&set_actor_immunities, m_dwGTAId, 1, 1, 1, 1, 1);
	} else {
		ScriptCommand(&set_actor_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
	}
	return;
}

CActorPed::~CActorPed() {
	Destroy();
	return;
}

void CActorPed::Destroy() {
	if(IsValidGamePed(m_pPed)) {
		// CPed::entity.vtable + 0x4 == CEntity::Remove
		((void (*)(PED_TYPE *))(*(void **)(m_pPed->entity.vtable + 0x4)))(m_pPed);
	}

	m_pPed = nullptr;
	m_pEntity = nullptr;
	m_dwGTAId = 0;
	return;
}

void CActorPed::SetHealth(float fHealth) {
	if(!IsValidGamePed(m_pPed)) {
		return;
	}

	m_pPed->fHealth = fHealth;
	return;
}

void CActorPed::ForceTargetRotation(float fRotation) {
	if(!IsValidGamePed(m_pPed)) {
		return;
	}

	m_pPed->fRotation1 = DegToRad(fRotation);
	m_pPed->fRotation2 = DegToRad(fRotation);

	ScriptCommand(&set_actor_z_angle, m_dwGTAId, fRotation);
	return;
}

void CActorPed::ApplyAnimation(char *szAnimName, char *szAnimFile, float fDelta, int bLoop, int bLockX, int bLockY, int bFreeze, int uiTime) {
	if(!IsValidGamePed(m_pPed)) {
		return;
	}

	if(!PreloadAnimFile(szAnimFile, 1000, 5)) {
		return;
	}

	ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimFile, fDelta, bLoop, bLockX, bLockY, bFreeze, uiTime);
	return;
}

void CActorPed::SetDead() {
	if(!IsValidGamePed(m_pPed)) {
		return;
	}
	
	MATRIX4X4 matEntity;
	GetMatrix(&matEntity);
	TeleportTo(matEntity.pos.X, matEntity.pos.Y, matEntity.pos.Z);
	
	SetHealth(0.0f);
	ScriptCommand(&kill_actor, m_dwGTAId);
}