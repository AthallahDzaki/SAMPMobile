#include "../main.h"
#include "game.h"

#define PI 3.14159265
extern CGame *pGame;

uintptr_t dwPlayerPedPtrs[PLAYER_PED_SLOTS];

extern char *PLAYERS_REALLOC;

PED_TYPE *GamePool_FindPlayerPed()
{
	return *(PED_TYPE **) PLAYERS_REALLOC;
}

PED_TYPE *GamePool_Ped_GetAt(int iID)
{
	return ((PED_TYPE * (*)(int))(g_libGTASA + 0x41DD7C + 1)) (iID);
}

int GamePool_Ped_GetIndex(PED_TYPE * pActor)
{
	return ((int (*)(PED_TYPE *))(g_libGTASA + 0x41DD60 + 1)) (pActor);
}

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID)
{
	return ((VEHICLE_TYPE * (*)(int))(g_libGTASA + 0x41DD44 + 1)) (iID);
}

int GamePool_Vehicle_GetIndex(VEHICLE_TYPE * pVehicle)
{
	return ((int (*)(VEHICLE_TYPE *))(g_libGTASA + 0x41DD28 + 1)) (pVehicle);
}

ENTITY_TYPE *GamePool_Object_GetAt(int iID)
{
	ENTITY_TYPE *(*GetPoolObj) (int iID);
	*(void **)(&GetPoolObj) = (void *)(g_libGTASA + 0x41DDB4 + 1);
	return (GetPoolObj) (iID);
}

int LineOfSight(VECTOR * start, VECTOR * end, void *colpoint, uintptr_t ent,
				char buildings, char vehicles, char peds, char objects, char dummies,
				bool seeThrough, bool camera, bool unk)
{
	return ((int (*)(VECTOR *, VECTOR *, void *, uintptr_t,
					 char, char, char, char, char, char, char,
					 char))(g_libGTASA + 0x3C70C0 + 1)) (start, end, colpoint, ent, buildings,
														 vehicles, peds, objects, dummies,
														 seeThrough, camera, unk);
}

// 0.3.7
bool IsPedModel(unsigned int iModelID)
{
	if (iModelID < 0 || iModelID > 20000)
		return false;
	uintptr_t *dwModelArray = (uintptr_t *) (g_libGTASA + 0x87BF48);

	uintptr_t ModelInfo = dwModelArray[iModelID];
	if (ModelInfo && *(uintptr_t *) ModelInfo == (uintptr_t) (g_libGTASA + 0x5C6E90	/* CPedModelInfo 
																					   vtable */ ))
		return true;

	return false;
}

// 0.3.7
bool IsValidModel(unsigned int uiModelID)
{
	if (uiModelID < 0 || uiModelID > 20000)
		return false;
	uintptr_t *dwModelArray = (uintptr_t *) (g_libGTASA + 0x87BF48);

	uintptr_t dwModelInfo = dwModelArray[uiModelID];
	if (dwModelInfo && *(uintptr_t *) (dwModelInfo + 0x34 /* pRwObject */ ))
		return true;

	return false;
}

uint16_t GetModelReferenceCount(int nModelIndex)
{
	uintptr_t *dwModelarray = (uintptr_t *) (g_libGTASA + 0x87BF48);
	uint8_t *pModelInfoStart = (uint8_t *) dwModelarray[nModelIndex];

	return *(uint16_t *) (pModelInfoStart + 0x1E);
}

void InitPlayerPedPtrRecords()
{
	memset(&dwPlayerPedPtrs[0], 0, sizeof(uintptr_t) * PLAYER_PED_SLOTS);
}

void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr)
{
	if (bytePlayer >= PLAYER_PED_SLOTS)
		return;
	dwPlayerPedPtrs[bytePlayer] = dwPedPtr;
}

uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr)
{
	uint8_t x = 0;
	while (x != PLAYER_PED_SLOTS)
	{
		if (dwPlayerPedPtrs[x] == dwPedPtr)
			return x;
		x++;
	}

	return 0;
}

bool IsPointInRect(float x, float y, RECT * rect)
{
	if (x >= rect->fLeft && x <= rect->fRight && y >= rect->fBottom && y <= rect->fTop)
		return true;

	return false;
}

uintptr_t GetTexture(const char *texture)
{
	Log("GetTexture: %s", texture);
	// GetTexture
	if (((uintptr_t(*)(const char *))(g_libGTASA + 0x258910 + 1)) (texture) != 0)
		return ((uintptr_t(*)(const char *))(g_libGTASA + 0x258910 + 1)) (texture);
	else
		return 0;
}

