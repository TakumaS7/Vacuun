#include "enemyShotBullet.h"
#include "player.h"
#include "collision.h"
#include "sprite.h"
#include "texture.h"
#include "enemyBase.h"

void EnemyShotBullet::Update()
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
	
	float angle = atan2(XMVectorGetY(m_EnemyVector), XMVectorGetX(m_EnemyVector));
	m_Coordinate.m_Rotation = angle;

	Damege();
}

void EnemyShotBullet::Draw()
{
	switch (m_BulletPattern)
	{
	case ENEMYTYPE_STONE:
		break;

	case ENEMYTYPE_WEAK:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[0]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;

	case ENEMYTYPE_WEAK_REGIST:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[1]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;

	case ENEMYTYPE_UNIQUE:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[1]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;
	case ENEMYTYPE_BOSS:
		if (!GetCanAbsorb()) {//吸収不可能な弾
			GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[4]);
			DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
			break;
		}
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[3]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;
	case ENEMYBULLET_STRONG_NUM - 4:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[2]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;

	case ENEMYBULLET_STRONG_NUM - 3:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[2]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;

	case ENEMYBULLET_STRONG_NUM - 2:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[2]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;

	case ENEMYBULLET_STRONG_NUM - 1:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[2]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;

	case ENEMYBULLET_STRONG_NUM - 0:
		GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[2]);
		DrawSpriteAnim(m_Coordinate, 8, 1, m_AnimationFrame);
		break;
	}

	DrawHitEffect();
}
