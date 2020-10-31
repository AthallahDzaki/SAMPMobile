#pragma once

#include "remoteactor.h"

#define MAX_ACTORS 1000
#define INVALID_ACTOR_ID 0xFFFF

#pragma pack(1)
class CActorPool
{
private:
	bool m_bActorSlotState[MAX_ACTORS];
	CRemoteActor *m_pActors[MAX_ACTORS];
	
public:
	PED_TYPE *m_pGTAPed[MAX_ACTORS];

	CActorPool();
	~CActorPool();

	bool Spawn(uint16_t actorId, int iSkin, VECTOR vecPos, float fRotation, float fHealth, float bInvulnerable);
	bool Delete(uint16_t actorId);
	
	bool IsValidActorId(uint16_t actorId) {
		if(actorId >= 0 && actorId < MAX_ACTORS) {
			return true;
		}
		return false;
	};

	bool GetSlotState(uint16_t actorId) {
		if(IsValidActorId(actorId)) {
			return m_bActorSlotState[actorId];
		}
		return false;
	};

	CRemoteActor *GetAt(uint16_t actorId) {
		if(!GetSlotState(actorId)) {
			return nullptr;
		}
		return m_pActors[actorId];
	};
};