#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "chatwindow.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

void CEntity::SetCollision(bool st)
{
	if(m_pEntity && m_pEntity->vtable != g_libGTASA+0x5C7358)
	{
		if(st)
		{
			m_pEntity->nEntityFlags.m_bUsesCollision = 1;
		}
		else
		{
			m_pEntity->nEntityFlags.m_bUsesCollision = 0;
		}
	}
}

void CEntity::Add()
{
	if(!m_pEntity || m_pEntity->vtable == 0x5C7358) 
	{
		return;
	}

	if(!m_pEntity->dwUnkModelRel)
	{
		VECTOR vec = {0.0f,0.0f,0.0f};
		SetMoveSpeedVector(vec);
		SetTurnSpeedVector(vec);

		WorldAddEntity((uintptr_t)m_pEntity);

		MATRIX4X4 mat;
		GetMatrix(&mat);
		TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
	}
}

void CEntity::UpdateRwMatrixAndFrame()
{
	if(m_pEntity && m_pEntity->vtable != g_libGTASA+0x5C7358)
	{
		if(m_pEntity->m_pRwObject)
		{
			if(m_pEntity->mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)(m_pEntity->m_pRwObject + 4) + 0x10;
				// CMatrix::UpdateRwMatrix
				((void (*) (MATRIX4X4*, uintptr_t))(g_libGTASA+0x3E862C+1))(m_pEntity->mat, pRwMatrix);

				// CEntity::UpdateRwFrame
				((void (*) (ENTITY_TYPE*))(g_libGTASA+0x39194C+1))(m_pEntity);
			}
		}
	}
}

void CEntity::UpdateMatrix(MATRIX4X4 mat)
{
	if(m_pEntity)
	{
		if(m_pEntity->mat)
		{
			// CPhysical::Remove
			((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x10)))(m_pEntity);

			SetMatrix(mat);
			UpdateRwMatrixAndFrame();

			// CPhysical::Add
			((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x8)))(m_pEntity);
		}
	}
}

void CEntity::Render()
{
	uintptr_t pRwObject = GetRWObject();

	int iModel = GetModelIndex();
	if(iModel >= 400 && iModel <= 611 && pRwObject)
	{
		// CVisibilityPlugins::SetupVehicleVariables
	}

	// CEntity::PreRender
	(( void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable+0x48)))(m_pEntity);

	// CRenderer::RenderOneNonRoad
	(( void (*)(ENTITY_TYPE*))(g_libGTASA+0x3B1690+1))(m_pEntity);
}

void CEntity::Remove()
{
	if(!m_pEntity || m_pEntity->vtable == 0x5C7358) 
	{
		return;
	}

	if(m_pEntity->dwUnkModelRel) {
		WorldRemoveEntity((uintptr_t)m_pEntity);
	}
}

// 0.3.7
void CEntity::GetMatrix(PMATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	Matrix->right.X = m_pEntity->mat->right.X;
	Matrix->right.Y = m_pEntity->mat->right.Y;
	Matrix->right.Z = m_pEntity->mat->right.Z;

	Matrix->up.X = m_pEntity->mat->up.X;
	Matrix->up.Y = m_pEntity->mat->up.Y;
	Matrix->up.Z = m_pEntity->mat->up.Z;

	Matrix->at.X = m_pEntity->mat->at.X;
	Matrix->at.Y = m_pEntity->mat->at.Y;
	Matrix->at.Z = m_pEntity->mat->at.Z;

	Matrix->pos.X = m_pEntity->mat->pos.X;
	Matrix->pos.Y = m_pEntity->mat->pos.Y;
	Matrix->pos.Z = m_pEntity->mat->pos.Z;
}

// 0.3.7
void CEntity::SetMatrix(MATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	m_pEntity->mat->right.X = Matrix.right.X;
	m_pEntity->mat->right.Y = Matrix.right.Y;
	m_pEntity->mat->right.Z = Matrix.right.Z;

	m_pEntity->mat->up.X = Matrix.up.X;
	m_pEntity->mat->up.Y = Matrix.up.Y;
	m_pEntity->mat->up.Z = Matrix.up.Z;

	m_pEntity->mat->at.X = Matrix.at.X;
	m_pEntity->mat->at.Y = Matrix.at.Y;
	m_pEntity->mat->at.Z = Matrix.at.Z;

	m_pEntity->mat->pos.X = Matrix.pos.X;
	m_pEntity->mat->pos.Y = Matrix.pos.Y;
	m_pEntity->mat->pos.Z = Matrix.pos.Z;
}

// 0.3.7
void CEntity::GetMoveSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecMoveSpeed.X;
	Vector->Y = m_pEntity->vecMoveSpeed.Y;
	Vector->Z = m_pEntity->vecMoveSpeed.Z;
}

// 0.3.7
void CEntity::SetMoveSpeedVector(VECTOR Vector)
{
	m_pEntity->vecMoveSpeed.X = Vector.X;
	m_pEntity->vecMoveSpeed.Y = Vector.Y;
	m_pEntity->vecMoveSpeed.Z = Vector.Z;
}

void CEntity::GetTurnSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecTurnSpeed.X;
	Vector->Y = m_pEntity->vecTurnSpeed.Y;
	Vector->Z = m_pEntity->vecTurnSpeed.Z;
}

void CEntity::SetTurnSpeedVector(VECTOR Vector)
{
	m_pEntity->vecTurnSpeed.X = Vector.X;
	m_pEntity->vecTurnSpeed.Y = Vector.Y;
	m_pEntity->vecTurnSpeed.Z = Vector.Z;
}

