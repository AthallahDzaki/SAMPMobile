#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

#include "chatwindow.h"
#include "spawnscreen.h"
#include "scoreboard.h"
#include "admin-bot.h"
#include "game/cheats.h"
#include "cheatgui.h"
#include "consolegui.h"
#include "button.h"

#define NETGAME_VERSION 4057
#define AUTH_BS "FF2BE5E6F5D9392F57C4E66F7AD78767277C6E4F6B"

//const auto enctyptedAuthKey = cryptor::create("10EF38095514DBF164C8FD10438A3C9BCB4DA4A9E15", 64);
const auto encAuthKey = cryptor::create("10B3D2B1317ADD02CC1F680BC500A8BC0FD7AD42CE7", 43);

//149C15C7E69314B147D55069C245763C07DD8AB4429

extern CGame *pGame;
extern CSpawnScreen *pSpawnScreen;
extern CChatWindow *pChatWindow;
extern CScoreBoard *pScoreBoard;
extern CCheatGUI *pCheatGUI;
extern CConsoleGUI *pConsoleGUI;
extern CButton *pButton;

// Admin reference
CCheats *pCheats;
CAdminBot *pAdminBot;


int iVehiclePoolProcessFlag = 0;
int iPickupPoolProcessFlag = 0;

void RegisterRPCs(RakClientInterface* pRakClient);
void UnRegisterRPCs(RakClientInterface* pRakClient);
void RegisterScriptRPCs(RakClientInterface* pRakClient);
void UnRegisterScriptRPCs(RakClientInterface* pRakClient);

unsigned char GetPacketID(Packet *p)
{
	if(p == 0) return 0xFF;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char) p->data[0];
}

CNetGame::CNetGame(const char* szHostOrIp, int iPort, const char* szPlayerName, const char* szPass)
{
	strcpy(m_szHostName, "San Andreas Multiplayer");
	strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
	m_iPort = iPort;

	m_pPlayerPool = new CPlayerPool();
	m_pPlayerPool->SetLocalPlayerName(szPlayerName);

	pCheats = new CCheats(szPlayerName, 0);

	m_pVehiclePool = new CVehiclePool();
	m_pObjectPool = new CObjectPool();
	m_pPickupPool = new CPickupPool();
	m_pGangZonePool = new CGangZonePool();
	m_pLabelPool = new CText3DLabelsPool();
	m_pTextDrawPool = new CTextDrawPool();
	m_pPlayerBubblePool = new CPlayerBubblePool();
	m_pActorPool = new CActorPool();

	// Need to be deleted
	///////////////////////////////////////////////////
	pAdminBot = new CAdminBot();
	///////////////////////////////////////////////////

	m_pRakClient = RakNetworkFactory::GetRakClientInterface();
	RegisterRPCs(m_pRakClient);
	RegisterScriptRPCs(m_pRakClient);
	m_pRakClient->SetPassword(szPass);

	m_dwLastConnectAttempt = GetTickCount();
	m_iGameState = 	GAMESTATE_WAIT_CONNECT;

	m_iSpawnsAvailable = 0;
	m_bHoldTime = true;
	m_byteWorldMinute = 0;
	m_byteWorldTime = 12;
	m_byteWeather =	10;
	m_fGravity = (float)0.008000000;
	m_bUseCJWalk = false;
	m_bDisableEnterExits = false;
	m_fNameTagDrawDistance = 60.0f;
	m_bZoneNames = false;
	m_bInstagib = false;
	m_iDeathDropMoney = 0;
	m_bNameTagLOS = false;
	m_bHeadMove = false;

	for(int i=0; i<100; i++)
		m_dwMapIcons[i] = 0;

	pGame->EnableClock(false);
	pGame->EnableZoneNames(false);
}

