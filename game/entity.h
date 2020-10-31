#pragma once

class CEntity
{
public:
	CEntity() {};
	virtual ~CEntity() {};
	virtual void Add();
	virtual void Remove();

	void GetMatrix(PMATRIX4X4 matEntity);
	void SetMatrix(MATRIX4X4 matEntity);	
	void SetAndUpdateMatrix(MATRIX4X4 matEntity);
 
	void GetMoveSpeedVector(PVECTOR vecSpeed);
	void SetMoveSpeedVector(VECTOR vecSpeed);

	void GetTurnSpeedVector(PVECTOR vecSpeed);
	void SetTurnSpeedVector(VECTOR vecSpeed);

	uint16_t GetModelIndex();
	void SetCollision(bool st);

	void TeleportTo(VECTOR vecPosition);
	void TeleportTo(float fX, float fY, float fZ);

	bool SetModelIndex(uint16_t usModel);
	void UpdateMatrix(MATRIX4X4 mat);

	bool IsAdded();
	uintptr_t GetRWObject();
	void Render();

	float GetDistanceFromCamera();
	float GetDistanceFromLocalPlayerPed();
	float GetDistanceFromPoint(float x, float y, float z);

	bool GetCollisionChecking();
	void SetCollisionChecking(bool bCheck);
	void SetGravityProcessing(bool bProcess);
	void UpdateRwMatrixAndFrame();

public:
	ENTITY_TYPE		*m_pEntity;
	uint32_t		m_dwGTAId;
};