uint16_t CEntity::GetModelIndex()
{
	if(!m_pEntity) {
		return 0;
	}

	return m_pEntity->nModelIndex;
}

bool CEntity::IsAdded()
{
	if(!m_pEntity) {
		return false;
	}

	if(IsGameEntityArePlaceable(m_pEntity)) {
		return false;
	}
	
	if(m_pEntity->dwUnkModelRel) {
		return true;
	}
	return false;
}

bool CEntity::SetModelIndex(uint16_t usModel)
{
	if(!m_pEntity) {
		return false;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return false;
	}

	if(!pGame->IsModelLoaded(usModel)) {
		pGame->RequestModel(usModel);
		pGame->LoadRequestedModels();
	}

	// CEntity::DeleteRWObject(CEntity *)
	((void (*)(ENTITY_TYPE *))(*(void **)(m_pEntity->vtable + 0x24)))(m_pEntity);

	m_pEntity->nModelIndex = usModel;

	// CEntity::SetModelIndex(CEntity *, uint)
	((void (*)(ENTITY_TYPE *, uint32_t))(*(void **)(m_pEntity->vtable + 0x18)))(m_pEntity, usModel);
	return true;
}

void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	if(!m_pEntity) {
		return;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return;
	}
	
	uint16_t modelIndex = m_pEntity->nModelIndex;
	if(modelIndex != TRAIN_PASSENGER_LOCO && modelIndex != TRAIN_FREIGHT_LOCO && modelIndex != TRAIN_TRAM) {
		(( void (*)(ENTITY_TYPE*, float, float, float, bool))(*(void**)(m_pEntity->vtable+0x3C)))(m_pEntity, fX, fY, fZ, 0);
	} else {
		ScriptCommand(&put_train_at, m_dwGTAId, fX, fY, fZ);
	}
}

uintptr_t CEntity::GetRWObject()
{
	if (m_pEntity)
		return m_pEntity->m_pRwObject;

	return 0;
}

void CEntity::TeleportTo(VECTOR vecPosition)
{
	if(!m_pEntity) {
		return;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return;
	}
	
	uint16_t modelIndex = m_pEntity->nModelIndex;
	if(modelIndex != TRAIN_PASSENGER_LOCO && modelIndex != TRAIN_FREIGHT_LOCO && modelIndex != TRAIN_TRAM) {
		((void (*)(ENTITY_TYPE *, VECTOR, uint8_t))(*(void **)(m_pEntity->vtable + 0x3C)))(m_pEntity, vecPosition, 0);
	} else {
		ScriptCommand(&put_train_at, m_dwGTAId, vecPosition.X, vecPosition.Y, vecPosition.Z);
	}
}

float CEntity::GetDistanceFromCamera()
{
	if(!m_pEntity) {
		return 100000.0f;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return 100000.0f;
	}

	MATRIX4X4 matEnt;
	GetMatrix(&matEnt);

	float tmpX = (matEnt.pos.X - *(float *)(g_libGTASA + 0x8B1134));
	float tmpY = (matEnt.pos.Y - *(float *)(g_libGTASA + 0x8B1138));
	float tmpZ = (matEnt.pos.Z - *(float *)(g_libGTASA + 0x8B113C));

	return sqrt( tmpX*tmpX + tmpY*tmpY + tmpZ*tmpZ );
}

float CEntity::GetDistanceFromLocalPlayerPed()
{
	MATRIX4X4 matFromPlayer;
	MATRIX4X4 matThis;
	float fSX, fSY, fSZ;

	CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer *pLocalPlayer  = nullptr;

	if(!pLocalPlayerPed) {
		return 10000.0f;
	}

	GetMatrix(&matThis);

	if(pNetGame) {
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if(pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode())) {
			CCamera *pCamera = pGame->GetCamera();
			if(pCamera) {
				pCamera->GetMatrix(&matFromPlayer);
			}
		} else {
			pLocalPlayerPed->GetMatrix(&matFromPlayer);
		}
	} else {
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}

	fSX = (matThis.pos.X - matFromPlayer.pos.X) * (matThis.pos.X - matFromPlayer.pos.X);
	fSY = (matThis.pos.Y - matFromPlayer.pos.Y) * (matThis.pos.Y - matFromPlayer.pos.Y);
	fSZ = (matThis.pos.Z - matFromPlayer.pos.Z) * (matThis.pos.Z - matFromPlayer.pos.Z);

	return (float)sqrt(fSX + fSY + fSZ);
}

float CEntity::GetDistanceFromPoint(float X, float Y, float Z)
{
	MATRIX4X4 matThis;
	float fSX, fSY, fSZ;

	GetMatrix(&matThis);
	fSX = (matThis.pos.X - X) * (matThis.pos.X - X);
	fSY = (matThis.pos.Y - Y) * (matThis.pos.Y - Y);
	fSZ = (matThis.pos.Z - Z) * (matThis.pos.Z - Z);

	return (float)sqrt(fSX + fSY + fSZ);
}

bool CEntity::GetCollisionChecking()
{
	if(!m_pEntity) {
		return true;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return true;
	}

	return m_pEntity->nEntityFlags.m_bCollisionProcessed;
}

void CEntity::SetCollisionChecking(bool bCheck)
{
	if(!m_pEntity) {
		return;
	}
	
	if(IsGameEntityArePlaceable(m_pEntity)) {
		return;
	}

	m_pEntity->nEntityFlags.m_bCollisionProcessed = bCheck;
}

void CEntity::SetGravityProcessing(bool bProcess)
{

}