#include "../main.h"
#include "actionstuff.h"

uint32_t iLocalPlayerDucking = 0;
uint32_t iRemotePlayerDucking[MAX_PLAYERS];
uint32_t iPlayerDuckingReset[MAX_PLAYERS];

uint32_t IsLocalPlayerDucking()
{
    return iLocalPlayerDucking;
}

void SetLocalPlayerDucking(uint32_t ducking)
{
    iLocalPlayerDucking = ducking;
}

uint32_t IsRemotePlayerDucking(uint16_t player)
{
    if(player >= MAX_PLAYERS) return 0;
    return iRemotePlayerDucking[player];
}

void SetRemotePlayerDucking(uint16_t player, uint32_t ducking)
{
    if(player >= MAX_PLAYERS) return;
    iRemotePlayerDucking[player] = ducking;
}

void SetRemotePlayerDuckingReset(uint16_t player, uint32_t reset)
{
    if(player >= MAX_PLAYERS) return;
    iPlayerDuckingReset[player] = reset;
}

uint32_t IsRemotePlayerDuckingReset(uint16_t player)
{
    if(player >= MAX_PLAYERS) return 0;
    return iPlayerDuckingReset[player];
}

void ToggleLocalPlayerDucking()
{
    iLocalPlayerDucking = !iLocalPlayerDucking;
}