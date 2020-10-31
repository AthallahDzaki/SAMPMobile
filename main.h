#pragma once

#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>
#include <malloc.h>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdarg>
#include <sys/mman.h>
#include <unistd.h>



// закомментировать если собирается в релиз
#define BUILD_BETA

#define FLINRP_VER	"1.0.2"
#define FLINRP_COPYRIGHT	"King: Angga \n Author Client: Desca Kun :v \n Modifed: ZTzTopia"

#ifdef BUILD_BETA
	#define BUILD_TAG	"Beta 2"
#else
	#define BUILD_TAG	"Stable"
#endif

#define SAMP_VERSION	"0.3.7"
#define PORT_VERSION	"0.72"

#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24

#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_BLACK 	0xFF000000
#define COLOR_ORANGE 	0xFF00A5FF
#define COLOR_ROSE		0xFFFF99FF
#define COLOR_BRED		0xFF9933FF
#define COLOR_BLUE		0xFF6C2713
#define COLOR_CYAN		0xFFCE6816
#define COLOR_1			0xFFB58891
#define COLOR_2			0xFF3291FF
#define FLIN_COLOR_ONE	0x9342F500
#define FLIN_COLOR_TWO	0xF54266FF

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969
#include "vendor/RakNet/SAMP/samp_netencr.h"
#include "vendor/RakNet/SAMP/SAMPRPC.h"

#include "str_obfuscator_no_template.hpp"
#include "util/util.h"

extern uintptr_t g_libGTASA;
extern const char* g_pszStorage;

// Инициализация `CNetGame` - подключение к игровому серверу
void initCNetGame(const char *szEncryptedHostOrIp, int port);

// AUDIO STREAM
extern int (*BASS_Init)(uint32_t, uint32_t, uint32_t);
extern int (*BASS_Free)(void);
extern int (*BASS_SetConfigPtr)(uint32_t, const char*);
extern int (*BASS_SetConfig)(uint32_t, uint32_t);
extern int (*BASS_ChannelStop)(uint32_t);
extern int (*BASS_StreamCreateURL)(char*, uint32_t, uint32_t, uint32_t);
extern int (*BASS_ChannelPlay)(uint32_t);
extern int *BASS_ChannelGetTags;
extern int *BASS_ChannelSetSync;
extern int *BASS_StreamGetFilePosition;
extern int (*BASS_StreamFree)(uint32_t);

void LoadBassLibrary();
