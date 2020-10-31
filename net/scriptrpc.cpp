#include "main.h"
#include "game/game.h"
#include "netgame.h"
#include "chatwindow.h"
#include "game/cheats.h"
#include "cheatgui.h"
#include "game/audiostream.h"

#define WEAPONTYPE_PISTOL_SKILL 69
#define WEAPONTYPE_PISTOL_SILENCED_SKILL 70
#define WEAPONTYPE_DESERT_EAGLE_SKILL 71
#define WEAPONTYPE_SHOTGUN_SKILL 72
#define WEAPONTYPE_SAWNOFF_SHOTGUN_SKILL 73
#define WEAPONTYPE_SPAS12_SHOTGUN_SKILL 74
#define WEAPONTYPE_MICRO_UZI_SKILL 75
#define WEAPONTYPE_MP5_SKILL 76
#define WEAPONTYPE_AK47_SKILL 77
#define WEAPONTYPE_M4_SKILL 78
#define WEAPONTYPE_SNIPERRIFLE_SKILL 79

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CCheats *pCheats;
extern CCheatGUI *pCheatGUI;
extern CWorld *pWorld;
extern CAudioStream *pAudioStream;

void ScrDisplayGameText(RPCParameters *rpcParams)
{
	Log("RPC: ScrDisplayGameText");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	char szMessage[512];
	int iType;
	int iTime;
	int iLength;

	bsData.Read(iType);
	bsData.Read(iTime);
	bsData.Read(iLength);

	if(iLength > 512) return;

	bsData.Read(szMessage,iLength);
	szMessage[iLength] = '\0';

	pGame->DisplayGameText(szMessage, iTime, iType);
}

void ScrSetGravity(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetGravity");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fGravity;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fGravity);

	pGame->SetGravity(fGravity);
}

void ScrForceSpawnSelection(RPCParameters *rpcParams)
{
	Log("RPC: ScrForceSpawnSelection");
	pNetGame->GetPlayerPool()->GetLocalPlayer()->ReturnToClassSelection();
}

void ScrSetPlayerPos(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerPos");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);

	if(pLocalPlayer) pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.X,vecPos.Y,vecPos.Z);
}

void ScrSetCameraPos(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetCameraPos");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	pGame->GetCamera()->SetPosition(vecPos.X, vecPos.Y, vecPos.Z, 0.0f, 0.0f, 0.0f);
}

void ScrSetCameraLookAt(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetCameraLookAt");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	pGame->GetCamera()->LookAtPoint(vecPos.X,vecPos.Y,vecPos.Z,2);
}

void ScrSetPlayerFacingAngle(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerFacingAngle");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fAngle;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fAngle);
	pGame->FindPlayerPed()->ForceTargetRotation(fAngle);
}

void ScrSetFightingStyle(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetFightingStyle");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteFightingStyle = 0;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(byteFightingStyle);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CPlayerPed *pPlayerPed = nullptr;

	if(pPlayerPool)
	{
		if(playerId == pPlayerPool->GetLocalPlayerID())
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		else if(pPlayerPool->GetSlotState(playerId))
			pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();

		if(pPlayerPed)
				pPlayerPed->SetFightingStyle(byteFightingStyle);
	}
}

void ScrSetPlayerSkin(RPCParameters *rpcParams)
{
	//Log("RPC: ScrSetPlayerSkin");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iPlayerID;
	unsigned int uiSkin;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(iPlayerID);
	bsData.Read(uiSkin);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(iPlayerID == pPlayerPool->GetLocalPlayerID())
		pPlayerPool->GetLocalPlayer()->GetPlayerPed()->SetModelIndex(uiSkin);
	else
	{
		if(pPlayerPool->GetSlotState(iPlayerID) && pPlayerPool->GetAt(iPlayerID)->GetPlayerPed())
			pPlayerPool->GetAt(iPlayerID)->GetPlayerPed()->SetModelIndex(uiSkin);
	}
}

void ScrApplyPlayerAnimation(RPCParameters *rpcParams)
{
	Log("RPC: ScrApplyPlayerAnimation");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteAnimLibLen;
	uint8_t byteAnimNameLen;
	char szAnimLib[256];
	char szAnimName[256];
	float fS;
	bool opt1, opt2, opt3, opt4;
	int opt5;
	CPlayerPool *pPlayerPool = nullptr;
	CPlayerPed *pPlayerPed = nullptr;

	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(byteAnimLibLen);
	bsData.Read(szAnimLib, byteAnimLibLen);
	bsData.Read(byteAnimNameLen);
	bsData.Read(szAnimName, byteAnimNameLen);
	bsData.Read(fS);
	bsData.Read(opt1);
	bsData.Read(opt2);
	bsData.Read(opt3);
	bsData.Read(opt4);
	bsData.Read(opt5);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';

	pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool)
	{
		if(pPlayerPool->GetLocalPlayerID() == playerId)
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		else if(pPlayerPool->GetSlotState(playerId))
			pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();

		Log("%s, %s", szAnimLib, szAnimName);

		if(pPlayerPed)
			pPlayerPed->ApplyAnimation(szAnimName, szAnimLib, fS, (int)opt1, (int)opt2, (int)opt3, (int)opt4, (int)opt5);
	}
}

