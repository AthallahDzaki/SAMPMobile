#include "main.h"
#include "admin-bot.h"
#include "chatwindow.h"
#include "game/game.h"
#include "net/netgame.h"

extern CChatWindow *pChatWindow;
extern CNetGame *pNetGame;
#define BOT_NICKNAME        "theasmgirl"

CAdminBot::CAdminBot()
{
    m_bIsActive = false;
}

CAdminBot::~CAdminBot()
{
    m_bIsActive = false;
}

void CAdminBot::ToggleBotState(bool toggle)
{
    m_bIsActive = toggle;
}

void CAdminBot::ProcessCommand(const char *szCommand)
{
    admin_command_t adm_cmd = new admin_command;
    bool m_bPush = false;
    if(strcmp(szCommand, "/status") == 0)
    {
        if(m_bIsActive)
        {
            adm_cmd->cmd = "/a [ADMBOT] Статус бота: включен";
        }
        else
        {
            adm_cmd->cmd = "/a [ADMBOT] Статус бота: выключен";
        }
        m_bPush = true;
    }
    else if(strcmp(szCommand, "/enable") == 0)
    {
        if(m_bIsActive)
        {
            adm_cmd->cmd = "/a [ADMBOT] Бот уже включен.";
        }
        else
        {
            adm_cmd->cmd = "/a [ADMBOT] Бот был включен. Используте обращение по нику для выполнения команды.";
            m_bIsActive = true;
        }
        m_bPush = true;
    }
    else if(strcmp(szCommand, "/disable") == 0)
    {
        if(m_bIsActive == false)
        {
            adm_cmd->cmd = "/a [ADMBOT] Бот уже выключен.";
        }
        else
        {
            adm_cmd->cmd = "/a [ADMBOT] Бот был выключен.";
            m_bIsActive = false;
        }
        m_bPush = true;
    }
    else
    {
        if(m_bIsActive == false)
        {
            adm_cmd->cmd = "/a [ADMBOT] Не могу выполнить команду. Бот отключен (Используйте /enable)";
        }
        else
        {
            adm_cmd->cmd = szCommand;
        }
        m_bPush = true;
    }

    if(m_bPush)
    {
        adm_cmd->passed_time = GetTickCount();
        m_pCommandQueue.push(adm_cmd);
    }
    else
    {
        delete adm_cmd;
    }
}

void CAdminBot::CheckForCommand(const char *szCommand)
{
    if(
        szCommand[0] == '[' &&
        szCommand[1] == 'A' &&
        szCommand[4] == ']'
    )
    {
        std::string text(szCommand);
        if(text.find(BOT_NICKNAME) != std::string::npos)
        {
            std::string cmd = text.substr(text.find(BOT_NICKNAME) + 10);
            if(cmd[0] == '['){ // Command was sent by me.
                if(cmd.find(BOT_NICKNAME) != std::string::npos)
                {
                    cmd = cmd.substr(cmd.find(BOT_NICKNAME) + 11);
                    if(cmd[0] == '/')
                        ProcessCommand(cmd.c_str());
                }
            }
            else if(cmd[1] == '/')
                ProcessCommand(&cmd.c_str()[1]);
        }
    }
}

void CAdminBot::Process()
{
    if((GetTickCount() - m_lastSentMessage) >= 1500)
    {
        if(!m_pCommandQueue.empty())
        {
            admin_command_t adm_cmd = m_pCommandQueue.front();
            if((GetTickCount() - adm_cmd->passed_time) >= 2000)
            {
                m_pCommandQueue.pop();
                pNetGame->SendChatCommand(adm_cmd->cmd.c_str());
                delete adm_cmd;
                m_lastSentMessage = GetTickCount();
            }
        }
    }
}