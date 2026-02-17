//-------------------------------------------------------------
// enemyWeakRegist.h
// 製作：楜澤旭渉日付：2024/12/11
//-------------------------------------------------------------

//--------------------------概要-------------------------------
//吸い込まれ時,抵抗あり
//-------------------------------------------------------------

#ifndef _ENEMY_WEAK_REGIST_H
#define _ENEMY_WEAK_REGIST_H

#include "EnemyBase.h"
#include "texture.h"

//雑魚強エネミー
class EnemyWeakRegist : public Enemy
{
private:
	int m_MovePattern = 0;

	XMVECTOR m_Center = {};   //回転の中心
	float m_Radius = 0.0f;    //半径
	float m_Omega = 0.0f;;    //角速度
	float m_Angle = 0.0f;;    //現在の角度
	XMVECTOR m_Position = {}; //現在の位置

	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
	bool m_isUse = false;

	XMVECTOR m_PlayerVector; //プレイヤーのベクター
	XMVECTOR m_EnemyVector;  //エネミーのベクター
	XMVECTOR direction;      //プレイヤーとエネミーの距離

	ENEMYTYPE m_type = ENEMYTYPE_WEAK_REGIST;	//エネミーの種類
	ID3D11ShaderResourceView* m_BarrierTexture = InitTexture(L"asset\\texture\\ENEMY_3\\EFFECT_BARRIER.png");

public:
	EnemyWeakRegist() = default;
	EnemyWeakRegist(float x, float y, int MovePattern)
	{
		m_Coordinate.m_Pos.x = x;
		m_Coordinate.m_Pos.y = y;
		m_Coordinate.m_Size = { 60.0f, 60.0f };
		m_Speed = 5.0f;
		m_OrgSpeed = m_Speed;
		m_Texture[ENEMYSTATE_NORMAL] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_IDLE.png");
		m_Texture[ENEMYSTATE_ATTACK] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_IDLE.png");
		m_Texture[ENEMYSTATE_DAMEGED] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_DAMAGE.png");
		m_Texture[ENEMYSTATE_KNOCKOUT] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_DEFEATED.png");
		m_Texture[ENEMYSTATE_ABSORBED] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_RESIST.png");
		m_Texture[ENEMYSTATE_BULLET] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_DAMAGE.png");
		m_Texture[ENEMYSTATE_BARRIER] = InitTexture(L"asset\\texture\\ENEMY_3\\ENEMY_3_BARRIER.png");

		m_HitTexture = InitTexture(L"asset\\texture\\EFFECT_HIT.png");

		m_MovePattern = MovePattern;
	}
	~EnemyWeakRegist() {
		for (int i = 0; i < ENEMYSTATE_MAX; i++) {
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