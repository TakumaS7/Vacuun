////-------------------------------------------------------------
//// enemy_01.h
//// 製作：笠原凜　日付：2024/11/06
////-------------------------------------------------------------
//
////--------------------------概要-------------------------------
//// まっすぐ動く敵です。
//// 物によっては画面外から登場し、所定の位置についてから
//// 移動を開始する敵も用意できる形になっています。
////-------------------------------------------------------------
//
//
//#ifndef _ENEMY_01_H
//#define _ENEMY_01_H
//
//#include "main.h"
//#include "EnemyBase.h"
//#include "texture.h"
//
//static constexpr int ENEMY_MAX = 1000;
//
//
//class EnemyStraight :public Enemy {
//
//private:
//	float m_Speed = 6.0f;		//速度
//	XMFLOAT2 m_TargetPos = { 0.0f,0.0f };	//まっすぐ移動する"前"の目的地
//
//	bool m_MoveStart = false;	//方角に従って動いてオッケーなら true にする
//
//
//	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
//	EnemyData  m_EnemyState;
//	bool m_isUse = false;
//	int m_MovePattern = 0;
//	
//
//	ID3D11ShaderResourceView* m_Texture[2];
//
//
//public:
//	
//
//	EnemyStraight() = default;
//	EnemyStraight(float x, float y, int MovePattern) {
//		m_Coordinate.m_Pos.x = x;
//		m_Coordinate.m_Pos.y = y;
//		m_Coordinate.m_Size = { 100.0f, 100.0f };
//		m_EnemyState.m_Speed = 6.0f;
//		g_Texture[0] = InitTexture(L"asset\\texture\\tako01.png");
//		g_Texture[1] = InitTexture(L"asset\\texture\\tako01.png");
//		m_MovePattern = MovePattern;
//	};
//
//	~EnemyStraight() {
//		for (int i = 0; i < 2; i++){
//			SAFE_RELEASE(g_Texture[i]);
//		}
//	};
//
//	
//
//	int GetMovePattern() override { return m_MovePattern; }
//	EnemyData GetEnemyData()override { return m_EnemyState; }
//	void SetMoveVec(XMFLOAT3 MVec) override { m_EnemyState.m_MoveVector = MVec; };
//	void SetIsChase()override { m_EnemyState.isChase = false; }
//	void SetStep(int num)override { m_EnemyState.step = num; }
//	int  GetStep() override {return  m_EnemyState.step;};
//	Coordinate GetCoordinate() const override { return m_Coordinate; }
//	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat;}
//	ENEMYSTATE GetState(void)override { return m_State; };
//	void SetState(ENEMYSTATE state)override { m_State = state; };
//	void Update()override;
//	void Draw() override;
//};
//
//
////雑魚弱エネミー
//class EnemyWeak : public Enemy
//{
//private:
//	int m_MovePattern = 0;
//
//	XMVECTOR m_Center = {};   //回転の中心
//	float m_Radius = 0.0f;      //半径
//	float m_Omega = 0.0f;;       //角速度
//	float m_Angle = 0.0f;;       //現在の角度
//	XMVECTOR m_Position = {}; //現在の位置
//
//	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
//	EnemyData m_EnemyState;
//	bool m_isUse = false;
//
//	XMVECTOR m_PlayerVector; //プレイヤーのベクター
//	XMVECTOR m_EnemyVector; //エネミーのベクター
//	XMVECTOR direction; //プレイヤーとエネミーの距離
//
//	ID3D11ShaderResourceView* g_Texture[2];
//public:
//	EnemyWeak() {};
//	EnemyWeak(float x, float y, int MovePattern)
//	{
//		m_Coordinate.m_Pos.x = x;
//		m_Coordinate.m_Pos.y = y;
//		m_Coordinate.m_Size = {100.0f, 100.0f};
//		m_EnemyState.m_Speed = 6.0f;
//		g_Texture[0] = InitTexture(L"asset\\texture\\tako01.png");
//		g_Texture[1] = InitTexture(L"asset\\texture\\tako01.png");
//		m_MovePattern = MovePattern;
//	}
//	~EnemyWeak() {};
//
//	int GetMovePattern() override { return m_MovePattern; }
//	EnemyData GetEnemyData()override { return m_EnemyState; }
//	void SetMoveVec(XMFLOAT3 MVec) override { m_EnemyState.m_MoveVector = MVec; };
//	void SetIsChase()override { m_EnemyState.isChase = false; }
//	void SetStep(int num)override { m_EnemyState.step = num; }
//	int  GetStep() override { return  m_EnemyState.step; };
//	Coordinate GetCoordinate() const override{ return m_Coordinate; }
//	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
//	ENEMYSTATE GetState(void)override { return m_State; };
//	void SetState(ENEMYSTATE state)override { m_State = state; };
//	void Draw() override;
//	void Update()override;
//};
//
////雑魚強エネミー
//class EnemyWeakRegist : public Enemy
//{
//private:
//	int m_MovePattern = 0;
//
//	XMVECTOR m_Center = {};   //回転の中心
//	float m_Radius = 0.0f;      //半径
//	float m_Omega = 0.0f;;       //角速度
//	float m_Angle = 0.0f;;       //現在の角度
//	XMVECTOR m_Position = {}; //現在の位置
//
//	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
//	EnemyData m_EnemyState;
//	bool m_isUse = false;
//
//	XMVECTOR m_PlayerVector; //プレイヤーのベクター
//	XMVECTOR m_EnemyVector; //エネミーのベクター
//	XMVECTOR direction; //プレイヤーとエネミーの距離
//
//	ID3D11ShaderResourceView* g_Texture[2];
//public:
//	EnemyWeakRegist() = default;
//	EnemyWeakRegist(float x, float y, int MovePattern)
//	{
//		m_Coordinate.m_Pos.x = x;
//		m_Coordinate.m_Pos.y = y;
//		m_Coordinate.m_Size = { 100.0f, 100.0f };
//		m_EnemyState.m_Speed = 6.0f;
//		g_Texture[0] = InitTexture(L"asset\\texture\\tako01.png");
//		g_Texture[1] = InitTexture(L"asset\\texture\\tako01.png");
//		m_MovePattern = MovePattern;
//	}
//	~EnemyWeakRegist() {};
//
//	int GetMovePattern() override { return m_MovePattern; }
//	EnemyData GetEnemyData()override { return m_EnemyState; }
//	void SetMoveVec(XMFLOAT3 MVec) override { m_EnemyState.m_MoveVector = MVec; };
//	void SetIsChase()override { m_EnemyState.isChase = false; }
//	void SetStep(int num)override { m_EnemyState.step = num; }
//	int  GetStep() override { return  m_EnemyState.step; };
//	Coordinate GetCoordinate() const override { return m_Coordinate; }
//	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
//	ENEMYSTATE GetState(void)override { return m_State; };
//	void SetState(ENEMYSTATE state)override { m_State = state; };
//	void Draw() override;
//	void Update()override;
//};
//
////雑魚特殊エネミー
//class EnemyUnique : public Enemy
//{
//private:
//	int m_MovePattern = 0;
//
//	Coordinate m_BarrierState;
//	bool isBarrier = false;//バリア判定
//
//	ID3D11ShaderResourceView* g_TextureEnemyUnique;
//	ID3D11ShaderResourceView* g_TextureBarrier;
//
//	XMVECTOR m_Center = {};   //回転の中心
//	float m_Radius = 0.0f;      //半径
//	float m_Omega = 0.0f;;       //角速度
//	float m_Angle = 0.0f;;       //現在の角度
//	XMVECTOR m_Position = {}; //現在の位置
//
//	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
//	EnemyData  m_EnemyState;
//	bool m_isUse = false;
//
//	XMVECTOR m_PlayerVector; //プレイヤーのベクター
//	XMVECTOR m_EnemyVector; //エネミーのベクター
//	XMVECTOR direction; //プレイヤーとエネミーの距離
//
//	
//public:
//	EnemyUnique() = default;
//	EnemyUnique(float x, float y, int MovePattern)
//	{
//		//素材
//		g_TextureEnemyUnique = InitTexture(L"asset\\texture\\tako01.png");
//		g_TextureBarrier = InitTexture(L"asset\\texture\\maruRed2.png");
//		m_Pos = XMFLOAT3(x, y, 0.0f);
//		m_MovePattern = MovePattern;
//		m_BarrierState = m_Coordinate;
//		m_EnemyState.m_Speed = 6.0f;
//		m_Coordinate.m_Size = { 100.0f, 100.0f };
//		m_BarrierState.m_Size = { 150.0, 150.0 };
//		isBarrier = true;
//	}
//
//	~EnemyUnique()
//	{
//		SAFE_RELEASE(g_TextureEnemyUnique);
//		SAFE_RELEASE(g_TextureBarrier);
//	};
//
//	int GetMovePattern() override { return m_MovePattern; }
//	EnemyData GetEnemyData()override { return m_EnemyState; }
//	void SetMoveVec(XMFLOAT3 MVec) override { m_EnemyState.m_MoveVector = MVec; };
//	void SetIsChase()override { m_EnemyState.isChase = false; }
//	void SetStep(int num)override { m_EnemyState.step = num; }
//	int  GetStep() override { return  m_EnemyState.step; }
//	Coordinate GetCoordinate() const override { return m_Coordinate; }
//	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
//	ENEMYSTATE GetState(void)override { return m_State; };
//	void SetState(ENEMYSTATE state)override { m_State = state; };
//	void Update()override;
//	void Draw() override;
//};
//
//
////強エネミー
//class EnemyStrong : public Enemy
//{
//private:
//	int m_MovePattern = 0;
//
//	XMVECTOR m_Center = {};   //回転の中心
//	float m_Radius = 0.0f;      //半径
//	float m_Omega = 0.0f;;       //角速度
//	float m_Angle = 0.0f;;       //現在の角度
//	XMVECTOR m_Position = {}; //現在の位置
//
//	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
//	EnemyData m_EnemyState;
//	bool m_isUse = false;
//
//	XMVECTOR m_PlayerVector; //プレイヤーのベクター
//	XMVECTOR m_EnemyVector; //エネミーのベクター
//	XMVECTOR direction; //プレイヤーとエネミーの距離
//
//	ID3D11ShaderResourceView* g_Texture[2];
//public:
//	EnemyStrong() {};
//	EnemyStrong(float x, float y, int MovePattern)
//	{
//		m_Coordinate.m_Pos.x = x;
//		m_Coordinate.m_Pos.y = y;
//		m_EnemyState.m_Speed = 6.0f;
//		g_Texture[0] = InitTexture(L"asset\\texture\\tako01.png");
//		g_Texture[1] = InitTexture(L"asset\\texture\\tako01.png");
//		m_MovePattern = MovePattern;
//	}
//
//	~EnemyStrong() {};
//
//	int GetMovePattern() override { return m_MovePattern; }
//	EnemyData GetEnemyData() override { return m_EnemyState; }
//	void SetMoveVec(XMFLOAT3 MVec) override { m_EnemyState.m_MoveVector = MVec; };
//	void SetIsChase()override { m_EnemyState.isChase = false; }
//	void SetStep(int num)override { m_EnemyState.step = num; }
//	int  GetStep() override { return  m_EnemyState.step; };
//	void SetCodinate(Coordinate coordinat) override { m_Coordinate = coordinat; }
//	Coordinate GetCoordinate() const override { return m_Coordinate; }
//	ENEMYSTATE GetState(void)override { return m_State; };
//	void SetState(ENEMYSTATE state)override { m_State = state; };
//	void Draw() override;
//	void Update()override;
//};
//
//
////ボスエネミー
//class EnemyBoss : public Enemy
//{
//private:
//	int m_MovePattern = 0;
//
//	XMVECTOR m_Center = {};   //回転の中心
//	float m_Radius = 0.0f;      //半径
//	float m_Omega = 0.0f;;       //角速度
//	float m_Angle = 0.0f;;       //現在の角度
//	XMVECTOR m_Position = {}; //現在の位置
//
//	ENEMYSTATE m_State = ENEMYSTATE_NORMAL;		//エネミーの状態
//	EnemyData m_EnemyState;
//	bool m_isUse = false;
//
//	XMVECTOR m_PlayerVector; //プレイヤーのベクター
//	XMVECTOR m_EnemyVector; //エネミーのベクター
//	XMVECTOR direction; //プレイヤーとエネミーの距離
//	ID3D11ShaderResourceView* g_Texture[2];
//public:
//	EnemyBoss() {};
//	EnemyBoss(float x, float y, int MovePattern)
//	{
//		m_Coordinate.m_Pos.x = x;
//		m_Coordinate.m_Pos.y = y;
//		m_EnemyState.m_Speed = 6.0f;
//		g_Texture[0] = InitTexture(L"asset\\texture\\tako01.png");
//		g_Texture[1] = InitTexture(L"asset\\texture\\tako01.png");
//		m_MovePattern = MovePattern;
//	}
//
//	~EnemyBoss() {};
//
//	int GetMovePattern() override { return m_MovePattern; }
//	EnemyData GetEnemyData()override { return m_EnemyState; }
//	void SetMoveVec(XMFLOAT3 MVec) override { m_EnemyState.m_MoveVector = MVec; };
//	void SetIsChase()override { m_EnemyState.isChase = false; }
//	void SetStep(int num)override { m_EnemyState.step = num; }
//	int  GetStep() override { return  m_EnemyState.step; };
//	Coordinate GetCoordinate() const override { return m_Coordinate; }
//	ENEMYSTATE GetState(void)override { return m_State; };
//	void SetState(ENEMYSTATE state)override { m_State = state; };
//	void Draw() override;
//	void Update()override;
//};
//
//
////void UpdateEnemy_Straight(void);
////void DrawEnemy_Straight(void);
////EnemyStraight* GetEnemy(void);
////void SetEnemy(Coordinate c, STRAIGHT_DIRECTION d);
////
////void InitializeEnemy_Straight(void);
////void FinalizeEnemy_Straight(void);
//
//#endif // !_ENEMY_01_H