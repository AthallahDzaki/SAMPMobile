#include "../main.h"
#include "../util/armhook.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../chatwindow.h"
#include "../scoreboard.h"
#include "world.h"

extern CScoreBoard* pScoreBoard;
extern CGame* pGame;
extern CNetGame* pNetGame;
extern CGUI* pGUI;
extern CChatWindow* pChatWindow;
extern CWorld* pWorld;
extern bool g_bMainMenuStage;
// Neiae/SAMP
bool g_bPlaySAMP = false;
int g_iSelectedMenuItem = 1;
extern bool bGameInited;

void InitSAMP();
void InitInMenu();
void MainLoop();
void HookCPad();

void InitInGame();
void ProcessSAMPGraphic();
void ProcessSAMPGraphicFrame();
void ProcessSAMPNetwork();

uint32_t hookDelay;
std::vector<ENTITY_TYPE*> AddedEntities;

/* ================ ie?oee aey ani. anoaaie =================== */

extern "C" uintptr_t get_lib()
{
	return g_libGTASA;
}

/* ====================================================== */

struct stFile
{
	int isFileExist;
	FILE* f;
};

stFile* (*NvFOpen)(const char*, const char*, int, int);
stFile* NvFOpen_hook(const char* r0, const char* r1, int r2, int r3)
{
	char path[0xFF] = { 0 };
	// ----------------------------
	if (!strncmp(r1 + 12, "mainV1.scm", 10))
	{
		sprintf(path, "%sSAMP/main.scm", g_pszStorage);
		Log("Loading mainV1.scm..");
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1 + 12, "SCRIPTV1.IMG", 12))
	{
		sprintf(path, "%sSAMP/script.img", g_pszStorage);
		Log("Loading script.img..");
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1, "DATA/PEDS.IDE", 13))
	{
		sprintf(path, "%s/SAMP/peds.ide", g_pszStorage);
		Log("Loading peds.ide..");
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1, "DATA/VEHICLES.IDE", 17))
	{
		sprintf(path, "%s/SAMP/vehicles.ide", g_pszStorage);
		Log("Loading vehicles.ide..");
		goto open;
	}
	if (!strncmp(r1, "DATA/GTA.DAT", 12))
	{
		sprintf(path, "%s/SAMP/gta.dat", g_pszStorage);
		goto open;
	}
orig:
	return NvFOpen(r0, r1, r2, r3);

open:
	stFile* st = (stFile*)malloc(8);
	st->isFileExist = false;

	FILE* f = fopen(path, "rb");
	if (f)
	{
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		Log("NVFOpen hook | Error: file not found (%s)", path);
		free(st);
		st = nullptr;
		return 0;
	}
}

/* ====================================================== */

// Aucuaaaony ec Java
int Init_hook(int r0, int r1, int r2)
{
	int result = ((int (*)(int, int, int))(g_libGTASA + 0x244F2C + 1))(r0, r1, r2);

	InitSAMP();

	return result;
}

/* ====================================================== */

bool bGameStarted = false;
void (*Render2dStuff)();
void Render2dStuff_hook()
{
	Render2dStuff();
	if (g_bPlaySAMP)
	{
		InitInGame();
	}

	/*bGameStarted = true;
	MainLoop();
	Render2dStuff();*/
	return;
}

/* ====================================================== */

void (*Render2dStuffAfterFade)();
void Render2dStuffAfterFade_hook()
{
	// CHud::DrawAfterFade(void) — 0x003D7258
	((void (*)(void))(g_libGTASA + 0x003D7258 + 1)) ();

	// TemporaryFPSVisualization(void) — 0x0039B054
	((void (*)(void))(g_libGTASA + 0x0039B054 + 1)) ();

	if (g_bPlaySAMP)
	{
		ProcessSAMPGraphic();
	}

	// emu_GammaSet(uchar) — 0x00198010
	((void (*)(uint8_t))(g_libGTASA + 0x00198010 + 1)) (1);

	// CMessages::Display(uchar) — 0x004D240C
	((void (*)(uint8_t))(g_libGTASA + 0x004D240C + 1)) (0);

	// CFont::RenderFontBuffer(void) — 0x0053411C
	((void (*)(void))(g_libGTASA + 0x0053411C + 1)) ();

	// CCredits::Render(void) — 0x003FC43C
	((void (*)(void))(g_libGTASA + 0x003FC43C + 1)) ();

	// emu_GammaSet(uchar) — 0x00198010
	((void (*)(uint8_t))(g_libGTASA + 0x00198010 + 1)) (0);

	if (g_bPlaySAMP)
	{
		ProcessSAMPGraphicFrame();
	}

	/*Render2dStuffAfterFade();
	if (pGUI && bGameStarted) pGUI->Render();*/
	return;
}

int32_t(*CRunningScript__Process) (uint32_t * _this);
int32_t CRunningScript__Process_hook(uint32_t * _this)
{
	if (!bGameInited)
	{
		return CRunningScript__Process(_this);
	}

	//InitNetwork();
	ProcessSAMPNetwork();
	return CRunningScript__Process(_this);
}

/* ====================================================== */

uint16_t gxt_string[0x7F];
uint16_t* (*CText_Get)(uintptr_t thiz, const char* text);
uint16_t* CText_Get_hook(uintptr_t thiz, const char* text)
{
	if (text[0] == 'S' && text[1] == 'A' && text[2] == 'M' && text[3] == 'P')
	{
		const char* code = &text[5];
		if (!strcmp(code, "MP")) CFont::AsciiToGxtChar("Flin RP One", gxt_string);
		if (!strcmp(code, "MP_2")) CFont::AsciiToGxtChar("Flin RP Two", gxt_string);

		#ifdef BUILD_BETA
		if (!strcmp(code, "MP_3")) CFont::AsciiToGxtChar("Flin RP Test", gxt_string);
		#endif

		return gxt_string;
	}

	return CText_Get(thiz, text);
}

/* ====================================================== */

void MainMenu_OnStartSAMP(int selectedMenuItem = 1)
{
	// если уже всё готово, то не продолжаем выполнение
	if (g_bPlaySAMP) return;
	// инициализируем меню и основные компоненты интерфейса
	InitInMenu();
	// выбранный пункт меню (на основе выбора определяется порт для подключения)
	g_iSelectedMenuItem = selectedMenuItem;
	// StartGameScreen::OnNewGameCheck()
	((void (*)())(g_libGTASA + 0x261C8C + 1))();
	// всё готово для запуска (подключения к серверу)
	g_bPlaySAMP = true;
	return;
}

