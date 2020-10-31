#pragma once

class CCheatGUI
{
private:
    bool m_bStatus;
    bool m_bRender;

public:
    CCheatGUI();

    void SetStatus(bool toggle);
    bool GetStatus();
    void ToggleRender();
    void Render();
};