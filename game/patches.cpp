#include "../main.h"
#include "../util/armhook.h"
#include "common.h"
#include "settings.h"

extern CSettings *pSettings;

char* PLAYERS_REALLOC = nullptr;

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
} *ATOMIC_MODELS = nullptr;

void ApplyFPSPatch(uint8_t fps)
{
	char fpsch[15];
	sprintf(fpsch, "\\x%x", fps);
	WriteMemory(g_libGTASA+0x463FE8, (uintptr_t)fpsch, 1);
	WriteMemory(g_libGTASA+0x56C1F6, (uintptr_t)fpsch, 1);
	WriteMemory(g_libGTASA+0x56C126, (uintptr_t)fpsch, 1);
	WriteMemory(g_libGTASA+0x95B074, (uintptr_t)fpsch, 1);
}

void ApplyCrashFixPatches()
{
	// reallocate CPools::ms_pEntryInfoNodePool
	WriteMemory(g_libGTASA+0x3AF27A, (uintptr_t)"\x4f\xf4\x20\x40", 4); // MOV.W	R0, #0xA000 | size = 0x14

	WriteMemory(g_libGTASA+0x3AF284, (uintptr_t)"\x4f\xf4\x00\x60", 4); // MOV.W R0, #0x800
	WriteMemory(g_libGTASA+0x3AF28C, (uintptr_t)"\x4f\xf4\x00\x62", 4); // MOV.W R2, #0x800
	WriteMemory(g_libGTASA+0x3AF2BA, (uintptr_t)"\xb3\xf5\x00\x6f", 4); // CMP.W R3, #0x800

	WriteMemory(g_libGTASA+0x3AF284, (uintptr_t)"\x4f\xf4\x90\x50", 4); // MOV.W R0, #0x1200
	WriteMemory(g_libGTASA+0x3AF28C, (uintptr_t)"\x4f\xf4\x90\x52", 4); // MOV.W R2, #0x1200
	WriteMemory(g_libGTASA+0x3AF2BA, (uintptr_t)"\xb3\xf5\x90\x5f", 4); // CMP.W R3, #0x1200

	// reallocate CPools::ms_pPtrNodeDoubleLinkPool
	WriteMemory(g_libGTASA+0x3AF21C, (uintptr_t)"\x4F\xF4\x00\x30", 4); // MOV.W R0, #0x20000
}

void ApplyPatches_level0()
{
	// reallocate CWorld::Players[]
	//PLAYERS_REALLOC = new char[404*PLAYER_PED_SLOTS];
	PLAYERS_REALLOC = (( char* (*)(int))(g_libGTASA+0x179B40))(404*MAX_PLAYERS);
	UnFuck(g_libGTASA+0x5D021C);
	*(char**)(g_libGTASA+0x5D021C) = PLAYERS_REALLOC;
	Log("CWorld::Player new address = 0x%X", PLAYERS_REALLOC);

	ATOMIC_MODELS = new _ATOMIC_MODEL[20000];
	for (int i = 0; i < 20000; i++) {
		// CBaseModelInfo::CBaseModelInfo
		((void(*)(_ATOMIC_MODEL*))(g_libGTASA + 0x33559C + 1))(&ATOMIC_MODELS[i]);
		// vtable
		ATOMIC_MODELS[i].func_tbl = g_libGTASA + 0x5C6C68;
		memset(ATOMIC_MODELS[i].data, 0, sizeof(ATOMIC_MODELS->data));
	}

	// reallocate Pads[]
	//UnFuck(g_libGTASA+0x5CF914);
	//*(int8_t **)(g_libGTASA+0x5CF914) = new int8_t[(PLAYER_PED_SLOTS + 1) * 340];

	// CdStreamInit(6);
	WriteMemory(g_libGTASA+0x3981EC, (uintptr_t)"\x06\x20", 2);

	// Apply crash fix patches
	#ifdef RELEASE_BETA
	ApplyCrashFixPatches();
	#endif
}

