#include "main.h"
#include "game.h"
#include "net/netgame.h"
#include "util/armhook.h"
#include "../chatwindow.h"
#include "actionstuff.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow* pChatWindow;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

uintptr_t dwCurPlayerActor = 0;
uintptr_t dwCurWeaponProcessingPlayer = 0;
uint8_t byteCurPlayer = 0;
uint8_t byteCurDriver = 0;
uint8_t byteInternalPlayer = 0;
uint16_t wSavedCameraMode2 = 0;
uint8_t byteSavedCameraMode;

uint16_t (*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
uint16_t CPad__GetPedWalkLeftRight_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if((dwResult == 0xFF80 || dwResult == 0x80) &&  
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x40;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyLR = CPad__GetPedWalkLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetPedWalkUpDown)(uintptr_t thiz);
uint16_t CPad__GetPedWalkUpDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if((dwResult == 0xFF80 || dwResult == 0x80) &&  
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK])
		{
			dwResult = 0x40;
		}
		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyUD = CPad__GetPedWalkUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint32_t (*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_hook(uintptr_t thiz, uint32_t unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

uint32_t (*CPad__JumpJustDown)(uintptr_t thiz);
uint32_t CPad__JumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreJump &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			!RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetJump)(uintptr_t thiz);
uint32_t CPad__GetJump_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t (*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_hook()
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t (*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_hook(uintptr_t thiz, int unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(IsRemotePlayerDucking(byteCurPlayer) && IsRemotePlayerDuckingReset(byteCurPlayer))
		{
			SetRemotePlayerDucking(byteCurPlayer, 0);
			SetRemotePlayerDuckingReset(byteCurPlayer, 0);
			return 1;
		}
		return 0;
	}
	else
	{
		return CPad__DuckJustDown(thiz, unk);
	}
}

uint32_t (*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_hook(uintptr_t thiz)
{
	/*
		0 - �� ����
		1 - ������� ���� (���)
		2 - ������� ���� (��� + F)
	*/

	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK])
			return 2;

		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = true;

		//if(dwResult == 2) 
		//{
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		//}
		//else if(dwResult == 1)
		//{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = false;
		//}

		return dwResult;
	}
}

uint32_t (*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

int16_t (*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyLR;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyLR = CPad__GetSteeringLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetSteeringUpDown)(uintptr_t thiz);
uint16_t CPad__GetSteeringUpDown_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].wKeyUD;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyUD = CPad__GetSteeringUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint16_t (*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wAccelerate = CPad__GetAccelerate(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
		return wAccelerate;
	}
}

uint16_t (*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
		return wBrake;
	}
}

uint32_t (*CPad__GetHandBrake)(uintptr_t thiz);
uint32_t CPad__GetHandBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint32_t handBrake = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = handBrake;
		return handBrake;
	}
}

uint32_t (*CPad__GetHorn)(uintptr_t thiz);
uint32_t CPad__GetHorn_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		// local player
		uint32_t horn = CPad__GetHorn(thiz);
		//Log("horn: %d", horn);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__GetHorn(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

uint32_t (*CPad__ExitVehicleJustDown)(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec);
uint32_t CPad__ExitVehicleJustDown_hook(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec)
{
	CPlayerPool *pPlayerPool;
	CLocalPlayer *pLocalPlayer;

	if(pNetGame)
	{
		pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
		{
			pLocalPlayer = pPlayerPool->GetLocalPlayer();
			if(pLocalPlayer)
			{
				if( pLocalPlayer->HandlePassengerEntry() )
					return 0;
			}
		}
	}

	return CPad__ExitVehicleJustDown(thiz, a2, vehicle, a4, vec);
}

