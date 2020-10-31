#include "../main.h"
#include "game/game.h"
#include "netgame.h"

CActorPool::CActorPool() {
	for(uint16_t actorId = 0; actorId < MAX_ACTORS; actorId++) {
		m_bActorSlotState[actorId] = 0;
		m_pActors[actorId] = nullptr;
		m_pGTAPed[actorId] = nullptr;
	}
}

CActorPool::~CActorPool() {
	for(uint16_t actorId = 0; actorId < MAX_ACTORS; actorId++) {
		Delete(actorId);
	}
}

bool CActorPool::Spawn(uint16_t actorId, int iSkin, VECTOR vecPos, float fRotation, float fHealth, float bInvulnerable) {
	if(!IsValidActorId(actorId)) {
		return false;
	}
	
	if(m_pActors[actorId] != NULL) {
		Delete(actorId);
	}

	m_pActors[actorId] = new CRemoteActor();

	if(m_pActors[actorId]) {	
		m_bActorSlotState[actorId] = true;
		return m_pActors[actorId]->Spawn(actorId, iSkin, vecPos, fRotation, fHealth, bInvulnerable);
	}
	
	return false;
}

bool CActorPool::Delete(uint16_t actorId) {
	if(!IsValidActorId(actorId)) {
		return false;
	}
	
	m_bActorSlotState[actorId] = false;
	
	if(m_pActors[actorId]) {
		delete m_pActors[actorId];
		m_pActors[actorId] = nullptr;
	}

	return true;
}