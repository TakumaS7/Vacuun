//-------------------------------------------------------------
// enemyStraight.cpp
// 製作：楜澤旭渉　日付：2024/12/11
//-------------------------------------------------------------
#include "enemyStraight.h"
#include "sprite.h"
#include "texture.h"

void EnemyStraight::Update()
{
	Player* pPlayer = GetPlayer();
	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
	if (GetUse())	{
		m_Coordinate.m_Pos = EnemyMove(GetMovePattern(), m_Coordinate);
	}

	Damege();

	m_Time++;
}

void EnemyStraight::Draw()
{
	GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[GetState()]);


	if (GetState() == ENEMYSTATE_ATTACK) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, 0);
	}
	else if (GetState() == ENEMYSTATE_ABSORBED) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, 0);
	}
	else if (GetState() == ENEMYSTATE_KNOCKOUT) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, 0);
	}
	else if (GetState() == ENEMYSTATE_BULLET) {
		DrawSpriteAnim(GetCoordinate(), 1, 1, 0);
	}
	else if (GetState() == ENEMYSTATE_DAMEGED) {
		DrawSpriteAnim(GetCoordinate(), 1, 1, 0);
	}
	else {
		DrawSpriteAnim(GetCoordinate(), 1, 1, 0);
	}

	DrawHitEffect();
}