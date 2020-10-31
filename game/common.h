#pragma once

#define PLAYER_PED_SLOTS	140

typedef unsigned short VEHICLEID;
typedef unsigned short PLAYERID;

#define PADDING(x,y) uint8_t x[y]

#define IN_VEHICLE(x) ((x->dwStateFlags & 0x100) >> 8)

enum eStreamingFlags {
    GAME_REQUIRED = 0x2,
    MISSION_REQUIRED = 0x4,
    KEEP_IN_MEMORY = 0x8,
    PRIORITY_REQUEST = 0x10
};

//-----------------------------------------------------------
typedef struct _RECT
{
	float fLeft;
	float fBottom;
	float fRight;
	float fTop;
} RECT, *PRECT;

#pragma pack(1)
typedef struct _VECTOR
{
	float X,Y,Z;
} VECTOR, *PVECTOR;

#pragma pack(1)
typedef struct _MATRIX4X4
{
	VECTOR right;		// 0-12 	; r11 r12 r13
	uint32_t  flags;	// 12-16
	VECTOR up;			// 16-28	; r21 r22 r23
	float  pad_u;		// 28-32
	VECTOR at;			// 32-44	; r31 r32 r33
	float  pad_a;		// 44-48
	VECTOR pos;			// 48-60
	float  pad_p;		// 60-64
} MATRIX4X4, *PMATRIX4X4;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _ENTITY_TYPE {
	uint32_t vtable;

	PADDING(_pad0, 12);

	float fRotZBeforeMat;

	MATRIX4X4 *mat;

	union {
		uintptr_t m_pRwObject;
		uintptr_t m_pRpClump;
		uintptr_t m_pRpAtomic;
		uintptr_t pdwRenderWare;	// 24-28	;rwObject
	};

	union {
		uint32_t m_nEntityFlags;
		struct {
			uint32_t m_bUsesCollision : 1;
			uint32_t m_bCollisionProcessed : 1;
			uint32_t m_bIsStatic : 1;
			uint32_t m_bHasContacted : 1;
			uint32_t m_bIsStuck : 1;
			uint32_t m_bIsInSafePosition : 1;
			uint32_t m_bWasPostponed : 1;
			uint32_t m_bIsVisible : 1;

			uint32_t m_bIsBIGBuilding : 1;
			uint32_t m_bRenderDamaged : 1;
			uint32_t m_bStreamingDontDelete : 1;
			uint32_t m_bRemoveFromWorld : 1;
			uint32_t m_bHasHitWall : 1;
			uint32_t m_bImBeingRendered : 1;
			uint32_t m_bDrawLast :1;
			uint32_t m_bDistanceFade : 1;

			uint32_t m_bDontCastShadowsOn : 1;
			uint32_t m_bOffscreen : 1;
			uint32_t m_bIsStaticWaitingForCollision : 1;
			uint32_t m_bDontStream : 1;
			uint32_t m_bUnderwater : 1;
			uint32_t m_bHasPreRenderEffects : 1;
			uint32_t m_bIsTempBuilding : 1;
			uint32_t m_bDontUpdateHierarchy : 1;

			uint32_t m_bHasRoadsignText : 1;
			uint32_t m_bDisplayedSuperLowLOD : 1;
			uint32_t m_bIsProcObject : 1;
			uint32_t m_bBackfaceCulled : 1;
			uint32_t m_bLightObject : 1;
			uint32_t m_bUnimportantStream : 1;
			uint32_t m_bTunnel : 1;
			uint32_t m_bTunnelTransition : 1;
		} nEntityFlags;
	};

	PADDING(_pad1, 2);

	uint16_t nModelIndex;

	PADDING(_pad2, 32);

	VECTOR vecMoveSpeed;
	VECTOR vecTurnSpeed;

	PADDING(_pad3, 88);

	uint32_t dwUnkModelRel;
} ENTITY_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _WEAPON_SLOT_TYPE
{
	uint32_t dwType;
	uint32_t dwState;
	uint32_t dwAmmoInClip;
	uint32_t dwAmmo;
	PADDING(_pwep1,12);
} WEAPON_SLOT_TYPE;  // MUST BE EXACTLY ALIGNED TO 28 bytes

