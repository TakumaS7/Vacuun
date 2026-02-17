//-------------------------------------------------------------
// enemyBoss.h
// 製作：大岡優剛	日付：2025/01/21
//-------------------------------------------------------------

//--------------------------概要-------------------------------
//ボス敵です
//-------------------------------------------------------------

#ifndef _ENEMY_BOSS_H
#define _ENEMY_BOSS_H

#include "EnemyBase.h"
#include "texture.h"
#include "sprite.h"
#include "video_texture.h"
#include <queue>
#include "sound.h"

#define BOSS_RESET_POS_X 1000
#define BOSS_RESET_POS_Y 400

enum BossAttack
{
	ATTACK_NONE = 0,
	ATTACK_IDLE,
	ATTACK_SINGLE_POINST,
	ATTACK_DISPERSION,
	ATTACK_STRAIGHT,
	ATTACK_RISE,
	ATTACK_RESET_POSITION,
	ATTACK_ULTIMATE,        // 新規追加: 必殺技
	ATTACK_ULTIMATE_PREPARE,         // ボス移動準備
	ATTACK_BULLET_FALL,     // 弾落下フェーズ
	ATTACK_BOSS_FALL,       // ボス落下フェーズ
	ATTACK_MAX
};

class EnemyBoss : public Enemy
{
private:
	int m_MovePattern = 0;

	


	XMVECTOR m_Position = {}; //現在の位置

	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
	bool m_isUse = false;
	float m_AttackStartTime = 0.0f;
	float m_RiseTime = 0.0f;
	int m_RisePhase = 0;
	XMVECTOR m_PlayerVector; //プレイヤーのベクター
	XMVECTOR m_EnemyVector; //エネミーのベクター
	XMVECTOR direction; //プレイヤーとエネミーの距離
	float m_IdleStartTime = 0.0f;
	int m_AttackPhase = 0;
	float dispersionTime = 0.0f;
	BossAttack m_BossAttack = ATTACK_IDLE;	//現在の攻撃

	bool IsEntry = false; //入場判定

	bool m_isMovePhase = false; //フェーズ変更

	bool isBreaked = false; //撃破判定

	ID3D11ShaderResourceView* m_SadowTexture; //ボスの影
	XMFLOAT2 m_SadowSize = { 0.0f ,0.0f};//影の大きさ
	//XMFLOAT2 m_SadowSizeMax = { 250.0f ,150.0f };//影の最大の大きさ
	
	ID3D11ShaderResourceView* m_FrameTexture;	//HPフレーム
	ID3D11ShaderResourceView* m_HpTexture;		//HPバー
	ID3D11ShaderResourceView* m_NameTexture;		//名前
	ID3D11ShaderResourceView* m_SecondNameTexture;		//二つ名

	ID3D11ShaderResourceView* m_CrashTexture;		//ひび割れ
	ID3D11ShaderResourceView* m_DyingTexture;	//瀕死エフェクト

	ID3D11ShaderResourceView* m_TargetTexture;	//ターゲット

	VideoTexture g_VideoTexture;

	std::queue<XMFLOAT2> playerHistory;

	bool m_UsedSpecial = false;

	bool m_IsBossShot = false;

	bool m_IsShowCrackEffect = false;

	int m_AnimFrame = 0;

	XMFLOAT3 m_CrashPos = {};

	bool isPrepare = false;

	bool isDyingEffect = false;

	int m_BounceStartTime = 0;

	float m_StartPosX = 0.0f;
	float m_MaxPosY = 0.0f;
	float m_TargetPosX = 0.0f;
	float m_TargetPosY = 0.0f;
	bool  isBound = false;

	std::vector<XMFLOAT3> m_TargetPositions; // ターゲットの位置リスト
	int m_TargetStartTime = 0; // ターゲットマーカー開始時間
	int m_BulletFallCount = 0; // 落下カウント
	
	int m_SE[6]; //サウンド
	bool isEntrySE;
	bool Entry();

	void Attack();

	void FirstPhase();

	void SecondPhase();

	void AttackIdle();

	void EnemyBossShot(XMFLOAT2 pos, int type, int dir);

	void AttackStraight();

	void ResetPosition();

	void AttackSinglePoint();

	void AttackDispersion(XMFLOAT2 pos);