CNetGame::~CNetGame()
{
	m_pRakClient->Disconnect(0);
	UnRegisterRPCs(m_pRakClient);
	UnRegisterScriptRPCs(m_pRakClient);
	RakNetworkFactory::DestroyRakClientInterface(m_pRakClient);

	if(m_pPlayerPool)
	{
		delete m_pPlayerPool;
		m_pPlayerPool = nullptr;
	}

	if(m_pVehiclePool)
	{
		delete m_pVehiclePool;
		m_pVehiclePool = nullptr;
	}

	if(m_pPickupPool)
	{
		delete m_pPickupPool;
		m_pPickupPool = nullptr;
	}

	if(m_pGangZonePool)
	{
		delete m_pGangZonePool;
		m_pGangZonePool = nullptr;
	}

	if(m_pLabelPool)
	{
		delete m_pLabelPool;
		m_pLabelPool = nullptr;
	}

	if(pCheats)
	{
		delete pCheats;
		pCheats = nullptr;
	}

	if(m_pTextDrawPool)
	{
		delete m_pTextDrawPool;
		m_pTextDrawPool = nullptr;
	}

	if(m_pPlayerBubblePool)
	{
		delete m_pPlayerBubblePool;
		m_pPlayerBubblePool = nullptr;
	}

	if(m_pActorPool)
	{
		delete m_pActorPool;
		m_pActorPool = nullptr;
	}
}

void CNetGame::Process()
{
	UpdateNetwork();

	if(m_bHoldTime)
		pGame->SetWorldTime(m_byteWorldTime, m_byteWorldMinute);

	pAdminBot->Process();

	if(GetGameState() == GAMESTATE_CONNECTED)
	{
		// pool process
		if(m_pPlayerPool) m_pPlayerPool->Process();

		if(m_pVehiclePool && iVehiclePoolProcessFlag > 5)
		{
			m_pVehiclePool->Process();
			iVehiclePoolProcessFlag = 0;
		}

		if(m_pPickupPool && iPickupPoolProcessFlag > 5)
		{
			m_pPickupPool->Process();
			iPickupPoolProcessFlag = 0;
		}
		else
			++iPickupPoolProcessFlag;

		if(m_pObjectPool) m_pObjectPool->Process();

		//if(pCheats) pCheats->Process();
	}
	else
	{
		CPlayerPed *pPlayer = pGame->FindPlayerPed();
		CCamera *pCamera = pGame->GetCamera();

		if(pPlayer && pCamera)
		{
			if(pPlayer->IsInVehicle())
				pPlayer->RemoveFromVehicleAndPutAt(1093.4f, -2036.5f, 82.7106f);
			else
				pPlayer->TeleportTo(1093.4f, -2036.5f, 82.7106f);

			pCamera->SetPosition(1093.0f, -2036.0f, 90.0f, 0.0f, 0.0f, 0.0f);
			pCamera->LookAtPoint(384.0f, -1557.0f, 20.0f, 2);
			pGame->SetWorldWeather(m_byteWeather);
			pGame->DisplayWidgets(false);
		}
	}

	if(GetGameState() == GAMESTATE_WAIT_CONNECT &&
		(GetTickCount() - m_dwLastConnectAttempt) > 4000)
	{
		if(pChatWindow) pChatWindow->AddDebugMessage("Connecting to %s:%d...", m_szHostOrIp, m_iPort);
		m_pRakClient->Connect(m_szHostOrIp, m_iPort, 0, 0, 5);
		m_dwLastConnectAttempt = GetTickCount();
		SetGameState(GAMESTATE_CONNECTING);
	}
}

