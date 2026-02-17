//-------------------------------------------------------------
// enemyUnique.cpp
// 製作：楜澤旭渉　日付：2024/12/11
//-------------------------------------------------------------
#include "enemyUnique.h"
#include "sprite.h"
#include "texture.h"

void EnemyUnique::Update()
{
	m_frameNum++;

	
	//3フレームごとに更新
	if (m_frameNum % 3 == 0) {
		m_AnimationFrame += 1;

		//アニメーションのループ
		if (m_AnimationFrame >= 9) {
			m_AnimationFrame = 1;
		}
	}

	m_Coordinate.m_OldPos = m_Coordinate.m_Pos;
	m_Coordinate.m_Pos = EnemyMove(GetMovePattern(), m_Coordinate);

	m_Time++;

	Damege();
}


void EnemyUnique::Draw()
{
	GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[GetState()]);


	if (GetState() == ENEMYSTATE_ATTACK) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, m_AnimationFrame);
	}
	else if (GetState() == ENEMYSTATE_ABSORBED) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, m_AnimationFrame);
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
