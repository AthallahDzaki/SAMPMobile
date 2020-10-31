#include "../main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "chatwindow.h"
#include "netgame.h"
#include "../dialog.h"

extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CGUI *pGUI;
extern CDialogWindow *pDialogWindow;


CText3DLabelsPool::CText3DLabelsPool()
{
	for(int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		m_pTextLabels[x] = nullptr;
		m_bSlotState[x] = false;
	}
}

CText3DLabelsPool::~CText3DLabelsPool()
{
	for (int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		if (m_pTextLabels[x])
		{
			m_pTextLabels[x]->text[0] = '\0';
			delete m_pTextLabels[x];
			m_pTextLabels[x] = nullptr;
		}
	}
}

void FilterColors(char* szStr)
{
	if(!szStr) return;

	char szNonColored[2048+1];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;
	strcpy(szStr, szNonColored);
}

void CText3DLabelsPool::CreateTextLabel(int labelID, char* text, uint32_t color, float posX, float posY, float posZ, float drawDistance, bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID)
{
	Delete(labelID);

	TEXT_LABELS* pTextLabel = new TEXT_LABELS;
	if (pTextLabel)
	{
		//pTextLabel->text = text;
		cp1251_to_utf8(pTextLabel->text, text);
		cp1251_to_utf8(pTextLabel->textWithoutColors, text);
		FilterColors(pTextLabel->textWithoutColors);

		pTextLabel->color = color;
		pTextLabel->pos.X = posX;
		pTextLabel->pos.Y = posY;
		pTextLabel->pos.Z = posZ;
		pTextLabel->drawDistance = drawDistance;
		pTextLabel->useLineOfSight = useLOS;
		pTextLabel->attachedToPlayerID = attachedToPlayerID;
		pTextLabel->attachedToVehicleID = attachedToVehicleID;

		pTextLabel->m_fTrueX = -1;

		if (attachedToVehicleID != INVALID_VEHICLE_ID || attachedToPlayerID != INVALID_PLAYER_ID)
		{
			pTextLabel->offsetCoords.X = posX;
			pTextLabel->offsetCoords.Y = posY;
			pTextLabel->offsetCoords.Z = posZ;
		}

		m_pTextLabels[labelID] = pTextLabel;
		m_bSlotState[labelID] = true;
	}
}

void CText3DLabelsPool::Delete(int labelID)
{
	if (m_pTextLabels[labelID])
	{
		m_pTextLabels[labelID]->text[0] = '\0';
		delete m_pTextLabels[labelID];
		m_pTextLabels[labelID] = nullptr;
		m_bSlotState[labelID] = false;
	}
}

