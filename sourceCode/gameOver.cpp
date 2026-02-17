//==================================================================
//	ゲームオーバー [gameOver.cpp]
//
// Author :　佐藤琢磨		Date	: 2024:01/10
//==================================================================
#include "gameOver.h"
#include "texture.h"
#include "sprite.h"
#include "keyboard.h"
#include "title.h"
#include "result.h"
#include "controller.h"
#include "wiimote/WiimoteManager.h"
#include "audio.h"
#include "fade.h"
#include "sound.h"

static ID3D11ShaderResourceView* g_Texture[4];
static bool g_OldKeystateSpace = false; // スペース長押しでも一回だけ反応

// 背景
static XMFLOAT2 g_BgSize = { 256.0f, 144.0f };
static XMFLOAT2 g_BgPosition = { g_BgSize.x * 0.5f, g_BgSize.y * 0.5f };
static XMFLOAT4 g_BgColor = { 1.0f, 1.0f, 1.0f, 0.0f };

// GAME OVERのテキスト
static XMFLOAT2 g_GameOverTextSize = { 612.0f, 99.0f }; // 204:33の3倍
static XMFLOAT2 g_GameOverTextPosition = { SCREEN_XCENTER, 200.0f };

// RETRYボタン
static XMFLOAT2 g_RetrySize = { 200.0f, 100.0f }; // 80:30の2.5倍
static XMFLOAT2 g_RetryPosition = { SCREEN_XCENTER, SCREEN_YCENTER * 1.05f };

// TITLEボタン
static XMFLOAT2 g_TitleSize = { 200.0f, 100.0f }; // 80:30の2.5倍
static XMFLOAT2 g_TitlePosition = { SCREEN_XCENTER, SCREEN_YCENTER * 1.35 };

static bool g_GameOverAnim = false; // UIのアニメーションフラグ

// UIアニメスピード
static XMFLOAT2 g_RetrySizeSpeed = { 0.0f, 0.0f };
static XMFLOAT2 g_TitleSizeSpeed = { 0.0f, 0.0f };

static float g_ButtonSizeSpeed = 0.0f;

bool g_ButtonPush = false; // ボタン押せるフラグ
bool g_RetrySelect = true; // リトライ選択フラグ

Controller g_Controller{}; //コントローラークラス

/* Wiimote */
static WiimoteManager* m = nullptr;
//ヌンチャク
static XMVECTOR g_GameOverNunchuckJoystickVec{};
//Wiiリモコン
static const char* g_GameOverWiiButton{};
bool g_GameOverWiiButtonDown = false;

bool g_GameOverWiiButtonA = false;
bool g_GameOverOldWiiButtonA = false;
bool g_GameOverNextWiiButtonA = false;

static int g_GameOverBGM;
static int g_GameOverDecisionSE;
static int g_GameOverSelectSE;