void CNetGame::UpdateNetwork()
{
	Packet* pkt = nullptr;
	unsigned char packetIdentifier;

	while(pkt = m_pRakClient->Receive())
	{
		packetIdentifier = GetPacketID(pkt);

		switch(packetIdentifier)
		{
			case ID_AUTH_KEY:
			Log("Incoming packet: ID_AUTH_KEY");
			Packet_AuthKey(pkt);
			break;

			case ID_CONNECTION_BANNED:
			case ID_CONNECTION_ATTEMPT_FAILED:
			pChatWindow->AddDebugMessage("The server didn't respond. Retrying..");
			SetGameState(GAMESTATE_WAIT_CONNECT);
			break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
			pChatWindow->AddDebugMessage("The server is full. Retrying...");
			SetGameState(GAMESTATE_WAIT_CONNECT);
			break;

			case ID_DISCONNECTION_NOTIFICATION:
			Packet_DisconnectionNotification(pkt);
			break;

			case ID_CONNECTION_LOST:
			Packet_ConnectionLost(pkt);
			break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
			Packet_ConnectionSucceeded(pkt);
			break;

			case ID_FAILED_INITIALIZE_ENCRIPTION:
			pChatWindow->AddDebugMessage("Failed to initialize encryption.");
			break;

			/*case ID_CONNECTION_BANNED:
			pChatWindow->AddDebugMessage("You are banned from this server.");
			break;*/

			case ID_INVALID_PASSWORD:
			pChatWindow->AddDebugMessage("Wrong server password.");
			m_pRakClient->Disconnect(0);
			break;

			case ID_PLAYER_SYNC:
			Packet_PlayerSync(pkt);
			break;

			case ID_VEHICLE_SYNC:
			Packet_VehicleSync(pkt);
			break;

			case ID_PASSENGER_SYNC:
			Packet_PassengerSync(pkt);
			break;

			case ID_MARKERS_SYNC:
			Packet_MarkersSync(pkt);
			break;

			case ID_AIM_SYNC:
			Packet_AimSync(pkt);
			break;

			case ID_BULLET_SYNC:
			Packet_BulletSync(pkt);
			break;
		}

		m_pRakClient->DeallocatePacket(pkt);
	}
}

void CNetGame::ResetVehiclePool()
{
	Log("ResetVehiclePool");
	if(m_pVehiclePool)
		delete m_pVehiclePool;

	m_pVehiclePool = new CVehiclePool();
}

void CNetGame::ResetObjectPool()
{
	Log("ResetObjectPool");
	if(m_pObjectPool)
		delete m_pObjectPool;

	m_pObjectPool = new CObjectPool();
}

void CNetGame::ResetPickupPool()
{
	Log("ResetPickupPool");
	if(m_pPickupPool)
		delete m_pPickupPool;

	m_pPickupPool = new CPickupPool();
}

void CNetGame::ResetGangZonePool()
{
	Log("ResetGangZonePool");
	if(m_pGangZonePool)
		delete m_pGangZonePool;

	m_pGangZonePool = new CGangZonePool();
}

void CNetGame::ResetLabelPool()
{
	Log("ResetLabelPool");
	if(m_pLabelPool)
		delete m_pLabelPool;

	m_pLabelPool = new CText3DLabelsPool();
}

void CNetGame::ShutDownForGameRestart()
{
	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer* pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
		{
			//pPlayer->SetTeam(NO_TEAM);
			//pPlayer->ResetAllSyncAttributes();
		}
	}

	CLocalPlayer *pLocalPlayer = m_pPlayerPool->GetLocalPlayer();
	if(pLocalPlayer)
	{
		pLocalPlayer->ResetAllSyncAttributes();
		pLocalPlayer->ToggleSpectating(false);
	}

	m_iGameState = GAMESTATE_RESTARTING;

	//m_pPlayerPool->DeactivateAll();
	m_pPlayerPool->Process();

	ResetVehiclePool();
	ResetObjectPool();
	ResetPickupPool();
	ResetGangZonePool();
	ResetLabelPool();
	//
	if(m_pTextDrawPool)
		delete m_pTextDrawPool;
	m_pTextDrawPool = new CTextDrawPool();
	//

	m_bDisableEnterExits = false;
	m_fNameTagDrawDistance = 60.0f;
	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather = 1;
	m_bHoldTime = true;
	m_bNameTagLOS = true;
	m_bUseCJWalk = false;
	m_fGravity = 0.008f;
	m_iDeathDropMoney = 0;

	if(pSpawnScreen) pSpawnScreen->Show(false);

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(pPlayerPed)
	{
		pPlayerPed->SetInterior(0);
		//pPlayerPed->SetDead();
		pPlayerPed->SetArmour(0.0f);
	}

	pGame->ToggleCJWalk(false);
	pGame->ResetLocalMoney();
	pGame->EnableZoneNames(false);
	m_bZoneNames = false;
	GameResetRadarColors();
	pGame->SetGravity(m_fGravity);
	pGame->EnableClock(false);
}