void ScrClearPlayerAnimations(RPCParameters *rpcParams)
{
	Log("RPC: ScrClearPlayerAnimations");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	PLAYERID playerId;
	bsData.Read(playerId);
	MATRIX4X4 mat;

	CPlayerPool *pPlayerPool=NULL;
	CPlayerPed *pPlayerPed=NULL;

	pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool) {
		// Get the CPlayerPed for this player
		if(playerId == pPlayerPool->GetLocalPlayerID())
		{
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else
		{
			if(pPlayerPool->GetSlotState(playerId))
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
		}

		if(pPlayerPed)
		{
			pPlayerPed->GetMatrix(&mat);
			pPlayerPed->TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
		}
	}
}

void ScrSetSpawnInfo(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetSpawnInfo");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYER_SPAWN_INFO SpawnInfo;

	RakNet::BitStream bsData(Data, (iBitLength/8)+1, false);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bsData.Read((char*)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

	pPlayerPool->GetLocalPlayer()->SetSpawnInfo(&SpawnInfo);
}

void ScrCreateExplosion(RPCParameters *rpcParams)
{
	Log("RPC: ScrCreateExplosion");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	float X, Y, Z, Radius;
	int   iType;

	bsData.Read(X);
	bsData.Read(Y);
	bsData.Read(Z);
	bsData.Read(iType);
	bsData.Read(Radius);

	ScriptCommand(&create_explosion_with_radius, X, Y, Z, iType, Radius);
}

void ScrSetPlayerHealth(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerHealth");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	float fHealth;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fHealth);

	pLocalPlayer->GetPlayerPed()->SetHealth(fHealth);
}

void ScrSetPlayerArmour(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerArmour");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	float fHealth;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fHealth);

	pLocalPlayer->GetPlayerPed()->SetArmour(fHealth);
}

void ScrSetPlayerColor(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerColor");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	PLAYERID playerId;
	uint32_t dwColor;

	bsData.Read(playerId);
	bsData.Read(dwColor);

	Log("Color: 0x%X", dwColor);

	if(playerId == pPlayerPool->GetLocalPlayerID())
	{
		pPlayerPool->GetLocalPlayer()->SetPlayerColor(dwColor);
	}
	else
	{
		CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);
		if(pPlayer)	pPlayer->SetPlayerColor(dwColor);
	}
}

void ScrSetPlayerName(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerName");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteNickLen;
	char szNewName[MAX_PLAYER_NAME+1];
	uint8_t byteSuccess;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bsData.Read(playerId);
	bsData.Read(byteNickLen);

	if(byteNickLen > MAX_PLAYER_NAME) return;

	bsData.Read(szNewName, byteNickLen);
	bsData.Read(byteSuccess);

	szNewName[byteNickLen] = '\0';

	Log("byteSuccess = %d", byteSuccess);
	if (byteSuccess == 1) pPlayerPool->SetPlayerName(playerId, szNewName);

	// Extra addition which we need to do if this is the local player;
	if( pPlayerPool->GetLocalPlayerID() == playerId )
		pPlayerPool->SetLocalPlayerName( szNewName );
}

void ScrSetPlayerPosFindZ(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerPosFindZ");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	VECTOR vecPos;

	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);

	vecPos.Z = pGame->FindGroundZForCoord(vecPos.X, vecPos.Y, vecPos.Z);
	vecPos.Z += 1.5f;

	pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.X, vecPos.Y, vecPos.Z);
}

void ScrSetPlayerInterior(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerInterior");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;


	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteInterior;
	bsData.Read(byteInterior);

	pGame->FindPlayerPed()->SetInterior(byteInterior);
}

void ScrSetMapIcon(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetMapIcon");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteIndex;
	uint8_t byteIcon;
	uint32_t dwColor;
	float fPos[3];
	uint8_t byteStyle;

	bsData.Read(byteIndex);
	bsData.Read(fPos[0]);
	bsData.Read(fPos[1]);
	bsData.Read(fPos[2]);
	bsData.Read(byteIcon);
	bsData.Read(dwColor);
	bsData.Read(byteStyle);

	pNetGame->SetMapIcon(byteIndex, fPos[0], fPos[1], fPos[2], byteIcon, dwColor, byteStyle);
}

void ScrDisableMapIcon(RPCParameters *rpcParams)
{
	Log("RPC: ScrDisableMapIcon");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteIndex;

	bsData.Read(byteIndex);

	pNetGame->DisableMapIcon(byteIndex);
}

void ScrSetCameraBehindPlayer(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetCameraBehindPlayer");

	pGame->GetCamera()->SetBehindPlayer();
}

void ScrSetPlayerSpecialAction(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerSpecialAction");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteSpecialAction;
	bsData.Read(byteSpecialAction);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(pPlayerPool) pPlayerPool->GetLocalPlayer()->ApplySpecialAction(byteSpecialAction);
}

void ScrTogglePlayerSpectating(RPCParameters *rpcParams)
{
	Log("RPC: ScrTogglePlayerSpectating");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t bToggle;
	bsData.Read(bToggle);
	pPlayerPool->GetLocalPlayer()->ToggleSpectating(bToggle);
	Log("toggle: %d", bToggle);
}

void ScrSetPlayerSpectating(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerSpectating");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool->GetSlotState(playerId))
		pPlayerPool->GetAt(playerId)->SetState(PLAYER_STATE_SPECTATING);
}

