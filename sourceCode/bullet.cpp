//=====================================
// bullet.cpp
// 
// 2024/11/1
//=====================================

#include "bullet.h"
#include "player.h"
#include "texture.h"
#include "sprite.h"
#include "coordinate.h"
#include "keyboard.h"
#include "enemy_01.h"
#include "collision.h"
#include "score.h"
#include "main.h"
#include <vector>

//=========================================
// グローバル変数
//=========================================
static unsigned int constexpr BULLET_MAX = 1;

static ID3D11ShaderResourceView* g_Texture;
static ID3D11ShaderResourceView* g_MaxBulletTexture;
static ID3D11ShaderResourceView* g_TextureULT;
Coordinate g_BulletDraw;
Bullet g_Bullet;
Bullet* g_AttackBullet = nullptr;
float g_AnimationFrame = 1;		//今描画したいフレーム番号
int g_FrameCount = 0; // フレームカウント
vector<Bullet>g_Bullets;
int countEnemy = 0;

void InitializeBullet(void)
{
	g_Bullet.m_Coordinate.m_Pos = GetPlayer()->m_Coordinate.m_Pos;
	g_Bullet.m_Coordinate.m_Size = XMFLOAT2(0.5f, 0.5f);

	//弾の素材(デバッグ用)
	g_Texture = InitTexture(L"asset\\texture\\EFFECT_BULLET_RANGElv2.png");
	g_MaxBulletTexture = InitTexture(L"asset\\texture\\EFFECT_BULLET_RANGElv3.png");
	g_TextureULT = InitTexture(L"asset\\texture\\ULT_BULLET_TEMPT.png");
}

void UpdateBullet(EnemyProduct* EnemyProduct)
{
	vector<Enemy*>* enemies = EnemyProduct->GetEnemy();
	Player* pPlayer = GetPlayer();

	g_FrameCount++;

	//3フレームごとに更新
	if (g_FrameCount % 3 == 0) {
		g_AnimationFrame += 1;

		//アニメーションのループ
		if (g_AnimationFrame >= 9) {
			g_AnimationFrame = 1;
		}
	}

	if (g_Bullet.GetUse())
	{
		switch (g_Bullet.GetState())
		{
		case BULLETSTATE_ATTRACT:
			break;
		case BULLETSTATE_HOLDING:
			g_Bullet.m_Coordinate.m_Pos = { pPlayer->m_Coordinate.m_Pos.x + 200.0f, pPlayer->m_Coordinate.m_Pos.y, 0.0f };
			break;
		case BULLETSTATE_ATTACK:
			//g_Bullet.SetUse(true);

			g_Bullet.m_Coordinate.m_Vel.x = 10.0f * g_Bullet.GetBulletEffectHitStop();

			break;
		default:
			break;
		}

		g_Bullet.m_Coordinate.m_Pos.x += g_Bullet.m_Coordinate.m_Vel.x;
		g_Bullet.m_Coordinate.m_Pos.y += g_Bullet.m_Coordinate.m_Vel.y;


		//if (g_Bullet.GetExp() == 0)
		//{
		//	g_Bullet.SetUse(false);
		//}

		if (g_Bullet.m_Coordinate.m_Pos.x > SCREEN_WIDTH + 100.0f)
		{
			g_Bullet.SetUse(false);
		}
	}
	//弾消す
	for (Enemy* enemy : *enemies) {
		if (enemy == nullptr) { continue; };

		if (enemy->GetIsBullet() && enemy->GetUse()) { countEnemy++; }
		
	}

	if(countEnemy <= 0) {
		g_Bullet.SetUse(false); 
	}

	//プレイヤーが死んだら、弾も消える
	if (pPlayer->GetState() == PLAYERSTATE_DEATH){
		g_Bullet.SetUse(false);
	}
}

void DrawBullet(void)
{
	if (g_Bullet.GetUse() == true)
	{
		if (countEnemy >= 1) {
			if ((g_Bullet.GetState() == BULLETSTATE_ATTRACT || g_Bullet.GetState() == BULLETSTATE_HOLDING || g_Bullet.GetState() == BULLETSTATE_ATTACK) &&
				g_Bullet.GetLevel() < 3.0f)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);
				DrawSpriteAnim(g_Bullet.m_Coordinate, 8, 1, g_AnimationFrame);
			}
			else if ((g_Bullet.GetState() == BULLETSTATE_ATTRACT || g_Bullet.GetState() == BULLETSTATE_HOLDING || g_Bullet.GetState() == BULLETSTATE_ATTACK) &&
				g_Bullet.GetLevel() >= 3.0f) {
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_MaxBulletTexture);
				DrawSpriteAnim(g_Bullet.m_Coordinate, 8, 1, g_AnimationFrame);
			}

		}
	}
}

void FinalizeBullet(void)
{
	UnInitTexture(g_Texture);
	UnInitTexture(g_MaxBulletTexture);
	UnInitTexture(g_TextureULT);
}

//bool CheckBulletStateAttack(void)
//{
//	if (g_Bullet.GetState() == BULLETSTATE_ATTACK) {
//		return true;
//	}
//	return false;
//}

Bullet* GetBullet(void)
{
	return&g_Bullet;
}

void SetBullet(Bullet b)
{
	g_Bullet = {};
	g_Bullet = b;
	g_Bullet.SetUse(true);
}

