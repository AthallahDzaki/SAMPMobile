#include "main.h"
#include "gui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "chatwindow.h"
#include "spawnscreen.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "debug.h"
#include "settings.h"
#include "scoreboard.h"
#include "cheatgui.h"
#include "consolegui.h"
#include "button.h"
#include "net/textdrawpool.h"
#include "customserver.h"

extern CChatWindow *pChatWindow;
extern CSpawnScreen *pSpawnScreen;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CDebug *pDebug;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CScoreBoard *pScoreBoard;
extern CCheatGUI *pCheatGUI;
extern CConsoleGUI *pConsoleGUI;
extern CCustomServerWindow* pCustomServer;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

/*
	Все координаты GUI-элементов задаются
	относительно разрешения 1920x1080
*/
#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080

CGUI::CGUI()
{
	Log("Initializing GUI..");

	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;
	// font Size
	m_fFontSize = ScaleY( pSettings->Get().fFontSize );

	// mouse/touch
	m_bMousePressed = false;
	m_vecMousePos = ImVec2(0, 0);

	// buttons
	m_bKeysStatus = false;

	Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);

	// ImGui::StyleColorsClassic();
	ImGui::StyleColorsDark();
	StyleColorsBrand();

	// load fonts
	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, pSettings->Get().szFont);
	// cp1251 ranges
	static const ImWchar ranges[] =
	{
		0x0020, 0x0080,
		0x00A0, 0x00C0,
		0x0400, 0x0460,
		0x0490, 0x04A0,
		0x2010, 0x2040,
		0x20A0, 0x20B0,
		0x2110, 0x2130,
		0
	};
	Log("GUI | Loading font: %s", pSettings->Get().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	Log("GUI | ImFont pointer = 0x%X", m_pFont);
}

void CGUI::StyleColorsBrand()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	//style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	//style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 1.0f;
	//style->ItemSpacing = ImVec2(12, 8);
	//style->ItemInnerSpacing = ImVec2(8, 6);
	//style->IndentSpacing = 25.0f;
	style->ScrollbarSize = ScaleY(38.0f);
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->WindowBorderSize = 0.0f;
	style->ChildBorderSize  = 0.1f;
	style->PopupBorderSize  = 0.1f;
	style->FrameBorderSize  = 0.8f;
	style->Alpha            = 0.85f;

	colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	/*
	colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.78f);
	colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.12f);
	*/
	colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);

	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 0.75f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	// colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	/*
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	*/

	colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 0.80f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 0.90f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);

	colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	//colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	//colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	//colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}

