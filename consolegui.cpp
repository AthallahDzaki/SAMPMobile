#include "main.h"
#include "consolegui.h"
#include "game/game.h"
#include "game/playerped.h"
#include "net/netgame.h"

extern CNetGame *pNetGame;

static void Strtrim(char *str)
{
    char *str_end = str + strlen(str);
    while(str_end > str && str_end[-1] == ' ')
    {
        str_end--;
        *str_end = 0;
    }
}

static int Strnicmp(const char *str1, const char *str2, int n)
{
    int d = 0;
    while(n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
    {
        str1++;
        str2++;
        n--;
    }
    return d;
}

static int TextEditCallbackStub(ImGuiInputTextCallbackData *data)
{
    CConsoleGUI *console = (CConsoleGUI*)data->UserData;
    return console->TextEditCallback(data);
}

CConsoleGUI::CConsoleGUI()
{
    ClearLog();
    memset(m_szBuffer, 0, sizeof(m_szBuffer));
    m_iHistoryPos = -1;
    RegisterCommands();
    m_bAutoScroll = true;
    m_bScrollToBottom = true;
    m_bDraw = false;
    AddLog("SA-MP Flin RolePlay Console Loaded.");
}

CConsoleGUI::~CConsoleGUI()
{
    ClearLog();
    for(int i = 0; i < m_vecHistory.Size; i++)
        free(m_vecHistory[i]);
}

void CConsoleGUI::ClearLog()
{
    for(int i = 0; i < m_vecItems.Size; i++)
        free(m_vecItems[i]);
    m_vecItems.clear();
    m_bScrollToBottom = true;
}

void CConsoleGUI::AddLog(const char *fmt, ...) IM_FMTARGS(2)
{
    char buf[1024];
    va_list va_args;
    va_start(va_args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, va_args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(va_args);
    m_vecItems.push_back(strdup(buf));
    if(m_bAutoScroll)
        m_bScrollToBottom = true;
}

void CConsoleGUI::Draw()
{
    #ifdef BUILD_BETA
    if(!m_bDraw) return;

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Flin RolePlay Console", nullptr))
    {
        ImGui::End();
        return;
    }

    if(ImGui::BeginPopupContextItem())
    {
        if(ImGui::MenuItem("Close Console"))
        {
            m_bDraw = false;
        }
        ImGui::EndPopup();
    }

    if(!m_bDraw)
    {
        ImGui::End();
        return;
    }

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    if(ImGui::BeginPopupContextWindow())
    {
        if(ImGui::Selectable("Clear")) ClearLog();
        ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1));
    
    for(int i = 0; i < m_vecItems.Size; i++)
    {
        const char *item = m_vecItems[i];
        if(!m_TextFilter.PassFilter(item))
            continue;

        bool pop_color = false;
        if(strstr(item, "[error]"))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            pop_color = true;
        }
        else if(strncmp(item, "#", 2) == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
            pop_color = true;
        }
        ImGui::TextUnformatted(item);
        if(pop_color)
        {
            ImGui::PopStyleColor();
        }
    }

    if(m_bScrollToBottom)
        ImGui::SetScrollHereY(1.0f);

    m_bScrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    if(ImGui::InputText("Input", m_szBuffer, IM_ARRAYSIZE(m_szBuffer), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
    {
        char *s = m_szBuffer;
        Strtrim(s);
        if(s[0])
            ProcessCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    ImGui::SetItemDefaultFocus();
    if(reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1);

    ImGui::End();
    #endif
}

void CConsoleGUI::ToggleDraw()
{
    m_bDraw = !m_bDraw;
    if(m_bDraw)
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

void CConsoleGUI::RegisterCommands()
{
    m_vecCommands.push_back("help");
    m_vecCommands.push_back("history");
    m_vecCommands.push_back("clear");
}

void CConsoleGUI::ProcessCommand(const char *cmd)
{
    AddLog("# %s\n", cmd);

    m_iHistoryPos = -1;
    for(int i = m_vecHistory.Size-1; i >= 0; i--)
    {
        if(strcmp(m_vecHistory[i], cmd) == 0)
        {
            free(m_vecHistory[i]);
            m_vecHistory.erase(m_vecHistory.begin() + i);
        }
    }
    m_vecHistory.push_back(strdup(cmd));

    if(strcmp(cmd, "clear") == 0)
    {
        ClearLog();
    }
    else if(strcmp(cmd, "help") == 0)
    {
        AddLog("Commands:");
        for(int i = 0; i < m_vecCommands.Size; i++)
            AddLog("- %s", m_vecCommands[i]);
    }
    else
    {
        AddLog("Unknown command: '%s'\n", cmd);
    }

    m_bScrollToBottom = true;
}

int CConsoleGUI::TextEditCallback(ImGuiTextEditCallbackData *data)
{
    switch(data->EventFlag)
    {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            const char *word_end = data->Buf + data->CursorPos;
            const char *word_start = word_end;

            while(word_start > data->Buf)
            {
                const char c = word_start[-1];
                if(c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            ImVector<const char *> candidates;
            for(int i = 0; i < m_vecCommands.Size; i++)
                if(Strnicmp(m_vecCommands[i], word_start, (int)(word_end-word_start)) == 0)
                    candidates.push_back(m_vecCommands[i]);

            if(candidates.Size == 0)
            {
                AddLog("No match for \"%.*s\"!\n");
            }
            else if(candidates.Size == 1)
            {
                data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                int match_len = (int)(word_end-word_start);
                for(;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for(int i = 0; i < candidates.Size && all_candidates_matches; i++)
                    {
                        if(i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if(c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    }
                    if(!all_candidates_matches)
                        break;
                    match_len++;
                }

                if(match_len > 0)
                {
                    data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                AddLog("Possible matches:\n");
                for(int i = 0; i < candidates.Size; i++)
                    AddLog("- %s\n", candidates[i]);
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            break;
        }
    }
}