#pragma once

#include "main.h"
#include "game/RW/RenderWare.h"
#include "game/game.h"
#include "gui/gui.h"

class CButton
{
private:
    RwTexture*  m_pButtonTexture;
    bool        m_bActive;
    bool        m_bToggle;

public:
    CButton();
    ~CButton();

    void LoadButtonTexture();
    void Toggle();
    void Draw();
};