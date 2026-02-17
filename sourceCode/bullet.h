//============================
// bullet.h
// 
// 2024/11/1
//============================

#ifndef BULLET_H
#define BULLET_H

#include "main.h"
#include "coordinate.h"
#include "bullet.h"
#include "enemyProduct.h"

// ===================================================
// マクロ定義
// ===================================================

#define BULLET_SPEED (10.0f)	//弾の攻撃時の速度		

// ===================================================
// 列挙体宣言
// ===================================================
enum BULLETSTATE {
	BULLETSTATE_NONE,				//
	BULLETSTATE_ATTRACT,			//引き寄せられている
	BULLETSTATE_HOLDING,			//盾になっている
	BULLETSTATE_ATTACK,				//打ち出された
};


// ===================================================
// 構造体宣言
// ===================================================
class Bullet
{
private:
	float m_Atk = 0.0f;
	
	float m_Weight = 0.0f;
	float m_Level = 1.0f;
	float m_Exp = 0.0f;
	int m_NeedExp = 3;
	int m_MaxExp = 16;

	BULLETSTATE m_State = BULLETSTATE_NONE;
	bool m_isUse = false;
	bool m_ULT = false;

	float m_HitStop = 1.0f;

public:
	Coordinate m_Coordinate;

	Bullet() {}

	Bullet(float atk, float hp, float weight)
	{
		m_Atk = atk;
		
		m_Weight = weight;

	}

	~Bullet() { m_isUse = false; }

	void AddAttack(float num)
	{
		m_Atk += num;
		if (m_Atk < 1.0f) { m_Atk = 1.0f; }
	}

	void AddWeight(float num)
	{
		m_Weight += num;
		if (m_Weight < 0.0f) { m_Weight = 0.0f; }
	}

	//isUse切り替え　引数ありなら設定したほう、なければスイッチします
	void SetUse(bool isUse) { m_isUse = isUse; }
	void SetUse(void)
	{
		if (m_isUse) { m_isUse = false; }
		else { m_isUse = true; }
	}
	bool GetUse(void) { return m_isUse; }

	void SetULT(bool ult) { m_ULT = ult; }
	bool GetULT(void) { return m_ULT; }

	//レベル関連の関数(セッター、ゲッター、加算など)
	float GetLevel(void) { return m_Level; }
	//void UpLevel(void) {
	//	m_Level += 1;
	//}
	//void DownLevel(void) {
	//	m_Level -= 1;
	//	if (m_Level < 0) { m_Level = 0; }
	//}
	float GetExp(void) { return m_Exp; }
	void AddExp(float Add) { m_Exp += Add; }
	//void DecExp() { m_Exp -= 0.5f; }
	unsigned int GetNeedExp(void) { return m_NeedExp; }
	unsigned int GetMaxExp(void) { return m_MaxExp; }

	void CheckBulletLevelUp(void)
	{
		if (m_Exp == 0.0f) {
			m_Level = 0.0f;
		}
		else if (0.5f <= m_Exp && m_Exp < 2.5f) {
			m_Level = 0.5f;
		}
		else if (2.5f <= m_Exp && m_Exp < 4.5f) {
			m_Level = 1.0f;
		}
		else if (4.5f <= m_Exp && m_Exp < 6.5f) {
			m_Level = 1.5f;
		}
		else if (6.5f <= m_Exp && m_Exp < 8.5f) {
			m_Level = 2.0f;
		}
		else if (8.5f <= m_Exp && m_Exp < 10.5f) {
			m_Level = 2.5f;
		}
		else if (10.5f <= m_Exp && m_Exp < 12.5f) {
			m_Level = 3.0f;
		}
		else if (12.5f <= m_Exp && m_Exp < 14.5f) {
			m_Level = 3.5f;
		}
		else if (14.5f <= m_Exp && m_Exp < 15.5f) {
			m_Level = 4.0f;
		}
		else if (15.5f <= m_Exp && m_Exp < m_MaxExp) {
			m_Level = 5.0f;
		}
		else if (m_MaxExp < m_Exp) {
			m_Exp = m_MaxExp;
		}


		////現在の経験値が必要数以上に達したとき、レベルアップ処理を行う
		//if (m_Exp >= m_NeedExp)
		//{
		//	AddExp(m_NeedExp * -1);
		//	UpLevel();
		//	//もう一回自身を実行する
		//	CheckBulletLevelUp();
		//}
		////現在の経験値がマイナスになったとき、レベルダウン処理を行う
		//if (m_Exp < 0)
		//{
		//	AddExp(m_NeedExp);
		//	DownLevel();
		//	//もう一回自信を実行する
		//	CheckBulletLevelUp();
		//}
	}

	void SetState(BULLETSTATE state) { m_State = state; }
	BULLETSTATE GetState(void) { return m_State; }

	void SetBulletEffectHitStop(float vel) { m_HitStop = vel; }
	float GetBulletEffectHitStop() const { return m_HitStop; }
};


// ===================================================
// プロトタイプ宣言
// ===================================================
void InitializeBullet(void);
void UpdateBullet(EnemyProduct* enemyProduct);
void DrawBullet(void);
void FinalizeBullet(void);

//bool CheckBulletStateAttack(void);
Bullet* GetBullet(void);
void SetBullet(Bullet b);

#endif