void CGUI::Render()
{
	ImGuiIO& io = ImGui::GetIO();

	/*if(pNetGame && pNetGame->GetTextDrawPool())
	{
		pNetGame->GetTextDrawPool()->Draw();
	}*/

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	RenderVersion();

	#ifdef BUILD_BETA
		// TODO: RenderRakNetStatistics();
	#endif

	if(pPlayerTags) pPlayerTags->Render();

	if(pNetGame && pNetGame->GetLabelPool())
	{
		pNetGame->GetLabelPool()->Render();
	}

	if(pNetGame && pNetGame->GetPlayerBubblePool())
	{
		pNetGame->GetPlayerBubblePool()->Render();
	}

	if(pChatWindow) pChatWindow->Render();

	if(pDialogWindow) pDialogWindow->Render();

	if(pSpawnScreen) pSpawnScreen->Render();
	if(pKeyBoard) pKeyBoard->Render();

	if(pScoreBoard) pScoreBoard->Draw();
	if(pCheatGUI) pCheatGUI->Render();

	if(pConsoleGUI) pConsoleGUI->Draw();

	if(pCustomServer) pCustomServer->Render();


	/* Левое плавающее раскрывающееся GUI меню */
	// не показываем меню на экране выбора сервера
	if (pNetGame) {
		ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.5, ImGui::GetFontSize() * 2.5);

		ImGui::SetNextWindowPos(ImVec2(2.0f, (io.DisplaySize.y / 3) - (vecButSize.x / 2) + io.DisplaySize.y / 30));
		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::Begin("###keys", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button(m_bKeysStatus ? "<<<" : ">>>", vecButSize))
		{
			// меняем состояние кнопки
			m_bKeysStatus = !m_bKeysStatus;
		}


		ImGui::SameLine();

		// FIXME: тут не нужна лишняя проверка
		if(pNetGame)
		{
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			if(pVehiclePool)
			{
				VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
				if(ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
					if(pVehicle)
					{
						if(pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f)
						{
							if (ImGui::Button("G", vecButSize))
							{
								CPlayerPool *pPlayerPool;
								CLocalPlayer *pLocalPlayer;

								if(pNetGame)
								{
									pPlayerPool = pNetGame->GetPlayerPool();
									if(pPlayerPool)
									{
										pLocalPlayer = pPlayerPool->GetLocalPlayer();
										if(pLocalPlayer)
										{
											pLocalPlayer->HandlePassengerEntryEx();
										}
									}
								}
							}
							ImGui::SameLine();
						}
					}
				}
			}
		}

		// если кнопку переключили в раскрытое состояние, рендерим остальные кнопки
		if(m_bKeysStatus)
		{
		// если нажали на Alt
		if (ImGui::Button("ALT", vecButSize))
		{
			//pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->
			LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
		}

		ImGui::SameLine();
		if(ImGui::Button("TAB", vecButSize))
		{
			pScoreBoard->Toggle();
		}
		ImGui::SameLine();
		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if(ImGui::Button("ESC", vecButSize))
		{
			if(!pTextDrawPool)
				return;
			else
				pTextDrawPool->SetSelectState(false, -1);
		}
		ImGui::SameLine();
		if (ImGui::Button("H", vecButSize)) LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;
		ImGui::SameLine();
		if (ImGui::Button("Y", vecButSize)) LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
		ImGui::SameLine();
		if (ImGui::Button("N", vecButSize)) LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
		ImGui::SameLine();
		if (ImGui::Button("ENTER", vecButSize)) LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
		ImGui::SameLine();
		if (ImGui::Button("C", vecButSize)) LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = true;
		ImGui::SameLine();
		if (ImGui::Button("LCTRL", vecButSize)) LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		ImGui::SameLine();
		if (ImGui::Button("2", vecButSize)) LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION] = true;
		}

		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	if(m_bNeedClearMousePos)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNeedClearMousePos = false;
	}
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();

	if(pNetGame)
		pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y);
	if(!pChatWindow->OnTouchEvent(type, multi, x, y))
	{
		return false;
	}
	if(!pKeyBoard->OnTouchEvent(type, multi, x, y))
	{
		return false;
	}
	//if(!pScoreBoard->OnTouchEvent(type, multi, x, y)) return false;

	switch(type)
	{
		case TOUCH_PUSH:
		io.MousePos = ImVec2(x, y);
		io.MouseDown[0] = true;
		break;

		case TOUCH_POP:
		io.MouseDown[0] = false;
		m_bNeedClearMousePos = true;
		break;

		case TOUCH_MOVE:
		io.MousePos = ImVec2(x, y);
		break;
	}

	return true;
}

void CGUI::RenderVersion()
{
	ImVec2 _ImVec2 = ImVec2(ScaleX(10), ScaleY(5));
	RenderText(_ImVec2, ImColor(0, 0, 0, 200), false, "v" FLINRP_VER " (" BUILD_TAG ")", nullptr, ImGui::GetFontSize() * 0.85);
}

void CGUI::RenderRakNetStatistics()
{
	// TODO: добавить вывод статистики ракнета
		//StatisticsToString(rss, message, 0);

		/*ImGui::GetOverlayDrawList()->AddText(
			ImVec2(ScaleX(10), ScaleY(400)),
			ImColor(IM_COL32_BLACK), message);*/
}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
	int iOffset = pSettings->Get().iFontOutline;

	if (bOutline)
	{
		// left
		posCur.x -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right
		posCur.x += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetBackgroundDrawList()->AddText(NULL, font_size, posCur, col, text_begin, text_end);
}

void CGUI::RenderOverlayText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->Get().iFontOutline;

	if(bOutline)
	{
		posCur.x -= iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right
		posCur.x += iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetOverlayDrawList()->AddText(posCur, col, text_begin, text_end);
}
