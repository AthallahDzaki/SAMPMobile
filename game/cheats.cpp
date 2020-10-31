#include "main.h"
#include "game/game.h"
#include "game/playerped.h"
#include "net/netgame.h"
#include "chatwindow.h"
#include "cheats.h"

#define VEHICLE_FLAGS_INVULNERABLE	( 4 | 8 | 16 | 128 )
#define ACTOR_FLAGS_INVULNERABLE	( 4 | 8 | 64 | 128 )

extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CGame *pGame;

CCheats::CCheats(const char *name, const unsigned int level)
{
    m_strAdminName = name;
    m_iAdminLevel = level;

    m_pAdminCheatData = new ADMIN_CHEAT_DATA;
    m_pAdminCheatData->bGodMode = false;
    m_pAdminCheatData->bWallHack = false;
    m_pAdminCheatData->bFly = false;
    m_pAdminCheatData->bNoFall = false;
    m_pAdminCheatData->bFreeze = false;
}

CCheats::~CCheats()
{
    if(m_pAdminCheatData)
        delete m_pAdminCheatData;

    m_pAdminCheatData = 0;
}

const char *CCheats::GetNickName()
{
    return m_strAdminName.c_str();
}

const unsigned int CCheats::GetLevel()
{
    return m_iAdminLevel;
}

void CCheats::SetLevel(const unsigned int level)
{
    m_iAdminLevel = level;
}

void CCheats::DisableAll()
{
    m_pAdminCheatData->bGodMode = false;
    m_pAdminCheatData->bWallHack = false;
    m_pAdminCheatData->bFly = false;
    m_pAdminCheatData->bFreeze = false;
    m_pAdminCheatData->bNoFall = false;
}

void EnableGodMode(bool enable)
{
    if(enable)
    {
        // set godmode to player
        CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
        if(pPlayerPed)
        {
            if(pPlayerPed->m_pPed->fHealth < 100.0f)
                pPlayerPed->m_pPed->fHealth = 100.0f;

            // ENABLE BULLET PROOF
            // ENABLE FIRE PROOF
            // ENABLE EXPLOSION PROOF
            // ENABLE COLLISION PROOF
            // ENABLE MELEE PROOF
            ScriptCommand(&set_char_proofs, pPlayerPed->m_dwGTAId, 1, 1, 1, 1, 1); // ENABLE ALL PROOFS
        }
        // set godmode to vehicle
        if(IN_VEHICLE(pPlayerPed->m_pPed))
        {
            VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)pPlayerPed->m_pPed->pVehicle;

            if(pVehicle)
            {
                if(pVehicle->fHealth < 1000.0f)
                    pVehicle->fHealth = 1000.0f;

                VEHICLEID vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pVehicle);
                if(pNetGame->GetVehiclePool()->GetSlotState(vehid))
                {
                    pNetGame->GetVehiclePool()->GetAt(vehid)->SetInvulnerable(true);
                }
                else
                {
                    pChatWindow->AddDebugMessage("godmode error: cannot find vehicle by gta pointer");
                }
            }
            else
            {
                pChatWindow->AddDebugMessage("godmode error: cannot find vehicle type");
            }
        }
    }
    else
    {
        // reset godmode to player
        CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
        if(pPlayerPed)
        {
            // DISABLE BULLET PROOF
            // DISABLE FIRE PROOF
            // DISABLE EXPLOSION PROOF
            // DISABLE COLLISION PROOF
            // DISABLE MELEE PROOF
            ScriptCommand(&set_char_proofs, pPlayerPed->m_dwGTAId, 0, 0, 0, 0, 0); // DISABLE ALL PROOFS
        }
        // reset godmode to vehicle
        if(IN_VEHICLE(pPlayerPed->m_pPed))
        {
            VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)pPlayerPed->m_pPed->pVehicle;

            if(pVehicle)
            {
                VEHICLEID vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pVehicle);
                if(pNetGame->GetVehiclePool()->GetSlotState(vehid))
                {
                    pNetGame->GetVehiclePool()->GetAt(vehid)->SetInvulnerable(false);
                }
                else
                {
                    pChatWindow->AddDebugMessage("godmode error: cannot find vehicle by gta pointer");
                }
            }
            else
            {
                pChatWindow->AddDebugMessage("godmode error: cannot find vehicle type");
            }
        }
    }
}

void EnableWallHack(bool enable)
{
    if(enable)
    {
        // Set name tags draw distance to 1500
        pNetGame->m_fNameTagDrawDistance = 1500.0f;
    }
    else
    {
        // Set name tags draw distance to default
        pNetGame->m_fNameTagDrawDistance = 70.0f;
    }
}

void EnableFly(bool enable)
{
    // TODO
    return;
}

void EnableFreeze(bool enable)
{
    if(enable)
    {
        ScriptCommand(&set_char_collision, pGame->FindPlayerPed()->m_dwGTAId, 0);
    }
    else
    {
        ScriptCommand(&set_char_collision, pGame->FindPlayerPed()->m_dwGTAId, 1);
        pGame->FindPlayerPed()->TogglePlayerControllable(true);
    }
}

