//-------------------------------------------------------------
// enemyUnique.h
// 製作：楜澤旭渉日付：2024/12/11
//-------------------------------------------------------------

//--------------------------概要-------------------------------
//バリア付与の敵
//バリア付与時は吸込み不可
// バリアは一度攻撃されると破壊される
//-------------------------------------------------------------
#ifndef _ENEMY_UNIQUE_H
#define _ENEMY_UNIQUE_H

#include "EnemyBase.h"
#include "texture.h"

class EnemyUnique : public Enemy
{
private:
	int m_MovePattern = 0;

	Coordinate m_BarrierState;
	bool isBarrier = false;//バリア判定

	ID3D11ShaderResourceView* g_TextureBarrier;

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

	ENEMYTYPE m_type = ENEMYTYPE_UNIQUE;	//エネミーの種類

public:
	EnemyUnique() = default;
	EnemyUnique(float x, float y, int MovePattern)
	{
		//素材
		m_Texture[ENEMYSTATE_NORMAL] = InitTexture(L"asset\\texture\\ENEMY_6\\ENEMY_6_IDLE.png");
		m_Texture[ENEMYSTATE_ATTACK] = InitTexture(L"asset\\texture\\ENEMY_6\\ENEMY_6_IDLE.png");
		m_Texture[ENEMYSTATE_DAMEGED] = InitTexture(L"asset\\texture\\ENEMY_6\\ENEMY_6_DAMAGE.png");
		m_Texture[ENEMYSTATE_KNOCKOUT] = InitTexture(L"asset\\texture\\ENEMY_6\\ENEMY_6_DEFEATED.png");
		m_Texture[ENEMYSTATE_ABSORBED] = InitTexture(L"asset\\texture\\ENEMY_6\\ENEMY_6_RESIST.png");
		m_Texture[ENEMYSTATE_BULLET] = InitTexture(L"asset\\texture\\ENEMY_6\\ENEMY_6_DAMAGE.png");

		m_HitTexture = InitTexture(L"asset\\texture\\EFFECT_HIT.png");

		m_Coordinate.m_Pos = XMFLOAT3(x, y, 0.0f);
		m_MovePattern = MovePattern;
		m_BarrierState = m_Coordinate;
		m_Speed = 6.0f;
		m_Coordinate.m_Size = { 100.0f, 100.0f };
		m_BarrierState.m_Size = { 150.0, 150.0 };
		isBarrier = true;
	}

	~EnemyUnique()
	{
		for (int i = 0; i < ENEMYSTATE_MAX; i++) {
			SAFE_RELEASE(m_Texture[i]);
		}
	};

	int GetMovePattern() override { return m_MovePattern; }
	Coordinate GetCoordinate() const override { return m_Coordinate; }
	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
	ENEMYSTATE GetState(void)const override { return m_State; }
	void SetState(ENEMYSTATE state)override { m_State = state; }
	int LocalGetEnemyType() const override { return m_type; }
	void Update()override;
	void Draw() override;
};

#endif