typedef struct
{
	char unk[0x14];
	int iNodeId;

} AnimBlendFrameData;

#pragma pack(1)
typedef struct _PED_TYPE
{
	ENTITY_TYPE entity; 		// 0000-0184	;entity
	PADDING(_pad106, 174);		// 0184-0358
	uint32_t _pad107;			// 0358-0362	;dwPedType
	PADDING(_pad101, 734);		// 0362-1096
	uint32_t dwAction;			// 1096-1100	;Action
	PADDING(_pad102, 36);		// 1100-1136
	uintptr_t dwInvulFlags; 	// 1136-1140	0x1000 = can_decap
	PADDING(_pad228, 8); 		// 1140-1148
	uintptr_t Tasks; 			// 1148-1152
	uint32_t dwStateFlags; 		// 1152-1156	;StateFlags
	PADDING(_pad103, 12);		// 1156-1168
	AnimBlendFrameData* aPedBones[19];	// 1168 - 1244
	PADDING(_pad103_, 100);		// 1244-1344
	float fHealth;		 		// 1344-1348	;Health
	float fMaxHealth;			// 1348-1352	;MaxHealth
	float fArmour;				// 1352-1356	;Armour
	float fAim;
	PADDING(_pad104, 8);		// 1356-1368
	float fRotation1;			// 1368-1372	;Rotation1
	float fRotation2;			// 1372-1376	;Rotation2
	PADDING(_pad105, 44);		// 1376-1420
	uint32_t pVehicle;			// 1420-1424	;pVehicle
	PADDING(_pad108, 8);		// 1424-1432
	uint32_t dwPedType;			// 1432-1436	;dwPedType
	uint32_t dwUnk1;	 // 1436-1440
	WEAPON_SLOT_TYPE WeaponSlots[13]; // 1440-1804
	PADDING(_pad270, 12); // 1804-1816
	uint8_t byteCurWeaponSlot; // 1816-1817
	PADDING(_pad280, 23); // 1817-1840
	uint32_t pFireObject;	 // 1840-1844
	PADDING(_pad281, 44); // 1844-1888
	uint32_t  dwWeaponUsed; // 1888-1892
	uintptr_t pdwDamageEntity; // 1892-1896
} PED_TYPE;

#pragma pack(1)
struct AIM_SYNC_DATA
{
	uint8_t	byteCamMode;
	float	vecAimf1[3];
	float	vecAimPos[3];
	float	fAimZ;
	uint8_t	byteCamExtZoom : 6;	// 0-63 normalized
	uint8_t	byteWeaponState : 2;		// see eWeaponState
	uint8_t	bUnk;
};
//-----------------------------------------------------------

#pragma pack(1)
struct BULLET_SYNC
{
	uint8_t hitType;
	uint16_t hitId;
	float origin[3];
	float hitPos[3];
	float offsets[3];
	uint8_t weapId;
};

#pragma pack(1)
typedef struct _VEHICLE_TYPE
{
	ENTITY_TYPE entity;			// 0000-0184	;entity
	PADDING(_pad201, 885);		// 0184-1069
	uint8_t _pad2011 : 7;		// 1069-1070 	(bits 0..6)
	uint8_t byteSirenOn : 1;	// 1069-1070	(bit 7)
	PADDING(_pad2012, 6);		// 1070-1076
	uint8_t byteColor1;			// 1076-1077	;byteColor1
	uint8_t byteColor2;			// 1077-1078	;byteColor2
	PADDING(_pad204, 42);		// 1078-1120
	PED_TYPE *pDriver;			// 1120-1124	;driver
	PED_TYPE *pPassengers[7];	// 1124-1152	;pPassengers
	PADDING(_pad202, 72);		// 1152-1224
	float fHealth;				// 1224-1228	;fHealth
	PADDING(_pad203, 56);		// 1228-1284
	uint32_t dwDoorsLocked;		// 1284-1288	;dwDoorsLocked
} VEHICLE_TYPE;

