#pragma once

#include <map>

struct RemoveDataStruct
{
    unsigned short usModelIndex;
    VECTOR vecPos;
    float fRange;
};

class CWorld
{
private:
    std::map<ENTITY_TYPE *, bool>           m_pAddedEntities;
    std::map<ENTITY_TYPE *, bool>           m_pRemovedEntities;
    std::map<RemoveDataStruct *, bool>      m_pRemoveBuildingData;

public:
    CWorld();
    ~CWorld();

    void AddDataBuilding(ENTITY_TYPE *pBuildingInterface);
    void RemoveBuilding(unsigned short usModelToRemove, float fRange, float fX, float fY, float fZ);

    bool ProcessRemovedEntities(ENTITY_TYPE *pInterface);
};