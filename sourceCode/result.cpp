#include "result.h"
#include "texture.h"
#include "sprite.h"
#include "score.h"
#include "keyboard.h"
#include "title.h"
#include "controller.h"
#include "wiimote/WiimoteManager.h"
#include "audio.h"
#include "fade.h"
#include "sound.h"

static ID3D11ShaderResourceView* g_Texture[6];
static bool g_OldKeystateSpace = false;

// 背景
static XMFLOAT2 g_BgSize = { 256.0f, 144.0f };
static XMFLOAT2 g_BgPosition = { g_BgSize.x * 0.5f, g_BgSize.y * 0.5f };
static XMFLOAT4 g_BgColor = { 1.0f, 1.0f, 1.0f, 0.0f };

//// GAME OVERのテキスト(素材が完成したら、result用のに変えておくように)
//static XMFLOAT2 g_GameOverTextSize = { 612.0f, 99.0f }; // 204:33の3倍
//static XMFLOAT2 g_GameOverTextPosition = { SCREEN_XCENTER, 200.0f };

// Scoreテキスト
static XMFLOAT2 g_ScoreSize = { 240.0f, 65.0f }; // 160:32の1.5倍
static XMFLOAT2 g_ScorePosition = { SCREEN_XCENTER - 300.0f, SCREEN_YCENTER * 1.2 };

// Scoreポイント
static XMFLOAT2 g_ScorePointSize = { 60.0f, 60.0f }; // 
static XMFLOAT2 g_ScorePointPosition = { 540.0f, SCREEN_YCENTER * 1.45f };

// ネクストボタン
static XMFLOAT2 g_RetrySize = { 200.0f, 100.0f }; // 80:30の2.5倍
static XMFLOAT2 g_RetryPosition = { SCREEN_XCENTER + 300.0f, SCREEN_YCENTER * 1.25f };

// TITLEボタン
static XMFLOAT2 g_TitleSize = { 200.0f, 100.0f }; // 80:30の2.5倍
static XMFLOAT2 g_TitlePosition = { SCREEN_XCENTER + 300.0f, SCREEN_YCENTER * 1.55 };


static bool g_GameOverAnim = false; // UIのアニメーションフラグ

// UIアニメスピード
static XMFLOAT2 g_ScoreSizeSpeed = { 0.0f, 0.0f };
static XMFLOAT2 g_ScorePointSizeSpeed = { 0.0f, 0.0f };
static XMFLOAT2 g_RetrySizeSpeed = { 0.0f, 0.0f };
static XMFLOAT2 g_TitleSizeSpeed = { 0.0f, 0.0f };

static float g_ButtonSizeSpeed = 0.0f;

bool g_PushButton = false; // ボタン押せるフラグ
bool g_SelectRetry = true; // リトライ選択フラグ

Controller g_Controlller{}; //コントローラークラス

/* Wiimote */
static WiimoteManager* m = nullptr;
//ヌンチャク
static XMVECTOR g_ResultNunchuckJoystickVec{};
//Wiiリモコン
static const char* g_ResultWiiButton{};
bool g_ResultWiiButtonDown = false;

bool g_ResultWiiButtonA = false;
bool g_ResultOldWiiButtonA = false;
bool g_ResultNextWiiButtonA = false;
static bool	g_AllGameClear = false;

void SetAllClear(bool b) { g_AllGameClear = b; }

static int g_ResultBGM;
static int g_ResultDecisionSE;
static int g_ResultSelectSE;