#pragma pack(1)
typedef struct _VEHICLE_PARAMS_STATUS
{
	int bEngine;
	int bLights;
	int bAlarm;
	int bDoors;
	int bBonnet;
	int bBoot;
	int bObjective;
	//doors
	int bDriver;
	int bPassenger;
	int bBackleft;
	int bBackright;
} VEHICLE_PARAMS_STATUS;

//-----------------------------------------------------------

#define	VEHICLE_SUBTYPE_CAR				1
#define	VEHICLE_SUBTYPE_BIKE			2
#define	VEHICLE_SUBTYPE_HELI			3
#define	VEHICLE_SUBTYPE_BOAT			4
#define	VEHICLE_SUBTYPE_PLANE			5
#define	VEHICLE_SUBTYPE_PUSHBIKE		6
#define	VEHICLE_SUBTYPE_TRAIN			7

//-----------------------------------------------------------

#define TRAIN_PASSENGER_LOCO			538
#define TRAIN_FREIGHT_LOCO				537
#define TRAIN_PASSENGER					570
#define TRAIN_FREIGHT					569
#define TRAIN_TRAM						449
#define HYDRA							520

//-----------------------------------------------------------

#define ACTION_WASTED					55
#define ACTION_DEATH					54
#define ACTION_INCAR					50
#define ACTION_NORMAL					1
#define ACTION_SCOPE					12
#define ACTION_NONE						0

//-----------------------------------------------------------

#define WEAPON_MODEL_BRASSKNUCKLE		331 // was 332
#define WEAPON_MODEL_GOLFCLUB			333
#define WEAPON_MODEL_NITESTICK			334
#define WEAPON_MODEL_KNIFE				335
#define WEAPON_MODEL_BAT				336
#define WEAPON_MODEL_SHOVEL				337
#define WEAPON_MODEL_POOLSTICK			338
#define WEAPON_MODEL_KATANA				339
#define WEAPON_MODEL_CHAINSAW			341
#define WEAPON_MODEL_DILDO				321
#define WEAPON_MODEL_DILDO2				322
#define WEAPON_MODEL_VIBRATOR			323
#define WEAPON_MODEL_VIBRATOR2			324
#define WEAPON_MODEL_FLOWER				325
#define WEAPON_MODEL_CANE				326
#define WEAPON_MODEL_GRENADE			342 // was 327
#define WEAPON_MODEL_TEARGAS			343 // was 328
#define WEAPON_MODEL_MOLOTOV			344 // was 329
#define WEAPON_MODEL_COLT45				346
#define WEAPON_MODEL_SILENCED			347
#define WEAPON_MODEL_DEAGLE				348
#define WEAPON_MODEL_SHOTGUN			349
#define WEAPON_MODEL_SAWEDOFF			350
#define WEAPON_MODEL_SHOTGSPA			351
#define WEAPON_MODEL_UZI				352
#define WEAPON_MODEL_MP5				353
#define WEAPON_MODEL_AK47				355
#define WEAPON_MODEL_M4					356
#define WEAPON_MODEL_TEC9				372
#define WEAPON_MODEL_RIFLE				357
#define WEAPON_MODEL_SNIPER				358
#define WEAPON_MODEL_ROCKETLAUNCHER		359
#define WEAPON_MODEL_HEATSEEKER			360
#define WEAPON_MODEL_FLAMETHROWER		361
#define WEAPON_MODEL_MINIGUN			362
#define WEAPON_MODEL_SATCHEL			363
#define WEAPON_MODEL_BOMB				364
#define WEAPON_MODEL_SPRAYCAN			365
#define WEAPON_MODEL_FIREEXTINGUISHER	366
#define WEAPON_MODEL_CAMERA				367
#define WEAPON_MODEL_NIGHTVISION		368	// newly added
#define WEAPON_MODEL_INFRARED			369	// newly added
#define WEAPON_MODEL_JETPACK			370	// newly added
#define WEAPON_MODEL_PARACHUTE			371