#define SPECTATE_TYPE_NORMAL	1
#define SPECTATE_TYPE_FIXED		2
#define SPECTATE_TYPE_SIDE		3

void ScrPlayerSpectatePlayer(RPCParameters *rpcParams)
{
	Log("RPC: ScrPlayerSpectatePlayer");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	PLAYERID playerId;
    uint8_t byteMode;

	bsData.Read(playerId);
	bsData.Read(byteMode);

	switch (byteMode)
	{
		case SPECTATE_TYPE_FIXED:
			byteMode = 15;
			break;
		case SPECTATE_TYPE_SIDE:
			byteMode = 14;
			break;
		default:
			byteMode = 4;
	}

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectatePlayer(playerId);
}

void ScrPlayerSpectateVehicle(RPCParameters *rpcParams)
{
	Log("RPC: ScrPlayerSpectateVehicle");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	VEHICLEID VehicleID;
	uint8_t byteMode;

	bsData.Read(VehicleID);
	bsData.Read(byteMode);

	switch (byteMode)
	{
		case SPECTATE_TYPE_FIXED:
			byteMode = 15;
			break;
		case SPECTATE_TYPE_SIDE:
			byteMode = 14;
			break;
		default:
			byteMode = 3;
	}

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectateVehicle(VehicleID);
}

void ScrPutPlayerInVehicle(RPCParameters *rpcParams)
{
	Log("RPC: ScrPutPlayerInVehicle");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID vehicleid;
	uint8_t seatid;
	bsData.Read(vehicleid);
	bsData.Read(seatid);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	int iVehicleIndex = pNetGame->GetVehiclePool()->FindGtaIDFromID(vehicleid);
	CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(vehicleid);

	if(iVehicleIndex && pVehicle)
		 pGame->FindPlayerPed()->PutDirectlyInVehicle(iVehicleIndex, seatid);
}

void ScrVehicleParams(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetVehicleParams");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID VehicleID;
	uint8_t byteObjectiveVehicle;
	uint8_t byteDoorsLocked;

	bsData.Read(VehicleID);
	bsData.Read(byteObjectiveVehicle);
	bsData.Read(byteDoorsLocked);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	pVehiclePool->AssignSpecialParamsToVehicle(VehicleID,byteObjectiveVehicle,byteDoorsLocked);
}

void ScrVehicleParamsEx(RPCParameters *rpcParams)
{
	Log("RPC: ScrVehicleParamsEx");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	VEHICLEID VehicleId;
	uint8_t engine, lights, alarm, doors, bonnet, boot, objective;
	bsData.Read(VehicleId);
	bsData.Read(engine);
	bsData.Read(lights);
	bsData.Read(alarm);
	bsData.Read(doors);
	bsData.Read(bonnet);
	bsData.Read(boot);
	bsData.Read(objective);
	Log("VehicleId: %d", VehicleId);
	Log("engine: %d, lights: %d, alarm: %d, doors: %d, bonnet: %d, boot: %d, obj: %d",
		engine, lights, alarm, doors, bonnet, boot, objective);

	if(pNetGame && pNetGame->GetVehiclePool())
	{
		if(pNetGame->GetVehiclePool()->GetSlotState(VehicleId))
		{
			// doors
			pNetGame->GetVehiclePool()->GetAt(VehicleId)->SetDoorState(doors);
			// engine
			pNetGame->GetVehiclePool()->GetAt(VehicleId)->SetEngineState(engine);
			// lights
			pNetGame->GetVehiclePool()->GetAt(VehicleId)->SetLightsState(lights);
		}
	}
}

void ScrHaveSomeMoney(RPCParameters *rpcParams)
{
	Log("RPC: ScrHaveSomeMoney");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iAmmount;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(iAmmount);
	pGame->AddToLocalMoney(iAmmount);
}

void ScrResetMoney(RPCParameters *rpcParams)
{
	Log("RPC: ScrResetMoney");

	pGame->ResetLocalMoney();
}

void ScrLinkVehicle(RPCParameters *rpcParams)
{
	Log("RPC: ScrLinkVehicle");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID VehicleID;
	uint8_t byteInterior;

	bsData.Read(VehicleID);
	bsData.Read(byteInterior);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	pVehiclePool->LinkToInterior(VehicleID, (int)byteInterior);
}

void ScrRemovePlayerFromVehicle(RPCParameters *rpcParams)
{
	Log("RPC: ScrRemovePlayerFromVehicle");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	pPlayerPool->GetLocalPlayer()->GetPlayerPed()->ExitCurrentVehicle();
}

void ScrSetVehicleHealth(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetVehicleHealth");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	float fHealth;
	VEHICLEID VehicleID;

	bsData.Read(VehicleID);
	bsData.Read(fHealth);

	if(pNetGame->GetVehiclePool()->GetSlotState(VehicleID))
		pNetGame->GetVehiclePool()->GetAt(VehicleID)->SetHealth(fHealth);
}

void ScrSetVehiclePos(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetVehiclePos");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID VehicleId;
	float fX, fY, fZ;
	bsData.Read(VehicleId);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

	if(pNetGame && pNetGame->GetVehiclePool())
	{
		if(pNetGame->GetVehiclePool()->GetSlotState(VehicleId))
			pNetGame->GetVehiclePool()->GetAt(VehicleId)->TeleportTo(fX, fY, fZ);
	}
}