void CText3DLabelsPool::AttachToPlayer(int labelID, PLAYERID playerID, VECTOR pos)
{
	if (m_bSlotState[labelID] == true)
	{
		m_pTextLabels[labelID]->attachedToPlayerID = playerID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::AttachToVehicle(int labelID, VEHICLEID vehicleID, VECTOR pos)
{
	if (m_bSlotState[labelID] == true)
	{
		m_pTextLabels[labelID]->attachedToVehicleID = vehicleID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::Update3DLabel(int labelID, uint32_t color, char* text)
{
	if (m_bSlotState[labelID] == true)
	{
		m_pTextLabels[labelID]->color = color;
		cp1251_to_utf8(m_pTextLabels[labelID]->text, text);
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr)
{
	char tempStr[4096];

	//va_list argPtr;
	//va_start(argPtr, fmt);
	//vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
	//va_end(argPtr);

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				//ImGui::TextUnformatted(textStart, textCur);
				pGUI->RenderText(vecPos, col, true, textStart, textCur);
				//ImGui::SameLine(0.0f, 0.0f);
				vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
			{
				//ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				//ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			//ImGui::TextUnformatted(textStart, textCur);
			pGUI->RenderText(vecPos, col, true, textStart, textCur);
			vecPos.x = pos.x;//+= ImGui::CalcTextSize(textStart, textCur).x;
			vecPos.y += pGUI->GetFontSize();
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
		//ImGui::TextUnformatted(textStart, textCur);
	{
		pGUI->RenderText(vecPos, col, true, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
	}
	else
		//ImGui::NewLine();
		vecPos.y += pGUI->GetFontSize();

	//if(pushedColorStyle)
	//	ImGui::PopStyleColor();
}

ImVec2 CalcTextSizeWithoutTags(char* szStr)
{
	if(!szStr) return ImVec2(0, 0);

	char szNonColored[2048];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos + 7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos + 7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;
	return ImGui::CalcTextSize(szNonColored);
}

void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor)
{
	uint16_t linesCount = 0;
	std::string strUtf8 = utf8string;
	int size = strUtf8.length();
	std::string color;

	for(uint32_t i = 0; i < size; i++)
	{
		if(i+7 < strUtf8.length())
		{
			if(strUtf8[i] == '{' && strUtf8[i+7] == '}' )
			{
				color = strUtf8.substr(i, 7+1);
			}
		}
		if(strUtf8[i] == '\n')
		{
			linesCount++;
			if(i+1 < strUtf8.length() && !color.empty())
			{
				strUtf8.insert(i+1 , color);
				size += color.length();
				color.clear();
			}
		}
		if(strUtf8[i] == '\t')
		{
			strUtf8.replace(i, 1, " ");
		}
	}
	pos.y += pGUI->GetFontSize()*(linesCount / 2);
	if(linesCount)
	{
		uint16_t curLine = 0;
		uint16_t curIt = 0;
		for(uint32_t i = 0; i < strUtf8.length(); i++)
		{
			if(strUtf8[i] == '\n')
			{
				if(strUtf8[curIt] == '\n' )
				{
					curIt++;
				}
				ImVec2 _pos = pos;
				_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, i-curIt).c_str()).x / 2;
				_pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
				TextWithColors( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, i-curIt).c_str() );
				curIt = i;
				curLine++;
			}
		}
		if(strUtf8[curIt] == '\n')
		{
			curIt++;
		}
		if(strUtf8[curIt] != '\0')
		{
			ImVec2 _pos = pos;
			_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, strUtf8.size()-curIt).c_str()).x / 2;
			_pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
			TextWithColors( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, strUtf8.length()-curIt).c_str() );
		}
	}
	else
	{
		pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.c_str()).x / 2;
		TextWithColors( pos, __builtin_bswap32(dwColor), (char*)strUtf8.c_str() );
	}
}

void CText3DLabelsPool::UpdateFlag()
{
	if(m_bFlag)
		m_bFlag = false;
	else 
		m_bFlag = true;
}

