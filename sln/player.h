#pragma once

#include "main.h"
#include "coordinate.h"
#include "enemyProduct.h"
#include "sprite.h"

#define PLAYER_INVINCIBILITY 120

//プレイヤーの状態
enum  PLAYERSTATE {
	//通常状態
	PLAYERSTATE_DEFAULT = 0,
	//被ダメ後の無敵時間
	PLAYERSTATE_DAMAGED,
	//吸い込んでいる状態
	PLAYERSTATE_ABSORB,
	//弾を引き寄せている状態
	PLAYERSTATE_ATTRACT,
	//弾を保持している状態
	PLAYERSTATE_HOLD,
	//弾を放った後
	PLAYERSTATE_COOLDOWN,
	//プレイヤー死亡
	PLAYERSTATE_DEATH,
	//弾が爆発
	PLAYERSTATE_EXPLODE,
	//ダメージ後の無敵時間


	PLAYERSTATE_MAX
};

class EnemyProduct;

class Player
{
private:
	PLAYERSTATE m_State = PLAYERSTATE_DEFAULT;

	XMFLOAT2 m_AimingRange = { 400.0f, 400.0f };									//プレイヤーが吸い込める範囲の初期状態
	XMFLOAT2 m_AimingRangeMax = { 700.0f, 700.0f };									//プレイヤーが吸い込める範囲の最大値。ここを変えれば強化要素になる
	XMFLOAT2 m_AimingCenter = { SCREEN_WIDTH - 600.0f, SCREEN_HEIGHT / 2.0f };		//プレイヤーの吸い込む範囲の中心点
	XMFLOAT2 m_ARExpansionSpeed = { 5.0f, 5.0f };									//吸い込める範囲が拡大するスピード。ここを変えれば強化要素になる
	float m_AimingSpeed;															//吸い込める範囲が移動するスピード

	unsigned int m_Exp = 0;											//現在取得している経験値
	unsigned int m_NeedExp;											//レベルアップまでに必要な経験値
	unsigned int m_Level = 1;										//現在のレベル

	unsigned int m_CoolTime = 30;										//次攻撃できるまでのクールタイム
	unsigned int m_LifeMax = 6;										//体力の最大値
	int m_Life;														//現在の体力
	bool m_Damaged = false;											//ダメージを受けた後の無敵時間。trueで無敵
	unsigned int m_DamageInterval = 0;								//ダメージを受けた後の無敵時間のカウント
	unsigned int m_AbsorbTimeLimit = 180;							//一度に吸い込める制限時間
	unsigned int m_HoldingTimeLimit = 600;							//作った球を目の前に保持していられる時間

	bool m_Death = false;				//死亡スイッチ
	bool m_DeathJump = true;			//死んだ時にジャンプするスイッチ

	bool m_UltStart = false;	/************ 追加 ************/

	/***　任天堂コントローラー　*****************************************************************/

	//Wiiリモコン
	XMVECTOR m_WiiAccelerometerVec{};
	XMVECTOR m_OldAccelerometerVec{};
	const char* m_WiiButton;
	bool m_WiiButtonDown = false;
	bool m_Ranble = false;

	//ヌンチャク
	XMVECTOR m_NunchuckJoyconVec{};
	const char* m_NunchuckButton;
	bool m_NunchuckButtonDown = false;
	/********************************************************************************************/

public:
	/***　任天堂コントローラー　*****************************************************************/
	bool m_WiiButtonA = false;
	bool m_WiiButtonB = false;

	bool m_NunchuckButtonZ = false;
	bool m_NunchuckButtonC = false;
	bool m_OldNunchuckButtonC = false;
	bool m_NextNunchuckButtonC = false;
	/********************************************************************************************/

	Coordinate m_Coordinate;

	void GainExp(int exp) { m_Exp += exp; }



	void Damage(int damage) {
		if (m_Damaged == false) {
			m_Life -= damage;
			m_DamageInterval = 0;
			m_Damaged = true;
			SetShake(5);

			if (m_Life <= 0) {
				m_Life = 0;
			}
		}
	}
	void AddDamageInterval(void) {
		m_DamageInterval++;
		if (m_DamageInterval >= 60) { m_Damaged = false; }
	}

	void Heal(int heal) { m_Life += heal; }

	Player()
	{
		m_Coordinate.m_Vel = { 0.0f, 0.0f, 0.0f };
		m_CoolTime = 0;
	}

	~Player() {};


	//各種privateステータスのゲッター、セッター
	//プレイヤーの状態STATE
	PLAYERSTATE GetState(void) { return m_State; };														//PLAYER_STATE
	void SetState(PLAYERSTATE state) { m_State = state; }
	bool GetDeath(void) { return m_Death; }																//m_Deth
	void SetDeath(bool death) { m_Death = death; }
	bool GetDeathJump(void) { return m_DeathJump; }														//m_DethJump
	void SetDeathJump(bool DeathJump) { m_DeathJump = DeathJump; }