void ScrSetVehicleVelocity(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetVehicleVelocity");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t turn = false;
	VECTOR vecMoveSpeed;
	bsData.Read(turn);
	bsData.Read(vecMoveSpeed.X);
	bsData.Read(vecMoveSpeed.Y);
	bsData.Read(vecMoveSpeed.Z);
	Log("X: %f, Y: %f, Z: %f", vecMoveSpeed.X, vecMoveSpeed.Y, vecMoveSpeed.Z);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();

	if(pPlayerPed)
	{
		CVehicle *pVehicle = pVehiclePool->GetAt( pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle()));
		if(pVehicle)
			pVehicle->SetMoveSpeedVector(vecMoveSpeed);
	}
}

void ScrNumberPlate(RPCParameters *rpcParams)
{
	Log("RPC: ScrNumberPlate");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	VEHICLEID VehicleID;
	char len;
	char szNumberPlate[32+1];

	bsData.Read(VehicleID);
	bsData.Read(len);
	bsData.Read(szNumberPlate, len);
	szNumberPlate[len] = '\0';

	Log("plate: %s", szNumberPlate);
}

void ScrInterpolateCamera(RPCParameters *rpcParams)
{
	Log("ScrInterpolateCamera");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bool bSetPos = true;
	VECTOR vecFrom, vecDest;
	int time;
	uint8_t mode;

	bsData.Read(bSetPos);
	bsData.Read(vecFrom.X);
	bsData.Read(vecFrom.Y);
	bsData.Read(vecFrom.Z);
	bsData.Read(vecDest.X);
	bsData.Read(vecDest.Y);
	bsData.Read(vecDest.Z);
	bsData.Read(time);
	bsData.Read(mode);

	if(mode < 1 || mode > 2)
		mode = 2;

	if(time > 0)
	{
		// pNetGame->GetPlayerPool()->GetLocalPlayer()->m_b.. = true;
		if(bSetPos)
		{
			pGame->GetCamera()->InterpolateCameraPos(&vecFrom, &vecDest, time, mode);
		}
		else
			pGame->GetCamera()->InterpolateCameraLookAt(&vecFrom, &vecDest, time, mode);
	}
}

void ScrAddGangZone(RPCParameters *rpcParams)
{
	Log("RPC: ScrAddGangZone");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		float minx, miny, maxx, maxy;
		uint32_t dwColor;
		bsData.Read(wZoneID);
		bsData.Read(minx);
		bsData.Read(miny);
		bsData.Read(maxx);
		bsData.Read(maxy);
		bsData.Read(dwColor);
		pGangZonePool->New(wZoneID, minx, miny, maxx, maxy, dwColor);
	}
}

void ScrRemoveGangZone(RPCParameters *rpcParams)
{
	Log("RPC: ScrRemoveGangZone");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();

	if (pGangZonePool)
	{
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->Delete(wZoneID);
	}
}

void ScrFlashGangZone(RPCParameters *rpcParams)
{
	Log("RPC: ScrFlashGangZone");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		uint32_t dwColor;
		bsData.Read(wZoneID);
		bsData.Read(dwColor);
		pGangZonePool->Flash(wZoneID, dwColor);
	}
}

void ScrStopFlashGangZone(RPCParameters *rpcParams)
{
	Log("RPC: ScrStopFlashGangZone");

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->StopFlash(wZoneID);
	}
}

int iTotalObjects = 0;
void ScrCreateObject(RPCParameters *rpcParams)
{
	//Log("RPC_SCRCREATEOBJECT(%d)", iTotalObjects);

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t wObjectID;
	unsigned long ModelID;
	float fDrawDistance;

	uint8_t bNoCameraCol;
	uint16_t AttachedObjectID;
	VEHICLEID AttachedVehicleID;
	VECTOR AttachOffset, AttachRot;
	uint8_t bSyncRotation;

	VECTOR vecPos, vecRot;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(wObjectID);
	bsData.Read(ModelID);

	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);

	bsData.Read(vecRot.X);
	bsData.Read(vecRot.Y);
	bsData.Read(vecRot.Z);

	bsData.Read(fDrawDistance);

	bsData.Read(bNoCameraCol);
	bsData.Read(AttachedVehicleID);
	bsData.Read(AttachedObjectID);

	if(AttachedObjectID != INVALID_OBJECT_ID || AttachedVehicleID != INVALID_VEHICLE_ID)
	{
		bsData.Read(AttachOffset.X);
		bsData.Read(AttachOffset.Y);
		bsData.Read(AttachOffset.Z);
		bsData.Read(AttachRot.X);
		bsData.Read(AttachRot.Y);
		bsData.Read(AttachRot.Z);
		bsData.Read(bSyncRotation);
	}


	iTotalObjects++;

	CObjectPool *pObjectPool = pNetGame->GetObjectPool();
	pObjectPool->New(wObjectID, ModelID, vecPos, vecRot, fDrawDistance);

	CObject *pObject = pNetGame->GetObjectPool()->GetAt(wObjectID);

	if(pObjectPool && AttachedVehicleID != INVALID_VEHICLE_ID)
	{
		if (pObject)
		{
			pObject->SetCollision(false);
			pObject->AttachProccess(AttachedVehicleID, AttachOffset, AttachRot);
		}
	}
}

