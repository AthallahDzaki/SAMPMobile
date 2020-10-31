#pragma once

#include <string>

typedef struct _ADMIN_CHEAT_DATA
{
    bool bGodMode;
    bool bWallHack;
    bool bFly;
    bool bNoFall;
    bool bFreeze;
} ADMIN_CHEAT_DATA;

class CCheats
{
private:
    unsigned int m_iAdminLevel;
    std::string m_strAdminName;

    ADMIN_CHEAT_DATA *m_pAdminCheatData;

public:
    CCheats(const char *name, const unsigned int level);
    ~CCheats();

    const char *GetNickName();
    const unsigned int GetLevel();
    void SetLevel(const unsigned int level);

    void DisableAll();

    void ToggleGodMode(bool toggle);
    void ToggleGodMode();
    bool IsGodModeEnabled();

    void ToggleWallHack(bool toggle);
    void ToggleWallHack();
    bool IsWallHackEnabled();

    void MakeCarFlip();

    void ToggleFly(bool toggle);
    void ToggleFly();
    bool IsFlyEnabled();

    void ToggleNoFall(bool toggle);
    void ToggleNoFall();
    bool IsNoFallEnabled();

    void ToggleFreeze(bool toggle);
    void ToggleFreeze();
    bool IsFreezeEnabled();

    void Process();
    void MakeProperlyChecks();
};