void InitResult() {

	g_Texture[0] = InitTexture(L"asset\\texture\\UI_STAGECLEAR_MOJI.png");		//resultの背景
	g_Texture[1] = InitTexture(L"asset\\texture\\UI_GAMEOVER_BT_03.png");	//retryボタン
	g_Texture[2] = InitTexture(L"asset\\texture\\UI_GAMEOVER_BT_02.png");	//titleボタン
	g_Texture[3] = InitTexture(L"asset\\texture\\UI_TEXT_SCORE_02.png");	//scoreテキスト
	g_Texture[4] = InitTexture(L"asset\\texture\\UI_NUMBER_SCORE_02.png");	//score数字
	g_Texture[5] = InitTexture(L"asset\\texture\\white.png");	


	g_BgColor = { 1.0f, 1.0f, 1.0f, 0.0f }; // カラーの初期化

	// UIサイズの初期化
	g_ScoreSize = { 0.0f, 0.0f };
	
	g_ScorePointSize = { 0.0f, 0.0f };
	g_RetrySize = { 0.0f, 0.0f };
	g_TitleSize = { 0.0f, 0.0f };

	g_GameOverAnim = false; // UIアニメーションフラグの初期化

	// UIサイズスピードリセット
	g_ScoreSizeSpeed = { 0.0f, 0.0f };
	g_ScorePointSizeSpeed = { 0.0f, 0.0f };
	g_RetrySizeSpeed = { 0.0f, 0.0f };
	g_TitleSizeSpeed = { 0.0f, 0.0f };

	g_ButtonSizeSpeed = 0.0f;

	g_PushButton = false; // ボタン押せるフラグOFF
	g_SelectRetry = true; // リトライ選択フラグ

	m = new WiimoteManager;

	m->onConnection([](Wiimote* wiimote) {

		wiimote->onButton([wiimote](WiimoteButton button, bool down) {
			const char* buttonName = WiimoteButtons::toString(button);

			if (buttonName) {
				//buttonName：ボタンの種類, down：押したかのフラグ
				g_ResultWiiButton = buttonName;
				g_ResultWiiButtonDown = down;
			}
			});

		wiimote->onNunchuckButton([wiimote](NunchuckButton button, bool down) {
			const char* buttonName = NunchuckButtons::toString(button);
			if (buttonName) {
				//buttonName：ボタンの種類, down：押したかのフラグ
			}
			});

		wiimote->onAccelerometer([wiimote](const Vector3& v) {
			//v：Wiiリモコンの加速度
			}, true);

		wiimote->onNunchuckAccelerometer([wiimote](const Vector3& v) {
			//v：ヌンチャクの加速度
			}, true);

		wiimote->onMotionPlus([wiimote](const Vector3& v) {
			//v：モーションプラスの加速度
			}, true);

		wiimote->onNunchuckJoystick([wiimote](const Vector2& v) {
			//v：スティックの傾き
			g_ResultNunchuckJoystickVec = { v.x, v.y };
			}, true);

		wiimote->start(WiimoteFeatures::Buttons | WiimoteFeatures::Accelerometer | WiimoteFeatures::Extension);
		});

	m->onDisconnection([](Wiimote* wiimote) {
		wiimote->clearEvents();
		});

	m->continuousScan();

	g_ResultBGM = LoadSound("asset\\BGM\\BGM_GAMECLEAR.wav");
	g_ResultDecisionSE = LoadSound("asset\\SE\\Decision.wav");
	g_ResultSelectSE = LoadSound("asset\\SE\\select.wav");

	PlaySound(g_ResultBGM, true);
}

void UninitResult()
{
	g_AllGameClear = false;

	StopSound(g_ResultSelectSE);
	StopSound(g_ResultDecisionSE);
	StopSound(g_ResultBGM);

	delete m;

	//テクスチャの解放
	for (int i = 0; i < 5; ++i) {
		UnInitTexture(g_Texture[i]);
	}
}