#define WEAPON_MODEL_PARACHUTE			371
#define WEAPON_FIST                        0
#define WEAPON_BRASSKNUCKLE                1
#define WEAPON_GOLFCLUB                    2
#define WEAPON_NITESTICK                3
#define WEAPON_KNIFE                    4
#define WEAPON_BAT                        5
#define WEAPON_SHOVEL                    6
#define WEAPON_POOLSTICK                7
#define WEAPON_KATANA                    8
#define WEAPON_CHAINSAW                    9
#define WEAPON_DILDO                    10
#define WEAPON_DILDO2                    11
#define WEAPON_VIBRATOR                    12
#define WEAPON_VIBRATOR2                13
#define WEAPON_FLOWER                    14
#define WEAPON_CANE                        15
#define WEAPON_GRENADE                    16
#define WEAPON_TEARGAS                    17
#define WEAPON_MOLTOV                    18
#define WEAPON_COLT45                    22
#define WEAPON_SILENCED                    23
#define WEAPON_DEAGLE                    24
#define WEAPON_SHOTGUN                    25
#define WEAPON_SAWEDOFF                    26
#define WEAPON_SHOTGSPA                    27
#define WEAPON_UZI                        28
#define WEAPON_MP5                        29
#define WEAPON_AK47                        30
#define WEAPON_M4                        31
#define WEAPON_TEC9                        32
#define WEAPON_RIFLE                    33
#define WEAPON_SNIPER                    34
#define WEAPON_ROCKETLAUNCHER            35
#define WEAPON_HEATSEEKER                36
#define WEAPON_FLAMETHROWER                37
#define WEAPON_MINIGUN                    38
#define WEAPON_SATCHEL                    39
#define WEAPON_BOMB                        40
#define WEAPON_SPRAYCAN                    41
#define WEAPON_FIREEXTINGUISHER            42
#define WEAPON_CAMERA                    43
#define WEAPON_PARACHUTE                46
#define WEAPON_VEHICLE                    49
#define WEAPON_HELIBLADES				50
#define WEAPON_EXPLOSION				51
#define WEAPON_DROWN                    53
#define WEAPON_COLLISION                54
#define WEAPON_UNKNOWN					0xFF

#define OBJECT_PARACHUTE				3131
#define OBJECT_CJ_CIGGY					1485
#define OBJECT_DYN_BEER_1				1486
#define OBJECT_CJ_BEER_B_2				1543
#define OBJECT_CJ_PINT_GLASS			1546

// PED BONES
enum ePedBones
{
	BONE_PELVIS1 = 1,
	//BONE_PELVIS = 2,
	BONE_SPINE1 = 3,
	BONE_UPPERTORSO = 4,
	BONE_NECK = 5,
	BONE_HEAD2 = 6,
	BONE_HEAD1 = 7,
	BONE_HEAD = 8,
	BONE_RIGHTUPPERTORSO = 21,
	BONE_RIGHTSHOULDER = 22,
	BONE_RIGHTELBOW = 23,
	BONE_RIGHTWRIST = 24,
	BONE_RIGHTHAND = 25,
	BONE_RIGHTTHUMB = 26,
	BONE_LEFTUPPERTORSO = 31,
	BONE_LEFTSHOULDER = 32,
	BONE_LEFTELBOW = 33,
	BONE_LEFTWRIST = 34,
	BONE_LEFTHAND = 35,
	BONE_LEFTTHUMB = 36,
	BONE_LEFTHIP = 41,
	BONE_LEFTKNEE = 42,
	BONE_LEFTANKLE = 43,
	BONE_LEFTFOOT = 44,
	BONE_RIGHTHIP = 51,
	BONE_RIGHTKNEE = 52,
	BONE_RIGHTANKLE = 53,
	BONE_RIGHTFOOT = 54,
};
