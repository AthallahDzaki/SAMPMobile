#include "main.h"
#include "game.h"
#include "entity.h"
#include "object.h"
#include "world.h"
#include "chatwindow.h"

extern CChatWindow *pChatWindow;

CWorld::CWorld()
{
    m_pAddedEntities.clear();
}

CWorld::~CWorld()
{
    m_pAddedEntities.clear();
}

void CWorld::AddDataBuilding(ENTITY_TYPE *pBuildingInterface)
{
    if(m_pAddedEntities.find(pBuildingInterface) == m_pAddedEntities.end() || m_pAddedEntities[pBuildingInterface] == false)
    {
        m_pAddedEntities[pBuildingInterface] = true;
        m_pRemovedEntities[pBuildingInterface] = false;
    }
}

void CWorld::RemoveBuilding(unsigned short usModelToRemove, float fRange, float fX, float fY, float fZ)
{
    RemoveDataStruct *pRemoveData = new RemoveDataStruct;
    pRemoveData->usModelIndex = usModelToRemove;
    pRemoveData->fRange = fRange;
    pRemoveData->vecPos.X = fX;
    pRemoveData->vecPos.Y = fY;
    pRemoveData->vecPos.Z = fZ;

    m_pRemoveBuildingData.insert(std::make_pair(pRemoveData, true));
}

bool CWorld::ProcessRemovedEntities(ENTITY_TYPE *pInterface)
{
    if(pInterface && pInterface->vtable != g_libGTASA+0x5C7358 && pInterface->mat)
    {
        for(auto iter = m_pRemoveBuildingData.begin(); iter != m_pRemoveBuildingData.end(); iter++)
        {
            float fDistanceX = iter->first->vecPos.X - pInterface->mat->pos.X;
            float fDistanceY = iter->first->vecPos.Y - pInterface->mat->pos.Y;
            float fDistanceZ = iter->first->vecPos.Z - pInterface->mat->pos.Z;

            float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);

            if(fDistance <= iter->first->fRange)
            {
                if(pInterface->nModelIndex == iter->first->usModelIndex)
                    return false;
            }
        }
    }
    return true;
}