void UpdateResult() {

	m->update();

	/* Wiiリモコンボタン *************************************/
	if (g_ResultWiiButton == "A" && g_ResultWiiButtonDown) {
		g_ResultOldWiiButtonA = true;
		g_ResultNextWiiButtonA = true;
	}
	else if (g_ResultWiiButton == "A" && !g_ResultWiiButtonDown) {
		g_ResultOldWiiButtonA = false;
	}

	if (g_ResultOldWiiButtonA == g_ResultNextWiiButtonA) {
		g_ResultWiiButtonA = false;
	}
	else if (g_ResultOldWiiButtonA != g_ResultNextWiiButtonA) {
		g_ResultWiiButtonA = true;
		g_ResultNextWiiButtonA = false;
	}
	/*********************************************************/

	bool keystateSpace = Keyboard_IsKeyDown(KK_P);
	if (!g_OldKeystateSpace && keystateSpace) {
		SetFadeScene(SCENE_WII_WARNING);
	}
	g_OldKeystateSpace = keystateSpace;

	//すべてのゲームクリア時はリトライ無くす
	if (g_AllGameClear) { g_SelectRetry = false; }

	if (g_PushButton) {

		if (g_SelectRetry) {
			g_TitleSize = { 200.0f, 100.0f }; //未選択のUIの大きさ固定

			//矢印キーでボタンの選択・十字キー
			if (Keyboard_IsKeyDown(KK_DOWN) || Keyboard_IsKeyDown(KK_S) ||
				g_Controlller.LStickY() <= -20000.0f || XMVectorGetY(g_ResultNunchuckJoystickVec) <= -40) {

				g_ButtonSizeSpeed = 0.0f;
				g_SelectRetry = false;
				PlaySound(g_ResultSelectSE, false);
			}

			//選ぶ方のサイズを変える
			g_RetrySize.x += g_ButtonSizeSpeed * 1.6f;
			g_RetrySize.y += g_ButtonSizeSpeed * 0.9f;

			if (g_RetrySize.y <= 75.0f) {
				g_ButtonSizeSpeed = 6.0f;
			}

			g_ButtonSizeSpeed += -0.3f;

			//ゲームへリトライ
			bool keystateSpace = Keyboard_IsKeyDown(KK_ENTER);
			if (!g_OldKeystateSpace && keystateSpace || 
				g_Controlller.ControllerDownTrigger(BUTTON_B) || g_ResultWiiButtonA) {
				PlaySound(g_ResultDecisionSE, false);
				g_ButtonSizeSpeed = 0.0f;
				SetScene(GetNextStage());
				UpdateStage();
			}
			g_OldKeystateSpace = keystateSpace;
		}
		else {
			g_RetrySize = { 200.0f, 100.0f }; //未選択のUIの大きさ固定

			//矢印キーでボタンの選択・十字キー
			if (Keyboard_IsKeyDown(KK_UP) || Keyboard_IsKeyDown(KK_W) ||
				g_Controlller.LStickY() >= 20000.0f || XMVectorGetY(g_ResultNunchuckJoystickVec) >= 40) {

				g_ButtonSizeSpeed = 0.0f;
				g_SelectRetry = true;
				PlaySound(g_ResultSelectSE, false);
			}

			//選ぶ方のサイズを変える
			g_TitleSize.x += g_ButtonSizeSpeed * 1.6f;
			g_TitleSize.y += g_ButtonSizeSpeed * 0.9f;

			if (g_TitleSize.y <= 75.0f) {
				g_ButtonSizeSpeed = 6.0f;
			}

			g_ButtonSizeSpeed += -0.3f;

			//タイトルへ移動
			bool keystateSpace = Keyboard_IsKeyDown(KK_ENTER);
			if (!g_OldKeystateSpace && keystateSpace || 
				g_Controlller.ControllerDownTrigger(BUTTON_B) || g_ResultWiiButtonA) {
				PlaySound(g_ResultDecisionSE, false);
				g_ButtonSizeSpeed = 0.0f;
				SetFadeScene(SCENE_WII_WARNING);
			}
			g_OldKeystateSpace = keystateSpace;
		}
	}
	else {
		//UIアニメーション
		if (g_GameOverAnim) {


			g_ScoreSize.x += g_ScoreSizeSpeed.x;
			g_ScoreSize.y += g_ScoreSizeSpeed.y;
			g_ScorePointSize.x += g_ScorePointSizeSpeed.x;
			g_ScorePointSize.y += g_ScorePointSizeSpeed.y;
			if (g_ScoreSize.x < 240.0f && g_ScorePointSize.x < 80.0f) {
				g_ScoreSizeSpeed.x += 0.48f;
				g_ScoreSizeSpeed.y += 0.096f;
				g_ScorePointSizeSpeed.x += 0.12f;
				g_ScorePointSizeSpeed.y += 0.12f;
			}
			else {
				g_ScoreSize.x = 240.0f;
				g_ScoreSize.y = 48.0f;
				g_ScorePointSize.x = 80.0f;
				g_ScorePointSize.y = 80.0f;


				g_RetrySize.x += g_RetrySizeSpeed.x;
				g_RetrySize.y += g_RetrySizeSpeed.y;
				if (g_RetrySize.x < 200.0f) {
					g_RetrySizeSpeed.x += 0.4f;
					g_RetrySizeSpeed.y += 0.15f;
				}
				else {
					g_RetrySize.x = 200.0f;
					g_RetrySize.y = 100.0f;


					g_TitleSize.x += g_TitleSizeSpeed.x;
					g_TitleSize.y += g_TitleSizeSpeed.y;
					if (g_TitleSize.x < 200.0f) {
						g_TitleSizeSpeed.x += 0.4f;
						g_TitleSizeSpeed.y += 0.15f;
					}
					else {
						g_TitleSize.x = 200.0f;
						g_TitleSize.y = 100.0f;


						g_PushButton = true;
					}
				}
			}
		}		
	}
	//背景表示
	g_BgColor.w += 0.003f;
	g_BgColor.w = 0.5f; // アルファ値固定
	g_GameOverAnim = true; // UIアニメーションON

	//if (g_BgColor.w >= 0.5f) {
	//	g_BgColor.w = 0.5f; // アルファ値固定

	//	g_GameOverAnim = true; // UIアニメーションON
	//}
}

