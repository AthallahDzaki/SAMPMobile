#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "killlist.h"
#include "gui/gui.h"

extern CGUI* pGUI;
extern CGame* pGame;

CKillList::CKillList()
{
	m_pPlayerKillList.clear();
}

CKillList::~CKillList()
{
	m_pPlayerKillList.clear();
}

void CKillList::MakeRecord(const char* playername, const char *killername, uint8_t reason)
{
	PlayerKillListStruct* pPlayerKill = new PlayerKillListStruct;
	pPlayerKill->playerName = playername;
	pPlayerKill->killerName = killername;
	pPlayerKill->reason = reason;

	if (m_pPlayerKillList.size() >= 5)
	{
		m_pPlayerKillList.pop_front();
	}

	m_pPlayerKillList.push_back(pPlayerKill);
}

void CKillList::Render()
{
	if (m_pPlayerKillList.empty() == false)
	{
		int iScreenWidth = RsGlobal->maximumWidth;
		int iScreenHeight = RsGlobal->maximumHeight;

		ImVec2 vecPos;
		vecPos.y = iScreenHeight / 2 - pGUI->ScaleY(170.0f);
		vecPos.x = iScreenWidth / 2 + pGUI->ScaleX(350.0f);

		for (auto& playerkill : m_pPlayerKillList)
		{
			if (playerkill)
			{
				char szText[256];
				sprintf(szText, "%s >> (killed) %s (%s)", playerkill->playerName.c_str(), playerkill->killerName.c_str(), pGame->GetWeaponName(playerkill->reason));
				pGUI->RenderText(vecPos, 0xFFFFFFFF, true, szText);
			}
		}
	}
}