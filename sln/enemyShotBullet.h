//-------------------------------------------------------------
// enemyShotBullet.h
// 製作：佐藤琢磨　日付：2024/12/14
//-------------------------------------------------------------

//--------------------------概要-------------------------------
// 敵の弾の生成を管理する
//-------------------------------------------------------------
#ifndef _ENEMYSHOTBULLET_H
#define _ENEMYSHOTBULLET_H

#include "enemyBase.h"
#include "coordinate.h"
#include "texture.h"

class EnemyShotBullet :public Enemy
{
private:
	
	ID3D11ShaderResourceView* m_Texture[5] {};
	int m_BulletPattern = 0;			//エネミーによって弾のデザイン、挙動を変えるため(enemyProductにあるenumを使う)
	float m_AnimationFrame = 1;		//今描画したいフレーム番号

	
	XMFLOAT2 m_Direction;
	bool m_isUse = false;
	int m_MovePattern = 0;

public:
	EnemyShotBullet() = default;

	EnemyShotBullet(float x, float y, int pattern)
	{
		m_Coordinate.m_Pos.x = x;	//Ｘ座標セット
		m_Coordinate.m_Pos.y = y;	//Ｙ座標セット
		m_Coordinate.m_Size = { 30.0f, 30.0f };
		m_BulletPattern = pattern;	//どのバレットなのかをここで決める
		m_Speed = 7.0f;
		m_OrgSpeed = m_Speed;

		m_Texture[0] = InitTexture(L"asset\\texture\\ENEMY_2\\ENEMY_2_BULLET.png"); //雑魚弱の弾
		m_Texture[1] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_BULLET.png"); //強敵の弾
		m_Texture[2] = InitTexture(L"asset\\texture\\ENEMY_4\\ENEMY_4_BULLET.png"); //強敵の弾
		m_Texture[3] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_BULLET.png"); //ボスの弾
		m_Texture[4] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_BULLET_02.png"); //ボスの弾
		m_HitTexture = InitTexture(L"asset\\texture\\EFFECT_HIT.png");
		

		//SetRegistPoint(10);
		
		SetRegistPoint(5);
		
		m_CanAbsorb = true;
		m_MovePattern = 1;
		m_Direction = { 1.0f, 0.0f };
	}

	~EnemyShotBullet() {
		//テクスチャのリリース
		for (int i = 0; i < 4; ++i)
		{
			UnInitTexture(m_Texture[i]);
		}
	}

	void SetDirection(XMFLOAT2 dir) {
		m_Direction = dir;
	};

	int GetMovePattern() override { return m_MovePattern; }
	Coordinate GetCoordinate() const override { return m_Coordinate; }
	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
	void Update()override;
	
	void Draw() override;


};




#endif // !_ENEMYSHOTBULLET_H_
