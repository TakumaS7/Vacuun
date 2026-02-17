//浦部
#include "player.h"
#include "bullet.h"
#include "texture.h"
#include "sprite.h"
#include "keyboard.h"
#include "enemy_01.h"
#include "enemyProduct.h"
#include "collision.h"
#include "main.h"
#include "controller.h"
#include "score.h"
#include "stage3.h"
#include "wiimote/WiimoteManager.h"
#include "tutorial.h"
#include "sound.h"

static Player g_Player;
static Bullet g_MakingBullet;
static Controller g_Controller;
static WiimoteManager* m = nullptr;

static 	XMFLOAT2 g_AimingRange;
static unsigned int g_AimingRangeRotationSpeed = 1;
static float g_AimingRangeRotation = 0.0f;


static unsigned int g_AbsorbTime;
static unsigned int g_HoldingTime = 0;
static unsigned int g_AttackPause = 0;
static unsigned int g_CoolTime;
static unsigned int g_ULT_MAX = 20;
static float g_ULT_CHARGE = 0; /************ 変更 ************/
static unsigned int g_UltReverberationTime = PLAYER_INVINCIBILITY;

static bool g_moving = false;

static ID3D11ShaderResourceView* g_Texture[24];

static constexpr XMFLOAT3 PLAYER_INITIALIZE_POSITION = { SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f, 0.0f };
static constexpr float PLAYER_SIZE = 100.0f;

static unsigned int AnimNum = 0;
static unsigned int AnimNumMax = 0;
//static enemyInfo g_EnemyType;
static unsigned int g_DethAnimNum = 0;
static float g_DethFadeAlpha = 0.0f;		//死んだときのフェード演出
static float ExplodeAnim = 0;
static bool m_ExplodeSwitch = false;	//吸収するサークルが爆発するスイッチ
static int g_VacuumSE;
static int g_ShootSE;
static int g_DamageSE;
static int g_ExplodedSE;

void InitializePlayer()
{
	g_Player.SetState(PLAYERSTATE_DEFAULT);

	g_Player.m_Coordinate.m_Pos = PLAYER_INITIALIZE_POSITION;

	g_Player.m_Coordinate.m_Size = { PLAYER_SIZE , PLAYER_SIZE };
	g_Player.m_Coordinate.m_Scale = { 1.0f , 1.0f };
	g_Player.m_Coordinate.m_Vel = { 0.0f, 0.0f, 0.0f };

	g_Player.SetAimingCenter({ SCREEN_WIDTH - 600.0f, SCREEN_HEIGHT / 2.0f });
	g_Player.SetAimingSpeed(12.0f);

	g_Player.SetCoolTime(100);

	g_Player.SetLife(g_Player.GetLifeMax());

	g_Player.SetDeath(false);
	g_DethAnimNum = 0;

	g_AimingRangeRotation = 0.0f;
	g_AimingRangeRotationSpeed = 1;

	//g_EnemyType = GetEnemy();

//	g_AimingRange = { g_Player.GetAimingRange().x, g_Player.GetAimingRange().y };
	g_MakingBullet = {};
	SetBullet(g_MakingBullet);

	g_ULT_CHARGE = 10;

	g_Player.SetUltStart(false);

	//プレイヤー(普通)の素材
	g_Texture[0] = InitTexture(L"asset\\texture\\PLAYER\\PLAYER_IDLE_re.png");
	//プレイヤー(吸い込み中)の素材
	g_Texture[1] = InitTexture(L"asset\\texture\\PLAYER\\PLAYER_GATHER.png");
	//プレイヤー(弾発射)の素材
	g_Texture[2] = InitTexture(L"asset\\texture\\PLAYER\\PLAYER_ATTACK.png");
	//プレイヤー（死亡）の素材
	g_Texture[3] = InitTexture(L"asset\\texture\\PLAYER\\PLAYER_DEFEATED.png");
	//プレイヤーの体力(生きてる)
	g_Texture[4] = InitTexture(L"asset\\texture\\UI_HP_FULL.png");
	//プレイヤーの体力(死んでる)
	g_Texture[5] = InitTexture(L"asset\\texture\\UI_HP_IDLE.png");
	//死亡時フェード演出
	g_Texture[6] = InitTexture(L"asset\\texture\\white.png");
	//ウルトゲージの素材(ULT使える状態のゲージ)
	g_Texture[7] = InitTexture(L"asset\\texture\\ULT\\UI_ULT_ACTIVE_01.png");
	//ウルトゲージの素材(ULT使える状態の中身)
	g_Texture[8] = InitTexture(L"asset\\texture\\ULT\\UI_ULT_ACTIVE_02.png");
	//ウルトゲージの素材(ULT使えない状態のゲージ)
	g_Texture[9] = InitTexture(L"asset\\texture\\ULT\\UI_ULT_IDLE_01.png");
	//ウルトゲージの素材(ULT使えない状態の中身)
	g_Texture[10] = InitTexture(L"asset\\texture\\ULT\\UI_ULT_IDLE_02.png");
	//弾の爆発時エフェクト
	g_Texture[11] = InitTexture(L"asset\\texture\\ENEMY_3\\EFFECT_BULLET_EXPLODE.png");

	//エイミング範囲(外側からの後ろ→前の素材)
	//エイミング範囲うしろ4
	g_Texture[12] = InitTexture(L"asset\\texture\\AIMING_RANGE\\うしろ4.png");
	//エイミング範囲4
	g_Texture[13] = InitTexture(L"asset\\texture\\AIMING_RANGE\\aimingrange4.png");
	//エイミング範囲うしろ3
	g_Texture[14] = InitTexture(L"asset\\texture\\AIMING_RANGE\\うしろ3.png");
	//エイミング範囲3
	g_Texture[15] = InitTexture(L"asset\\texture\\AIMING_RANGE\\aimingrange3.png");
	//エイミング範囲うしろ2
	g_Texture[16] = InitTexture(L"asset\\texture\\AIMING_RANGE\\うしろ2.png");
	//エイミング範囲2
	g_Texture[17] = InitTexture(L"asset\\texture\\AIMING_RANGE\\aimingrange2.png");
	//エイミング範囲うしろ1
	g_Texture[18] = InitTexture(L"asset\\texture\\AIMING_RANGE\\うしろ.png");
	//エイミング範囲1
	g_Texture[19] = InitTexture(L"asset\\texture\\AIMING_RANGE\\aimingrange1.png");

	//プレイヤーアイコンUI
	g_Texture[20] = InitTexture(L"asset\\texture\\UI_ICON.png");

	//ULT中のプレイヤー
	g_Texture[21] = InitTexture(L"asset\\texture\\PLAYER\\PLAYER_ULT.png");
	g_Texture[22] = InitTexture(L"asset\\texture\\PLAYER\\PLAYER_ULT_GATHER.png");

	//エイミングの風
	g_Texture[23] = InitTexture(L"asset\\texture\\AIMING_RANGE\\EFFECT_BULLET_WIND.png");

	g_VacuumSE = LoadSound("asset\\SE\\wind.wav");
	g_ShootSE = LoadSound("asset\\SE\\Shot.wav");
	g_DamageSE = LoadSound("asset\\SE\\Damage.wav");
	g_ExplodedSE = LoadSound("asset\\SE\\Exploded.wav");


	/* Wiimote初期化 */
	m = new WiimoteManager;

	m->onConnection([](Wiimote* wiimote) {

		wiimote->onButton([wiimote](WiimoteButton button, bool down) {
			const char* buttonName = WiimoteButtons::toString(button);
			if (buttonName) {
				//buttonName：ボタンの種類, down：押したかのフラグ
				g_Player.SetWiiButton(buttonName);
				g_Player.SetWiiButtonDown(down);
			}
			});

		wiimote->onNunchuckButton([wiimote](NunchuckButton button, bool down) {
			const char* buttonName = NunchuckButtons::toString(button);
			if (buttonName) {
				//buttonName：ボタンの種類, down：押したかのフラグ
				g_Player.SetNunchuckButton(buttonName);
				g_Player.SetNunchuckButtonDown(down);
			}
			});

		wiimote->onAccelerometer([wiimote](const Vector3& v) {
			//v：Wiiリモコンの加速度
			g_Player.SetWiiAccelerometerVector({ v.x, v.y, v.z });
			}, true);

		wiimote->onNunchuckAccelerometer([wiimote](const Vector3& v) {
			//v：ヌンチャクの加速度
			}, true);

		wiimote->onMotionPlus([wiimote](const Vector3& v) {
			//v：モーションプラスの加速度
			}, true);

		wiimote->onNunchuckJoystick([&, wiimote](const Vector2& v) {
			//v：スティックの傾き
			g_Player.SetNunchuckJoyconVector({ v.x, v.y });
			}, true);

		wiimote->start(WiimoteFeatures::Buttons | WiimoteFeatures::Accelerometer | WiimoteFeatures::Extension);
		});

	m->onDisconnection([](Wiimote* wiimote) {
		wiimote->clearEvents();
		});

	m->continuousScan();
}