// в меню была выбрана 1-ая кнопка для подключения
void MainMenu_OnStartSAMP_1()
{
	MainMenu_OnStartSAMP(1);
}

// в меню была выбрана 2-ая кнопка для подключения
void MainMenu_OnStartSAMP_2()
{
	MainMenu_OnStartSAMP(3);
}

#ifdef BUILD_BETA
// в меню была выбрана 3-я кнопка для подключения
void MainMenu_OnStartSAMP_3()
{
	MainMenu_OnStartSAMP(3);
}
#endif

// OsArray<FlowScreen::MenuItem>::Add
void (*MenuItem_add)(int r0, uintptr_t r1);
void MenuItem_add_hook(int r0, uintptr_t r1)
{
	static bool bMenuInited = false;
	char* name = *(char**)(r1 + 4);

	if (!strcmp(name, cryptor::create("FEP_STG", 7).decrypt()) && !bMenuInited)
	{
		Log(cryptor::create("Creating \"MultiPlayer\" button.. (struct: 0x%X)", 48).decrypt(), r1);

		*(char**)(r1 + 4) = "SAMP_MP";
		*(uintptr_t*)r1 = LoadTextureFromDB(cryptor::create("samp", 4).decrypt(), cryptor::create("menu_mainmp", 11).decrypt());
		*(uintptr_t*)(r1 + 8) = (uintptr_t)MainMenu_OnStartSAMP_1;

		MenuItem_add(r0, r1);
		*(char**)(r1 + 4) = "SAMP_MP_2";
		*(uintptr_t*)r1 = LoadTextureFromDB(cryptor::create("samp", 4).decrypt(), cryptor::create("menu_mainmp", 11).decrypt());
		*(uintptr_t*)(r1 + 8) = (uintptr_t)MainMenu_OnStartSAMP_2;

		#ifdef BUILD_BETA
		MenuItem_add(r0, r1);
		*(char**)(r1 + 4) = "SAMP_MP_3";
		*(uintptr_t*)r1 = LoadTextureFromDB(cryptor::create("samp", 4).decrypt(), cryptor::create("menu_mainmp", 11).decrypt());
		*(uintptr_t*)(r1 + 8) = (uintptr_t)MainMenu_OnStartSAMP_3;
		#endif

		bMenuInited = true;
		goto ret;
	}

	// Eaii?e?oai nicaaiea "Start Game" e "Stats" ec iai? iaocu
	if (g_bPlaySAMP && (
		!strcmp(name, cryptor::create("FEP_STG", 7).decrypt()) ||
		!strcmp(name, cryptor::create("FEH_STA", 7).decrypt()) ||
		!strcmp(name, cryptor::create("FEH_BRI", 7).decrypt())))
		return;

ret:
	return MenuItem_add(r0, r1);
}

/* ====================================================== */

// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook()
{
	Log("Loading \"samp\" cd..");

	InitialiseRenderWare();
	// TextureDatabaseRuntime::Load()
	((void (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))("samp", 0, 5);
	return;
}

/* ====================================================== */

void RenderSplashScreen();
void (*CLoadingScreen_DisplayPCScreen)();
void CLoadingScreen_DisplayPCScreen_hook()
{
	RwCamera* camera = *(RwCamera**)(g_libGTASA + 0x95B064);

	if (RwCameraBeginUpdate(camera))
	{
		DefinedState2d();
		((void (*)())(g_libGTASA + 0x5519C8 + 1))(); // CSprite2d::InitPerFrame()
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
		((void (*)(bool))(g_libGTASA + 0x198010 + 1))(false); // emu_GammaSet()
		RenderSplashScreen();
		RwCameraEndUpdate(camera);
		RwCameraShowRaster(camera, 0, 0);
	}

	return;
}

/* ====================================================== */

void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	bool bRet = pGUI->OnTouchEvent(type, num, posX, posY);

	if (bRet)
		return TouchEvent(type, num, posX, posY);
}

/* ====================================================== */

