//-------------------------------------------------------------
// enemyUnique.cpp
// 製作：楜澤旭渉　日付：2024/12/11
//-------------------------------------------------------------
#include "enemyWeakRegist.h"
#include "sprite.h"
#include "texture.h"
#include "player.h"

Player g_Player{};

void EnemyWeakRegist::Draw()
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
	else if (GetState() == ENEMYSTATE_BARRIER) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, m_AnimationFrame);
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_BarrierTexture);
		DrawSpriteAnim(GetCoordinate(), 8, 1, m_AnimationFrame);
	}
	else {
		DrawSpriteAnim(GetCoordinate(), 8, 1, 0);
	}

}

void EnemyWeakRegist::Update()
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

	//弾を撃つ
	if (m_Time - m_SpanTime >= 60 && (GetState() == ENEMYSTATE_BARRIER || GetState() == ENEMYSTATE_ATTACK))
	{
		Enemy::EnemyShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYTYPE_WEAK_REGIST,0);
		m_SpanTime = m_Time;
	}
}