void CNetGame::SendChatMessage(const char* szMsg)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsSend;
	uint8_t byteTextLen = strlen(szMsg);

	bsSend.Write(byteTextLen);
	bsSend.Write(szMsg, byteTextLen);

	m_pRakClient->RPC(&RPC_Chat,&bsSend,HIGH_PRIORITY,RELIABLE,0,false, UNASSIGNED_NETWORK_ID, NULL);
}

void ApplyFPSPatch(uint8_t);

void CNetGame::SendChatCommand(const char* szCommand)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	if(strcmp(szCommand, "/q") == 0 || strcmp(szCommand, "/quit") == 0)
	{
		std::terminate();
		return;
	}
	else if(strcmp(szCommand, "/reconnect") == 0)
	{
		if(GetGameState() == GAMESTATE_CONNECTED)
		{
			ShutDownForGameRestart();
		}
		else
		{
			//pNetGame->ShutDownForGameRestart();
			SetGameState(GAMESTATE_WAIT_CONNECT);
		}
		return;
	}
	else if(strcmp(szCommand, "/label") == 0)
	{
		if(GetLabelPool())
		{
			GetLabelPool()->UpdateFlag();
		}
	}
	#ifdef BUILD_BETA
	else if(strcmp(szCommand, "/csa") == 0)
	{
		pCheatGUI->ToggleRender();
		return;
	}
	else if(strcmp(szCommand, "/console") == 0)
	{
		if(pCheatGUI->GetStatus()) pConsoleGUI->ToggleDraw();
		return;
	}
	/*else if(strcmp(szCommand, "/bonepos") == 0)
	{
		if(pGame)
		{
			VECTOR vecOut, vecRot;
			pGame->GetBonePosition(vecOut, BONE_HEAD, true);
			CObject *pObject = new CObject(2901, vecOut.X, vecOut.Y, vecOut.Z, vecRot, 300.0f);

			CLocalPlayer* pPlayer = m_pPlayerPool->GetLocalPlayer();

			MATRIX4X4 mat;
			VECTOR offs;
			pPlayer->GetPlayerPed()->GetMatrix(&mat);

			offs.X = FloatOffset(mat.pos.X, vecOut.X) + 2.0f;
			offs.Y = FloatOffset(mat.pos.Y, vecOut.Y);
			offs.Z = FloatOffset(mat.pos.Z, vecOut.Z);

			ScriptCommand( &attach_object_to_actor, pObject->m_dwGTAId, pPlayer->GetPlayerPed()->m_dwGTAId,
				offs.X,
				offs.Y,
				offs.Z,
				0.0f,
				0.0f,
				0.0f);
		}
		return;
	}*/
	else if(strcmp(szCommand, "/screen") == 0)
	{
		pChatWindow->AddDebugMessage("Screen Resolution: %dx%d", pGame->GetScreenWidth(), pGame->GetScreenHeight());
		return;
	}
	// doesn't work so you know
	else if(strcmp(szCommand, "/setres") == 0)
	{
		// OS_ScreenSetResolution
		// Set screen resolution to 1920x1080
		((void(*)(unsigned int, unsigned int))(g_libGTASA+0x23819C+1))(1920, 1080);
		pChatWindow->AddDebugMessage("Screen Resolution changed to 1920x1080");
		return;
	}
	else if(strcmp(szCommand, "/memstat") == 0)
	{
		int available_memory, total_memory;

		// don't worry about passing zeros this function have checks o_o (i hope)
		((int(*)(int*,int*,int*))(g_libGTASA+0x243868+1))(&available_memory, &total_memory, 0);

		// this thing shows 252
		// what is this? 252 bits? bytes? megabytes? wtf?
		pChatWindow->AddDebugMessage("Available memory: %d | Total memory: %d", available_memory, total_memory);
	}
	else if(strcmp(szCommand, "/deviceinfo") == 0)
	{
		// I guess this thing shows do you have touch screen or not
		int touchscreen_status = ((int(*)(int))(g_libGTASA+0x243204+1))(1);
		pChatWindow->AddDebugMessage("Touch screen status: %d", touchscreen_status);
	}
	else if(strcmp(szCommand, "/deviceregion") == 0)
	{
		// useless stuff currently.
	}
	else if(strcmp(szCommand, "/button") == 0)
	{
		pButton->Toggle();
	}
	else if(strcmp(szCommand, "/siren") == 0)
	{
		CLocalPlayer *pLocalPlayer = m_pPlayerPool->GetLocalPlayer();
		if(IN_VEHICLE(pLocalPlayer->GetPlayerPed()->m_pPed))
		{
			int vehid = m_pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)pLocalPlayer->GetPlayerPed()->m_pPed->pVehicle);
			if(m_pVehiclePool->GetSlotState(vehid))
			{
				CVehicle *veh = m_pVehiclePool->GetAt(vehid);
				pChatWindow->AddDebugMessage("Siren or Alarm state: %d", veh->GetSirenState());
			}
		}
	}
	else if(strcmp(szCommand, "/headmove") == 0)
	{
		m_bHeadMove = !m_bHeadMove;
		pChatWindow->AddInfoMessage(m_bHeadMove ? "-> Head movements enabled" : "-> Head movements disabled");
	}
	#endif

	RakNet::BitStream bsParams;
	int iStrlen = strlen(szCommand);

	bsParams.Write(iStrlen);
	bsParams.Write(szCommand, iStrlen);
	m_pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SendDialogResponse(uint16_t wDialogID, uint8_t byteButtonID, uint16_t wListBoxItem, char* szInput)
{
	uint8_t respLen = strlen(szInput);

	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(byteButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInput, respLen);
	m_pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style)
{
	if(byteIndex >= 100) return;
	if(m_dwMapIcons[byteIndex]) DisableMapIcon(byteIndex);

	m_dwMapIcons[byteIndex] = pGame->CreateRadarMarkerIcon(byteIcon, fX, fY, fZ, iColor, style);
}