void FinalizePlayer()
{
	//g_Player.SetWiiRanble(false);

	delete m;

	StopSound(g_ExplodedSE);
	StopSound(g_DamageSE);
	StopSound(g_ShootSE);
	StopSound(g_VacuumSE);

	//テクスチャの解放
	for (int i = 0; i < 24; i++){
		g_Texture[i]->Release();
	}
}

void UpdatePlayer(EnemyProduct* enemyProduct)
{
	m->update();

	//for (Wiimote* wiimote : m->connected()) {
	//	wiimote->setRumble(g_Player.GetWiiRanble());
	//}

	Bullet* pBullet = GetBullet();

	if (g_Player.GetLife() <= 0){
		g_Player.SetState(PLAYERSTATE_DEATH);
		//SetScene(SCENE_TITLE);
		//UpdateTitle();
	}

	//プレイヤーが死んでないとき
	if (!g_Player.GetDeath()) {
		//死亡ジャンプON
		g_Player.SetDeathJump(true);

		//プレイヤーの移動
		MovePlayer();
	}
	//	EnemyProduct* product;
	vector<Enemy*>* enemies = enemyProduct->GetEnemy();
	for (Enemy* enemy : *enemies){
		if (enemy == nullptr) { continue; };
	
		if (enemy->GetUse() && !GetIsClear()) {
			if (CheckCircleCollider(g_Player.m_Coordinate, enemy->GetCoordinate())) {
				//プレイヤーが死んでないとき
				if (!g_Player.GetDeath() && !enemy->GetEnemyExplode() && enemy->GetIsBullet() == false) {
					//敵のタイプが4(強敵)未満だったら
					if (enemy->GetEnemyType() <= 3 || enemy->GetEnemyType() == 5) {
						enemy->SetUse(false);
					}
					if (!g_Player.GetUltStart() && !GetIsTutorial()) {
						g_Player.Damage(1);
						PlaySound(g_DamageSE, false);
						if (g_Player.GetLife() < 1) {
							g_Player.SetLife(0);
						}
					}
				}
			}
		}
	}

	if (g_ULT_CHARGE >= g_ULT_MAX) {
		g_ULT_CHARGE = g_ULT_MAX; 
	}

	if (!g_Player.GetUltStart()) {
		g_UltReverberationTime++;

		if (g_UltReverberationTime >= PLAYER_INVINCIBILITY) { 
			g_UltReverberationTime = PLAYER_INVINCIBILITY; 
		}
	}
	else {
		g_UltReverberationTime = 0;
	}

	if (g_UltReverberationTime != 0 && g_UltReverberationTime < PLAYER_INVINCIBILITY - 60) {
		g_Player.SetDamageInterval();
	}

	//Lキーを押して、ウルト発動!!
	if ((Keyboard_IsKeyDownTrigger(KK_L) || g_Controller.ControllerDownTrigger(BUTTON_LEFT_SHOULDER) || g_Player.m_NunchuckButtonC) &&
		!GetIsTutorial())
	{
		/************ 追加↓ ************/
		if (g_ULT_CHARGE >= g_ULT_MAX)
		{
			g_Player.SetUltStart(true);
		}
		/************ 追加↑ ************/
	}
	
	g_Player.SetAimingRangeMax({ 700.0f ,700.0f });
	g_Player.SetARExpansionSpeed({ 5.0f, 5.0f });
	if (g_Player.GetUltStart()) {
		g_Player.SetAimingRangeMax({ 1400.0f ,1400.0f });
		g_Player.SetARExpansionSpeed({ 20.0f, 20.0f });
	}

	
	/********　Wiiリモコンボタン　***************************************************************************************/
	if (g_Player.GetWiiButton() == "A" && g_Player.GetWiiButtonDown()) {
		g_Player.m_WiiButtonA = true;
	}
	else if (g_Player.GetWiiButton() == "A" && !g_Player.GetWiiButtonDown()) {
		g_Player.m_WiiButtonA = false;
	}

	if (g_Player.GetWiiButton() == "B" && g_Player.GetWiiButtonDown()) {
		g_Player.m_WiiButtonB = true;
	}
	else if (g_Player.GetWiiButton() == "B" && !g_Player.GetWiiButtonDown()) {
		g_Player.m_WiiButtonB = false;
	}
	/********************************************************************************************************************/

	/********　ヌンチャクボタン　****************************************************************************************/
	if (g_Player.GetNunchuckButton() == "Z" && g_Player.GetNunchuckButtonDown()) {
		g_Player.m_NunchuckButtonZ = true;
	}
	else if (g_Player.GetNunchuckButton() == "Z" && !g_Player.GetNunchuckButtonDown()) {
		g_Player.m_NunchuckButtonZ = false;
	}

	if (g_Player.GetNunchuckButton() == "C" && g_Player.GetNunchuckButtonDown()) {
		g_Player.m_OldNunchuckButtonC = true;
		g_Player.m_NextNunchuckButtonC = true;
	}
	else if (g_Player.GetNunchuckButton() == "C" && !g_Player.GetNunchuckButtonDown()) {
		g_Player.m_OldNunchuckButtonC = false;
	}

	if (g_Player.m_OldNunchuckButtonC == g_Player.m_NextNunchuckButtonC) {
		g_Player.m_NunchuckButtonC = false;
	}
	else if (g_Player.m_OldNunchuckButtonC != g_Player.m_NextNunchuckButtonC) {
		g_Player.m_NunchuckButtonC = true;
		g_Player.m_NextNunchuckButtonC = false;
	}

	/********************************************************************************************************************/

	//プレイヤーの状態のよって行動が変化
	switch (g_Player.GetState()) {

		//通常状態
	case PLAYERSTATE_DEFAULT:
		g_Player.SetAimingRange({ 400.0f, 400.0f });
		g_AimingRangeRotationSpeed = 1;

		//g_Player.SetWiiRanble(false);

		//キーを押すことで吸い込み状態に移行・Rトリガー
		if (Keyboard_IsKeyDown(KK_SPACE) || g_Controller.ControllerDown(BUTTON_RIGHT_SHOULDER))
		{
			g_AbsorbTime = 0;
			g_Player.SetState(PLAYERSTATE_ABSORB);
		}
		else if (XMVectorGetX(XMVector3Length(g_Player.GetWiiAccelerometerVector())) >= 1.5f){
			g_AbsorbTime = 0;
			g_Player.SetState(PLAYERSTATE_ABSORB);
		}

		break;



		//吸い込んでいる状態
	case PLAYERSTATE_ABSORB:
		//キーを押し続けて吸い込みむ範囲の拡大・Rトリガー
		if (Keyboard_IsKeyDown(KK_SPACE) || g_Controller.ControllerDown(BUTTON_RIGHT_SHOULDER) ||
			XMVectorGetX(XMVector3Length(g_Player.GetWiiAccelerometerVector())) >= 1.0f)
		{
			//g_Player.SetWiiRanble(true);

			g_AimingRangeRotationSpeed = 15;
			g_Player.SetAimingRange({ g_Player.GetAimingRange().x + g_Player.GetARExpansionSpeed().x, g_Player.GetAimingRange().x + g_Player.GetARExpansionSpeed().x });
			if (g_Player.GetAimingRange().x >= g_Player.GetAimingRangeMax().x)
			{
				g_Player.SetAimingRange({g_Player.GetAimingRangeMax().x, g_Player.GetAimingRange().y});
			}
			if (g_Player.GetAimingRange().y >= g_Player.GetAimingRangeMax().y)
			{
				g_Player.SetAimingRange({ g_Player.GetAimingRange().x, g_Player.GetAimingRangeMax().y});
			}

			for (Enemy* enemy : *enemies)
			{
				PlaySound(g_VacuumSE, false);

				if (enemy->GetUse())
				{
					if (CheckCircleCollider({ g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y, 0.0f },
						enemy->GetCoordinate().m_Pos, 10.0f, 10.0f))
					{
						if (enemy->GetBulletExpGain() == true)
						{
							AbsorbedOnce();
							if (enemy->GetEnemyType() == ENEMYTYPE_BULLET) { DecMakingBulletExp(); }

							enemy->SetBulletExpGain(false);
						}
					}
				}
			}


			//吸い込む制限時間を過ぎた場合、弾を保持する形態に移行する
			if (g_AbsorbTime >= g_Player.GetAbsorbTimeLimit())
			{
				for (Enemy* enemy : *enemies)
				{
					if (enemy->GetUse())
					{
						if (enemy->GetState() == ENEMYSTATE_ABSORBED)
						{
							enemy->SetState(ENEMYSTATE_ATTACK);
							if (enemy->GetEnemyType() == ENEMYTYPE_WEAK_REGIST) {
								enemy->SetState(ENEMYSTATE_BARRIER);
							}
						}
					}
				}
				g_Player.SetAimingRange({ 400.0f, 400.0f });
				g_Player.SetState(PLAYERSTATE_ATTRACT);
				g_MakingBullet.m_Coordinate.m_Pos = { g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y, 0.0f };
				g_MakingBullet.SetState(BULLETSTATE_ATTRACT);
			}

		}
		//キーを離すことで弾を引き寄せて保持する状態に移行。制限時間と吸い込む範囲を元に戻す
		else if (Keyboard_IsKeyUp(KK_SPACE) || 
			!g_Controller.ControllerDown(BUTTON_RIGHT_SHOULDER) ||
			XMVectorGetX(XMVector3Length(g_Player.GetWiiAccelerometerVector())) < 1.0f)
		{
			for (Enemy* enemy : *enemies)
			{
				if (enemy->GetUse())
				{
					if (enemy->GetState() == ENEMYSTATE_ABSORBED)
					{
						enemy->SetState(ENEMYSTATE_ATTACK);
						if (enemy->GetEnemyType() == ENEMYTYPE_WEAK_REGIST) {
							enemy->SetState(ENEMYSTATE_BARRIER);
						}
					}
				}
			}
			g_Player.SetAimingRange({ 400.0f, 400.0f });
			g_Player.SetState(PLAYERSTATE_ATTRACT);
			g_MakingBullet.m_Coordinate.m_Pos = { g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y, 0.0f };
			g_MakingBullet.SetState(BULLETSTATE_ATTRACT);
		}
		g_AbsorbTime += 1;
		break;



		//弾を引き寄せている状態
	case PLAYERSTATE_ATTRACT:
		//g_Player.SetWiiRanble(false);

		if (g_MakingBullet.GetExp() == 0) {
			g_MakingBullet = {};
			pBullet->SetUse(false);
			g_CoolTime = g_Player.GetCoolTime();
			g_Player.SetState(PLAYERSTATE_COOLDOWN);
		}


		g_MakingBullet.m_Coordinate.m_Size.x = g_MakingBullet.m_Coordinate.m_Size.y = g_MakingBullet.GetLevel() * 55.0f + 70.0f;

		if (g_MakingBullet.m_Coordinate.m_Pos.x > g_Player.m_Coordinate.m_Pos.x)//弾の位置がプレイヤーの位置より右のとき
		{
			g_MakingBullet.m_Coordinate.m_Pos.x -= 20.0f;
		}
		if (g_MakingBullet.m_Coordinate.m_Pos.y < g_Player.m_Coordinate.m_Pos.y)//弾の位置がプレイヤーの位置より上のとき
		{
			g_MakingBullet.m_Coordinate.m_Pos.y += 10.0f;//弾の位置を下に動かす
		}
		if (g_MakingBullet.m_Coordinate.m_Pos.y > g_Player.m_Coordinate.m_Pos.y)//弾の位置がプレイヤーの位置より下のとき
		{
			g_MakingBullet.m_Coordinate.m_Pos.y -= 10.0f;//弾の位置を上に動かす
		}

		if (CheckCircleCollider(g_MakingBullet.m_Coordinate, g_Player.m_Coordinate))//引き寄せている弾をプレイヤーが取得した(扱いの)とき
		{
			g_MakingBullet.m_Coordinate.m_Pos = { g_Player.m_Coordinate.m_Pos.x + 200.0f, g_Player.m_Coordinate.m_Pos.y, 0.0f };
			g_Player.SetState(PLAYERSTATE_HOLD);
		}

		SetBullet(g_MakingBullet);

		break;


		//弾を保持している状態
	case PLAYERSTATE_HOLD:
		g_AttackPause++;
		//弾のEXPが0以外の時(吸われた敵が弾に1体でもいたとき)
		if (!g_MakingBullet.GetExp() == 0) 
		{
			g_MakingBullet.m_Coordinate.m_Pos = { g_Player.m_Coordinate.m_Pos.x + 200.0f, g_Player.m_Coordinate.m_Pos.y, 0.0f };
			g_MakingBullet.m_Coordinate.m_Size.x = g_MakingBullet.m_Coordinate.m_Size.y = g_MakingBullet.GetLevel() * 55.0f + 70.0f;

			SetBullet(g_MakingBullet);

			//キーを押して弾を発射。クールタイムを設定し、クールタイムに移行・Ｂボタン
			if (Keyboard_IsKeyDownTrigger(KK_SPACE) || g_Controller.ControllerDownTrigger(BUTTON_B) || g_Player.m_NunchuckButtonZ && g_AttackPause >= 6)
			{
				PlaySound(g_ShootSE, false);

				if (g_AttackPause >= 6)
				{
					g_MakingBullet.SetState(BULLETSTATE_ATTACK);

					SetBullet(g_MakingBullet);

					g_MakingBullet = {};

					g_HoldingTime = 0;
					g_CoolTime = g_Player.GetCoolTime();
					g_Player.SetState(PLAYERSTATE_COOLDOWN);
				}
			}

			//制限時間を超えたらクールタイムに移行
 			if (g_HoldingTime >= g_Player.GetHoldingTimeLimit())
			{
				for (Enemy* enemy : *enemies)
				{
					if (enemy->GetUse())
					{
						if (enemy->GetState() == ENEMYSTATE_BULLET)
						{
							enemy->SetUse(false);
						}
					}
				}

				g_HoldingTime = 0;
				g_MakingBullet.m_Coordinate.m_Size.x = g_MakingBullet.m_Coordinate.m_Size.y = 0.0f;
				SetBullet(g_MakingBullet);
				g_MakingBullet = {};
				g_CoolTime = g_Player.GetCoolTime();
 				g_Player.SetState(PLAYERSTATE_COOLDOWN);
			}
			g_HoldingTime += 1;
		}
		//弾のEXPが0の時(吸われた敵が弾に1体もいなくなったとき)
		if (g_MakingBullet.GetExp() == 0)
		{
			g_MakingBullet = {};
			//pBullet->SetUse(false);
			g_CoolTime = g_Player.GetCoolTime();
			g_Player.SetState(PLAYERSTATE_COOLDOWN);
		}
		break;

		//弾を放った後のクールタイム   
	case PLAYERSTATE_COOLDOWN:
		//設定した時間経過で、通常状態に戻る
		g_CoolTime -= 1;
		if (g_CoolTime <= 0)
		{
			g_AttackPause = 0;

			g_AimingRangeRotationSpeed = 1;
			//g_MakingBullet = {};
			//g_MakingBullet.SetULT(false);
			g_Player.SetAimingCenter({ g_Player.m_Coordinate.m_Pos.x + 200, g_Player.m_Coordinate.m_Pos.y });
			//g_Player.SetAimingCenter({ SCREEN_WIDTH - 600.0f, SCREEN_HEIGHT / 2.0f });
			g_Player.SetState(PLAYERSTATE_DEFAULT);

			//g_Player.SetAimingCenter({ SCREEN_WIDTH - 600.0f, SCREEN_HEIGHT / 2.0f });
		}
		break;



		//プレイヤー死亡
	case PLAYERSTATE_DEATH:
		g_Player.SetUltStart(false);	/************ 追加 ************/
		//g_Player.SetWiiRanble(false);

		g_Player.SetDeath(true);

		g_Player.m_Coordinate.m_Pos.y += g_Player.m_Coordinate.m_Vel.y;

		if (g_Player.GetDeathJump()) {
			g_DethFadeAlpha = 1.0f;
			g_Player.m_Coordinate.m_Vel.y = -15.0f;
			g_Player.SetDeathJump(false);
		}
		else if (g_Player.m_Coordinate.m_Vel.y > 0) {
			g_DethAnimNum = 1;
		}

		g_DethFadeAlpha += -0.05f;
		g_Player.m_Coordinate.m_Vel.y += 0.4f;

		if (g_DethFadeAlpha <= 0.0f) {
			g_DethFadeAlpha = 0.0f;
		}

		if (g_Player.m_Coordinate.m_Pos.y >= 720.0f + (g_Player.m_Coordinate.m_Size.y * 0.5f)) {
			g_Player.m_Coordinate.m_Pos.y = 720.0f + (g_Player.m_Coordinate.m_Size.y * 0.5f);
		}
		break;

		//プレイヤーの弾が爆発
	case PLAYERSTATE_EXPLODE:
		//g_Player.SetWiiRanble(false);
		PlaySound(g_ExplodedSE, false);

		g_AimingRange = { 300.0f, 300.0f };
		g_CoolTime = g_Player.GetCoolTime();
		m_ExplodeSwitch = true;

		g_Player.SetState(PLAYERSTATE_COOLDOWN);
		break;
	}

	CheckLevelUp();
	//弾のレベルチェック
	g_MakingBullet.CheckBulletLevelUp();
	g_Player.AddDamageInterval();

	/************ 追加↓ ************/
	if (g_Player.GetUltStart() == true) {
		g_ULT_CHARGE -= 20.0f / 360.0f;

		if (g_ULT_CHARGE <= 0.0f) {
			g_Player.SetUltStart(false);
			g_ULT_CHARGE = 0.0f;
		}
	}
	/************ 追加↑ ************/
}