	//吸い込む範囲関連
	XMFLOAT2 GetAimingRange(void) { return m_AimingRange; }												//AimingRange
	void SetAimingRange(XMFLOAT2 AimingRange) { m_AimingRange = AimingRange; }
	XMFLOAT2 GetAimingRangeMax(void) { return m_AimingRangeMax; }										//AimingRangeMax
	void SetAimingRangeMax(XMFLOAT2 AimingRangeMax) { m_AimingRangeMax = AimingRangeMax; }
	XMFLOAT2 GetAimingCenter(void) { return m_AimingCenter; }											//AimingCenter
	void SetAimingCenter(XMFLOAT2 AimingCenter) { m_AimingCenter = AimingCenter; }
	XMFLOAT2 GetARExpansionSpeed(void) { return m_ARExpansionSpeed; }									//ARExpansionSpeed
	void SetARExpansionSpeed(XMFLOAT2 ARExpansionSpeed) { m_ARExpansionSpeed = ARExpansionSpeed; }
	float GetAimingSpeed(void) { return m_AimingSpeed; }												//AimingSpeed
	void SetAimingSpeed(float AimingSpeed) { m_AimingSpeed = AimingSpeed; }

	//レベル関連
	unsigned int GetExp(void) { return m_Exp; }
	void SetExp(unsigned int Exp) { m_Exp = Exp; }
	unsigned int GetNeedExp(void) { return m_NeedExp; }
	void SetNeedExp(unsigned int NeedExp) { m_NeedExp = NeedExp; }
	unsigned int GetLevel(void) { return m_Level; }
	void SetLevel(unsigned int Level) { m_Level = Level; }

	//その他ステータス関連
	unsigned int GetCoolTime(void) { return m_CoolTime; }
	void SetCoolTime(unsigned int CoolTime) { m_CoolTime = CoolTime; }
	unsigned int GetLifeMax(void) { return m_LifeMax; }
	void SetLifeMax(unsigned int LifeMax) { m_LifeMax = LifeMax; }
	unsigned int GetLife(void) { return m_Life; }
	void SetLife(unsigned int Life) { m_Life = Life; }
	bool GetDamaged(void) { return m_Damaged; }
	unsigned int GetAbsorbTimeLimit(void) { return m_AbsorbTimeLimit; }
	void SetAbsorbTimeLimit(unsigned int AbsorbTimeLimit) { m_AbsorbTimeLimit = AbsorbTimeLimit; }
	unsigned int GetHoldingTimeLimit(void) { return m_HoldingTimeLimit; }
	void SetHoldingTimeLimit(unsigned int HoldingTimeLimit) { m_HoldingTimeLimit = HoldingTimeLimit; }
	
	/************ 追加↓ ************/
	bool GetUltStart() { return m_UltStart; }
	void SetUltStart(bool start) { m_UltStart = start; }
	/************ 追加↑ ************/

	void SetDamageInterval() {   
		m_DamageInterval = 0;
		m_Damaged = true;
	};

	//下の奴らはレベルアップ時の処理
	void ShortCool(int coolshort) { m_CoolTime -= coolshort; }				//攻撃後のクールタイムの短縮
	void AddMaxHp(int add) {												//最大体力の増加(現在の体力も増えた分回復する)
		m_LifeMax += add;
		m_Life += add;
	}
	void AddAbsorbTimeLimit(int add) { m_AbsorbTimeLimit += add; }			//吸い込む制限時間の延長

	/***　任天堂コントローラー　*****************************************************************/

	//Wiiリモコン
	void SetWiiAccelerometerVector(XMVECTOR vector) { m_WiiAccelerometerVec = vector; }
	XMVECTOR GetWiiAccelerometerVector() const { return m_WiiAccelerometerVec; }

	void SetWiiButton(const char* button) { m_WiiButton = button; }
	void SetWiiButtonDown(bool down) { m_WiiButtonDown = down; }
	const char* GetWiiButton() const { return m_WiiButton; }
	bool GetWiiButtonDown() { return m_WiiButtonDown; }

	void SetWiiRanble(const bool ranble) { m_Ranble = ranble; }
	bool GetWiiRanble() const { return m_Ranble; }

	//ヌンチャク
	void SetNunchuckJoyconVector(XMVECTOR vector) { m_NunchuckJoyconVec = vector; }
	XMVECTOR GetNunchuckJoyconVector() const { return m_NunchuckJoyconVec; }

	void SetNunchuckButton(const char* button) { m_NunchuckButton = button; }
	void SetNunchuckButtonDown(bool down) { m_NunchuckButtonDown = down; }
	const char* GetNunchuckButton() const { return m_NunchuckButton; }
	bool GetNunchuckButtonDown() { return m_NunchuckButtonDown; }
	/********************************************************************************************/
};

void InitializePlayer();
void FinalizePlayer();
void UpdatePlayer(EnemyProduct* enemyProduct);
void DrawPlayer();
void DrawPlayerUI();

void MovePlayer();

Player* GetPlayer(void);

void ChangeNeedExp();

void CheckLevelUp();

void UpLevel();

void AbsorbedOnce();

void DecMakingBulletExp();

float GetHoldingTime();