int (*CPlayerPed__UpdateCameraWeaponModes)(uintptr_t thiz, uintptr_t pad, int a3);
int CPlayerPed__UpdateCameraWeaponModes_hook(uintptr_t thiz, uintptr_t pad, int a3)
{
	int result;
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		*(uint8_t*)(g_libGTASA + 0x8E864C) = byteCurPlayer;
		result = CPlayerPed__UpdateCameraWeaponModes(dwCurPlayerActor, pad, a3);
		*(uint8_t*)(g_libGTASA + 0x8E864C) = 0;
	}
	else
	{
		result = CPlayerPed__UpdateCameraWeaponModes(thiz, pad, a3);
	}
	return result;
}

void (*CPed__ProcessControl)(uintptr_t thiz);
void CPed__ProcessControl_hook(uintptr_t thiz)
{
	dwCurPlayerActor = thiz;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// REMOTE PLAYER
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);
		
		/*Log("[2:Before PC]");
		UpdateCameraWeaponModes(thiz);
		Log("[2:After PC]");*/

		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);
		
		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);
		
		*(uint8_t*)(g_libGTASA + 0x8E864C) = byteCurPlayer;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		NOP(g_libGTASA+0x439B7A, 2);
		// call original
		CPed__ProcessControl(thiz);
		// restore
		WriteMemory(g_libGTASA+0x439B7A, (uintptr_t)"\xFA\xF7\x1D\xF8", 4);
		
		*(uint8_t*)(g_libGTASA + 0x008E864C) = 0;
		*pbyteCameraMode = byteSavedCameraMode;
		//*((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4) = wSavedCameraMode2;
		GameSetLocalPlayerCameraExtZoom();
		GameSetLocalPlayerAim();
	}
	else
	{
		// LOCAL PLAYER
		WriteMemory(g_libGTASA+0x4BED92, (uintptr_t)"\x10\x60", 2);

		(*CPed__ProcessControl)(thiz);

		// Reapply the no ped rots from Cam patch
		WriteMemory(g_libGTASA+0x4BED92, (uintptr_t)"\x00\x46", 2);
	}

	return;
}

void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)thiz;
	uintptr_t this_vtable = pVehicle->entity.vtable;
	this_vtable -= g_libGTASA;

	uintptr_t call_addr = 0;

	switch(this_vtable)
	{
		// CAutomobile
		case 0x5CC9F0:
		call_addr = 0x4E314C;
		break;

		// CBoat
		case 0x5CCD48:
		call_addr = 0x4F7408;
		break;

		// CBike
		case 0x5CCB18:
		call_addr = 0x4EE790;
		break;

		// CPlane
		case 0x5CD0B0:
		call_addr = 0x5031E8;
		break;

		// CHeli
		case 0x5CCE60:
		call_addr = 0x4FE62C;
		break;

		// CBmx
		case 0x5CCC30:
		call_addr = 0x4F3CE8;
		break;

		// CMonsterTruck
		case 0x5CCF88:
		call_addr = 0x500A34;
		break;

		// CQuadBike
		case 0x5CD1D8:
		call_addr = 0x505840;
		break;

		// CTrain
		case 0x5CD428:
		call_addr = 0x50AB24;
		break;
	}

	if(pVehicle && pVehicle->pDriver)
	{
		byteCurDriver = FindPlayerNumFromPedPtr((uintptr_t)pVehicle->pDriver);
	}

	if(pVehicle->pDriver && pVehicle->pDriver->dwPedType == 0 &&
		pVehicle->pDriver != GamePool_FindPlayerPed() && 
		*(uint8_t*)(g_libGTASA+0x8E864C) == 0) // CWorld::PlayerInFocus
	{
		*(uint8_t*)(g_libGTASA+0x8E864C) = 0;

		pVehicle->pDriver->dwPedType = 4;
		//CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(g_libGTASA+0x364B64+1))(thiz+0x138);
		pVehicle->pDriver->dwPedType = 0;
	}
	else
	{
		(( void (*)(uintptr_t))(g_libGTASA+0x364B64+1))(thiz+0x138);
	}

	// VEHTYPE::ProcessControl()
    (( void (*)(VEHICLE_TYPE*))(g_libGTASA+call_addr+1))(pVehicle);
}