void DrawPlayer()
{

	//プレイヤー本体の描画
	AnimNumMax = 8;

	if (g_Player.GetState() == PLAYERSTATE_DEATH)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
		DrawSpriteAnim(g_Player.m_Coordinate, 2, 1, g_DethAnimNum);
	}
	else if (g_Player.GetDamaged() == false)
	{
		if (!g_Player.GetUltStart()) {
			if (g_Player.GetState() == PLAYERSTATE_ABSORB)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
				DrawSpriteAnim(g_Player.m_Coordinate, 8, 1, AnimNum);
			}
			else
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
				DrawSpriteAnim(g_Player.m_Coordinate, 8, 1, AnimNum);
			}
		}
		else {
			if (g_Player.GetState() == PLAYERSTATE_ABSORB)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[22]);
				DrawSpriteAnim(g_Player.m_Coordinate, 8, 1, AnimNum);
			}
			else
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[21]);
				DrawSpriteAnim(g_Player.m_Coordinate, 8, 1, AnimNum);
			}
		}
	}
	else if (g_Player.GetDamaged() == true)
	{
		if (AnimNum > 4)
		{
			if (g_Player.GetState() == PLAYERSTATE_ABSORB)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
				DrawSpriteAnim(g_Player.m_Coordinate, 8, 1, AnimNum);
			}
			else
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
				DrawSpriteAnim(g_Player.m_Coordinate, 8, 1, AnimNum);
			}
		}
	}
	
	AnimNum++;

	if (AnimNum == AnimNumMax) { AnimNum -= AnimNumMax; }



	
	g_AimingRangeRotation += 0.01f * g_AimingRangeRotationSpeed;






	//吸い込む範囲の描画
	if (g_Player.GetState() == PLAYERSTATE_DEFAULT || g_Player.GetState() == PLAYERSTATE_ABSORB)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);
		DrawSprite(g_Player.GetAimingCenter(), -g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);
		DrawSprite(g_Player.GetAimingCenter(), -g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);
		DrawSprite(g_Player.GetAimingCenter(), -g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);
		DrawSprite(g_Player.GetAimingCenter(), -g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);
		DrawSprite(g_Player.GetAimingCenter(), g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[17]);
		DrawSprite(g_Player.GetAimingCenter(), g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[18]);
		DrawSprite(g_Player.GetAimingCenter(), g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[19]);
		DrawSprite(g_Player.GetAimingCenter(), g_AimingRangeRotation, { g_Player.GetAimingRange().x / 2, g_Player.GetAimingRange().y / 2 });
	}
	if (g_Player.GetState() == PLAYERSTATE_ABSORB) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[23]);
		DrawSpriteAnimColor(g_Player.GetAimingCenter(), 0.0f, {g_Player.GetAimingRange().x / 2 - 40, g_Player.GetAimingRange().y / 2 - 40 }, 8, 1, AnimNum, {1.0f, 0.0f, 1.0f, 1.0f});
	}

	//吸い込むサークルが爆発する描画
	if (m_ExplodeSwitch) {
		ExplodeAnim += 0.3f;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);
		DrawSpriteAnim(g_Player.GetAimingCenter(), 0.0f, g_AimingRange, 6, 1, ExplodeAnim);

		if (ExplodeAnim >= 6) {
			ExplodeAnim = 0;
			m_ExplodeSwitch = false;
		}
	}

	//プレイヤー死亡時のフェード
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);
	DrawSpriteColor({ SCREEN_XCENTER, SCREEN_YCENTER }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1.0f, 1.0f, 1.0f, g_DethFadeAlpha });

}


