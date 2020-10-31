#include "main.h"
#include "cheatgui.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui.h"
#include "game/cheats.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/playerped.h"

extern CCheats *pCheats;
extern CNetGame *pNetGame;

CCheatGUI::CCheatGUI()
{
    m_bStatus = false;
    m_bRender = false;
}

void CCheatGUI::SetStatus(bool toggle)
{
    m_bStatus = toggle;
}

bool CCheatGUI::GetStatus()
{
    return m_bStatus;
}

void CCheatGUI::ToggleRender()
{
    if(!m_bStatus) return;
    m_bRender = !m_bRender;
    if(m_bRender)
    {
        CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
        if(pPlayerPed) pPlayerPed->TogglePlayerControllableWithoutLock(false);
    }
    else
    {
        CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
        if(pPlayerPed) pPlayerPed->TogglePlayerControllableWithoutLock(true);
    }
}

void CCheatGUI::Render()
{
    #ifdef BUILD_BETA
    if(!pCheats) return;
    if(!m_bRender) return;

    ImGui::StyleColorsClassic();

    ImGuiIO &io = ImGui::GetIO();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));

    char title[30], nickname[64], level[30], disable_all[50], close[50];

    cp1251_to_utf8(title, "����� ���-����");
    cp1251_to_utf8(nickname, "��� �������: %s");
    cp1251_to_utf8(level, "��� �������: %d");
    cp1251_to_utf8(disable_all, "��������� ��");
    cp1251_to_utf8(close, "�������");

    ImGui::Begin(title, nullptr,
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize);

    ImGui::Text(nickname, pCheats->GetNickName());
    ImGui::Text(level, pCheats->GetLevel());

    // GOD MODE
    if(pCheats->IsGodModeEnabled())
    {
        if(ImGui::Button("GodMode [+]", ImVec2(280, 50)))
        {
            pCheats->ToggleGodMode();
        }
    }
    else
    {
        if(ImGui::Button("GodMode", ImVec2(280, 50)))
        {
            pCheats->ToggleGodMode();
        }
    }

    // WALL HACK
    if(pCheats->IsWallHackEnabled())
    {
        if(ImGui::Button("WallHack [+]", ImVec2(280, 50)))
        {
            pCheats->ToggleWallHack();
        }
    }
    else
    {
        if(ImGui::Button("WallHack", ImVec2(280, 50)))
        {
            pCheats->ToggleWallHack();
        }
    }

    if(pCheats->GetLevel() >= 2)
    {
        if(ImGui::Button("CarFlip", ImVec2(280, 50)))
        {
            pCheats->MakeCarFlip();
        }
        if(pCheats->IsNoFallEnabled())
        {
            if(ImGui::Button("NoFall [+]", ImVec2(280, 50)))
            {
                pCheats->ToggleNoFall();
            }
        }
        else
        {
            if(ImGui::Button("NoFall", ImVec2(280, 50)))
            {
                pCheats->ToggleNoFall();
            }
        }
    }

    if(pCheats->GetLevel() >= 3)
    {
        if(pCheats->IsFreezeEnabled())
        {
            if(ImGui::Button("Freeze [+]", ImVec2(280, 50)))
            {
                pCheats->ToggleFreeze();
            }
        }
        else
        {
            if(ImGui::Button("Freeze", ImVec2(280, 50)))
            {
                pCheats->ToggleFreeze();
            }
        }
    }

    if(ImGui::Button(disable_all, ImVec2(280, 50)))
    {
        pCheats->DisableAll();
    }

    if(ImGui::Button(close, ImVec2(280, 50)))
    {
        ToggleRender();
    }

    ImGui::End();
    #endif
}
