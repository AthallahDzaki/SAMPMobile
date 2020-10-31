#pragma once 

struct CHATBUBBLE_DATA
{
    uint8_t iTextlen;
    char szText[0xFF];
    uint32_t dwColor;
    size_t iElapseTime;
    float fDrawDistance;
    bool bInited;
    uint32_t iFirstTick;
};

class CPlayerBubblePool
{
public:
    CPlayerBubblePool();
    ~CPlayerBubblePool();
    void AddChatBubble(PLAYERID playerid, CHATBUBBLE_DATA watafuk);
    void Render();
    
private:
    void Draw(size_t id, float dist, VECTOR *vec);
    CHATBUBBLE_DATA m_ChatBubbleArray[MAX_PLAYERS];
};