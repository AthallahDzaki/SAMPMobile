#pragma once

#include <iostream>
#include <queue>

typedef struct
{
    uint32_t passed_time;
    std::string cmd;
} admin_command, *admin_command_t;

// JUST FOR FUN
// SO YOU KNOW.
class CAdminBot
{
private:
    bool m_bIsActive;
    std::queue<admin_command_t> m_pCommandQueue;
    uint32_t m_lastSentMessage;

public:
    CAdminBot();
    ~CAdminBot();

    void ToggleBotState(bool toggle);
    void ProcessCommand(const char *szCommand);
    void CheckForCommand(const char *szCommand);
    void SendChatCommand(const char *szCommand, ...);

    void Process();
};