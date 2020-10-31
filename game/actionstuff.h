#pragma once

uint32_t IsLocalPlayerDucking();
void SetLocalPlayerDucking(uint32_t ducking);
void ToggleLocalPlayerDucking();
uint32_t IsRemotePlayerDucking(uint16_t player);
void SetRemotePlayerDucking(uint16_t playerid, uint32_t ducking);
void SetRemotePlayerDuckingReset(uint16_t player, uint32_t reset);
uint32_t IsRemotePlayerDuckingReset(uint16_t player);