uintptr_t LoadTextureFromDB(const char *dbname, const char *texture)
{
	// TextureDatabaseRuntime::GetDatabase(dbname)
	uintptr_t db_handle = ((uintptr_t(*)(const char *))(g_libGTASA + 0x1BF530 + 1)) (dbname);
	if (!db_handle)
	{
		Log("Error: Database not found! (%s)", dbname);
		return 0;
	}
	// TextureDatabaseRuntime::Register(db)
	((void (*)(uintptr_t))(g_libGTASA + 0x1BE898 + 1)) (db_handle);
	uintptr_t tex = GetTexture(texture);

	if (!tex)
		Log("Error: Texture (%s) not found in database (%s)", dbname, texture);

	// TextureDatabaseRuntime::Unregister(db)
	((void (*)(uintptr_t))(g_libGTASA + 0x1BE938 + 1)) (db_handle);

	return tex;
}

void DefinedState2d()
{
	return ((void (*)())(g_libGTASA + 0x5590B0 + 1)) ();
}

void SetScissorRect(void *pRect)
{
	return ((void (*)(void *))(g_libGTASA + 0x273E8C + 1)) (pRect);
}

float DegToRad(float fDegrees)
{
	if (fDegrees > 360.0f || fDegrees < 0.0f)
		return 0.0f;

	if (fDegrees > 180.0f)
		return (float)(-(PI - (((fDegrees - 180.0f) * PI) / 180.0f)));
	else
		return (float)((fDegrees * PI) / 180.0f);
}

// 0.3.7
float FloatOffset(float f1, float f2)
{
	if (f1 >= f2)
		return f1 - f2;
	else
		return (f2 - f1);
}

void ProjectMatrix(VECTOR * vecOut, MATRIX4X4 * mat, VECTOR * vecPos)
{
	vecOut->X =
		mat->at.X * vecPos->Z + mat->up.X * vecPos->Y + mat->right.X * vecPos->X + mat->pos.X;
	vecOut->Y =
		mat->at.Y * vecPos->Z + mat->up.Y * vecPos->Y + mat->right.Y * vecPos->X + mat->pos.Y;
	vecOut->Z =
		mat->at.Z * vecPos->Z + mat->up.Z * vecPos->Y + mat->right.Z * vecPos->X + mat->pos.Z;
}

void RwMatrixRotate(MATRIX4X4 * mat, int axis, float angle)
{
	static float matt[3][3] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	((void (*)(MATRIX4X4 *, float *, float, int))(g_libGTASA + 0x1B9118 + 1)) (mat, matt[axis],
																			   angle, 1);
}

void RwMatrixScale(MATRIX4X4 * matrix, VECTOR * vecScale)
{
	matrix->right.X *= vecScale->X;
	matrix->right.Y *= vecScale->X;
	matrix->right.Z *= vecScale->X;

	matrix->up.X *= vecScale->Y;
	matrix->up.Y *= vecScale->Y;
	matrix->up.Z *= vecScale->Y;

	matrix->at.X *= vecScale->Z;
	matrix->at.Y *= vecScale->Z;
	matrix->at.Z *= vecScale->Z;

	matrix->flags &= 0xFFFDFFFC;
}

void WorldAddEntity(uintptr_t pEnt)
{
	((void (*)(uintptr_t))(g_libGTASA + 0x3C14B0 + 1)) (pEnt);
}

void WorldRemoveEntity(uintptr_t pEnt)
{
	((void (*)(uintptr_t))(g_libGTASA + 0x3C1500 + 1)) (pEnt);
}

uintptr_t GetModelInfoByID(int iModelID)
{
	if (iModelID < 0 || iModelID > 20000)
	{
		return false;
	}

	uintptr_t *dwModelArray = (uintptr_t *) (g_libGTASA + 0x87BF48);
	return dwModelArray[iModelID];
}


uintptr_t ModelInfoCreateInstance(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);
	if (modelInfo)
	{
		return ((uintptr_t(*)(uintptr_t)) *
				(uintptr_t *) (*(uintptr_t *) modelInfo + 0x2C)) (modelInfo);
	}

	return 0;
}

void RenderClumpOrAtomic(uintptr_t rwObject)
{
	if (rwObject)
	{
		if (*(uint8_t *) rwObject == 1)
		{
			// Atomic
			((void (*)(uintptr_t))(*(uintptr_t *) (rwObject + 0x48))) (rwObject);
		}
		else if (*(uint8_t *) rwObject == 2)
		{
			// rpClumpRender
			((void (*)(uintptr_t))(g_libGTASA + 0x1E0E60 + 1)) (rwObject);
		}
	}
}