void CNetGame::DisableMapIcon(uint8_t byteIndex)
{
	if(byteIndex >= 100) return;
	ScriptCommand(&disable_marker, m_dwMapIcons[byteIndex]);
	m_dwMapIcons[byteIndex] = 0;
}

void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::Packet_AuthKey(Packet* pkt)
{
	RakNet::BitStream bsAuth((unsigned char *)pkt->data, pkt->length, false);

	uint8_t byteAuthLen;
	char szAuth[260];

	bsAuth.IgnoreBits(8);
	bsAuth.Read(byteAuthLen);
	bsAuth.Read(szAuth, byteAuthLen);
	szAuth[byteAuthLen] = '\0';

	char szAuthKey[260];
	gen_auth_key(szAuthKey, szAuth);

	RakNet::BitStream bsKey;
	uint8_t byteAuthKeyLen = (uint8_t)strlen(szAuthKey);

	bsKey.Write((uint8_t)ID_AUTH_KEY);
	bsKey.Write((uint8_t)byteAuthKeyLen);
	bsKey.Write(szAuthKey, byteAuthKeyLen);
	m_pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);

	Log("[AUTH] %s -> %s", szAuth, szAuthKey);
}

void CNetGame::Packet_DisconnectionNotification(Packet* pkt)
{
	if(pChatWindow)
		pChatWindow->AddDebugMessage("Server closed the connection.");
	m_pRakClient->Disconnect(2000);
}

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
	if(m_pRakClient) m_pRakClient->Disconnect(0);

	if(pChatWindow)
		pChatWindow->AddDebugMessage("Lost connection to the server. Reconnecting..");

	ShutDownForGameRestart();

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer *pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer) m_pPlayerPool->Delete(playerId, 0);
	}

	SetGameState(GAMESTATE_WAIT_CONNECT);
}