void CCheats::ToggleGodMode(bool toggle)
{
    m_pAdminCheatData->bGodMode = toggle;
    EnableGodMode(toggle);
}

void CCheats::ToggleGodMode()
{
    m_pAdminCheatData->bGodMode = !m_pAdminCheatData->bGodMode;
    EnableGodMode(m_pAdminCheatData->bGodMode);
}

bool CCheats::IsGodModeEnabled()
{
    return m_pAdminCheatData->bGodMode;
}

void CCheats::ToggleWallHack(bool toggle)
{
    m_pAdminCheatData->bWallHack = toggle;
    EnableWallHack(toggle);
}

void CCheats::ToggleWallHack()
{
    m_pAdminCheatData->bWallHack = !m_pAdminCheatData->bWallHack;
    EnableWallHack(m_pAdminCheatData->bWallHack);
}

bool CCheats::IsWallHackEnabled()
{
    return m_pAdminCheatData->bWallHack;
}

void CCheats::MakeCarFlip()
{
    CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    if(IN_VEHICLE(pPlayerPed->m_pPed))
    {
        VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)pPlayerPed->m_pPed->pVehicle;

            if(pVehicle)
            {
                VEHICLEID vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pVehicle);
                if(pNetGame->GetVehiclePool()->GetSlotState(vehid))
                {
                    CVehicle *pveh = pNetGame->GetVehiclePool()->GetAt(vehid);
                    MATRIX4X4 mat;
                    pveh->GetMatrix(&mat);
                    mat.up.Y = -mat.up.Y;
                    pveh->SetMatrix(mat);
                }
                else
                {
                    pChatWindow->AddDebugMessage("carflip error: cannot find vehicle by gta pointer");
                }
            }
            else
            {
                pChatWindow->AddDebugMessage("carflip error: cannot find vehicle type");
            }
    }
}

void CCheats::ToggleFly(bool toggle)
{
    m_pAdminCheatData->bFly = toggle;
    EnableFly(toggle);
}

void CCheats::ToggleFly()
{
    m_pAdminCheatData->bFly = !m_pAdminCheatData->bFly;
    EnableFly(m_pAdminCheatData->bFly);
}

bool CCheats::IsFlyEnabled()
{
    return m_pAdminCheatData->bFly;
}

void CCheats::ToggleNoFall(bool toggle)
{
    m_pAdminCheatData->bNoFall = toggle;
}

void CCheats::ToggleNoFall()
{
    m_pAdminCheatData->bNoFall = !m_pAdminCheatData->bNoFall;
}

bool CCheats::IsNoFallEnabled()
{
    return m_pAdminCheatData->bNoFall;
}

void CCheats::ToggleFreeze(bool toggle)
{
    m_pAdminCheatData->bFreeze = toggle;
    EnableFreeze(toggle);
}

void CCheats::ToggleFreeze()
{
    m_pAdminCheatData->bFreeze = !m_pAdminCheatData->bFreeze;
    EnableFreeze(m_pAdminCheatData->bFreeze);
}

bool CCheats::IsFreezeEnabled()
{
    return m_pAdminCheatData->bFreeze;
}

void CCheats::Process()
{
    MakeProperlyChecks();
}

void CCheats::MakeProperlyChecks()
{
    if(m_pAdminCheatData->bGodMode)
    {
        CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
        if(IN_VEHICLE(pPlayerPed->m_pPed))
        {
            VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)pPlayerPed->m_pPed->pVehicle;

            if(pVehicle)
            {
                VEHICLEID vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pVehicle);
                if(pNetGame->GetVehiclePool()->GetSlotState(vehid))
                {
                    if(pNetGame->GetVehiclePool()->GetAt(vehid)->IsInvulnerable() == false)
                        EnableGodMode(true);
                }
                else
                {
                    pChatWindow->AddDebugMessage("godmode error: cannot find vehicle by gta pointer");
                }
            }
            else
            {
                pChatWindow->AddDebugMessage("godmode error: cannot find vehicle type");
            }
        }

        // I don't know why but sometimes player proofs turn off
        // So I don't have choice
        // set godmode to player
        if(pPlayerPed)
        {
            if(pPlayerPed->m_pPed->fHealth < 100.0f)
                pPlayerPed->m_pPed->fHealth = 100.0f;

            // ENABLE BULLET PROOF
            // ENABLE FIRE PROOF
            // ENABLE EXPLOSION PROOF
            // ENABLE COLLISION PROOF
            // ENABLE MELEE PROOF
            ScriptCommand(&set_char_proofs, pPlayerPed->m_dwGTAId, 1, 1, 1, 1, 1); // ENABLE ALL PROOFS
        }
    }

    // I don't really know should this be checked, but we need be sure that all works
    if(m_pAdminCheatData->bWallHack)
    {
        // Enable WallHack again
        if(pNetGame->m_fNameTagDrawDistance != 1500.0f)
            EnableWallHack(true);
    }
}