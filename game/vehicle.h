#pragma once
#include "object.h"

#pragma pack(1)
typedef struct _CAR_MOD_INFO
{
	uint8_t byteCarMod0;
	uint8_t byteCarMod1;
	uint8_t byteCarMod2;
	uint8_t byteCarMod3;
	uint8_t byteCarMod4;
	uint8_t byteCarMod5;
	uint8_t byteCarMod6;
	uint8_t byteCarMod7;
	uint8_t byteCarMod8;
	uint8_t byteCarMod9;
	uint8_t byteCarMod10;
	uint8_t byteCarMod11;
	uint8_t byteCarMod12;
	uint8_t byteCarMod13;
	uint8_t bytePaintJob;
	int iColor0;
	int iColor1;
} CAR_MOD_INFO;

class CVehicle : public CEntity
{
public:
	CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f, bool bSiren = false);
	~CVehicle();

	void LinkToInterior(int iInterior);
	void SetColor(int iColor1, int iColor2);

	void SetHealth(float fHealth);
	float GetHealth();

	// 0.3.7
	bool IsOccupied();

	// 0.3.7
	void SetInvulnerable(bool bInv);
	bool IsInvulnerable();
	// 0.3.7
	bool IsDriverLocalPlayer();
	// 0.3.7
	bool HasSunk();

	void ProcessMarkers();

	void RemoveEveryoneFromVehicle();

	void SetWheelPopped(uint8_t bytePopped);
	void SetDoorState(int iState);

	void UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage);

	unsigned int GetVehicleSubtype();

	void SetEngineState(int iState);
	void SetLightsState(int iState);

	bool GetSirenState();
	void SetSirenState(bool state);
	void SetAttachedObject(CObject *pObject);
	CObject* FindAttachedEntity(ENTITY_TYPE* pEntity);


public:
	VEHICLE_TYPE	*m_pVehicle;
	bool 			m_bIsLocked;
	CVehicle		*m_pTrailer;
	uint32_t		m_dwMarkerID;
	bool 			m_bIsInvulnerable;
	bool 			m_bDoorsLocked;
	uint8_t			m_byteObjectiveVehicle; // Is this a special objective vehicle? 0/1
	uint8_t			m_bSpecialMarkerEnabled;


	uint8_t			m_byteColor1;
	uint8_t			m_byteColor2;
	bool 			m_bColorChanged;

	// Attached objects
	std::list<CObject*> m_pAttachedObjects;
};