void ScrDestroyObject(RPCParameters *rpcParams)
{
	Log("RPC_SCRDELETEOBJECT");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t wObjectID;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(wObjectID);

	//LOGI("id: %d", wObjectID);
	iTotalObjects--;

	CObjectPool* pObjectPool =	pNetGame->GetObjectPool();
	if(pObjectPool->GetAt(wObjectID))
		pObjectPool->Delete(wObjectID);
}

void ScrSetObjectPos(RPCParameters *rpcParams)
{
	Log("RPC: SetObjectPos");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);

	uint16_t wObjectID;
	float fRotation;
	VECTOR vecPos, vecRot;
	bsData.Read(wObjectID);
	if(wObjectID < 0 || wObjectID >= MAX_OBJECTS)
		return;

	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	bsData.Read(fRotation);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if(pObjectPool)
	{
		if(pObjectPool->GetSlotState(wObjectID))
		{
			CObject* pObject = pObjectPool->GetAt(wObjectID);
			if(pObject)
				pObject->SetPos(vecPos.X, vecPos.Y, vecPos.Z);
		}
	}
}

void ScrSetObjectRotation(RPCParameters *rpcParams)
{
	Log("RPC: SetObjectRotation");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);

	uint16_t wObjectID;
	bsData.Read(wObjectID);
	if(wObjectID < 0 || wObjectID >= MAX_OBJECTS)
		return;

	VECTOR vecRot;
	bsData.Read(vecRot.X);
	bsData.Read(vecRot.Y);
	bsData.Read(vecRot.Z);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if(pObjectPool)
	{
		if(pObjectPool->GetSlotState(wObjectID))
		{
			CObject* pObject = pObjectPool->GetAt(wObjectID);
			if(pObject)
				pObject->InstantRotate(vecRot.X, vecRot.Y, vecRot.Z);
		}
	}
}
void ScrAttachObjectToPlayer(RPCParameters* rpcParams)
{
	Log("RPC: AttachObjectToPlayer");

	unsigned char * Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint16_t byteObjectID;
	uint16_t bytePlayerID;
	float OffsetX, OffsetY, OffsetZ, rX, rY, rZ;

	bsData.Read(byteObjectID);
	bsData.Read(bytePlayerID);

	bsData.Read(OffsetX);
	bsData.Read(OffsetY);
	bsData.Read(OffsetZ);

	bsData.Read(rX);
	bsData.Read(rY);
	bsData.Read(rZ);

	try {

	CObject* pObject =	pNetGame->GetObjectPool()->GetAt(	byteObjectID );

	if ( !pObject )
		return;

	if ( bytePlayerID == pNetGame->GetPlayerPool()->GetLocalPlayerID() )
	{
		CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		ScriptCommand( &attach_object_to_actor, pObject->m_dwGTAId, pPlayer->GetPlayerPed()->m_dwGTAId,
			OffsetX,
			OffsetY,
			OffsetZ,
			rX,
			rY,
			rZ);
	} else {
		CRemotePlayer* pPlayer =	pNetGame->GetPlayerPool()->GetAt(	bytePlayerID );

		if ( !pPlayer )
			return;

		ScriptCommand( &attach_object_to_actor, pObject->m_dwGTAId, pPlayer->GetPlayerPed()->m_dwGTAId,
																	OffsetX,
																	OffsetY,
																	OffsetZ,
																	rX,
																	rY,
																	rZ);
	}

	} catch(...) {}
}

void ScrPlaySound(RPCParameters *rpcParams)
{
	Log("RPC: ScrPlaySound");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	int iSound;
	float fX, fY, fZ;
	bsData.Read(iSound);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	pGame->PlaySound(iSound, fX, fY, fZ);
}

void ScrSetPlayerWantedLevel(RPCParameters *rpcParams)
{
	Log("RPC: ScrSetPlayerWantedLevel");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	if(!pGame) return;

	uint8_t byteLevel;
	bsData.Read(byteLevel);
	pGame->SetWantedLevel(byteLevel);
}

void ScrMoveObject(RPCParameters *rpcParams)
{
	Log("RPC: ScrMoveObject");

	unsigned char * Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	unsigned short byteObjectID;
	float curx, cury, curz, newx, newy, newz, speed, rotx, roty, rotz;

	bsData.Read(byteObjectID);
	bsData.Read(curx);
	bsData.Read(cury);
	bsData.Read(curz);
	bsData.Read(newx);
	bsData.Read(newy);
	bsData.Read(newz);
	bsData.Read(speed);
	bsData.Read(rotx);
	bsData.Read(roty);
	bsData.Read(rotz);

	CObject* pObject = pNetGame->GetObjectPool()->GetAt(byteObjectID);
	if (pObject)
	{
		pObject->TeleportTo(curx, cury, curz);
		pObject->MoveTo(newx, newy, newz, speed, rotx, roty, rotz);
	}
}

void ScrResetPlayerWeapons(RPCParameters *rpcParams)
{
	Log("RPC: ScrResetPlayerWeapons");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	pPlayerPed->ClearAllWeapons();
	//pChatWindow->AddDebugMessage("Cleared weapons");
}

void ScrGivePlayerWeapon(RPCParameters* rpcParams)
{
	Log("RPC: ScrGivePlayerWeapon");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID wPlayerID;
	int iWeaponID;
	int iAmmo;
	bsData.Read(iWeaponID);
	bsData.Read(iAmmo);

	CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pPlayer->GetPlayerPed()->GiveWeapon(iWeaponID, iAmmo);
}