uint32_t(*CPad__GetWeapon)(uintptr_t thiz, uintptr_t ped, bool unk);
uint32_t CPad__GetWeapon_hook(uintptr_t thiz, uintptr_t ped, bool unk)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint8_t old = *(uint8_t*)(g_libGTASA + 0x008E864C);
		*(uint8_t*)(g_libGTASA + 0x008E864C) = byteCurPlayer;
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetWeapon(thiz, ped, unk);
		*(uint8_t*)(g_libGTASA + 0x008E864C) = old;
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t(*CPad__GetEnterTargeting)(uintptr_t thiz);
uint32_t CPad__GetEnterTargeting_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE];
	}
	else
	{
		uint8_t old = *(uint8_t*)(g_libGTASA + 0x008E864C);
		*(uint8_t*)(g_libGTASA + 0x008E864C) = byteCurPlayer;

		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = CPad__GetEnterTargeting(thiz);

		*(uint8_t*)(g_libGTASA + 0x008E864C) = old;
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE];
	}
}

uintptr_t(*CPad__ProcessPlayerWeapon)(uintptr_t thiz, uintptr_t ped);
uintptr_t CPad__ProcessPlayerWeapon_hook(uintptr_t thiz, uintptr_t ped)
{
	dwCurWeaponProcessingPlayer = ped;
	return CPad__ProcessPlayerWeapon(thiz, ped);
}

uint32_t(*CPad__GetTarget)(uintptr_t thiz, int a2, int a3, int a4);
uint32_t CPad__GetTarget_hook(uintptr_t thiz, int a2, int a3, int a4)
{
	if(!dwCurWeaponProcessingPlayer) return 0;
	return *(uint8_t*)(*(uint32_t*)(dwCurWeaponProcessingPlayer + 1088) + 52) & 0b00001000;
}

uintptr_t(*CTaskSimplePlayerOnFoot__ProcessPlayerWeapon)(uintptr_t thiz, uintptr_t player_ped);
uintptr_t CTaskSimplePlayerOnFoot__ProcessPlayerWeapon_hook(uintptr_t thiz, uintptr_t player_ped)
{
	dwCurWeaponProcessingPlayer = player_ped;
	uintptr_t result;
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		*(uint8_t*)(g_libGTASA + 0x8E864C) = byteCurPlayer;
		result = CTaskSimplePlayerOnFoot__ProcessPlayerWeapon(thiz, dwCurPlayerActor);
		*(uint8_t*)(g_libGTASA + 0x8E864C) = 0;
	}
	else
	{
		result = CTaskSimplePlayerOnFoot__ProcessPlayerWeapon(thiz, dwCurPlayerActor);
	}
	return result;
}

uint32_t ExecuteOriginalTaskUseGun(uintptr_t thiz, uintptr_t a2)
{
	uintptr_t v2;
	uintptr_t v3;
	int v4;
	int v5;
	int v6;
	int result;
	char v8;
	
	v2 = thiz;
	v3 = a2;
	v4 = *((uintptr_t*)thiz + 12);
	v5 = *((uintptr_t*)a2 + 7 * *((unsigned char*)a2 + 1816) + 360);
	v6 = reinterpret_cast<int(*)(uintptr_t)>(g_libGTASA + 0x434F24 + 1)(a2);
	int weapon_info = reinterpret_cast<int(*)(int, int)>(g_libGTASA + 0x56BD60 + 1)(v5, v6);
	if(v4 == weapon_info)
	{
		v8 = *((unsigned char*)v2 + 13);
		if(*((unsigned char*)v2 + 13) & 1)
		{
			reinterpret_cast<int(*)(uintptr_t, uintptr_t, int)>(g_libGTASA + 0x46D4E4 + 1)(v2,v3,0);
			v8 = *((unsigned char*)v2 + 13);
		}
		if(v8 & 2)
		{
			reinterpret_cast<int(*)(uintptr_t, uintptr_t, int)>(g_libGTASA + 0x46D4E4 + 1)(v2,v3,1);
		}
		
		result = 0;
		*((unsigned char*)v2 + 13) = 0;
	}
	else
	{
		result = 0;
	}
	return result;
}