	void AttackRise();

	void BossDamege();

	

	//ボスの死亡判定
	bool CheckBossBreaked()
	{
		if (m_Hp <= 0) {
			return true;
		}

		return false;
	}


	void BossBreaked();

	void AttackUltimate();      // 必殺技
	void AttackPrepare();       // 画面外移動
	void AttackBulletFall();    // 弾落下フェーズ
	void AttackBossFall();      //ボス落下フェーズ
	void RecordPlayerPosition(); // プレイヤーの座標を記録

public:
	EnemyBoss() {};
	EnemyBoss(float x, float y, int MovePattern)
	{
		m_Coordinate.m_Pos.x = SCREEN_WIDTH + 300.0f;
		m_Coordinate.m_Pos.y = y;
		m_Coordinate.m_Size = { 500.0f, 500.0f };
		m_Speed = 5.0f;
		m_OrgSpeed = m_Speed;
		m_Hp = 1000;

		isEntrySE = false;

		m_Texture[ENEMYSTATE_NORMAL] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_IDLE.png");
		m_Texture[ENEMYSTATE_ATTACK] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_IDLE02.png");
		m_Texture[ENEMYSTATE_DAMEGED] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_DAMAGE.png");
		m_Texture[ENEMYSTATE_KNOCKOUT] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_DEFEATED.png");
		m_Texture[ENEMYSTATE_ABSORBED] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_IDLE.png");
		m_Texture[ENEMYSTATE_BULLET] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_DAMAGE.png");
		m_Texture[ENEMYSTATE_ATTACK_PREPARE] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_ATTACK02.png");
		m_Texture[ENEMYSTATE_ATTACK_02] = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_ATTACK_01.png");

		m_SadowTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_SHADOW.png");

		m_FrameTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_HPBAR_01.png");
		m_HpTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_HPBAR_02.png");

		m_NameTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_NAME_01.png");
		m_SecondNameTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_NAME_02.png");

		m_CrashTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\crash5.png");
		m_MovePattern = MovePattern;

		m_DyingTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_DYING.png");

		m_TargetTexture = InitTexture(L"asset\\texture\\ENEMY_BOSS\\ENEMY_5_EXPLOTIONRANGE.png");

		m_SE[0] = LoadSound("asset\\SE\\boss_entry.wav");
		m_SE[1] = LoadSound("asset\\SE\\boss_shot.wav");
		m_SE[2] = LoadSound("asset\\SE\\boss_steam.wav");
		m_SE[3] = LoadSound("asset\\SE\\boss_stright.wav");
		m_SE[4] = LoadSound("asset\\SE\\boss_fall.wav");
		m_SE[5] = LoadSound("asset\\SE\\crash.wav");

		m_HitShakeTime = 5.0f;

		VideoTexture::createAPI();  // Video API 初期化
		g_VideoTexture.create("asset\\texture\\ENEMY_BOSS\\crash.mp4");
		g_VideoTexture.setLoop(false);

		m_BounceStartTime = 0.0f;
		isBound = false;
		

		SetCanAbsorb(false);

		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		SetIsShake(true);
	}

	~EnemyBoss() {
		for (int i = 0; i < ENEMYSTATE_MAX; i++) {
			SAFE_RELEASE(m_Texture[i]);
		}

		StopSound(m_SE[0]);
		StopSound(m_SE[1]);
		StopSound(m_SE[2]);
		StopSound(m_SE[3]);
		StopSound(m_SE[4]);
		StopSound(m_SE[5]);
		SAFE_RELEASE(m_SadowTexture);
		SAFE_RELEASE(m_HpTexture);
		SAFE_RELEASE(m_NameTexture);
		SAFE_RELEASE(m_SecondNameTexture);

		g_VideoTexture.destroy();
		VideoTexture::destroyAPI();
	};

	int GetMovePattern() override { return m_MovePattern; }
	Coordinate GetCoordinate() const override { return m_Coordinate; }
	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
	ENEMYSTATE GetState(void)const override { return m_State; };
	void SetState(ENEMYSTATE state)override { m_State = state; };
	void MoveBreaked();
	float EaseOutBounce(float t);
	void BounceMovement();
	void Draw() override;
	void DrawUI()override;
	void Update()override;
};

#endif