#pragma once

#include <list>
#include <string>

struct PlayerKillListStruct
{
	std::string playerName;
	std::string killerName;
	uint8_t reason;
};

class CKillList
{
public:
	CKillList();
	~CKillList();

	void MakeRecord(const char* playerId, const char* killerId, uint8_t reason);
	void Render();

private:
	std::list<PlayerKillListStruct*> m_pPlayerKillList;

};