void DrawPlayerUI()
{
	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

	//プレイヤーと重なったときに透過
	if (GetPlayer()->m_Coordinate.m_Pos.x < 350 &&
		GetPlayer()->m_Coordinate.m_Pos.y < 100)
	{
		color = { 1.0f,1.0f,1.0f,0.5f };
	}


	//プレイヤーの体力の描画(空のも含む)
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
	for (unsigned int i = 0; i < g_Player.GetLife(); i++)
	{
		DrawSpriteColor({ 125.0f + 30.0f + i * 45.0f, 40.0f }, 0.0f, XMFLOAT2(GetWidth(g_Texture[4]), GetHeight(g_Texture[4])), color);
	}
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
	for (unsigned int i = 0; i < g_Player.GetLifeMax() - g_Player.GetLife(); i++)
	{
		DrawSpriteColor({ 125.0f + 255.0f - i * 45.0f, 40.0f }, 0.0f, XMFLOAT2(GetWidth(g_Texture[5]), GetHeight(g_Texture[5])), color);
	}


	//プレイヤーアイコンの描画
	if (g_Player.GetDamaged() == true && g_UltReverberationTime != 0 && g_UltReverberationTime >= PLAYER_INVINCIBILITY) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[20]);
		DrawSpriteAnimColor({ 60.0f, 60.0f }, 0.0f, { 120.0f, 120.0f }, 4, 1, 1,color);
	}
	else if (g_Player.GetState() == PLAYERSTATE_ABSORB || g_Player.GetState() == PLAYERSTATE_COOLDOWN) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[20]);
		DrawSpriteAnimColor({ 60.0f, 60.0f }, 0.0f, { 120.0f, 120.0f }, 4, 1, 2,color);
	}
	else if (g_Player.GetLife() == 1) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[20]);
		DrawSpriteAnimColor({ 60.0f, 60.0f }, 0.0f, { 120.0f, 120.0f }, 4, 1, 3,color);
	}
	else if (g_UltReverberationTime < PLAYER_INVINCIBILITY) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[20]);
		DrawSpriteAnimColor({ 60.0f, 60.0f }, 0.0f, { 120.0f, 120.0f }, 4, 1, 0, color);
	}
	else {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[20]);
		DrawSpriteAnimColor({ 60.0f, 60.0f }, 0.0f, { 120.0f, 120.0f }, 4, 1, 0,color);
	}


	XMFLOAT2 g_UltGageSize = { 160.0f, 50.0f };
	float g_UltPercentage = g_ULT_CHARGE / 20.0f;
	const float g_UltGageWidth = g_UltGageSize.x * g_UltPercentage;
	float UltGagePosX = 200.0f;
	if (g_ULT_CHARGE < g_ULT_MAX)
	{
		//ウルトゲージの描画(ULT使えない)
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);
		DrawSpriteAnimColor({ 150.0f + g_UltGageWidth / 3.3f, 90.0f }, 0.0f, { g_UltGageWidth, 50.0f }, 1, 1, 1,color);
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);
		DrawSpriteColor({ 200.0f, 90.0f }, 0.0f, { 160.0f, 50.0f },color);

	}
	else if (g_ULT_CHARGE >= g_ULT_MAX)
	{
		////ウルトゲージの描画(ULT使える)
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);
		DrawSpriteAnimColor({ 200.0f, 90.0f }, 0.0f, { 160.0f, 50.0f }, 1, 8, AnimNum, color);

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);
		for (int i = 0; i < g_ULT_CHARGE; i++) {
			DrawSpriteAnimColor({ 153.0f + (5.0f * i), 90.0f }, 0.0f, { 5.0f, 50.0f }, 41, 1, (int)(8 + g_ULT_CHARGE), {2.0f,2.0f,2.0f ,color.w });
		}
		
	}
}



