//-------------------------------------------------------------
// enemyStrong.cpp
// 製作：大岡優剛　日付：2024/12/13
//-------------------------------------------------------------
#include "enemyStrong.h"
#include "sprite.h"
#include "texture.h"
#include "enemyShotBullet.h"

void EnemyStrong::Draw()
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
		DrawSpriteAnim(GetCoordinate(), 8, 1, m_AnimationFrame);
	}
	else if (GetState() == ENEMYSTATE_DAMEGED) {
		DrawSpriteAnim(GetCoordinate(), 8, 1, m_AnimationFrame);
	}
	else {
		DrawSpriteAnim(GetCoordinate(), 8, 1, m_AnimationFrame);
	}

	DrawHitEffect();

}

void EnemyStrong::Update()
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
	if (m_Time - m_SpanTime >= 60 && GetState() != ENEMYSTATE_KNOCKOUT) {
		Enemy::EnemyShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYBULLET_STRONG_NUM - 4, ENEMYBULLET_STRONG_NUM - 4);

		Enemy::EnemyShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYBULLET_STRONG_NUM - 3, ENEMYBULLET_STRONG_NUM - 3);

		Enemy::EnemyShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYBULLET_STRONG_NUM - 2, ENEMYBULLET_STRONG_NUM - 2);

		Enemy::EnemyShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYBULLET_STRONG_NUM - 1, ENEMYBULLET_STRONG_NUM - 1);

		Enemy::EnemyShot({ m_Coordinate.m_Pos.x, m_Coordinate.m_Pos.y }, ENEMYBULLET_STRONG_NUM - 0, ENEMYBULLET_STRONG_NUM - 0);

		m_SpanTime = m_Time;
	}

	if (GetState() == ENEMYSTATE_KNOCKOUT) {
		SetUse(false);
	}

	Damege();
}