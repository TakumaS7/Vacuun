//-------------------------------------------------------------
// enemyWeak.h
// 製作：楜澤旭渉日付：2024/12/11
//-------------------------------------------------------------

//--------------------------概要-------------------------------
//吸い込まれるときに抵抗がなく弱い敵です
//-------------------------------------------------------------

#ifndef _ENEMY_WEAK_H
#define _ENEMY_WEAK_H

#include "EnemyBase.h"
#include "texture.h"

//雑魚弱エネミー
class EnemyWeak : public Enemy
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

	ENEMYTYPE m_type = ENEMYTYPE_WEAK;	//エネミーの種類

public:
	EnemyWeak() {};
	EnemyWeak(float x, float y, int MovePattern)
	{
		m_Coordinate.m_Pos.x = x;
		m_Coordinate.m_Pos.y = y;
		m_Coordinate.m_Size = { 60.0f, 60.0f };
		m_Speed = 5.0f;
		m_OrgSpeed = m_Speed;

		m_Texture[ENEMYSTATE_NORMAL] = InitTexture(L"asset\\texture\\ENEMY_2\\ENEMY_2_IDLE.png");
		m_Texture[ENEMYSTATE_ATTACK] = InitTexture(L"asset\\texture\\ENEMY_2\\ENEMY_2_IDLE.png");
		m_Texture[ENEMYSTATE_DAMEGED] = InitTexture(L"asset\\texture\\ENEMY_2\\ENEMY_2_DAMAGE.png");
		m_Texture[ENEMYSTATE_KNOCKOUT] = InitTexture(L"asset\\texture\\ENEMY_2\\ENEMY_2_DEFEATED.png");
		m_Texture[ENEMYSTATE_ABSORBED] = InitTexture(L"asset\\texture\\ENEMY_2\\ENEMY_2_RESIST.png");
		m_Texture[ENEMYSTATE_BULLET] = InitTexture(L"asset\\texture\\ENEMY_2\\ENEMY_2_DAMAGE.png");

		m_HitTexture = InitTexture(L"asset\\texture\\EFFECT_HIT.png");

		m_MovePattern = MovePattern;
	}
	~EnemyWeak() {
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