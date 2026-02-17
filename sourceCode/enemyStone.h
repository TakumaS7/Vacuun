//-------------------------------------------------------------
// enemyWeak.h
// 製作：大岡優剛日付：2024/12/13
//-------------------------------------------------------------

//--------------------------概要-------------------------------
//攻撃力を持たない敵です
//-------------------------------------------------------------

#ifndef _ENEMY_STONE_H
#define _ENEMY_STONE_H

#include "EnemyBase.h"
#include "texture.h"

//雑魚弱エネミー
class EnemyStone : public Enemy
{
private:
	int m_MovePattern = 0;

	XMVECTOR m_Center = {};   //回転の中心
	float m_Radius = 0.0f;      //半径
	float m_Omega = 0.0f;;       //角速度
	float m_Angle = 0.0f;;       //現在の角度
	XMVECTOR m_Position = {}; //現在の位置

	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
	bool m_isUse = false;

	XMVECTOR m_PlayerVector; //プレイヤーのベクター
	XMVECTOR m_EnemyVector; //エネミーのベクター
	XMVECTOR direction; //プレイヤーとエネミーの距離

	ENEMYTYPE m_type = ENEMYTYPE_STONE;	//エネミーの種類

	ID3D11ShaderResourceView* m_Texture[7];
public:
	EnemyStone() {};
	EnemyStone(float x, float y, int MovePattern)
	{
		m_Hp = 100.0f;
		m_Coordinate.m_Pos.x = x;
		m_Coordinate.m_Pos.y = y;
		m_Coordinate.m_Size = { 80.0f, 80.0f };
		m_Speed = 5.0f;
		m_Texture[ENEMYSTATE_NORMAL] = InitTexture(L"asset\\texture\\ENEMY_1\\ENEMY_1_IDLE.png");
		m_Texture[ENEMYSTATE_ATTACK] = InitTexture(L"asset\\texture\\ENEMY_1\\ENEMY_1_IDLE.png");
		m_Texture[ENEMYSTATE_DAMEGED] = InitTexture(L"asset\\texture\\ENEMY_1\\ENEMY_1_IDLE.png");
		m_Texture[ENEMYSTATE_KNOCKOUT] = InitTexture(L"asset\\texture\\ENEMY_1\\ENEMY_1_IDLE.png");
		m_Texture[ENEMYSTATE_ABSORBED] = InitTexture(L"asset\\texture\\ENEMY_1\\ENEMY_1_RESIST.png");
		m_Texture[ENEMYSTATE_BULLET] = InitTexture(L"asset\\texture\\ENEMY_1\\ENEMY_1_IDLE.png");
		m_Texture[ENEMYSTATE_ULT] = InitTexture(L"asset\\texture\\ENEMY_1\\ENEMY_1_ULT.png");

		m_HitTexture = InitTexture(L"asset\\texture\\EFFECT_HIT.png");

		m_MovePattern = MovePattern;
	}
	~EnemyStone() {
		for (int i = 0; i < ENEMYSTATE_MAX - 1; i++) {
			SAFE_RELEASE(m_Texture[i]);
		}
	};

	int GetMovePattern() override { return m_MovePattern; }
	Coordinate GetCoordinate() const override { return m_Coordinate; }
	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
	ENEMYSTATE GetState(void)const override { return m_State; };
	void SetState(ENEMYSTATE state)override { m_State = state; };
	int LocalGetEnemyType() const override { return m_type; }
	void Draw() override;
	void Update()override;
};

#endif