void (*CStreaming_InitImageList)();
void CStreaming_InitImageList_hook()
{
	char* ms_files = (char*)(g_libGTASA + 0x6702FC);
	ms_files[0] = 0;
	*(uint32_t*)&ms_files[44] = 0;
	ms_files[48] = 0;
	*(uint32_t*)&ms_files[92] = 0;
	ms_files[96] = 0;
	*(uint32_t*)&ms_files[140] = 0;
	ms_files[144] = 0;
	*(uint32_t*)&ms_files[188] = 0;
	ms_files[192] = 0;
	*(uint32_t*)&ms_files[236] = 0;
	ms_files[240] = 0;
	*(uint32_t*)&ms_files[284] = 0;
	ms_files[288] = 0;
	*(uint32_t*)&ms_files[332] = 0;
	ms_files[336] = 0;
	*(uint32_t*)&ms_files[380] = 0;

	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\GTA3.IMG", 1); // CStreaming::AddImageToList
	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\GTA_INT.IMG", 1); // CStreaming::AddImageToList
	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\SAMP.IMG", 1); // CStreaming::AddImageToList
	((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\SAMPCOL.IMG", 1); // CStreaming::AddImageToList
}

/* ====================================================== */
typedef struct _PED_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[88];
} PED_MODEL; // SIZE = 92

PED_MODEL PedsModels[315];
int PedsModelsCount = 0;

PED_MODEL* (*CModelInfo_AddPedModel)(int id);
PED_MODEL* CModelInfo_AddPedModel_hook(int id)
{
	//Log("loading skin model %d", id);

	PED_MODEL* model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL));
	model->vtable = (uintptr_t)(g_libGTASA + 0x5C6E90);

	// CClumpModelInfo::CClumpModelInit()
	((uintptr_t(*)(PED_MODEL*))(*(void**)(model->vtable + 0x1C)))(model);

	*(PED_MODEL**)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	PedsModelsCount++;
	return model;
}
/* ====================================================== */
void (*CPools_Initialise)(void);
void CPools_Initialise_hook(void)
{
	struct PoolAllocator {

		struct Pool {
			void* objects;
			uint8_t* flags;
			uint32_t count;
			uint32_t top;
			uint32_t bInitialized;
		};
		static_assert(sizeof(Pool) == 0x14);

		static Pool* Allocate(size_t count, size_t size) {

			Pool* p = new Pool;

			p->objects = new char[size * count];
			p->flags = new uint8_t[count];
			p->count = count;
			p->top = 0xFFFFFFFF;
			p->bInitialized = 1;

			for (size_t i = 0; i < count; i++) {
				p->flags[i] |= 0x80;
				p->flags[i] &= 0x80;
			}

			return p;
		}
	};



	// 600000 / 75000 = 8
	static auto ms_pPtrNodeSingleLinkPool = PoolAllocator::Allocate(100000, 8);		// 75000
	// 72000 / 6000 = 12
	static auto ms_pPtrNodeDoubleLinkPool = PoolAllocator::Allocate(60000, 12);	// 6000
	// 10000 / 500 = 20
	static auto ms_pEntryInfoNodePool = PoolAllocator::Allocate(20000, 20);	// 500
	// 279440 / 140 = 1996
	static auto ms_pPedPool = PoolAllocator::Allocate(240, 1996);	// 140
	// 286440 / 110 = 2604
	static auto ms_pVehiclePool = PoolAllocator::Allocate(2000, 2604);	// 110
	// 840000 / 14000 = 60
	static auto ms_pBuildingPool = PoolAllocator::Allocate(20000, 60);	// 14000
	// 147000 / 350 = 420
	static auto ms_pObjectPool = PoolAllocator::Allocate(3000, 420);	// 350
	// 210000 / 3500 = 60
	static auto ms_pDummyPool = PoolAllocator::Allocate(40000, 60);	// 3500
	// 487200 / 10150 = 48
	static auto ms_pColModelPool = PoolAllocator::Allocate(50000, 48);	// 10150
	// 64000 / 500 = 128
	static auto ms_pTaskPool = PoolAllocator::Allocate(5000, 128);	// 500
	// 13600 / 200 = 68
	static auto ms_pEventPool = PoolAllocator::Allocate(1000, 68);	// 200
	// 6400 / 64 = 100
	static auto ms_pPointRoutePool = PoolAllocator::Allocate(200, 100);	// 64
	// 13440 / 32 = 420
	static auto ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
	// 2304 / 64 = 36
	static auto ms_pNodeRoutePool = PoolAllocator::Allocate(200, 36);	// 64
	// 512 / 16 = 32
	static auto ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
	// 92960 / 140 = 664
	static auto ms_pPedIntelligencePool = PoolAllocator::Allocate(240, 664);	// 140
	// 15104 / 64 = 236
	static auto ms_pPedAttractorPool = PoolAllocator::Allocate(200, 236);	// 64

	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93E0) = ms_pPtrNodeSingleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93DC) = ms_pPtrNodeDoubleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D8) = ms_pEntryInfoNodePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D4) = ms_pPedPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D0) = ms_pVehiclePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93CC) = ms_pBuildingPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C8) = ms_pObjectPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C4) = ms_pDummyPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C0) = ms_pColModelPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93BC) = ms_pTaskPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B8) = ms_pEventPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B4) = ms_pPointRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B0) = ms_pPatrolRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93AC) = ms_pNodeRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A8) = ms_pTaskAllocatorPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A4) = ms_pPedIntelligencePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A0) = ms_pPedAttractorPool;
}

void (*CWorld__ProcessPedsAfterPreRender)();
void CWorld__ProcessPedsAfterPreRender_hook()
{
	CWorld__ProcessPedsAfterPreRender();
	//if(pWorld) pWorld->ProcessRemovedEntities();

	if (pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();

			if (pLocalPlayer)
			{
				CPlayerPed* pPlayerPed = pLocalPlayer->GetPlayerPed();
				if (pPlayerPed) {
					pPlayerPed->ProcessAttachedObjects();
				}
			}

			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
				if (pRemotePlayer)
				{
					CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
					if (pPlayerPed) {
						pPlayerPed->ProcessAttachedObjects();
					}
				}
			}
		}
	}
}

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
extern struct _ATOMIC_MODEL* ATOMIC_MODELS;

uintptr_t(*CModelInfo_AddAtomicModel)(int iModel);
uintptr_t CModelInfo_AddAtomicModel_hook(int iModel)
{
	uint32_t iCount = *(uint32_t*)(g_libGTASA + 0x7802C4);
	_ATOMIC_MODEL* model = &ATOMIC_MODELS[iCount];
	*(uint32_t*)(g_libGTASA + 0x7802C4) = iCount + 1;

	((void(*)(_ATOMIC_MODEL*))(*(uintptr_t*)(model->func_tbl + 0x1C)))(model);
	_ATOMIC_MODEL** ms_modelInfoPtrs = (_ATOMIC_MODEL**)(g_libGTASA + 0x87BF48);
	ms_modelInfoPtrs[iModel] = model;
	return (uintptr_t)model;
}
/* ==== */

