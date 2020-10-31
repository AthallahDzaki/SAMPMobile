#pragma once

#include <vector>
#include <vendor/imgui/imgui.h>

#define MAX_CONSOLE_INPUT_BUFFER        256

class CConsoleGUI
{
private:
    char                            m_szBuffer[MAX_CONSOLE_INPUT_BUFFER];
    ImVector<char*>                 m_vecItems;
    ImVector<const char*>           m_vecCommands;
    ImVector<char*>                 m_vecHistory;
    int                             m_iHistoryPos;
    ImGuiTextFilter                 m_TextFilter;
    bool                            m_bAutoScroll;
    bool                            m_bScrollToBottom;
    bool                            m_bDraw;

public:
    CConsoleGUI();
    ~CConsoleGUI();

    void ClearLog();
    void AddLog(const char *fmt, ...) IM_FMTARGS(2);
    void Draw();
    void ToggleDraw();

    void RegisterCommands();
    void ProcessCommand(const char *cmd);

    int TextEditCallback(ImGuiTextEditCallbackData *data);
};