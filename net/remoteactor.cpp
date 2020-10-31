#include "../main.h"
#include "game/game.h"
#include "netgame.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

CRemoteActor::CRemoteActor() {
	m_pActorPed = nullptr;
}

CRemoteActor::~CRemoteActor() {
	Remove();
}

void CRemoteActor::Remove() {
	if(m_pActorPed) {
		delete m_pActorPed;
		m_pActorPed = nullptr;
	}
}

bool CRemoteActor::Spawn(uint16_t actorId, int iSkin, VECTOR vecPos, float fRotation, float fHealth, float bInvulnerable) {
	Remove();
	
	CActorPool *pActorPool = pNetGame->GetActorPool();
	if(pActorPool) {
		m_pActorPed = pGame->NewActor(iSkin, vecPos, fRotation, fHealth, bInvulnerable);
		if(m_pActorPed) {
			pActorPool->m_pGTAPed[actorId] = m_pActorPed->m_pPed;
		}
		return true;
	}
	return false;
}