uint32_t(*CRadar__GetRadarTraceColor)(uint32_t color, uint8_t bright, uint8_t friendly);
uint32_t CRadar__GetRadarTraceColor_hook(uint32_t color, uint8_t bright, uint8_t friendly)
{
	return TranslateColorCodeToRGBA(color);
}

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char* name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char* name)
{
	if (pNetGame && !strncmp(name, "CODEWAY", 7))
	{
		float findZ = ((float (*)(float, float))(g_libGTASA + 0x3C3DD8 + 1))(X, Y);
		findZ += 1.5f;

		Log("OnPlayerClickMap: %f, %f, %f", X, Y, Z);
		RakNet::BitStream bsSend;
		bsSend.Write(X);
		bsSend.Write(Y);
		bsSend.Write(findZ);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

uint8_t bGZ = 0;
void (*CRadar__DrawRadarGangOverlay)(uint8_t v1);
void CRadar__DrawRadarGangOverlay_hook(uint8_t v1)
{
	bGZ = v1;
	if (pNetGame && pNetGame->GetGangZonePool())
		pNetGame->GetGangZonePool()->Draw();
}

uint32_t dwParam1, dwParam2;
extern "C" void pickup_ololo()
{
	if (pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool* pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp(((dwParam1 - (g_libGTASA + 0x70E264)) / 0x20));
	}
}

__attribute__((naked)) void PickupPickUp_hook()
{
	//LOGI("PickupPickUp_hook");

	// calculate and save ret address
	__asm__ volatile("push {lr}\n\t"
		"push {r0}\n\t"
		"blx get_lib\n\t"
		"add r0, #0x2D0000\n\t"
		"add r0, #0x009A00\n\t"
		"add r0, #1\n\t"
		"mov r1, r0\n\t"
		"pop {r0}\n\t"
		"pop {lr}\n\t"
		"push {r1}\n\t");

	//
	__asm__ volatile("push {r0-r11, lr}\n\t"
		"mov %0, r4" : "=r" (dwParam1));

	__asm__ volatile("blx pickup_ololo\n\t");


	__asm__ volatile("pop {r0-r11, lr}\n\t");

	// restore
	__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
		"sub.w r2, r1, #0xD\n\t"
		"sub.w r2, r1, #8\n\t"
		"cmp r1, #6\n\t"
		"pop {pc}\n\t");
}

extern "C" bool NotifyEnterVehicle(VEHICLE_TYPE * _pVehicle)
{
	//Log("NotifyEnterVehicle");

	if (!pNetGame) return false;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle;
	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);

	if (VehicleID == INVALID_VEHICLE_ID) return false;
	if (!pVehiclePool->GetSlotState(VehicleID)) return false;
	pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle->m_bDoorsLocked) return false;
	if (pVehicle->m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER) return false;

	if (pVehicle->m_pVehicle->pDriver &&
		pVehicle->m_pVehicle->pDriver->dwPedType != 0)
		return false;

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	//if(pLocalPlayer->GetPlayerPed() && pLocalPlayer->GetPlayerPed()->GetCurrentWeapon() == WEAPON_PARACHUTE)
	//  pLocalPlayer->GetPlayerPed()->SetArmedWeapon(0);

	pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);

	return true;
}

void (*CTaskComplexEnterCarAsDriver)(uint32_t thiz, uint32_t pVehicle);
extern "C" void call_taskEnterCarAsDriver(uintptr_t a, uint32_t b)
{
	CTaskComplexEnterCarAsDriver(a, b);
}
void __attribute__((naked)) CTaskComplexEnterCarAsDriver_hook(uint32_t thiz, uint32_t pVehicle)
{
	__asm__ volatile("push {r0-r11, lr}\n\t"
		"mov r2, lr\n\t"
		"blx get_lib\n\t"
		"add r0, #0x3A0000\n\t"
		"add r0, #0xEE00\n\t"
		"add r0, #0xF7\n\t"
		"cmp r2, r0\n\t"
		"bne 1f\n\t" // !=
		"mov r0, r1\n\t"
		"blx NotifyEnterVehicle\n\t" // call NotifyEnterVehicle
		"1:\n\t"  // call orig
		"pop {r0-r11, lr}\n\t"
		"push {r0-r11, lr}\n\t"
		"blx call_taskEnterCarAsDriver\n\t"
		"pop {r0-r11, pc}");
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, VEHICLE_TYPE* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_hook(uintptr_t** thiz, VEHICLE_TYPE* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x3AE905 || dwRetAddr == 0x3AE9CF)
	{
		if (pNetGame)
		{
			if (GamePool_FindPlayerPed()->pVehicle == (uint32_t)pVehicle)
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)GamePool_FindPlayerPed()->pVehicle);
				CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
				pLocalPlayer->SendExitVehicleNotification(VehicleID);
			}
		}
	}

	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

int (*CMainMenu__Update)(uintptr_t thiz, float a2);
int CMainMenu__Update_hook(uintptr_t thiz, float a2)
{
	if (g_bMainMenuStage)
	{
		if (g_bPlaySAMP) return CMainMenu__Update(thiz, a2);

		InitInMenu();

		// StartGameScreen::OnNewGameCheck()
		((void (*)())(g_libGTASA + 0x261C8C + 1))();

		g_bPlaySAMP = true;
		return 0;
	}

	g_bMainMenuStage = true;
	return CMainMenu__Update(thiz, a2);
}

int (*CMobileMenu__InitForPause)(uintptr_t thiz);
int CMobileMenu__InitForPause_hook(uintptr_t thiz)
{
	Log("Seems like game paused ?");
	return CMobileMenu__InitForPause(thiz);
}

uintptr_t(*DoGameState)(uintptr_t thiz);
uintptr_t DoGameState_hook(uintptr_t thiz)
{
	Log("fuckyou");
	return DoGameState(thiz);
}

void InstallSpecialHooks()
{
	InstallMethodHook(g_libGTASA + 0x5DDC60, (uintptr_t)Init_hook);
	SetUpHook(g_libGTASA + 0x269974, (uintptr_t)MenuItem_add_hook, (uintptr_t*)&MenuItem_add);
	SetUpHook(g_libGTASA + 0x4D3864, (uintptr_t)CText_Get_hook, (uintptr_t*)&CText_Get);
	SetUpHook(g_libGTASA + 0x40C530, (uintptr_t)InitialiseRenderWare_hook, (uintptr_t*)&InitialiseRenderWare);
	SetUpHook(g_libGTASA + 0x25E660, (uintptr_t)CMainMenu__Update_hook, (uintptr_t*)&CMainMenu__Update);
	//SetUpHook(g_libGTASA + 0x25C298, (uintptr_t)CMobileMenu__InitForPause_hook, (uintptr_t*)&CMobileMenu__InitForPause);
}

void onDamage(PED_TYPE* issuer, PED_TYPE* damaged)
{
	if (!pNetGame) return;
	PED_TYPE* pPedPlayer = GamePool_FindPlayerPed();
	if (damaged && (pPedPlayer == issuer))
	{
		if (pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)damaged) != INVALID_PLAYER_ID)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			CAMERA_AIM* caAim = pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

			VECTOR aim;
			aim.X = caAim->f1x;
			aim.Y = caAim->f1y;
			aim.Z = caAim->f1z;

			pPlayerPool->GetLocalPlayer()->SendBulletSyncData(pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)damaged), BULLET_HIT_TYPE_PLAYER, aim);
		}
	}
}