void DrawResult() {
	
	//白
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
	DrawSpriteColor({ XMFLOAT2(SCREEN_XCENTER, SCREEN_YCENTER)}, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, {1.0f,1.0f,1.0f,0.3f});


	//背景
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
	//DrawSpriteColor({ XMFLOAT2(SCREEN_XCENTER, SCREEN_YCENTER)}, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, g_BgColor);
	DrawSprite(XMFLOAT2(SCREEN_XCENTER, SCREEN_YCENTER / 2), 0.0f, XMFLOAT2(GetWidth(g_Texture[0]) * 0.5f, GetHeight(g_Texture[0]) * 0.5f));

	//ボタン
	//ネクスト
	if (!g_AllGameClear) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
		DrawSpriteAnim({ g_RetryPosition.x, g_RetryPosition.y }, 0.0f, g_RetrySize, 1, 2, g_SelectRetry == true ? 1 : 0);
	}

	//タイトル
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
	DrawSpriteAnim({ g_TitlePosition.x, g_TitlePosition.y }, 0.0f, g_TitleSize, 1, 2, g_SelectRetry == false ? 1 : 0);

	//スコア
	Score* pScore = pGetScore();
	//スコアテキスト
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
	DrawSprite({ g_ScorePosition.x, g_ScorePosition.y }, 0.0f, {static_cast<float>(GetWidth(g_Texture[3])) * 0.8f,static_cast<float>(GetHeight(g_Texture[3])) * 0.8f});

	//スコア数字
	int score = pScore->GetScore();
	for (int i = 0; i < 6; i++)
	{
		int Num = pScore->GetScore() % 10;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
		DrawSpriteAnim({ (g_ScorePointPosition.x - (g_ScorePointSize.x * i)), g_ScorePointPosition.y }, 0.0f, g_ScorePointSize, 8, 2, Num);
		pScore->SetScore(pScore->GetScore() / 10);
	}
	pScore->SetScore(score);
}