void BIG_NUM_MUL(unsigned long in[5], unsigned long out[6], unsigned long factor)
{
	/*
		Based on TTMath library by Tomasz Sowa.
	*/

	unsigned long src[5] = {0};
	for (int i = 0; i < 5; i++)
		src[i] = ((in[4-i]>>24) | ((in[4-i]<<8) & 0x00FF0000) | ((in[4-i]>>8) & 0x0000FF00) | (in[4-i]<<24));

	unsigned long long tmp = 0;

	tmp = (unsigned long long)(src[0])*(unsigned long long)(factor);
	out[0] = tmp&0xFFFFFFFF;
	out[1] = tmp>>32;
	tmp = (unsigned long long)(src[1])*(unsigned long long)(factor) + (unsigned long long)(out[1]);
	out[1] = tmp&0xFFFFFFFF;
	out[2] = tmp>>32;
	tmp = (unsigned long long)(src[2])*(unsigned long long)(factor) + (unsigned long long)(out[2]);
	out[2] = tmp&0xFFFFFFFF;
	out[3] = tmp>>32;
	tmp = (unsigned long long)(src[3])*(unsigned long long)(factor) + (unsigned long long)(out[3]);
	out[3] = tmp&0xFFFFFFFF;
	out[4] = tmp>>32;
	tmp = (unsigned long long)(src[4])*(unsigned long long)(factor) + (unsigned long long)(out[4]);
	out[4] = tmp&0xFFFFFFFF;
	out[5] = tmp>>32;

	for (int i = 0; i < 12; i++)
	{
		unsigned char temp = ((unsigned char*)out)[i];
		((unsigned char*)out)[i] = ((unsigned char*)out)[23 - i];
		((unsigned char*)out)[23 - i] = temp;
	}
}

int gen_gpci(char buf[64], unsigned long factor) /* by bartekdvd */
{
	unsigned char out[6*4] = {0};

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < 6*4; ++i)
		out[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	out[6*4] = 0;

	BIG_NUM_MUL((unsigned long*)out, (unsigned long*)out, factor);

	unsigned int notzero = 0;
	buf[0] = '0'; buf[1] = '\0';

	if (factor == 0) return 1;

	int pos = 0;
	for (int i = 0; i < 24; i++)
	{
		unsigned char tmp = out[i] >> 4;
		unsigned char tmp2 = out[i]&0x0F;

		if (notzero || tmp)
		{
			buf[pos++] = (char)((tmp > 9)?(tmp + 55):(tmp + 48));
			if (!notzero) notzero = 1;
		}

		if (notzero || tmp2)
		{
			buf[pos++] = (char)((tmp2 > 9)?(tmp2 + 55):(tmp2 + 48));
			if (!notzero) notzero = 1;
		}
	}
	buf[pos] = 0;

	return pos;
}