void CText3DLabelsPool::Render()
{
	int hitEntity = 0;
	for(int x = 0; x < MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2; x++)
	{
		if(x == MAX_TEXT_LABELS + MAX_PLAYER_TEXT_LABELS + 2 || x == INVALID_TEXT_LABEL) 
			continue;

		if (m_bSlotState[x])
		{
			CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
			if(!pPlayerPool)
				continue;

			// D3DXVECTOR3 textPos;
			VECTOR textPos;
			if(m_pTextLabels[x]->attachedToPlayerID != INVALID_PLAYER_ID)
			{
				if(m_pTextLabels[x]->attachedToPlayerID == pPlayerPool->GetLocalPlayerID())
					continue;

				if(pPlayerPool && pPlayerPool->GetSlotState(m_pTextLabels[x]->attachedToPlayerID) == true)
				{
					CRemotePlayer *pPlayer = pPlayerPool->GetAt(m_pTextLabels[x]->attachedToPlayerID);
					if(!pPlayer)
						continue;
					
					CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();
					if(!pPlayerPed)
						continue;

					VECTOR matPlayer;
					pPlayerPed->GetBonePosition(8, &matPlayer);

					textPos.X = matPlayer.X + m_pTextLabels[x]->offsetCoords.X;
					textPos.Y = matPlayer.Y + m_pTextLabels[x]->offsetCoords.Y;
					textPos.Z = matPlayer.Z + 0.23 + m_pTextLabels[x]->offsetCoords.Z;
				}
			}
			if(m_pTextLabels[x]->attachedToVehicleID != INVALID_VEHICLE_ID)
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				if(pVehiclePool && pVehiclePool->GetSlotState(m_pTextLabels[x]->attachedToVehicleID) == true)
				{
					CVehicle *pVehicle = pVehiclePool->GetAt(m_pTextLabels[x]->attachedToVehicleID);
					if(!pVehicle)
						continue;

					MATRIX4X4 matVehicle;
					pVehicle->GetMatrix(&matVehicle);

					textPos.X = matVehicle.pos.X + m_pTextLabels[x]->offsetCoords.X;
					textPos.Y = matVehicle.pos.Y + m_pTextLabels[x]->offsetCoords.Y;
					textPos.Z = matVehicle.pos.Z + m_pTextLabels[x]->offsetCoords.Z;
				}
			}
			if(m_pTextLabels[x]->attachedToVehicleID == INVALID_VEHICLE_ID && m_pTextLabels[x]->attachedToPlayerID == INVALID_PLAYER_ID)
			{
				textPos.X = m_pTextLabels[x]->pos.X;
				textPos.Y = m_pTextLabels[x]->pos.Y;
				textPos.Z = m_pTextLabels[x]->pos.Z;
			}

			if (m_pTextLabels[x]->useLineOfSight)
			{
				MATRIX4X4 mat;
				VECTOR playerPosition;

				CAMERA_AIM *pCam = GameGetInternalAim();
				CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
				if(!pLocalPlayer)
					continue;

				CPlayerPed *pPlayerPed = pLocalPlayer->GetPlayerPed();
				if(!pPlayerPed)
					continue;

				pPlayerPed->GetMatrix(&mat);

				playerPosition.X = mat.pos.X;
				playerPosition.Y = mat.pos.Y;
				playerPosition.Z = mat.pos.Z;

				if (m_pTextLabels[x]->useLineOfSight)
					hitEntity = ScriptCommand(&get_line_of_sight,
					playerPosition.X, playerPosition.Y, playerPosition.Z,
					pCam->pos1x, pCam->pos1y, pCam->pos1z,
					1, 0, 0, 0, 0);
			}
				
			m_pTextLabels[x]->pos.X = textPos.X;
			m_pTextLabels[x]->pos.Y = textPos.Y;
			m_pTextLabels[x]->pos.Z = textPos.Z;

			if (!m_pTextLabels[x]->useLineOfSight || hitEntity)
			{
				CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
				if(!pLocalPlayer)
					continue;

				CPlayerPed *pPlayerPed = pLocalPlayer->GetPlayerPed();
				if(!pPlayerPed)
					continue;
				
				if(pPlayerPed->GetDistanceFromPoint(m_pTextLabels[x]->pos.X, m_pTextLabels[x]->pos.Y, m_pTextLabels[x]->pos.Z) <= m_pTextLabels[x]->drawDistance)
					Draw(x, &textPos);
			}
		}
	}
}

void CText3DLabelsPool::Draw(size_t id, VECTOR *vec)
{
	VECTOR CPos;

	CPos.X = vec->X;
	CPos.Y = vec->Y;
	CPos.Z = vec->Z;

	VECTOR Out;

	// CSprite::CalcScreenCoors
	(( void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_libGTASA+0x54EEC0+1))(&CPos, &Out, 0, 0, 0, 0);
	if(Out.Z < 1.0f) return;
	ImVec2 pos = ImVec2(Out.X, Out.Y);
	// removed piece
	Render3DLabel(pos, m_pTextLabels[id]->text, m_pTextLabels[id]->color);
}