void ScrTogglePlayerControllable(RPCParameters *rpcParams)
{
	Log("RPC: TogglePlayerControllable");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	uint8_t byteControllable;
	bsData.Read(byteControllable);
	//Log("controllable = %d", byteControllable);
	//pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->SetPlayerControl((int)byteControllable);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable((int)byteControllable);
}

void ScrSetPlayerSkillLevel(RPCParameters *rpcParams)
{
	int iBitLength = rpcParams->numberOfBitsOfData;
	PLAYERID bytePlayerID;
	unsigned int ucSkillType;
	unsigned short uiSkillLevel;
	CPlayerPool * pPlayerPool = pNetGame->GetPlayerPool();
	RakNet::BitStream bsData(rpcParams->input, (iBitLength / 8) + 1, false);

	bsData.Read(bytePlayerID);
	bsData.Read(ucSkillType);
	bsData.Read(uiSkillLevel);

	if (ucSkillType < 0 || ucSkillType > 10) return;
	if (uiSkillLevel < 0 || uiSkillLevel > 1000) return;

	switch (ucSkillType)
	{
	case 0:
		ucSkillType = WEAPONTYPE_PISTOL_SKILL;
		break;
	case 1:
		ucSkillType = WEAPONTYPE_PISTOL_SILENCED_SKILL;
		break;
	case 2:
		ucSkillType = WEAPONTYPE_DESERT_EAGLE_SKILL;
		break;
	case 3:
		ucSkillType = WEAPONTYPE_SHOTGUN_SKILL;
		break;
	case 4:
		ucSkillType = WEAPONTYPE_SAWNOFF_SHOTGUN_SKILL;
		break;
	case 5:
		ucSkillType = WEAPONTYPE_SPAS12_SHOTGUN_SKILL;
		break;
	case 6:
		ucSkillType = WEAPONTYPE_MICRO_UZI_SKILL;
		break;
	case 7:
		ucSkillType = WEAPONTYPE_MP5_SKILL;
		break;
	case 8:
		ucSkillType = WEAPONTYPE_AK47_SKILL;
		break;
	case 9:
		ucSkillType = WEAPONTYPE_M4_SKILL;
		break;
	case 10:
		ucSkillType = WEAPONTYPE_SNIPERRIFLE_SKILL;
		break;
	default:
		return;
	}

	ScriptCommand(&change_stat, ucSkillType, uiSkillLevel);
	//g_pGame->GetStats()->SetStatValue(ucSkillType, uiSkillLevel);

	unsigned char ucWeaponSkill;
	if (uiSkillLevel >= 0 && uiSkillLevel < 333)
		ucWeaponSkill = 0;
	else if (uiSkillLevel >= 333 && uiSkillLevel < 666)
		ucWeaponSkill = 1;
	else
		ucWeaponSkill = 2;

	CPlayerPed *pPlayer = pGame->FindPlayerPed();
	if (pPlayer)
		ScriptCommand(&set_char_weapon_skill, pPlayer->m_dwGTAId, ucWeaponSkill);
}

void ScrAttachObjectToVehicle(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	VEHICLEID vehicleid;
	unsigned short objectid;
	float coords[6];

	bsData.Read(objectid);
	bsData.Read(vehicleid);
	for (unsigned char paramid = 0; paramid < 6; paramid++)
		bsData.Read(coords[paramid]);

	//pChatWindow->AddDebugMessage("obj=%d, vid=%d, %f %f %f, %f %f %f", objectid, vehicleid, coords[0], coords[1], coords[2], coords[3], coords[4], coords[5]);

	if (!pNetGame->GetVehiclePool()->GetSlotState(vehicleid) || !pNetGame->GetObjectPool()->GetSlotState(objectid)) return;

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(vehicleid);
	if (pVehicle)
	{
		pVehicle->SetAttachedObject(pNetGame->GetObjectPool()->GetAt(objectid));
	}

	ScriptCommand(&attach_object_to_car,
		pNetGame->GetObjectPool()->GetAt(objectid)->m_dwGTAId,
		pNetGame->GetVehiclePool()->GetAt(vehicleid)->m_dwGTAId,
		coords[0], coords[1], coords[2], //offsets
		coords[3], coords[4], coords[5]); //rotations
}

void ScrSetPlayerArmed(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	uint32_t weaponID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(weaponID);

	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->SetArmedWeapon(weaponID);
}