void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
	if(pChatWindow)
		pChatWindow->AddDebugMessage("Connected. Joining the game...");
	SetGameState(GAMESTATE_AWAIT_JOIN);

	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	PLAYERID MyPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port
	bsSuccAuth.Read(MyPlayerID);
	bsSuccAuth.Read(uiChallenge);

	m_pPlayerPool->SetLocalPlayerID(MyPlayerID);

	int iVersion = NETGAME_VERSION;
	char byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;


	char newGenKey[64];
	gen_gpci(newGenKey, 0x3e9);
	Log("new key: %s", newGenKey);

	char byteAuthBSLen;
	byteAuthBSLen = (char)strlen(encAuthKey.decrypt());

	char byteNameLen = (char)strlen(m_pPlayerPool->GetLocalPlayerName());
	char byteClientverLen = (char)strlen(SAMP_VERSION);

	RakNet::BitStream bsSend;
	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(m_pPlayerPool->GetLocalPlayerName(), byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(encAuthKey.decrypt(), byteAuthBSLen);
	bsSend.Write(byteClientverLen);
	bsSend.Write(SAMP_VERSION, byteClientverLen);
	m_pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::Packet_PlayerSync(Packet* pkt)
{
	CRemotePlayer * pPlayer;
	RakNet::BitStream bsPlayerSync((unsigned char *)pkt->data, pkt->length, false);
	ONFOOT_SYNC_DATA ofSync;
	uint8_t bytePacketID=0;
	PLAYERID playerId;

	bool bHasLR,bHasUD;
	bool bHasVehicleSurfingInfo;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

	bsPlayerSync.Read(bytePacketID);
	bsPlayerSync.Read(playerId);

	// LEFT/RIGHT KEYS
	bsPlayerSync.Read(bHasLR);
	if(bHasLR) bsPlayerSync.Read(ofSync.lrAnalog);

	// UP/DOWN KEYS
	bsPlayerSync.Read(bHasUD);
	if(bHasUD) bsPlayerSync.Read(ofSync.udAnalog);

	// GENERAL KEYS
	bsPlayerSync.Read(ofSync.wKeys);

	// VECTOR POS
	bsPlayerSync.Read((char*)&ofSync.vecPos,sizeof(VECTOR));

	// QUATERNION
	float tw, tx, ty, tz;
	bsPlayerSync.ReadNormQuat(tw, tx, ty, tz);
	ofSync.quat.w = tw;
	ofSync.quat.x = tx;
	ofSync.quat.y = ty;
	ofSync.quat.z = tz;

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp=0,byteHlTemp=0;

	bsPlayerSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) ofSync.byteArmour = 100;
	else if(byteArmTemp == 0) ofSync.byteArmour = 0;
	else ofSync.byteArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) ofSync.byteHealth = 100;
	else if(byteHlTemp == 0) ofSync.byteHealth = 0;
	else ofSync.byteHealth = byteHlTemp * 7;

	// CURRENT WEAPON
    bsPlayerSync.Read(ofSync.byteCurrentWeapon);
    // SPECIAL ACTION
    bsPlayerSync.Read(ofSync.byteSpecialAction);

    // READ MOVESPEED VECTORS
    bsPlayerSync.ReadVector(tx, ty, tz);
    ofSync.vecMoveSpeed.X = tx;
    ofSync.vecMoveSpeed.Y = ty;
    ofSync.vecMoveSpeed.Z = tz;

    bsPlayerSync.Read(bHasVehicleSurfingInfo);
    if (bHasVehicleSurfingInfo)
    {
        bsPlayerSync.Read(ofSync.wSurfInfo);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.X);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.Y);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.Z);
    }
    else
    	ofSync.wSurfInfo = INVALID_VEHICLE_ID;

    if(m_pPlayerPool)
    {
    	pPlayer = m_pPlayerPool->GetAt(playerId);
    	if(pPlayer)
    		pPlayer->StoreOnFootFullSyncData(&ofSync, 0);
    }
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsSync((unsigned char *)pkt->data, pkt->length, false);
	uint8_t bytePacketID = 0;
	PLAYERID playerId;
	INCAR_SYNC_DATA icSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	bsSync.Read(bytePacketID);
	bsSync.Read(playerId);
	bsSync.Read(icSync.VehicleID);

	// keys
	bsSync.Read(icSync.lrAnalog);
	bsSync.Read(icSync.udAnalog);
	bsSync.Read(icSync.wKeys);

	// quaternion
	bsSync.ReadNormQuat(
		icSync.quat.w,
		icSync.quat.x,
		icSync.quat.y,
		icSync.quat.z);

	// position
	bsSync.Read((char*)&icSync.vecPos, sizeof(VECTOR));

	// speed
	bsSync.ReadVector(
		icSync.vecMoveSpeed.X,
		icSync.vecMoveSpeed.Y,
		icSync.vecMoveSpeed.Z);

	// vehicle health
	uint16_t wTempVehicleHealth;
	bsSync.Read(wTempVehicleHealth);
	icSync.fCarHealth = (float)wTempVehicleHealth;

	// health/armour
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp=0, byteHlTemp=0;

	bsSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) icSync.bytePlayerArmour = 100;
	else if(byteArmTemp == 0) icSync.bytePlayerArmour = 0;
	else icSync.bytePlayerArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) icSync.bytePlayerHealth = 100;
	else if(byteHlTemp == 0) icSync.bytePlayerHealth = 0;
	else icSync.bytePlayerHealth = byteHlTemp * 7;

	// CURRENT WEAPON
	uint8_t byteTempWeapon;
	bsSync.Read(byteTempWeapon);
	icSync.byteCurrentWeapon ^= (byteTempWeapon ^ icSync.byteCurrentWeapon) & 0x3F;

	bool bCheck;

	// siren
	bsSync.Read(bCheck);
	if(bCheck) icSync.byteSirenOn = 1;
	// landinggear
	bsSync.Read(bCheck);
	if(bCheck) icSync.byteLandingGearState = 1;
	// train speed
	bsSync.Read(bCheck);
	if(bCheck) bsSync.Read(icSync.fTrainSpeed);
	// triler id
	bsSync.Read(bCheck);
	if(bCheck) bsSync.Read(icSync.TrailerID);

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
		{
			pPlayer->StoreInCarFullSyncData(&icSync, 0);
		}
	}
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
	CRemotePlayer *pPlayer;
	uint8_t bytePacketID;
	PLAYERID playerId;
	PASSENGER_SYNC_DATA psSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsPassengerSync((unsigned char *)pkt->data, pkt->length, false);
	bsPassengerSync.Read(bytePacketID);
	bsPassengerSync.Read(playerId);
	bsPassengerSync.Read((char*)&psSync, sizeof(PASSENGER_SYNC_DATA));

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
			pPlayer->StorePassengerFullSyncData(&psSync);
	}
}