uint32_t CPad__TaskUseGun(uintptr_t thiz, uintptr_t ped)
{	
	dwCurPlayerActor = ped;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	
	uint32_t result = 0;
	
	if (dwCurPlayerActor &&
		(byteCurPlayer != 0)) // not local player and local player's keys set.
	{	
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);
		
		/*Log("[2:Before] %d", *((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4));
		
		// save the second internal cammode, apply the context
		wSavedCameraMode2 = *((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4);
		*((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4) = GameGetPlayerCameraMode(byteCurPlayer);
		if (*((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4) == 4) *((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4) = 0;
		
		Log("[2:After] %d | %d", *((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4), wSavedCameraMode2);*/
		
		// save the camera zoom factor, apply the context
		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);
		
		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);
		*(uint8_t*)(g_libGTASA + 0x008E864C) = byteCurPlayer;
		
		result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x0046D6AC + 1))(thiz, ped);
		
		// restore the camera modes, internal id and local player's aim
		*pbyteCameraMode = byteSavedCameraMode;
		//*((uint16_t*)g_libGTASA + 0x8B0808 + 0x7B4) = wSavedCameraMode2;

		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoom();

		*(uint8_t*)(g_libGTASA + 0x008E864C) = 0;
		GameSetLocalPlayerAim();
	}
	else
	{
		result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x0046D6AC + 1))(thiz, ped);
	}
	
	return result;
}

uint32_t CPad__TaskProcess(uintptr_t thiz, uintptr_t ped, int unk, int unk1)
{
	dwCurPlayerActor = ped;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	uint8_t old = *(uint8_t*)(g_libGTASA + 0x008E864C);
	*(uint8_t*)(g_libGTASA + 0x008E864C) = byteCurPlayer;

	uint32_t result =  ((uint32_t(*)(uintptr_t, uintptr_t, int, int))(g_libGTASA + 0x004C2F7C + 1))(thiz, ped, unk, unk1);
	*(uint8_t*)(g_libGTASA + 0x008E864C) = old;
	return result;
}

// useless shit
/*uint32_t (*CPedIntelligence__GetTaskDuck)(uintptr_t thiz, int a2);
uint32_t CPedIntelligence__GetTaskDuck_hook(uintptr_t thiz, int a2)
{
	uint32_t result;
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		result = CPedIntelligence__GetTaskDuck(thiz, a2);
		if(!result && IsRemotePlayerDucking(byteCurPlayer) &&
		IsRemotePlayerDuckingReset(byteCurPlayer) == 0)
		{
			SetRemotePlayerDuckingReset(byteCurPlayer, 1);
		}
	}
	else
	{
		uint32_t result = CPedIntelligence__GetTaskDuck(thiz, a2);
		if(result) SetLocalPlayerDucking(1);
	}
	return result;
}*/

int (*CPad__GetNitroFired)(uintptr_t thiz);
int CPad__GetNitroFired_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetNitroFired(thiz);
				}
			}
		}
	}
	return 0;
}

int (*CPad__GetHydraulicJump)(uintptr_t thiz);
int CPad__GetHydraulicJump_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetHydraulicJump(thiz);
				}
			}
		}
	}
	return 0;
}

int (*CPad__GetTurretLeft)(uintptr_t thiz);
int CPad__GetTurretLeft_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetTurretLeft(thiz);
				}
			}
		}
	}
	return 0;
}