void MovePlayer() //プレイヤー移動
{

	//左移動・Lスティック
	if (Keyboard_IsKeyDown(KK_A) || Keyboard_IsKeyDown(KK_LEFT) || g_Controller.LStickX() <= -20000.0f)
	{
		g_moving = true;
		g_Player.m_Coordinate.m_Vel.x -= 10.0f;
		//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x - g_Player.GetAimingSpeed(), g_Player.GetAimingCenter().y });
	}
	//右移動・Lスティック
	if (Keyboard_IsKeyDown(KK_D) || Keyboard_IsKeyDown(KK_RIGHT) || g_Controller.LStickX() >= 20000.0f)
	{
		g_moving = true;
		g_Player.m_Coordinate.m_Vel.x += 10.0f;
		//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x + g_Player.GetAimingSpeed(), g_Player.GetAimingCenter().y });
	}
	//上移動・Lスティック
	if (Keyboard_IsKeyDown(KK_W) || Keyboard_IsKeyDown(KK_UP) || g_Controller.LStickY() >= 20000.0f)
	{
		g_moving = true;
		g_Player.m_Coordinate.m_Vel.y -= 10.0f;
		//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y - g_Player.GetAimingSpeed() });
	}
	//下移動・Lスティック
	if (Keyboard_IsKeyDown(KK_S) || Keyboard_IsKeyDown(KK_DOWN) || g_Controller.LStickY() <= -20000.0f)
	{
		g_moving = true;
		g_Player.m_Coordinate.m_Vel.y += 10.0f;
		//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y + g_Player.GetAimingSpeed() });
	}

	/* Wii＆ヌンチャクの操作 */
	if (!g_Player.m_WiiButtonB) {
		//左移動・ヌンチャクJoyStick
		if (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) <= -20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.x = -20.0f * (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) * -0.01f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x - g_Player.GetAimingSpeed(), g_Player.GetAimingCenter().y });
		}
		//右移動・ヌンチャクJoyStick
		if (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) >= 20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.x = 20.0f * (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) * 0.01f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x + g_Player.GetAimingSpeed(), g_Player.GetAimingCenter().y });
		}
		//上移動・ヌンチャクJoyStick
		if (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) >= 20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.y = -20.0f * (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) * 0.01f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y - g_Player.GetAimingSpeed() });
		}
		//下移動・ヌンチャクJoyStick
		if (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) <= -20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.y = 20.0f * (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) * -0.01f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y + g_Player.GetAimingSpeed() });
		}
	}
	else {
		//左移動・ヌンチャクJoyStick
		if (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) <= -20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.x = -10.0f * (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) * -0.1f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x - g_Player.GetAimingSpeed(), g_Player.GetAimingCenter().y });
		}
		//右移動・ヌンチャクJoyStick
		if (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) >= 20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.x = 10.0f * (XMVectorGetX(g_Player.GetNunchuckJoyconVector()) * 0.1f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x + g_Player.GetAimingSpeed(), g_Player.GetAimingCenter().y });
		}
		//上移動・ヌンチャクJoyStick
		if (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) >= 20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.y = -10.0f * (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) * 0.1f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y - g_Player.GetAimingSpeed() });
		}
		//下移動・ヌンチャクJoyStick
		if (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) <= -20.0f)
		{
			g_moving = true;
			g_Player.m_Coordinate.m_Vel.y = 10.0f * (XMVectorGetY(g_Player.GetNunchuckJoyconVector()) * -0.1f);
			//g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, g_Player.GetAimingCenter().y + g_Player.GetAimingSpeed() });
		}
	}

	//ここでエイミングの座標を固定する
	g_Player.SetAimingCenter({ g_Player.m_Coordinate.m_Pos.x + 200.0f,  g_Player.m_Coordinate.m_Pos.y });
	//プレイヤーから200離れる
	if (g_Player.m_Coordinate.m_Pos.x + 200.0f > g_Player.GetAimingCenter().x) {
		g_Player.SetAimingCenter({ g_Player.m_Coordinate.m_Pos.x + 200.0f, g_Player.GetAimingCenter().y });
	}

	//移動速度の制限
	if (g_Player.m_Coordinate.m_Vel.x >= 15.0f)
	{
		g_Player.m_Coordinate.m_Vel.x = 15.0f;
	}
	if (g_Player.m_Coordinate.m_Vel.x <= -15.0f)
	{
		g_Player.m_Coordinate.m_Vel.x = -15.0f;
	}
	if (g_Player.m_Coordinate.m_Vel.y >= 15.0f)
	{
		g_Player.m_Coordinate.m_Vel.y = 15.0f;
	}
	if (g_Player.m_Coordinate.m_Vel.y <= -15.0f)
	{
		g_Player.m_Coordinate.m_Vel.y = -15.0f;
	}


	//減速処理
	if (g_Player.GetState() == PLAYERSTATE_ABSORB || g_Player.GetState()==PLAYERSTATE_ATTRACT || g_Player.GetState() == PLAYERSTATE_HOLD)
	{
		//移動速度の制限
		if (g_Player.m_Coordinate.m_Vel.x >= 5.0f)
		{
			g_Player.m_Coordinate.m_Vel.x = 5.0f;
		}
		if (g_Player.m_Coordinate.m_Vel.x <= -5.0f)
		{
			g_Player.m_Coordinate.m_Vel.x = -5.0f;
		}
		if (g_Player.m_Coordinate.m_Vel.y >= 5.0f)
		{
			g_Player.m_Coordinate.m_Vel.y = 5.0f;
		}
		if (g_Player.m_Coordinate.m_Vel.y <= -5.0f)
		{
			g_Player.m_Coordinate.m_Vel.y = -5.0f;
		}
	}


	//加速度を座標に反映
	g_Player.m_Coordinate.m_Pos.x += g_Player.m_Coordinate.m_Vel.x;
	g_Player.m_Coordinate.m_Pos.y += g_Player.m_Coordinate.m_Vel.y;

	
	//移動範囲の制限
	//プレイヤー
	if (GetIsClear() == true)
	{
		if (g_Player.m_Coordinate.m_Pos.x >= SCREEN_WIDTH * 0.5 - (g_Player.m_Coordinate.m_Size.x / 2))
		{
			g_Player.m_Coordinate.m_Pos.x = SCREEN_WIDTH * 0.5 - (g_Player.m_Coordinate.m_Size.x / 2);
		}
	}
	else if (g_Player.m_Coordinate.m_Pos.x >= SCREEN_WIDTH - (g_Player.m_Coordinate.m_Size.x / 2))
	{
		g_Player.m_Coordinate.m_Pos.x = SCREEN_WIDTH - (g_Player.m_Coordinate.m_Size.x / 2);
	}
	if (g_Player.m_Coordinate.m_Pos.x <= 0 + (g_Player.m_Coordinate.m_Size.x / 2))
	{
		g_Player.m_Coordinate.m_Pos.x = 0 + (g_Player.m_Coordinate.m_Size.x / 2);
	}
	if (g_Player.m_Coordinate.m_Pos.y >= SCREEN_HEIGHT - (g_Player.m_Coordinate.m_Size.y / 2))
	{
		g_Player.m_Coordinate.m_Pos.y = SCREEN_HEIGHT - (g_Player.m_Coordinate.m_Size.y / 2);
	}
	if (g_Player.m_Coordinate.m_Pos.y <= 0 + (g_Player.m_Coordinate.m_Size.y / 2))
	{
		g_Player.m_Coordinate.m_Pos.y = 0 + (g_Player.m_Coordinate.m_Size.y / 2);
	}
	//吸い込む範囲
	//画面端
	if (g_Player.GetAimingCenter().x >= SCREEN_WIDTH - g_Player.GetAimingRange().x / 4)
	{
		g_Player.SetAimingCenter({ SCREEN_WIDTH - g_Player.GetAimingRange().x / 4, g_Player.GetAimingCenter().y });
	}
	if (g_Player.GetAimingCenter().x <= 0 + g_Player.GetAimingRange().x / 4)
	{
		g_Player.SetAimingCenter({ 0 + g_Player.GetAimingRange().x / 4, g_Player.GetAimingCenter().y });
	}
	if (g_Player.GetAimingCenter().y >= SCREEN_HEIGHT - g_Player.GetAimingRange().y / 4)
	{
		g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, SCREEN_HEIGHT - g_Player.GetAimingRange().y / 4 });
	}
	if (g_Player.GetAimingCenter().y <= g_Player.GetAimingRange().y / 4)
	{
		g_Player.SetAimingCenter({ g_Player.GetAimingCenter().x, g_Player.GetAimingRange().y / 4});
	}
	//プレイヤーを基準にした値
	if (g_Player.GetAimingCenter().x >= g_Player.m_Coordinate.m_Pos.x + 640.0f)
	{
		g_Player.SetAimingCenter({ g_Player.m_Coordinate.m_Pos.x + 640.0f, g_Player.GetAimingCenter().y });
	}
	if (g_Player.GetAimingCenter().x <= g_Player.m_Coordinate.m_Pos.x + g_Player.GetAimingRange().x / 3)
	{
		g_Player.SetAimingCenter({ g_Player.m_Coordinate.m_Pos.x + g_Player.GetAimingRange().x / 3, g_Player.GetAimingCenter().y });
	}
	if (g_Player.GetAimingCenter().y >= g_Player.m_Coordinate.m_Pos.y + 640.0f)
	{
		g_Player.SetAimingCenter({ g_Player.GetAimingCenter().y, g_Player.m_Coordinate.m_Pos.y + 640.0f });
	}
	if (g_Player.GetAimingCenter().y <= g_Player.m_Coordinate.m_Pos.y - 640.0f)
	{
		g_Player.SetAimingCenter({ g_Player.GetAimingCenter().y, g_Player.m_Coordinate.m_Pos.y - 640.0f });
	}


	//何もしていないと減速
	//プレイヤー
	if (g_Player.m_Coordinate.m_Vel.x < 0)
	{
		g_Player.m_Coordinate.m_Vel.x += 1.0f;

		if (g_Player.m_Coordinate.m_Vel.x > 0) {
			g_Player.m_Coordinate.m_Vel.x = 0;
		}
	}
	if (g_Player.m_Coordinate.m_Vel.x > 0)
	{
		g_Player.m_Coordinate.m_Vel.x -= 1.0f;

		if (g_Player.m_Coordinate.m_Vel.x < 0) {
			g_Player.m_Coordinate.m_Vel.x = 0;
		}
	}
	if (g_Player.m_Coordinate.m_Vel.y < 0)
	{
		g_Player.m_Coordinate.m_Vel.y += 1.0f;

		if (g_Player.m_Coordinate.m_Vel.y > 0) {
			g_Player.m_Coordinate.m_Vel.y = 0;
		}
	}
	if (g_Player.m_Coordinate.m_Vel.y > 0)
	{
		g_Player.m_Coordinate.m_Vel.y -= 1.0f;

		if (g_Player.m_Coordinate.m_Vel.y < 0) {
			g_Player.m_Coordinate.m_Vel.y = 0;
		}
	}

	if (g_moving == false)
	{
		//完全に停止
		if (g_Player.m_Coordinate.m_Vel.x < 0.1f && g_Player.m_Coordinate.m_Vel.x > -0.1f)
		{
			g_Player.m_Coordinate.m_Vel.x = 0.0f;
		}
		if (g_Player.m_Coordinate.m_Vel.y < 0.1f && g_Player.m_Coordinate.m_Vel.y > -0.1f)
		{
			g_Player.m_Coordinate.m_Vel.y = 0.0f;
		}
	}

	g_moving = false;
}