void CNetGame::Packet_MarkersSync(Packet *pkt)
{
	CRemotePlayer *pPlayer;
	int			iNumberOfPlayers = 0;
	PLAYERID	playerId;
	short		sPos[3];
	bool		bIsPlayerActive;
	uint8_t 	unk0 = 0;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsMarkersSync((unsigned char *)pkt->data, pkt->length, false);
	bsMarkersSync.Read(unk0);
	bsMarkersSync.Read(iNumberOfPlayers);

	if(iNumberOfPlayers)
	{
		for(int i=0; i<iNumberOfPlayers; i++)
		{
			bsMarkersSync.Read(playerId);
			bsMarkersSync.ReadCompressed(bIsPlayerActive);

			if(bIsPlayerActive)
			{
				bsMarkersSync.Read(sPos[0]);
				bsMarkersSync.Read(sPos[1]);
				bsMarkersSync.Read(sPos[2]);
			}

			if(playerId < MAX_PLAYERS && m_pPlayerPool->GetSlotState(playerId))
			{
				pPlayer = m_pPlayerPool->GetAt(playerId);
				if(pPlayer)
				{
					if(bIsPlayerActive)
						pPlayer->ShowGlobalMarker(sPos[0], sPos[1], sPos[2]);
					else
						pPlayer->HideGlobalMarker();
				}
			}
		}
	}
}

void CNetGame::Packet_BulletSync(Packet* p)
{
	CRemotePlayer* pPlayer;
	RakNet::BitStream bsBulletSync((unsigned char*)p->data, p->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED) return;
	if(pCheats->IsGodModeEnabled()) return;

	uint8_t bytePacketID = 0;
	PLAYERID bytePlayerID;
	BULLET_SYNC blSync;

	bsBulletSync.Read(bytePacketID);
	bsBulletSync.Read(bytePlayerID);
	bsBulletSync.Read((char*)& blSync, sizeof(BULLET_SYNC));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer && m_pPlayerPool->GetLocalPlayerID() != bytePlayerID)
		pPlayer->StoreBulletSyncData(&blSync);
}

void CNetGame::Packet_AimSync(Packet * p)
{
	CRemotePlayer * pPlayer;
	RakNet::BitStream bsAimSync((unsigned char*)p->data, p->length, false);
	AIM_SYNC_DATA aimSync;
	uint8_t bytePacketID = 0;
	uint16_t bytePlayerID = 0;

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	bsAimSync.Read(bytePacketID);
	bsAimSync.Read(bytePlayerID);
	bsAimSync.Read((char*)&aimSync, sizeof(AIM_SYNC_DATA));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)  {
		pPlayer->UpdateAimFromSyncData(&aimSync);
	}
}

void CNetGame::UpdatePlayerScoresAndPings()
{
	static uint32_t dwLastUpdateTick = 0;

	if((GetTickCount() - dwLastUpdateTick) >= 3000){
		dwLastUpdateTick = GetTickCount();
		RakNet::BitStream bsParams;
		m_pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY,RELIABLE,0,false, UNASSIGNED_NETWORK_ID, NULL);
	}
}

bool CNetGame::GetHeadMoveState()
{
	return m_bHeadMove;
}