uintptr_t(*ComputeDamageResponse)(uintptr_t, uintptr_t, int, int);
uintptr_t ComputeDamageResponse_hook(uintptr_t issuer, uintptr_t ped, int a3, int a4)
{
	if (issuer && ped) onDamage((PED_TYPE*)*(uintptr_t*)issuer, (PED_TYPE*)ped);
	return ComputeDamageResponse(issuer, ped, a3, a4);
}

int (*CWeapon__Fire)(uintptr_t thiz, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7);
int CWeapon__Fire_hook(uintptr_t thiz, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7)
{
	if (!pNetGame) return CWeapon__Fire(thiz, a2, a3, a4, a5, a6, a7);
	PED_TYPE* pPedPlayer = GamePool_FindPlayerPed();
	if (pPedPlayer == (PED_TYPE*)a2) // THIS IS THE LOCAL PLAYER
	{
		// Get the hit pos vec
		CAMERA_AIM* caAim = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

		VECTOR aim;
		aim.X = caAim->f1x;
		aim.Y = caAim->f1y;
		aim.Z = caAim->f1z;

		//pNetGame->GetPlayerPool()->GetLocalPlayer()->SendBulletSyncData(0xFFFF, 0, aim);
	}
	return CWeapon__Fire(thiz, a2, a3, a4, a5, a6, a7);
}

uint16_t(*CHud_DrawVitalStats)(uintptr_t thiz);
uint16_t CHud_DrawVitalStats_hook(uintptr_t thiz)
{
	if (pScoreBoard)
		pScoreBoard->Toggle();
	return 0;
}

uint32_t(*CWeapon__DoBulletImpact)(uintptr_t thiz, ENTITY_TYPE* pFiringEntity, ENTITY_TYPE* pCollideEntity, VECTOR* vecStart, VECTOR* vecEnd, void* pColPoint, int vecAhead);
uint32_t CWeapon__DoBulletImpact_hook(uintptr_t thiz, ENTITY_TYPE* pFiringEntity, ENTITY_TYPE* pCollideEntity, VECTOR* vecStart, VECTOR* vecEnd, void* pColPoint, int vecAhead)
{
	if (pNetGame)
	{
		CObjectPool* pObjectPool = pNetGame->GetObjectPool();
		uint16_t objID = pObjectPool->FindIDFromGtaPtr(pCollideEntity);
		if (objID != INVALID_OBJECT_ID)
		{
			if (pFiringEntity == (ENTITY_TYPE*)pGame->FindPlayerPed()->m_pEntity)
			{
				CAMERA_AIM* caAim = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();
				VECTOR aim;
				aim.X = caAim->f1x;
				aim.Y = caAim->f1y;
				aim.Z = caAim->f1z;

				pNetGame->GetPlayerPool()->GetLocalPlayer()->SendBulletSyncData(objID, 3, aim);
			}
		}

	}
	return CWeapon__DoBulletImpact(thiz, pFiringEntity, pCollideEntity, vecStart, vecEnd, pColPoint, vecAhead);
}

int (*CUpsideDownCarCheck__IsCarUpsideDown)(int thiz, uintptr_t a2);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int thiz, uintptr_t a2)
{
	if (*(uintptr_t*)(a2 + 20))
	{
		return CUpsideDownCarCheck__IsCarUpsideDown(thiz, a2);
	}
	return 0;
}

int (*RwFrameAddChild)(int a1, int a2);
int RwFrameAddChild_hook(int a1, int a2)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x338BD7)
	{
		if (a1 == 0)
			return 0;
	}

	return RwFrameAddChild(a1, a2);
}

