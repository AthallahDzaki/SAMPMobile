#pragma once

PED_TYPE* GamePool_FindPlayerPed();
PED_TYPE* GamePool_Ped_GetAt(int iID);
int GamePool_Ped_GetIndex(PED_TYPE *pActor);

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID);
int GamePool_Vehicle_GetIndex(VEHICLE_TYPE *pVehicle);

ENTITY_TYPE *GamePool_Object_GetAt(int iID);

int LineOfSight(VECTOR* start, VECTOR* end, void* colpoint, uintptr_t ent,
	char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk);

bool IsPedModel(unsigned int uiModel);
bool IsValidModel(unsigned int uiModelID);
uint16_t GetModelReferenceCount(int nModelIndex);

bool IsValidGameEntity(ENTITY_TYPE *pEntity);
bool IsGameEntityArePlaceable(ENTITY_TYPE *pEntity);

void InitPlayerPedPtrRecords();
void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr);
uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr);
bool PreloadAnimFile(char *szAnimFile, uint16_t usSleep, uint16_t usHowTimes);
bool IsValidGamePed(PED_TYPE *pPed);

uintptr_t GetTexture(const char* texture);
uintptr_t LoadTextureFromDB(const char* dbname, const char* texture);
void DefinedState2d();
void SetScissorRect(void* pRect);
float DegToRad(float fDegrees);
// 0.3.7
float FloatOffset(float f1, float f2);

bool IsPointInRect(float x, float y, RECT* rect);

/// kek
void ProjectMatrix(VECTOR* vecOut, MATRIX4X4* mat, VECTOR *vecPos);
void RwMatrixRotate(MATRIX4X4 *mat, int axis, float angle);
void RwMatrixScale(MATRIX4X4 *mat, VECTOR *vecScale);

int GetFreeTextDrawTextureSlot();
void DestroyTextDrawTexture(int index);
uintptr_t LoadTexture(const char* texname);
void DrawTextureUV(uintptr_t texture, RECT* rect, uint32_t dwColor, float *uv);

void WorldAddEntity(uintptr_t pEnt);
void WorldRemoveEntity(uintptr_t pEnt);
uintptr_t GetModelInfoByID(int iModelID);

void DestroyAtomicOrClump(uintptr_t rwObject);
void GetModelColSphereVecCenter(int iModel, VECTOR* vec);
float GetModelColSphereRadius(int iModel);
void RenderClumpOrAtomic(uintptr_t rwObject);
uintptr_t ModelInfoCreateInstance(int iModel);