float GetModelColSphereRadius(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t *) (modelInfo + 0x2C);
		if (colModel != 0)
		{
			return *(float *)(colModel + 0x24);
		}
	}

	return 0.0f;
}


void GetModelColSphereVecCenter(int iModel, VECTOR * vec)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t *) (modelInfo + 0x2C);
		if (colModel != 0)
		{
			VECTOR *v = (VECTOR *) (colModel + 0x18);

			vec->X = v->X;
			vec->Y = v->Y;
			vec->Z = v->Z;
		}
	}
}

void DestroyAtomicOrClump(uintptr_t rwObject)
{
	if (rwObject)
	{
		int type = *(int *)(rwObject);

		if (type == 1)
		{
			// RpAtomicDestroy
			((void (*)(uintptr_t))(g_libGTASA + 0x1E10D4 + 1)) (rwObject);

			uintptr_t parent = *(uintptr_t *) (rwObject + 4);
			if (parent)
			{
				// RwFrameDestroy
				((void (*)(uintptr_t))(g_libGTASA + 0x1AEC84 + 1)) (parent);
			}

		}
		else if (type == 2)
		{
			// RpClumpDestroy
			((void (*)(uintptr_t))(g_libGTASA + 0x1E1224 + 1)) (rwObject);
		}
	}
}

bool bTextDrawTextureSlotState[200];
uintptr_t TextDrawTexture[200];

int GetFreeTextDrawTextureSlot()
{
	for (int i = 0; i < 200; i++)
	{
		if (bTextDrawTextureSlotState[i] == false)
		{
			bTextDrawTextureSlotState[i] = true;
			return i;
		}
	}

	return -1;
}

void DestroyTextDrawTexture(int index)
{
	if (index >= 0 && index < 200)
	{
		bTextDrawTextureSlotState[index] = false;
		TextDrawTexture[index] = 0x0;
	}
}

uintptr_t LoadTexture(const char *texname)
{
	static char *texdb[] = { "samp", "gta3", "gta_int", "player", "txd" };

	for (int i = 0; i < 5; i++)
	{
		uintptr_t texture = LoadTextureFromDB(texdb[i], texname);
		if (texture != 0)
		{
			Log("%s loaded from %s", texname, texdb[i]);
			return texture;
		}
		else
		{
			return 0;
		}
	}

	Log("Texture %s not found!", texname);
	return 0;
}

void DrawTextureUV(uintptr_t texture, RECT * rect, uint32_t dwColor, float *uv)
{
	if (texture)
	{
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);
		// CSprite2d::Draw(CRect const& posn, CRGBA const& color, float u1,
		// float v1, float u2, float v2, float u3, float v3, float u4, float
		// v4);
		((void (*)
		  (uintptr_t, RECT *, uint32_t *, float, float, float, float, float, float, float,
		   float))(g_libGTASA + 0x5526CC + 1)) (texture, rect, &dwColor, uv[0], uv[1], uv[2],
												uv[3], uv[4], uv[5], uv[6], uv[7]);
	}
}

bool IsValidGamePed(PED_TYPE * pPed)
{
	// IsPedPointerValid(CPed *) — 0x00435614
	if (((bool(*)(PED_TYPE *)) (g_libGTASA + 0x00435614 + 1)) (pPed))
	{
		return true;
	}
	return false;
}

bool IsValidGameEntity(ENTITY_TYPE * pEntity)
{
	// IsEntityPointerValid(CEntity *) — 0x00393870
	if (((bool(*)(ENTITY_TYPE *)) (g_libGTASA + 0x00393870 + 1)) (pEntity))
	{
		return true;
	}
	return false;
}

bool IsGameEntityArePlaceable(ENTITY_TYPE * pEntity)
{
	if (pEntity)
	{
		if (pEntity->vtable == g_libGTASA + 0x005C7358)
		{
			return true;
		}
	}
	return false;
}

bool PreloadAnimFile(char *szAnimFile, uint16_t usSleep, uint16_t usHowTimes)
{
	if (pGame->IsAnimationLoaded(szAnimFile))
	{
		return true;
	}

	pGame->RequestAnimation(szAnimFile);

	int iWaitAnimLoad = 0;
	while (!pGame->IsAnimationLoaded(szAnimFile))
	{
		usleep(usSleep);
		if (usHowTimes != -1)
		{
			if (++iWaitAnimLoad > usHowTimes)
			{
				return false;
			}
		}
	}
	return true;
}