void ScrSetAdminLevel(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	unsigned int iLevel;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iLevel);

	pCheats->SetLevel(iLevel);
	pCheatGUI->SetStatus(true);

	//pChatWindow->AddDebugMessage("�� ������� ��������������. ��� �������� ���� %d-� ������ (/csa)", iLevel);
}
void ScrSetPlayerAttachedObject(RPCParameters *rpcParams)
{

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint16_t playerId;
	uint32_t index, model, bone;
	bool create;
	VECTOR vecOffset;
	VECTOR vecRotation;
	VECTOR vecScale;
	int32_t materialcolor1, materialcolor2;

	bsData.Read(playerId);
	bsData.Read(index);
	bsData.Read(create);
	bsData.Read(model);
	bsData.Read(bone);

	// offset
	bsData.Read(vecOffset.X);
	bsData.Read(vecOffset.Y);
	bsData.Read(vecOffset.Z);

	// rotation
	bsData.Read(vecRotation.X);
	bsData.Read(vecRotation.Y);
	bsData.Read(vecRotation.Z);

	// scale
	bsData.Read(vecScale.X);
	bsData.Read(vecScale.Y);
	bsData.Read(vecScale.Z);

	// materialcolor
	bsData.Read(materialcolor1);
	bsData.Read(materialcolor2);

	if(model > 0 && model < 20000)
	{
		uintptr_t *dwModelArray = (uintptr_t*)(g_libGTASA+0x87BF48);
    	if(dwModelArray[model] == 0)
    		model = 18631;
	}
	else model = 18631;

	int waitPreloaded;
	if(!pGame->IsModelLoaded(model))
	{
		Log("Warning: object model not preloaded. object: %d", model);
		pGame->RequestModel(model, GAME_REQUIRED);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(model))
		{
			usleep(500);
			waitPreloaded++;
			if(waitPreloaded >= 10)
			{
				Log("Object failed to preloaded. object: %d", model);
				model = 18631;
			}
		}

		Log("Object model was preloaded. object: %d", model);
	}

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(pPlayerPool)
	{
		CPlayerPed *pPlayerPed = nullptr;

		if(playerId == pPlayerPool->GetLocalPlayerID())
		{
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else
		{
			CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
			if(pRemotePlayer)
			{
				pPlayerPed = pRemotePlayer->GetPlayerPed();
			}
			else
			{
				pPlayerPed = nullptr;
			}
		}

		if(pPlayerPed)
		{
			pPlayerPed->UpdateAttachedObject(
				create,
				index, model, bone, vecOffset, vecRotation, vecScale, materialcolor1, materialcolor2
			);
		}
	}
}

void ScrSelectTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool bEnable = false;
	uint32_t dwColor = 0;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bEnable);
	bsData.Read(dwColor);

	pNetGame->GetTextDrawPool()->SetSelectState(bEnable ? true : false, dwColor);
}

void ScrShowTextDraw(RPCParameters *rpcParams)
{
	Log("RPC: ScrShowTextDraw");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength/8)+1, false);

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool)
	{
		uint16_t wTextID;
		TEXT_DRAW_TRANSMIT TextDrawTransmit;
		char cText[1024];
		unsigned short cTextLen = 0;
		bsData.Read(wTextID);
		bsData.Read(( char *)&TextDrawTransmit, sizeof(TEXT_DRAW_TRANSMIT));
		bsData.Read(cTextLen);
		bsData.Read(cText, cTextLen);
		cText[cTextLen] = '\0';

		Log("cText: %s", cText);
		pTextDrawPool->New(wTextID, &TextDrawTransmit, cText);
	}
}

void ScrHideTextDraw(RPCParameters *rpcParams)
{
	Log("RPC: HideTextDraw");
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();

	if (pTextDrawPool)
	{
		uint16_t wTextID;
		bsData.Read(wTextID);
		pTextDrawPool->Delete(wTextID);
	}
}

void ScrEditTextDraw(RPCParameters *rpcParams)
{
	Log("RPC: EditTextDraw");
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool)
	{
		uint16_t wTextID;
		char cText[MAX_TEXT_DRAW_LINE];
		unsigned short cTextLen = 0;
		bsData.Read(wTextID);
		bsData.Read(cTextLen);
		bsData.Read(cText, cTextLen);
		cText[cTextLen] = '\0';
		CTextDraw* pText = pTextDrawPool->GetAt(wTextID);
		if (pText)
			pText->SetText(cText);
	}
}

void ScrChatBubble(RPCParameters *rpcParams)
{
    unsigned char *Data = reinterpret_cast<unsigned char*>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData(Data, (iBitLength/8)+1, false);

    PLAYERID playerid;
	CHATBUBBLE_DATA data;

	bsData.Read(playerid);
	bsData.Read(data.dwColor);
	bsData.Read(data.fDrawDistance);
	bsData.Read(data.iElapseTime);
	bsData.Read(data.iTextlen);
	char szBuff[0xFF];
	bsData.Read(szBuff, data.iTextlen);
	szBuff[data.iTextlen] = '\0';
	cp1251_to_utf8(data.szText, szBuff);

	if(pNetGame)
	{
		CPlayerBubblePool *pBubblePool = pNetGame->GetPlayerBubblePool();
		if(pBubblePool)
			pBubblePool->AddChatBubble(playerid, data);
	}
}

void ScrRemoveBuilding(RPCParameters *rpcParams)
{
	unsigned char *Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

    uint32_t dObjectModel;
    float fX, fY, fZ, fRadius;

    bsData.Read(dObjectModel);
    bsData.Read(fX);
    bsData.Read(fY);
    bsData.Read(fZ);
    bsData.Read(fRadius);

    pWorld->RemoveBuilding(dObjectModel, fRadius, fX, fY, fZ);
}