Player* GetPlayer(void)
{
	return &g_Player;
}




//現在のレベルに応じて必要経験値を変える
void ChangeNeedExp()
{
	switch (g_Player.GetLevel())
	{
	case 1:
		g_Player.SetNeedExp(10);
		break;
	case 2:
		g_Player.SetNeedExp(40);
		break;
	case 3:
		g_Player.SetNeedExp(60);
		break;
	case 4:
		g_Player.SetNeedExp(80);
		break;
	case 5:
		g_Player.SetNeedExp(100);
		break;
	default:
		break;
	}
}

void CheckLevelUp()
{
	//現在の経験値が必要数以上に達したとき、レベルアップ処理を行う
	if (g_Player.GetExp() >= g_Player.GetNeedExp())
	{
		if (g_Player.GetLevel() < 5) {
			g_Player.SetExp(g_Player.GetExp() - g_Player.GetNeedExp());
			UpLevel();
			//上がったレベル分の必要経験値にしてからもう一回自身を実行する
			ChangeNeedExp();
			CheckLevelUp();
		}
	}
}

void UpLevel()
{
	//レベルアップ!!　能力上昇!!!!
	g_Player.SetLevel(g_Player.GetLevel() + 1);
	//g_Player.SetAimingRangeMax({ g_Player.GetAimingRangeMax().x + 30.0f, g_Player.GetAimingRangeMax().y + 30.0f });
	g_Player.ShortCool(6);
	g_Player.AddAbsorbTimeLimit(30);
}

void AbsorbedOnce()
{
	//if (g_MakingBullet.GetLevel() == 0)	{g_MakingBullet.UpLevel();}
	g_MakingBullet.AddExp(1.0f);

	/************ 追加↓ ************/
	if (!g_Player.GetUltStart() && !GetIsTutorial()) {
		g_ULT_CHARGE++;
	}
	/************ 追加↑ ************/

	if (!GetIsTutorial()){ AddScore(100); }
}

void DecMakingBulletExp()
{
	g_MakingBullet.AddExp(-0.5f);
	//g_MakingBullet.DecExp();
	//SetBullet(g_MakingBullet);
}

float GetHoldingTime() { return g_HoldingTime; }