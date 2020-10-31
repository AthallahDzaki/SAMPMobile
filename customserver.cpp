#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"
#include "settings.h"
#include "customserver.h"
#include "chatwindow.h"
#include "vendor/inih/cpp/INIReader.h"
#include "str_obfuscator_no_template.hpp"

#include <string.h>
#include <stdlib.h>

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;
extern CChatWindow *pChatWindow;
extern CSettings *pSettings;

char szIPInputBuffer[100];
char utf8IPInputBuffer[100*3];

char szPortInputBuffer[100];
char utf8PortInputBuffer[100*3];

CCustomServerWindow::CCustomServerWindow()
{
	m_bIsActive = false;
	m_bFixer = false;
	m_bFixer2 = false;
	m_bLamb = false;
}

CCustomServerWindow::~CCustomServerWindow()
{

}

void CCustomServerWindow::Show(bool bShow)
{
	m_bIsActive = bShow;
	m_bFixer = false;
}

void CCustomServerWindow::Clear()
{
	m_bIsActive = false;
	m_bFixer = false;

	memset(szIPInputBuffer, 0, 100);
	memset(utf8IPInputBuffer, 0, 100*3);

	memset(szPortInputBuffer, 0, 100);
	memset(utf8PortInputBuffer, 0, 100*3);
}

void IPWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	strcpy(szIPInputBuffer, str);
	cp1251_to_utf8(utf8IPInputBuffer, str);
}

void PortWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	strcpy(szPortInputBuffer, str);
	cp1251_to_utf8(utf8PortInputBuffer, str);
}

void CCustomServerWindow::Render()
{
	if(!m_bIsActive) return;
	ImGuiIO &io = ImGui::GetIO();

	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));

	ImGui::Begin("SELECT", nullptr,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
		if(ImGui::Button("INDO RP", ImVec2(125, 50)))
		{
			const auto encryptedAddress = cryptor::create("IP", 24); unsigned short port = 1392;

			pNetGame = new CNetGame(encryptedAddress.decrypt(), port, pSettings->Get().szNickName, "PASS");
			Show(false);

			if(pGame)
	        	pGame->FindPlayerPed()->TogglePlayerControllable(true);
		}

		ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 5) );
		if(ImGui::Button("UIF", ImVec2(125, 50)))
		{
			const auto encryptedAddress = cryptor::create("IP", 24); unsigned short port = 7776;

			pNetGame = new CNetGame(encryptedAddress.decrypt(), port, pSettings->Get().szNickName, "PASS");
			Show(false);

			if(pGame)
            	pGame->FindPlayerPed()->TogglePlayerControllable(true);
		}
        ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 5) );

    ImGui::SetWindowSize(ImVec2(-1, -1));
	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y - size.y)/2)) );
	ImGui::End();

	ImGui::PopStyleVar();
}