int (*CPad__GetTurretRight)(uintptr_t thiz);
int CPad__GetTurretRight_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor)
	{
		PED_TYPE* pPedPlayer = (PED_TYPE*)dwCurPlayerActor;
		// local ped
		if (pPedPlayer == GamePool_FindPlayerPed())
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->IsInVehicle())
				{
					return CPad__GetTurretRight(thiz);
				}
			}
		}
	}
	return 0;
}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	// Widget buttons
	#ifdef BUILD_BETA
	SetUpHook(g_libGTASA+0x39D54C, (uintptr_t)CPad__GetNitroFired_hook, (uintptr_t*)&CPad__GetNitroFired);
	SetUpHook(g_libGTASA+0x39D528, (uintptr_t)CPad__GetHydraulicJump_hook, (uintptr_t*)&CPad__GetHydraulicJump);
	SetUpHook(g_libGTASA+0x39D344, (uintptr_t)CPad__GetTurretLeft_hook, (uintptr_t*)&CPad__GetTurretLeft);
	SetUpHook(g_libGTASA+0x39D368, (uintptr_t)CPad__GetTurretRight_hook, (uintptr_t*)&CPad__GetTurretRight);
	#endif

	//SetUpHook(g_libGTASA+0x44DFB8, (uintptr_t)CPedIntelligence__GetTaskDuck_hook, (uintptr_t*)&CPedIntelligence__GetTaskDuck);
	//SetUpHook(g_libGTASA+0x39E74C, (uintptr_t)CPad__GetDuck_hook, (uintptr_t*)&CPad__GetDuck);
	// CPed::ProcessControl
	SetUpHook(g_libGTASA+0x45A280, (uintptr_t)CPed__ProcessControl_hook, (uintptr_t*)&CPed__ProcessControl);
	// CPlayerPed::UpdateCameraWeaponModes
	//SetUpHook(g_libGTASA+0x45464C, (uintptr_t)CPlayerPed__UpdateCameraWeaponModes_hook, (uintptr_t*)&CPlayerPed__UpdateCameraWeaponModes);
	// CTaskSimplePlayerOnFoot::ProcessPlayerWeapon
	SetUpHook(g_libGTASA+0x4C1748, (uintptr_t)CTaskSimplePlayerOnFoot__ProcessPlayerWeapon_hook, (uintptr_t*)&CTaskSimplePlayerOnFoot__ProcessPlayerWeapon);
	// all vehicles ProcessControl
	InstallMethodHook(g_libGTASA+0x5CCA1C, (uintptr_t)AllVehicles__ProcessControl_hook); // CAutomobile::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CCD74, (uintptr_t)AllVehicles__ProcessControl_hook); // CBoat::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CCB44, (uintptr_t)AllVehicles__ProcessControl_hook); // CBike::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CD0DC, (uintptr_t)AllVehicles__ProcessControl_hook); // CPlane::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CCE8C, (uintptr_t)AllVehicles__ProcessControl_hook); // CHeli::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CCC5C, (uintptr_t)AllVehicles__ProcessControl_hook); // CBmx::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CCFB4, (uintptr_t)AllVehicles__ProcessControl_hook); // CMonsterTruck::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CD204, (uintptr_t)AllVehicles__ProcessControl_hook); // CQuadBike::ProcessControl
	InstallMethodHook(g_libGTASA+0x5CD454, (uintptr_t)AllVehicles__ProcessControl_hook); // CTrain::ProcessControl
	InstallMethodHook(g_libGTASA+0x5C8610, (uintptr_t)CPad__TaskUseGun);				 // CPad::TaskUseGun
	//InstallMethodHook(g_libGTASA+0x5CC1D4, (uintptr_t)CPad__TaskProcess);  			     // CPad::TaskProcess

	// lr/ud (onfoot)
	SetUpHook(g_libGTASA+0x39D08C, (uintptr_t)CPad__GetPedWalkLeftRight_hook, (uintptr_t*)&CPad__GetPedWalkLeftRight);
	SetUpHook(g_libGTASA+0x39D110, (uintptr_t)CPad__GetPedWalkUpDown_hook, (uintptr_t*)&CPad__GetPedWalkUpDown);

	//SetUpHook(g_libGTASA + 0x)

	// sprint/jump stuff
	SetUpHook(g_libGTASA+0x39EAA4, (uintptr_t)CPad__GetSprint_hook, (uintptr_t*)&CPad__GetSprint);
	SetUpHook(g_libGTASA+0x39E9B8, (uintptr_t)CPad__JumpJustDown_hook, (uintptr_t*)&CPad__JumpJustDown);
	SetUpHook(g_libGTASA+0x39E96C, (uintptr_t)CPad__GetJump_hook, (uintptr_t*)&CPad__GetJump);
	SetUpHook(g_libGTASA+0x39E824, (uintptr_t)CPad__GetAutoClimb_hook, (uintptr_t*)&CPad__GetAutoClimb);
	SetUpHook(g_libGTASA+0x39E8C0, (uintptr_t)CPad__GetAbortClimb_hook, (uintptr_t*)&CPad__GetAbortClimb);

	// swimm
	SetUpHook(g_libGTASA+0x39EA0C, (uintptr_t)CPad__DiveJustDown_hook, (uintptr_t*)&CPad__DiveJustDown);
	SetUpHook(g_libGTASA+0x39EA4C, (uintptr_t)CPad__SwimJumpJustDown_hook, (uintptr_t*)&CPad__SwimJumpJustDown);

	SetUpHook(g_libGTASA+0x39DD9C, (uintptr_t)CPad__MeleeAttackJustDown_hook, (uintptr_t*)&CPad__MeleeAttackJustDown);

	SetUpHook(g_libGTASA+0x39E038, (uintptr_t)CPad__GetWeapon_hook, (uintptr_t*)&CPad__GetWeapon);
	SetUpHook(g_libGTASA+0x39E498, (uintptr_t)CPad__GetEnterTargeting_hook, (uintptr_t*)&CPad__GetEnterTargeting);
	SetUpHook(g_libGTASA+0x39E418, (uintptr_t)CPad__GetTarget_hook, (uintptr_t*)&CPad__GetTarget);

	SetUpHook(g_libGTASA+0x39E7B0, (uintptr_t)CPad__DuckJustDown_hook, (uintptr_t*)&CPad__DuckJustDown);
	SetUpHook(g_libGTASA+0x39DB50, (uintptr_t)CPad__GetBlock_hook, (uintptr_t*)&CPad__GetBlock);

	// steering lr/ud (incar)
	SetUpHook(g_libGTASA+0x39C9E4, (uintptr_t)CPad__GetSteeringLeftRight_hook, (uintptr_t*)&CPad__GetSteeringLeftRight);
	SetUpHook(g_libGTASA+0x39CBF0, (uintptr_t)CPad__GetSteeringUpDown_hook, (uintptr_t*)&CPad__GetSteeringUpDown);

	SetUpHook(g_libGTASA+0x39DB7C, (uintptr_t)CPad__GetAccelerate_hook, (uintptr_t*)&CPad__GetAccelerate);
	SetUpHook(g_libGTASA+0x39D938, (uintptr_t)CPad__GetBrake_hook, (uintptr_t*)&CPad__GetBrake);
	SetUpHook(g_libGTASA+0x39D754, (uintptr_t)CPad__GetHandBrake_hook, (uintptr_t*)&CPad__GetHandBrake);
	SetUpHook(g_libGTASA+0x39D4C8, (uintptr_t)CPad__GetHorn_hook, (uintptr_t*)&CPad__GetHorn);
	SetUpHook(g_libGTASA+0x39DA1C, (uintptr_t)CPad__ExitVehicleJustDown_hook, (uintptr_t*)&CPad__ExitVehicleJustDown);
}