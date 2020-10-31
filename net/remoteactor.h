#pragma once

#include "game/actorped.h"

#pragma pack(1)

class CRemoteActor
{
public:
	CActorPed *m_pActorPed;
	CActorPed *GetActorPed() { return m_pActorPed; };

	CRemoteActor();
	~CRemoteActor();

	bool Spawn(uint16_t actorId, int iSkin, VECTOR vecPos, float fRotation, float fHealth, float bInvulnerable);
	void Remove();
};