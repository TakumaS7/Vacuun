//-------------------------------------------------------------
// enemyStone.h
// 製作：楜澤旭渉日付：2024/12/11
//-------------------------------------------------------------

//--------------------------概要-------------------------------
// まっすぐ動く敵です。
//-------------------------------------------------------------
#ifndef _ENEMY_STONE_H
#define _ENEMY_STONE_H

#include "EnemyBase.h"
#include "texture.h"

static constexpr int ENEMY_MAX = 1000;


class EnemyStraight :public Enemy {

private:
	float m_Speed = 6.0f;		//速度
	XMFLOAT2 m_TargetPos = { 0.0f,0.0f };	//まっすぐ移動する"前"の目的地

	bool m_MoveStart = false;	//方角に従って動いてオッケーなら true にする


	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
	bool m_isUse = false;
	int m_MovePattern = 0;
	
public:


	EnemyStraight() {}
	EnemyStraight(float x, float y, int MovePattern) {
		m_Coordinate.m_Pos.x = x;
		m_Coordinate.m_Pos.y = y;
		m_Coordinate.m_Size = { 100.0f, 100.0f };
		m_Speed = 5.0f;
		m_Texture[ENEMYSTATE_NORMAL] = InitTexture(L"asset\\texture\\tako01.png");
		m_Texture[ENEMYSTATE_ATTACK] = InitTexture(L"asset\\texture\\tako01.png");
		m_Texture[ENEMYSTATE_DAMEGED] = InitTexture(L"asset\\texture\\tako01.png");
		m_Texture[ENEMYSTATE_KNOCKOUT] = InitTexture(L"asset\\texture\\tako01.png");
		m_Texture[ENEMYSTATE_ABSORBED] = InitTexture(L"asset\\texture\\tako01.png");
		m_Texture[ENEMYSTATE_BULLET] = InitTexture(L"asset\\texture\\tako01.png");

		m_HitTexture = InitTexture(L"asset\\texture\\EFFECT_HIT.png");

		m_MovePattern = MovePattern;
	};
	~EnemyStraight() {
		for (int i = 0; i < ENEMYSTATE_MAX; i++) {
			SAFE_RELEASE(m_Texture[i]);
		}
	};

	

	int GetMovePattern() override { return m_MovePattern; }
	Coordinate GetCoordinate() const override { return m_Coordinate; }
	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
	ENEMYSTATE GetState(void)const override { return m_State; };
	void SetState(ENEMYSTATE state)override { m_State = state; };
	void Update()override;
	void Draw() override;
};

#endif