void ScrDeathMessage(RPCParameters* rpcParams)
{
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	PLAYERID playerId, killerId;
	uint8_t reason;

	bsData.Read(playerId);
	bsData.Read(killerId);
	bsData.Read(reason);

	std::string killername, playername;
	killername.clear();
	playername.clear();

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool)
	{
		if (pPlayerPool->GetLocalPlayerID() == playerId)
			playername = pPlayerPool->GetLocalPlayerName();

		if (pPlayerPool->GetLocalPlayerID() == killerId)
			killername = pPlayerPool->GetLocalPlayerName();

		if (killername.empty() || playername.empty())
		{
			if (killername.empty())
			{
				if (pPlayerPool->GetSlotState(killerId))
				{
					killername = pPlayerPool->GetPlayerName(killerId);
				}
			}
			if (playername.empty())
			{
				if (pPlayerPool->GetSlotState(playerId))
				{
					playername = pPlayerPool->GetPlayerName(playerId);
				}
			}
		}
	}

}

void ScrSetVehicleZAngle(RPCParameters *rpcParams)
{
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t vehicleId;
	float fZAngle;

	bsData.Read(vehicleId);
	bsData.Read(fZAngle);

	CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(vehicleId);
	if(pVehicle) {
		ScriptCommand(&set_car_z_angle, pVehicle->m_dwGTAId, fZAngle);
	}
}

void ScrPlayAudioStream(RPCParameters *rpcParams)
{
    Log("RPC: PlayAudioStreamForPlayer");

    RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

    uint8_t byteTextLen;
    char szURL[1024];

    float X, Y, Z;
    float fRadius;

    bool bUsePos;

    bsData.Read(byteTextLen);
    bsData.Read(szURL, byteTextLen);

    bsData.Read(X);
    bsData.Read(Y);
    bsData.Read(Z);

    bsData.Read(fRadius);

    bsData.Read(bUsePos);

    szURL[byteTextLen] = '\0';

    if(pAudioStream) pAudioStream->Play(szURL, X, Y, Z, fRadius, bUsePos);

    if(pChatWindow) {
        pChatWindow->AddInfoMessage("{FFFFFF}Audio Stream: %s", szURL);
    }
}

void ScrStopAudioStream(RPCParameters *rpcParams)
{
    Log("RPC: StopAudioStreamForPlayer");

    if(pAudioStream) pAudioStream->Stop(true);
}

void RegisterScriptRPCs(RakClientInterface* pRakClient)
{
	Log("Registering ScriptRPC's..");
	//pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkillLevel, ScrSetPlayerSkillLevel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ScrDisplayGameText);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetGravity, ScrSetGravity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrForceSpawnSelection,ScrForceSpawnSelection);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ScrSetPlayerPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ScrSetCameraPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ScrSetCameraLookAt);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ScrSetPlayerFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetFightingStyle, ScrSetFightingStyle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ScrSetPlayerSkin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ScrApplyPlayerAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ScrClearPlayerAnimations);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ScrSetSpawnInfo);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ScrCreateExplosion);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ScrSetPlayerHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ScrSetPlayerArmour);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ScrSetPlayerColor);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ScrSetPlayerName);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ScrSetPlayerPosFindZ);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ScrSetPlayerInterior);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ScrSetMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ScrDisableMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ScrSetCameraBehindPlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpecialAction, ScrSetPlayerSpecialAction);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ScrTogglePlayerSpectating);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSpectating, ScrSetPlayerSpectating);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ScrPlayerSpectatePlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ScrPlayerSpectateVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ScrPutPlayerInVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ScrVehicleParams);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ScrVehicleParamsEx);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ScrHaveSomeMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ScrResetMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ScrLinkVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ScrRemovePlayerFromVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ScrSetVehicleHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ScrSetVehiclePos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ScrSetVehicleVelocity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrNumberPlate, ScrNumberPlate);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ScrInterpolateCamera);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone,ScrAddGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone,ScrRemoveGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone,ScrFlashGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone,ScrStopFlashGangZone);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ScrCreateObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos, ScrSetObjectPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ScrDestroyObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ScrSetObjectRotation);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlaySound, ScrPlaySound);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ScrSetPlayerWantedLevel);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ScrGivePlayerWeapon);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ScrTogglePlayerControllable);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToPlayer, ScrAttachObjectToPlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ScrResetPlayerWeapons);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ScrMoveObject);
	//pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToVehicle, ScrAttachObjectToVehicle);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ScrSetPlayerArmed);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetAdminLevel, ScrSetAdminLevel);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ScrSetPlayerAttachedObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClickTextDraw, ScrSelectTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ScrShowTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ScrHideTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ScrEditTextDraw);

	// objMaterial
	//pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectMaterial, ScrSetObjectMaterial);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ScrSetVehicleZAngle);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrChatBubble, ScrChatBubble);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveBuilding, ScrRemoveBuilding);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDeathMessage, ScrDeathMessage);

	#ifdef BUILD_BETA;
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ScrPlayAudioStream);
  pRakClient->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ScrStopAudioStream);
	#endif

}

void UnRegisterScriptRPCs(RakClientInterface* pRakClient)
{
	Log("Unregistering ScriptRPC's..");
	//pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkillLevel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetGravity);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrForceSpawnSelection);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetFightingStyle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetInterior);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpecialAction);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSpectating);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrVehicleParams);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetMoney);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrNumberPlate);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAddGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDestroyObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlaySound);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToPlayer);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrMoveObject);
	//pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToVehicle);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetArmedWeapon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetAdminLevel);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClickTextDraw);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrChatBubble);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveBuilding);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDeathMessage);

	#ifdef BUILD_BETA
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_PlayAudioStream);
  pRakClient->UnregisterAsRemoteProcedureCall(&RPC_StopAudioStream);
	#endif
}