void ApplyPatches()
{
	Log("Installing patches..");

	// CAudioEngine::StartLoadingTune
	NOP(g_libGTASA+0x56C150, 2);

	// DefaultPCSaveFileName
	char* DefaultPCSaveFileName = (char*)(g_libGTASA+0x60EAE8);
	memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);

	// CWidgetRegionSteeringSelection::Draw
	WriteMemory(g_libGTASA+0x284BB8, (uintptr_t)"\xF7\x46", 2);

	// fix bg viewmodel
	WriteMemory(g_libGTASA+0x1859FC, (uintptr_t)"\x01\x22", 2);
	
	// CHud::SetHelpMessage
	WriteMemory(g_libGTASA+0x3D4244, (uintptr_t)"\xF7\x46", 2);

	// CHud::SetHelpMessageStatUpdate
	WriteMemory(g_libGTASA+0x3D42A8, (uintptr_t)"\xF7\x46", 2);

	// CVehicleRecording::Load
	WriteMemory(g_libGTASA+0x2DC8E0, (uintptr_t)"\xF7\x46", 2);

	// nop calling CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical
	NOP(g_libGTASA+0x3A019C, 2);

	// CPed pool (old: 140, new: 210)
 	/* 	MOVW R0, #0x5EC8
 		MOVT R0, #6 */
 	WriteMemory(g_libGTASA+0x3AF2D0, (uintptr_t)"\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // MOV  R0, #0x0065EC8 | size=0x7C4 (old: 0x43F30)
 	WriteMemory(g_libGTASA+0x3AF2DE, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
 	WriteMemory(g_libGTASA+0x3AF2E4, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
 	WriteMemory(g_libGTASA+0x3AF310, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, #0xD2

 	// CPedIntelligence pool (old: 140, new: 210)
	// movw r0, #0x20B0
 	// movt r0, #2
 	// nop
 	WriteMemory(g_libGTASA+0x3AF7E6, (uintptr_t)"\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // MOVS R0, #0x00220B0 | size=0x298 (old: 0x0016B20)
 	WriteMemory(g_libGTASA+0x3AF7F6, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
 	WriteMemory(g_libGTASA+0x3AF7FC, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
 	WriteMemory(g_libGTASA+0x3AF824, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, 0xD2

 	// Task pool (old: 500, new: 1524 (1536))
 	WriteMemory(g_libGTASA+0x3AF4EA, (uintptr_t)"\x4F\xF4\x40\x30", 4); // MOV.W R0, #30000 | size = 0x80 (old: 0xFA00)
 	WriteMemory(g_libGTASA+0x3AF4F4, (uintptr_t)"\x4F\xF4\xC0\x60", 4); // MOV.W R0, #0x600
 	WriteMemory(g_libGTASA+0x3AF4FC, (uintptr_t)"\x4F\xF4\xC0\x62", 4); // MOV.W R2, #0x600
 	WriteMemory(g_libGTASA+0x3AF52A, (uintptr_t)"\xB3\xF5\xC0\x6F", 4); // CMP.W R3, #0x600

 	// Event pool (old: 200, new: 512)
 	
 	// ColModel pool (old:10150, new: 32511)
 	// mov r0, #0xCFD0
 	// movt r0, #0x17
 	WriteMemory(g_libGTASA+0x3AF48E, (uintptr_t)"\x4C\xF6\xD0\x70\xC0\xF2\x17\x00", 8); // MOV R0, #0x0017CFD0 | size=0x30 (old: 0x0076F20)
 	WriteMemory(g_libGTASA+0x3AF49C, (uintptr_t)"\x47\xF6\xFF\x60", 4); // MOVW R0, #0x7EFF
 	WriteMemory(g_libGTASA+0x3AF4A4, (uintptr_t)"\x47\xF6\xFF\x62", 4); // MOVW R2, #0x7EFF

	// VehicleStruct increase (0x32C * 0x50 = 0xFDC0)
    WriteMemory(g_libGTASA+0x405338, (uintptr_t)"\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
    WriteMemory(g_libGTASA+0x405342, (uintptr_t)"\x50\x20", 2);			// MOVS R0, #0x50
    WriteMemory(g_libGTASA+0x405348, (uintptr_t)"\x50\x22", 2);			// MOVS R2, #0x50
    WriteMemory(g_libGTASA+0x405374, (uintptr_t)"\x50\x2B", 2);	

	//NOP(g_libGTASA + 0x25A102, 2);
    // Increase matrix count in CPlaceable::InitMatrixArray 
 	WriteMemory(g_libGTASA+0x3ABB0A, (uintptr_t)"\x4F\xF4\x7A\x61", 4); // MOV.W R1, #4000
}

void ApplyInGamePatches()
{
	Log("Installing patches (ingame)..");

	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA+0x8EA7B0), 1, 100);

	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_libGTASA+0x8EA7A8) = 100;

	// CTaskSimplePlayerOnFoot::PlayIdleAnimations 
	WriteMemory(g_libGTASA+0x4BDB18, (uintptr_t)"\x70\x47", 2);

	// CarCtl::GenerateRandomCars nulled from CGame::Process
	UnFuck(g_libGTASA+0x398A3A);
	NOP(g_libGTASA+0x398A3A, 2);

	// CTheCarGenerators::Process nulled from CGame::Process
	UnFuck(g_libGTASA+0x398A34);
	NOP(g_libGTASA+0x398A34, 2);

	// Health multiply
	UnFuck(g_libGTASA+0x3BAC68);
	*(float*)(g_libGTASA+0x3BAC68) = 176.0f;

	// Armour multiply
	UnFuck(g_libGTASA+0x27D884);
	*(float*)(g_libGTASA+0x27D884) = 176.0f;

	// CEntryExit::GenerateAmbientPeds nulled from CEntryExit::TransitionFinished
	UnFuck(g_libGTASA+0x2C2C22);
	NOP(g_libGTASA+0x2C2C22, 4);

	// Wasted
	WriteMemory(g_libGTASA+0x3D62FC, (uintptr_t)"\xF7\x46", 2);

	// CPlayerPed::CPlayerPed task fix
	WriteMemory(g_libGTASA+0x458ED1, (uintptr_t)"\xE0", 1);

	// ReapplyPlayerAnimation (�� �����)
	NOP(g_libGTASA+0x45477E, 5);

	// radar draw blips
    UnFuck(g_libGTASA+0x3DCA90);
    NOP(g_libGTASA+0x3DCA90, 2);
    UnFuck(g_libGTASA+0x3DD4A4);
    NOP(g_libGTASA+0x3DD4A4, 2);

    // CCamera::CamShake from CExplosion::AddExplosion
    NOP(g_libGTASA+0x55EFB8, 2);
    NOP(g_libGTASA+0x55F8F8, 2);
    
    // Engine lights patch
	NOP(g_libGTASA+0x408AAA, 2);

    // camera_on_actor path
    UnFuck(g_libGTASA+0x2F7B68);
    *(uint8_t*)(g_libGTASA+0x2F7B6B) = 0xBE;

    // CPed::RemoveWeaponWhenEnteringVehicle (GetPlayerInfoForThisPlayerPed)
    UnFuck(g_libGTASA+0x434D94);
    NOP(g_libGTASA+0x434D94, 6);

    // CBike::ProcessAI
    UnFuck(g_libGTASA+0x4EE200);
    *(uint8_t*)(g_libGTASA+0x4EE200) = 0x9B;

    // CWidgetPlayerInfo::DrawWanted
    WriteMemory(g_libGTASA+0x27D8D0, (uintptr_t)"\x4F\xF0\x00\x08", 4);

    // no vehicle audio processing
    UnFuck(g_libGTASA+0x4E31A6);
    NOP(g_libGTASA+0x4E31A6, 2);
    UnFuck(g_libGTASA+0x4EE7D2);
    NOP(g_libGTASA+0x4EE7D2, 2);
    UnFuck(g_libGTASA+0x4F741E);
    NOP(g_libGTASA+0x4F741E, 2);
    UnFuck(g_libGTASA+0x50AB4A);
    NOP(g_libGTASA+0x50AB4A, 2);

    // CPed::SetWeaponLockOnTarget
	// UnFuck(g_libGTASA + 0x00438DB4);
	// WriteMemory(g_libGTASA + 0x00438DB4, (uintptr_t)"\x00\x20\xF7\x46", 4);
	
	// CPlayerInfo::FindObjectToSteal
	// WriteMemory(g_libGTASA + 0x003AC114, (uintptr_t)"\x00\x20\xF7\x46", 4);
	
	// CPed::Say
	// WriteMemory(g_libGTASA + 0x0043E264, (uintptr_t)"\xF7\x46", 2);
    
    // Display FPS
	*(bool *)(g_libGTASA + 0x008ED875) = pSettings->Get().bDrawFPS ? true : false;

	// Disable vehicle name rendering
    NOP(g_libGTASA + 0x003D6FDC, 2);

    // Interior_c::AddPickups
	// WriteMemory(g_libGTASA + 0x003E17F0, (uintptr_t)"\x00\x20\xF7\x46", 4);
}