void InitGameOver()
{
	// テクスチャの読み込み
	g_Texture[0] = InitTexture(L"asset\\texture\\UI_GAMEOVER_BG.png");
	g_Texture[1] = InitTexture(L"asset\\texture\\UI_GAMEOVER_MOJI.png");
	g_Texture[2] = InitTexture(L"asset\\texture\\UI_GAMEOVER_BT_01.png");
	g_Texture[3] = InitTexture(L"asset\\texture\\UI_GAMEOVER_BT_02.png");

	g_BgColor = { 1.0f, 1.0f, 1.0f, 0.0f }; // カラーの初期化

	// UIポジションの初期化
	g_GameOverTextPosition = { SCREEN_XCENTER, -(g_GameOverTextSize.y * 0.5f) };

	// UIサイズの初期化
	g_RetrySize = { 0.0f, 0.0f };
	g_TitleSize = { 0.0f, 0.0f };

	g_GameOverAnim = false; // UIアニメーションフラグの初期化

	// UIサイズスピードリセット
	g_RetrySizeSpeed = { 0.0f, 0.0f };
	g_TitleSizeSpeed = { 0.0f, 0.0f };

	g_ButtonSizeSpeed = 0.0f;

	g_ButtonPush = false; // ボタン押せるフラグOFF
	g_RetrySelect = true; // リトライ選択フラグ

	/* Wiimote初期化 */
	m = new WiimoteManager;

	m->onConnection([](Wiimote* wiimote) {

		wiimote->onButton([wiimote](WiimoteButton button, bool down) {
			const char* buttonName = WiimoteButtons::toString(button);

			if (buttonName) {
				//buttonName：ボタンの種類, down：押したかのフラグ
				g_GameOverWiiButton = buttonName;
				g_GameOverWiiButtonDown = down;
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
			g_GameOverNunchuckJoystickVec = { v.x, v.y };
			}, true);

		wiimote->start(WiimoteFeatures::Buttons | WiimoteFeatures::Accelerometer | WiimoteFeatures::Extension);
		});

	m->onDisconnection([](Wiimote* wiimote) {
		wiimote->clearEvents();
		});

	m->continuousScan();

	g_GameOverBGM = LoadSound("asset\\BGM\\BGM_GAMEOVER.wav");
	g_GameOverDecisionSE = LoadSound("asset\\SE\\Decision.wav");
	g_GameOverSelectSE = LoadSound("asset\\SE\\select.wav");
	PlaySound(g_GameOverBGM, true);
}

void UninitGameOver()
{
	StopSound(g_GameOverSelectSE);
	StopSound(g_GameOverDecisionSE);
	StopSound(g_GameOverBGM);

	delete m;

	//テクスチャの解放
	for (int i = 0; i < 4; ++i) {
		UnInitTexture(g_Texture[i]);
	}
}

void UpdateGameOver()
{
	m->update();

	/* Wiiリモコンボタン *************************************/
	if (g_GameOverWiiButton == "A" && g_GameOverWiiButtonDown) {
		g_GameOverOldWiiButtonA = true;
		g_GameOverNextWiiButtonA = true;
	}
	else if (g_GameOverWiiButton == "A" && !g_GameOverWiiButtonDown) {
		g_GameOverOldWiiButtonA = false;
	}

	if (g_GameOverOldWiiButtonA == g_GameOverNextWiiButtonA) {
		g_GameOverWiiButtonA = false;
	}
	else if (g_GameOverOldWiiButtonA != g_GameOverNextWiiButtonA) {
		g_GameOverWiiButtonA = true;
		g_GameOverNextWiiButtonA = false;
	}
	/*********************************************************/

	if (g_ButtonPush) {

		if (g_RetrySelect) {
			g_TitleSize = { 200.0f, 100.0f }; //未選択のUIの大きさ固定

			//矢印キーでボタンの選択・十字キー
			if (Keyboard_IsKeyDown(KK_DOWN) || Keyboard_IsKeyDown(KK_S) ||
				g_Controller.LStickY() <= -20000.0f || XMVectorGetY(g_GameOverNunchuckJoystickVec) <= -40) {

				g_ButtonSizeSpeed = 0.0f;
				g_RetrySelect = false;
				PlaySound(g_GameOverSelectSE, false);
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
			if (!g_OldKeystateSpace && keystateSpace || g_Controller.ControllerDownTrigger(BUTTON_B) || g_GameOverWiiButtonA) {
				PlaySound(g_GameOverDecisionSE, false);
				g_ButtonSizeSpeed = 0.0f;
				SetScene(GetStage());
				UpdateStage();
			}
			g_OldKeystateSpace = keystateSpace;
		}
		else {
			g_RetrySize = { 200.0f, 100.0f }; //未選択のUIの大きさ固定

			//矢印キーでボタンの選択・十字キー
			if (Keyboard_IsKeyDown(KK_UP) || Keyboard_IsKeyDown(KK_W) ||
				g_Controller.LStickY() >= 20000.0f || XMVectorGetY(g_GameOverNunchuckJoystickVec) >= 40) {

				g_ButtonSizeSpeed = 0.0f;
				g_RetrySelect = true;
				PlaySound(g_GameOverSelectSE, false);
			}

			//選ぶ方のサイズを変える
			g_TitleSize.x += g_ButtonSizeSpeed * 1.6f;
			g_TitleSize.y += g_ButtonSizeSpeed * 0.9f;

			if (g_TitleSize.y <= 75.0f) {
				g_ButtonSizeSpeed = 6.0f;
			}

			g_ButtonSizeSpeed += -0.3f;

			//タイトルへ移動
			bool keystateSpace = Keyboard_IsKeyDownTrigger(KK_ENTER);
			if (!g_OldKeystateSpace && keystateSpace || g_Controller.ControllerDownTrigger(BUTTON_B) || g_GameOverWiiButtonA) 	/************ 変更 ************/
			{
				PlaySound(g_GameOverDecisionSE, false);
				g_ButtonSizeSpeed = 0.0f;
				SetFadeScene(SCENE_WII_WARNING);
			}
			g_OldKeystateSpace = keystateSpace;
		}

		g_BgPosition.x += 3.0f;
		g_BgPosition.y += 3.04f;

		//背景ループ
		if (g_BgPosition.x >= SCREEN_WIDTH + (g_BgSize.x * 0.5)) {
			// 位置リセット
			g_BgPosition.x = g_BgSize.x * 0.5f;
			g_BgPosition.y = g_BgSize.y * 0.5f;
		}
	}
	else {
		//UIアニメーション
		if (g_GameOverAnim) {
			g_GameOverTextPosition.y += 5.0f; //下に降りてくる

			if (g_GameOverTextPosition.y >= 200.0f) {
				g_GameOverTextPosition.y = 200.0f;

				g_RetrySize.x += g_RetrySizeSpeed.x;
				g_RetrySize.y += g_RetrySizeSpeed.y;

				if (g_RetrySize.x < 200.0f) {
					g_RetrySizeSpeed.x += 0.4f;
					g_RetrySizeSpeed.y += 0.2f;
				}
				else {
					g_RetrySize.x = 200.0f;
					g_RetrySize.y = 100.0f;

					g_TitleSize.x += g_TitleSizeSpeed.x;
					g_TitleSize.y += g_TitleSizeSpeed.y;

					if (g_TitleSize.x < 200.0f) {
						g_TitleSizeSpeed.x += 0.4f;
						g_TitleSizeSpeed.y += 0.2f;
					}
					else {
						g_TitleSize.x = 200.0f;
						g_TitleSize.y = 100.0f;

						g_ButtonPush = true;
					}
				}
			}
		}

	}

	//背景表示
	g_BgColor.w += 0.003f;

	if (g_BgColor.w >= 0.5f) {
		g_BgColor.w = 0.5f; // アルファ値固定

		g_GameOverAnim = true; // UIアニメーションON
	}
}

void DrawGameOver()
{
	for (int y = -9; y < 5; ++y) {
		//画像を画面上に置く
		for (int x = -5; x < 5; ++x) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
			DrawSpriteColor({ g_BgPosition.x + (g_BgSize.x * x), g_BgPosition.y + (g_BgSize.y * y) }, 0.0f, g_BgSize, g_BgColor);
		}
	}

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
	DrawSprite({ g_GameOverTextPosition.x, g_GameOverTextPosition.y }, 0.0f, g_GameOverTextSize);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
	DrawSpriteAnim({ g_RetryPosition.x, g_RetryPosition.y }, 0.0f, g_RetrySize, 1, 2, g_RetrySelect == true ? 1 : 0);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
	DrawSpriteAnim({ g_TitlePosition.x, g_TitlePosition.y }, 0.0f, g_TitleSize, 1, 2, g_RetrySelect == false ? 1 : 0);
}