int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, uintptr_t a2);
int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, uintptr_t a2)
{
	//if(!thiz || !a2) return CTaskSimpleUseGun__SetPedPosition(thiz, a2);

	unsigned char v1 = *((unsigned char*)thiz + 13);
	bool bChangeTheResult = false;
	/*PED_TYPE *pPedPlayer = (PED_TYPE*)a2;
	if(pPedPlayer != 0 && pNetGame != 0)
	{
		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
		{
			// LOCAL PED
			if(pPedPlayer == GamePool_FindPlayerPed())
			{
				CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
				if(pPlayerPed)
				{
					if((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
					{
						if(v1 == 0)
						{
							if(GetTickCount() - testTimePrepareWeapon >= 250)
							{
								*((unsigned char*)thiz + 13) |= 1;
								testTimePrepareWeapon = GetTickCount();
							}
						}
					}
				}
			}
			// REMOTE PED
			else
			{
				if(v1 == 0)
				{
					PLAYERID remotePlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPedPlayer);
					if(remotePlayerID != INVALID_PLAYER_ID)
					{
						CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(remotePlayerID);
						if(pRemotePlayer)
						{
							CPlayerPed *pPlayerPed = pRemotePlayer->GetPlayerPed();
							if((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
							{
								if(GetTickCount() - testTimePrepareWeaponRemote[remotePlayerID] >= 250)
								{
									*((unsigned char*)thiz + 13) |= 1;
									testTimePrepareWeaponRemote[remotePlayerID] = GetTickCount();
								}
							}
						}
					}
				}
			}
		}
	}*/

	PED_TYPE* pPedPlayer = (PED_TYPE*)a2;
	if (pPedPlayer != 0 && pNetGame != 0)
	{
		if (v1 == 0)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				// LOCAL PED
				if (pPedPlayer == GamePool_FindPlayerPed())
				{
					CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
					if (pPlayerPed)
					{
						if ((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
						{
							CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
							if (pVehiclePool)
							{
								for (VEHICLEID veh = 0; veh < MAX_VEHICLES; veh++)
								{
									if (pVehiclePool->GetSlotState(veh))
									{
										CVehicle* pVehicle = pVehiclePool->GetAt(veh);
										if (pVehicle)
										{
											MATRIX4X4 vehMat, playerMat;
											pVehicle->GetMatrix(&vehMat);
											pPlayerPed->GetMatrix(&playerMat);

											float fSX = (vehMat.pos.X - playerMat.pos.X) * (vehMat.pos.X - playerMat.pos.X);
											float fSY = (vehMat.pos.Y - playerMat.pos.Y) * (vehMat.pos.Y - playerMat.pos.Y);
											float fSZ = (vehMat.pos.Z - playerMat.pos.Z) * (vehMat.pos.Z - playerMat.pos.Z);

											float fDistance = (fSX + fSY + fSZ);

											if (fDistance <= 100.0f)
												bChangeTheResult = true;
										}
									}
								}
							}
						}
					}
				}
				// REMOTE PED
				else
				{
					PLAYERID remotePlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPedPlayer);
					if (remotePlayerID != INVALID_PLAYER_ID)
					{
						CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(remotePlayerID);
						if (pRemotePlayer)
						{
							CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
							if ((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
							{
								CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
								if (pVehiclePool)
								{
									for (VEHICLEID veh = 0; veh < MAX_VEHICLES; veh++)
									{
										if (pVehiclePool->GetSlotState(veh))
										{
											CVehicle* pVehicle = pVehiclePool->GetAt(veh);
											if (pVehicle)
											{
												MATRIX4X4 vehMat, playerMat;
												pVehicle->GetMatrix(&vehMat);
												pPlayerPed->GetMatrix(&playerMat);

												float fSX = (vehMat.pos.X - playerMat.pos.X) * (vehMat.pos.X - playerMat.pos.X);
												float fSY = (vehMat.pos.Y - playerMat.pos.Y) * (vehMat.pos.Y - playerMat.pos.Y);
												float fSZ = (vehMat.pos.Z - playerMat.pos.Z) * (vehMat.pos.Z - playerMat.pos.Z);

												float fDistance = (fSX + fSY + fSZ);

												if (fDistance <= 100.0f)
													bChangeTheResult = true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (bChangeTheResult)
		*((unsigned char*)thiz + 13) |= 1;

	return CTaskSimpleUseGun__SetPedPosition(thiz, a2);
}

void (*CPhysical__RemoveAndAdd)(uintptr_t thiz);
void CPhysical__RemoveAndAdd_hook(uintptr_t thiz)
{
	//char **result = 0;

	if (pNetGame)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_bObjectSlotUsed[i])
				{
					if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == thiz)
					{
						CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
						if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
							pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
						{
							/*if(pChatWindow)
							{
								pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
							}*/
							return;
						}
						//Log("Processing local attached object");
						//result = CPhysical__Add(thiz);
					}
				}
			}
		}

		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

		if (pVehiclePool)
		{
			for (size_t i = 0; i < MAX_VEHICLES; i++)
			{
				if (pVehiclePool->GetSlotState(i))
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(i);
					if (pVehicle && pVehicle->IsAdded())
					{
						CObject* pObject = pVehicle->FindAttachedEntity((ENTITY_TYPE*)thiz);
						if (pObject != nullptr)
						{
							if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
								pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
							{
								/*if(pChatWindow)
								{
									pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
								}*/
								return;
							}
						}
					}
				}
			}
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_bObjectSlotUsed[i])
								{
									if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == thiz)
									{
										CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											/*if(pChatWindow)
											{
												pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
											}*/
											return;
										}
										//Log("Processing remote attached object. Player: %d", i);
										//Log("is added: %d | model index: %d | gta id: %d | x: %.2f | y: %.2f | z: %.2f | flags: %d | vtable: %d | unk: %d", pObject->IsAdded(), pObject->GetModelIndex(), pObject->m_dwGTAId, pObject->m_pEntity->mat->pos.X, pObject->m_pEntity->mat->pos.Y, pObject->m_pEntity->mat->pos.Z, pObject->m_pEntity->dwProcessingFlags, pObject->m_pEntity->vtable, pObject->m_pEntity->dwUnkModelRel);
										//result = CPhysical__Add(thiz);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/*if(result == 0)
	{
		Log("Processing unknown object.");
		result = CPhysical__Add(thiz);
	}*/

	//Log("Processed.");

	return CPhysical__RemoveAndAdd(thiz);
}

int (*CWeaponInfo__GetWeaponReloadTime)(uintptr_t thiz);
int CWeaponInfo__GetWeaponReloadTime_hook(uintptr_t thiz)
{
	int result = CWeaponInfo__GetWeaponReloadTime(thiz);

	if (pChatWindow) pChatWindow->AddDebugMessage("weapon reload time: %d", result);

	return result;
}

char** (*CPhysical__Add)(uintptr_t thiz);
char** CPhysical__Add_hook(uintptr_t thiz)
{
	//char **result = 0;

	if (pNetGame)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_bObjectSlotUsed[i])
				{
					if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == thiz)
					{
						CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
						if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
							pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
						{
							/*if(pChatWindow)
							{
								pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
							}*/
							return 0;
						}
						//Log("Processing local attached object");
						//result = CPhysical__Add(thiz);
					}
				}
			}
		}

		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

		if (pVehiclePool)
		{
			for (size_t i = 0; i < MAX_VEHICLES; i++)
			{
				if (pVehiclePool->GetSlotState(i))
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(i);
					if (pVehicle && pVehicle->IsAdded())
					{
						CObject* pObject = pVehicle->FindAttachedEntity((ENTITY_TYPE*)thiz);
						if (pObject != nullptr)
						{
							if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
								pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
							{
								/*if(pChatWindow)
								{
									pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
								}*/
								return 0;
							}
						}
					}
				}
			}
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_bObjectSlotUsed[i])
								{
									if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == thiz)
									{
										CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											/*if(pChatWindow)
											{
												pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
											}*/
											return 0;
										}
										//Log("Processing remote attached object. Player: %d", i);
										//Log("is added: %d | model index: %d | gta id: %d | x: %.2f | y: %.2f | z: %.2f | flags: %d | vtable: %d | unk: %d", pObject->IsAdded(), pObject->GetModelIndex(), pObject->m_dwGTAId, pObject->m_pEntity->mat->pos.X, pObject->m_pEntity->mat->pos.Y, pObject->m_pEntity->mat->pos.Z, pObject->m_pEntity->dwProcessingFlags, pObject->m_pEntity->vtable, pObject->m_pEntity->dwUnkModelRel);
										//result = CPhysical__Add(thiz);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/*if(result == 0)
	{
		Log("Processing unknown object.");
		result = CPhysical__Add(thiz);
	}*/

	//Log("Processed.");

	return CPhysical__Add(thiz);
}

char* (*RenderQueue__ProcessCommand)(uintptr_t thiz, uintptr_t a2);
char* RenderQueue__ProcessCommand_hook(uintptr_t thiz, uintptr_t a2)
{
	if (!thiz || !a2) return 0;
	return RenderQueue__ProcessCommand(thiz, a2);
}

int (*CRealTimeShadowManager__Update)(uintptr_t thiz);
int CRealTimeShadowManager__Update_hook(uintptr_t thiz)
{
	return 0;
}


void (*CObject_Render)(uintptr_t thiz);
void CObject_Render_hook(uintptr_t thiz)
{
	ENTITY_TYPE* object = (ENTITY_TYPE*)thiz;
	if (pNetGame && object != 0)
	{
		CObject* pObject = pNetGame->GetObjectPool()->GetObjectFromGtaPtr(object);
	}
	CObject_Render(thiz);
}


int (*RLEDecompress)(int a1, unsigned int a2, const char* a3, unsigned int a4, unsigned int a5);
int RLEDecompress_hook(int a1, unsigned int a2, const char* a3, unsigned int a4, unsigned int a5)
{
	if (!a3) return 0;
	return RLEDecompress(a1, a2, a3, a4, a5);
}

int (*CAnimBlendNode__FindKeyFrame)(uintptr_t thiz, float a2, int a3, int a4);
int CAnimBlendNode__FindKeyFrame_hook(uintptr_t thiz, float a2, int a3, int a4)
{
	if (!thiz || !*((uintptr_t*)thiz + 4)) return 0;
	return CAnimBlendNode__FindKeyFrame(thiz, a2, a3, a4);
}

int (*CWorld__Add)(ENTITY_TYPE* entity);
int CWorld__Add_hook(ENTITY_TYPE* entity)
{
	//Log("[CWorld::Add] Entity %d added", entity);

	if (entity)
	{
		pWorld->AddDataBuilding(entity);
	}

	return CWorld__Add(entity);
}

int (*CWorld__Remove)(uintptr_t entity);
int CWorld__Remove_hook(uintptr_t entity)
{
	for (size_t i = 0; i < AddedEntities.size(); i++)
	{
		if (entity == (uintptr_t)AddedEntities.at(i))
			AddedEntities.erase(AddedEntities.begin() + i);
	}

	return CWorld__Remove(entity);
}

int (*SetCompAlphaCB)(int a1, char a2);
int SetCompAlphaCB_hook(int a1, char a2)
{
	if (!a1) return 0;
	return SetCompAlphaCB(a1, a2);
}

int (*CEntity__Render)(ENTITY_TYPE* pInterface);
int CEntity__Render_hook(ENTITY_TYPE* pInterface)
{
	if (pWorld)
	{
		if (pWorld->ProcessRemovedEntities(pInterface) == false)
		{
			pInterface->nEntityFlags.m_bUsesCollision = 0;
			return 0;
		}
	}
	return CEntity__Render(pInterface);
}

void (*CGame__Process)();
void CGame__Process_hook()
{
	CGame__Process();

	if (pNetGame)
	{
		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool) {
			pTextDrawPool->SnapshotProcess();
		}
	}
}

void (*CStreaming__Init2)(uintptr_t thiz);
void CStreaming__Init2_hook(uintptr_t thiz)
{
	CStreaming__Init2(thiz);
	UnFuck(g_libGTASA + 0x5DE734);
	*((uint32_t*)g_libGTASA + 0x5DE734) = 0x10000000;
}

int (*CTextureDatabaseRuntime__GetEntry)(uintptr_t thiz, const char* a2, bool* a3);
int CTextureDatabaseRuntime__GetEntry_hook(uintptr_t thiz, const char* a2, bool* a3)
{
	if (!thiz)
	{
		return -1;
	}
	return CTextureDatabaseRuntime__GetEntry(thiz, a2, a3);
}

int (*_rwFreeListFreeReal)(int a1, unsigned int a2);
int _rwFreeListFreeReal_hook(int a1, unsigned int a2)
{
	if (a1 == 0 || !a1) return 0;
	return _rwFreeListFreeReal(a1, a2);
}

uintptr_t(*CPlayerPedDataSaveStructure__Construct)(int a1, int a2);
uintptr_t CPlayerPedDataSaveStructure__Construct_hook(int a1, int a2)
{
	if (!a1 || !a2) return 0;
	if (!*(int*)a2) return 0;
	return CPlayerPedDataSaveStructure__Construct(a1, a2);
}

void InstallHooks()
{
	 // g_libGTASA + 0x9E75B8 - gGameState
	SetUpHook(g_libGTASA + 0x3EF518, (uintptr_t)CObject_Render_hook, (uintptr_t*)&CObject_Render);

#ifdef BUILD_BETA
	//SetUpHook(g_libGTASA+0x3C14B0, (uintptr_t)CWorld__Add_hook, (uintptr_t*)&CWorld__Add);
	//SetUpHook(g_libGTASA+0x3C1500, (uintptr_t)CWorld__Remove_hook, (uintptr_t*)&CWorld__Remove);
	SetUpHook(g_libGTASA + 0x391E20, (uintptr_t)CEntity__Render_hook, (uintptr_t*)&CEntity__Render);
	SetUpHook(g_libGTASA + 0x50C5F8, (uintptr_t)SetCompAlphaCB_hook, (uintptr_t*)&SetCompAlphaCB);
	SetUpHook(g_libGTASA + 0x3986CC, (uintptr_t)CGame__Process_hook, (uintptr_t*)&CGame__Process);
	SetUpHook(g_libGTASA + 0x1B9D74, (uintptr_t)_rwFreeListFreeReal_hook, (uintptr_t*)&_rwFreeListFreeReal);
	SetUpHook(g_libGTASA + 0x3AA5C8, (uintptr_t)CPhysical__RemoveAndAdd_hook, (uintptr_t*)&CPhysical__RemoveAndAdd);
	//SetUpHook(g_libGTASA + 0x4042A8, (uintptr_t)CStreaming__Init2_hook, (uintptr_t*)&CStreaming__Init2);
	//SetUpHook(g_libGTASA + 0x1BDC3C, (uintptr_t)CTextureDatabaseRuntime__GetEntry_hook, (uintptr_t*)&CTextureDatabaseRuntime__GetEntry);
	SetUpHook(g_libGTASA + 0x41EAB4, (uintptr_t)CPlayerPedDataSaveStructure__Construct_hook, (uintptr_t*)&CPlayerPedDataSaveStructure__Construct);
#endif

	// F8C0 8008
	SetUpHook(g_libGTASA + 0x33AD78, (uintptr_t)CAnimBlendNode__FindKeyFrame_hook, (uintptr_t*)&CAnimBlendNode__FindKeyFrame);
	SetUpHook(g_libGTASA + 0x1BC314, (uintptr_t)RLEDecompress_hook, (uintptr_t*)&RLEDecompress);
	SetUpHook(g_libGTASA + 0x541AC4, (uintptr_t)CRealTimeShadowManager__Update_hook, (uintptr_t*)&CRealTimeShadowManager__Update);
	SetUpHook(g_libGTASA + 0x1A8530, (uintptr_t)RenderQueue__ProcessCommand_hook, (uintptr_t*)&RenderQueue__ProcessCommand);
	SetUpHook(g_libGTASA + 0x3AA3C0, (uintptr_t)CPhysical__Add_hook, (uintptr_t*)&CPhysical__Add);
	//CodeInject(g_libGTASA+0x3AA4F0, (uintptr_t)CPhysical__Add_hook, 1);
	//SetUpHook(g_libGTASA+0x394610, (uintptr_t)CPhysical__Add_hook, (uintptr_t*)&CPhysical__Add);
	//SetUpHook(g_libGTASA+0x56BE40, (uintptr_t)CWeaponInfo__GetWeaponReloadTime_hook, (uintptr_t*)&CWeaponInfo__GetWeaponReloadTime);
	//SetUpHook(g_libGTASA+0x565864, (uintptr_t)CWeapon__DoWeaponEffect_hook, (uintptr_t*)&CWeapon__DoWeaponEffect);
	//SetUpHook(g_libGTASA+0x46D4E4, (uintptr_t)CTaskSimpleUseGun__FireGun_hook, (uintptr_t*)&CTaskSimpleUseGun__FireGun);
	SetUpHook(g_libGTASA + 0x46D6AC, (uintptr_t)CTaskSimpleUseGun__SetPedPosition_hook, (uintptr_t*)&CTaskSimpleUseGun__SetPedPosition);
	SetUpHook(g_libGTASA + 0x1AECC0, (uintptr_t)RwFrameAddChild_hook, (uintptr_t*)&RwFrameAddChild);
	SetUpHook(g_libGTASA + 0x2DFD30, (uintptr_t)CUpsideDownCarCheck__IsCarUpsideDown_hook, (uintptr_t*)&CUpsideDownCarCheck__IsCarUpsideDown);
	SetUpHook(g_libGTASA + 0x566CAC, (uintptr_t)CWeapon__DoBulletImpact_hook, (uintptr_t*)&CWeapon__DoBulletImpact);
	//SetUpHook(g_libGTASA+0x3D4EAC, (uintptr_t)CHud_DrawVitalStats_hook, (uintptr_t*)&CHud_DrawVitalStats);
	SetUpHook(g_libGTASA + 0x3AF1A0, (uintptr_t)CPools_Initialise_hook, (uintptr_t*)&CPools_Initialise);
	SetUpHook(g_libGTASA + 0x3C1BF8, (uintptr_t)CWorld__ProcessPedsAfterPreRender_hook, (uintptr_t*)&CWorld__ProcessPedsAfterPreRender);
	SetUpHook(g_libGTASA + 0x336268, (uintptr_t)CModelInfo_AddAtomicModel_hook, (uintptr_t*)&CModelInfo_AddAtomicModel);

	SetUpHook(g_libGTASA + 0x569374, (uintptr_t)CWeapon__Fire_hook, (uintptr_t*)&CWeapon__Fire);
	SetUpHook(g_libGTASA + 0x327528, (uintptr_t)ComputeDamageResponse_hook, (uintptr_t*)(&ComputeDamageResponse));

	SetUpHook(g_libGTASA + 0x23B3DC, (uintptr_t)NvFOpen_hook, (uintptr_t*)&NvFOpen);
	SetUpHook(g_libGTASA + 0x3D7CA8, (uintptr_t)CLoadingScreen_DisplayPCScreen_hook, (uintptr_t*)&CLoadingScreen_DisplayPCScreen);
	SetUpHook(g_libGTASA + 0x39AEF4, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
	SetUpHook(g_libGTASA + 0x39B098, (uintptr_t)Render2dStuffAfterFade_hook, (uintptr_t*)&Render2dStuffAfterFade);
	SetUpHook(g_libGTASA+0x2E1E00, (uintptr_t)CRunningScript__Process_hook, (uintptr_t*)&CRunningScript__Process);
	SetUpHook(g_libGTASA + 0x239D5C, (uintptr_t)TouchEvent_hook, (uintptr_t*)&TouchEvent);
	SetUpHook(g_libGTASA + 0x28E83C, (uintptr_t)CStreaming_InitImageList_hook, (uintptr_t*)&CStreaming_InitImageList);
	SetUpHook(g_libGTASA + 0x336690, (uintptr_t)CModelInfo_AddPedModel_hook, (uintptr_t*)&CModelInfo_AddPedModel);
	SetUpHook(g_libGTASA + 0x3DBA88, (uintptr_t)CRadar__GetRadarTraceColor_hook, (uintptr_t*)&CRadar__GetRadarTraceColor);
	SetUpHook(g_libGTASA + 0x3DAF84, (uintptr_t)CRadar__SetCoordBlip_hook, (uintptr_t*)&CRadar__SetCoordBlip);
	SetUpHook(g_libGTASA + 0x3DE9A8, (uintptr_t)CRadar__DrawRadarGangOverlay_hook, (uintptr_t*)&CRadar__DrawRadarGangOverlay);
	//InstallMethodHook(g_libGTASA+0x326EE0, (uintptr_t)CPedDamageResponseCalculator_constructor_hook);

	SetUpHook(g_libGTASA + 0x482E60, (uintptr_t)CTaskComplexEnterCarAsDriver_hook, (uintptr_t*)&CTaskComplexEnterCarAsDriver);
	SetUpHook(g_libGTASA + 0x4833CC, (uintptr_t)CTaskComplexLeaveCar_hook, (uintptr_t*)&CTaskComplexLeaveCar);
	//SetUpHook(g_libGTASA + 0x0044A4CC, (uintptr_t)PointGunInDirection_hook, (uintptr_t*)&PointGunInDirection);
	CodeInject(g_libGTASA + 0x2D99F4, (uintptr_t)PickupPickUp_hook, 1);

	HookCPad();
}
