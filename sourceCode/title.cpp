#include "title.h"
#include "texture.h"
#include "sprite.h"
#include "main.h"
#include "keyboard.h"
#include "controller.h"
#include "videoPlayer.h"
#include "video_texture.h"
#include "wiimote/WiimoteManager.h"
#include "stage3.h"
#include "sound.h"

#define TITLE_LOGO_POS   XMFLOAT2(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.22f)
#define TITLE_START_POS   XMFLOAT2(SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.49f)
#define TITLE_END_POS   XMFLOAT2(SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.65f)

enum TitleTexture
{
	TITLE_LOGO = 0,
	TITLE_START,
	TITLE_END,
	TITLE_MAX,
};

enum MenuSelection
{
	MENU_START = 0,
	MENU_END,
	MENU_MAX
};

static unsigned int PushKeyCool = 0;
static bool g_OldKeystateSpace = false;
static ID3D11ShaderResourceView* g_Texture[TITLE_MAX];

static Controller g_Controller{};
static VideoTexture g_VideoTexture;

static int g_SelectedMenu = MENU_START;  // 0: スタート, 1: エンド

/* Wiimote */
static WiimoteManager* m = nullptr;
//Wiiリモコン
static const char* g_TitleWiiButton{};
bool g_TitleWiiButtonDown = false;

bool g_TitleWiiButtonA = false;
bool g_TitleOldWiiButtonA = false;
bool g_TitleNextWiiButtonA = false;
//ヌンチャク
static XMVECTOR g_TitleNunchuckJoystickVec{};

static int g_TitleBGM;
static int g_TitleDecisionSE;
static int g_TitleSelectSE;

void InitTitle() 
{
	PushKeyCool = 0;
	g_OldKeystateSpace = false;
	g_SelectedMenu = MENU_START;

	g_Texture[TITLE_LOGO] = InitTexture(L"asset\\texture\\TITLE\\UI_GAMENAME.png");
	g_Texture[TITLE_START] = InitTexture(L"asset\\texture\\TITLE\\UI_BT_START.png");
	g_Texture[TITLE_END] = InitTexture(L"asset\\texture\\TITLE\\UI_BT_END.png");

	VideoTexture::createAPI();  // Video API 初期化
	g_VideoTexture.create("asset\\Movie\\title.mp4");
	
	//ゲームBGMロード
	g_TitleBGM = LoadSound("asset\\BGM\\BGM_TITLE.wav");
	g_TitleDecisionSE = LoadSound("asset\\SE\\Decision.wav");
	g_TitleSelectSE = LoadSound("asset\\SE\\select.wav");

	PlaySound(g_TitleBGM, true); //再生

	/* Wiimote初期化 */
	m = new WiimoteManager;

	m->onConnection([](Wiimote* wiimote) {

		wiimote->onButton([wiimote](WiimoteButton button, bool down) {
			const char* buttonName = WiimoteButtons::toString(button);

			if (buttonName) {
				//buttonName：ボタンの種類, down：押したかのフラグ
				g_TitleWiiButton = buttonName;
				g_TitleWiiButtonDown = down;
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
			g_TitleNunchuckJoystickVec = { v.x, v.y };
			}, true);

		wiimote->start(WiimoteFeatures::Buttons | WiimoteFeatures::Accelerometer | WiimoteFeatures::Extension);
		});

	m->onDisconnection([](Wiimote* wiimote) {
		wiimote->clearEvents();
		});

	m->continuousScan();
}

void UninitTitle() 
{
	delete m;
	StopSound(g_TitleSelectSE);
	StopSound(g_TitleDecisionSE);
	StopSound(g_TitleBGM);

	for (int i = 0; i < 3; i++) {
		UnInitTexture(g_Texture[i]);
	}
	
	g_VideoTexture.destroy();
	VideoTexture::destroyAPI();  // Video API 解放
}

void UpdateTitle() {

	m->update();

	/* Wiiリモコンボタン *************************************/
	if (g_TitleWiiButton == "A" && g_TitleWiiButtonDown) {
		g_TitleOldWiiButtonA = true;
		g_TitleNextWiiButtonA = true;
	}
	else if (g_TitleWiiButton == "A" && !g_TitleWiiButtonDown) {
		g_TitleOldWiiButtonA = false;
	}

	if (g_TitleOldWiiButtonA == g_TitleNextWiiButtonA) {
		g_TitleWiiButtonA = false;
	}
	else if (g_TitleOldWiiButtonA != g_TitleNextWiiButtonA) {
		g_TitleWiiButtonA = true;
		g_TitleNextWiiButtonA = false;
	}
	/*********************************************************/

	g_VideoTexture.update(1.0f);
	// 上キーまたは W キーでメニューを上へ
	if (Keyboard_IsKeyDownTrigger(KK_UP) || Keyboard_IsKeyDownTrigger(KK_W) || g_Controller.ControllerDownTrigger(BUTTON_UP) || XMVectorGetY(g_TitleNunchuckJoystickVec) >= 20)
	{
		PlaySound(g_TitleSelectSE, false);
		g_SelectedMenu = MENU_START;
	}

	// 下キーまたは S キーでメニューを下へ
	if (Keyboard_IsKeyDownTrigger(KK_DOWN) || Keyboard_IsKeyDownTrigger(KK_S) || g_Controller.ControllerDownTrigger(BUTTON_DOWN) || XMVectorGetY(g_TitleNunchuckJoystickVec) <= -20)
	{
		PlaySound(g_TitleSelectSE, false);
		g_SelectedMenu = MENU_END;
	}

	// 決定（Enterキー または Bボタン）
	bool keystateSpace = Keyboard_IsKeyDown(KK_ENTER) || g_Controller.ControllerDownTrigger(BUTTON_B) || g_TitleWiiButtonA;
	if (!g_OldKeystateSpace && keystateSpace)
	{
		if (PushKeyCool >= 90) {
			if (g_SelectedMenu == MENU_START)
			{
				PlaySound(g_TitleDecisionSE, false);
				SetScene(SCENE_PROLOGUE);
			}
			else if (g_SelectedMenu == MENU_END)
			{
				PlaySound(g_TitleDecisionSE, false);
				PostQuitMessage(0);  // ゲームを終了
			}
		}
	}
	g_OldKeystateSpace = keystateSpace;

	if (Keyboard_IsKeyDownTrigger(KK_N)) {
		SetScene(SCENE_STAGE1);
	}

	PushKeyCool++;
}

void DrawTitle()
{
	TextureForVideo* videoTex = g_VideoTexture.getTexture();
	
	//動画再生
	if (videoTex && videoTex->shader_resource_view) {
		DrawVideoSprite(XMFLOAT2(SCREEN_XCENTER, SCREEN_YCENTER), 0, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), g_VideoTexture);
	}
	
	//タイトルロゴ
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TITLE_LOGO]);
	DrawSprite(TITLE_LOGO_POS, 0.0f, XMFLOAT2(GetWidth(g_Texture[TITLE_LOGO]) * 0.8f , GetHeight(g_Texture[TITLE_LOGO]) * 0.8f));


	// スタートボタン描画（選択されている場合 1、されていない場合 0）
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TITLE_START]);
	DrawSpriteAnim(TITLE_START_POS, 0.0f, XMFLOAT2(GetWidth(g_Texture[TITLE_START]) * 0.9f, GetHeight(g_Texture[TITLE_START]) * 0.7f), 1, 2, g_SelectedMenu == MENU_START ? 1 : 0);

	// エンドボタン描画（選択されている場合 1、されていない場合 0）
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TITLE_END]);
	DrawSpriteAnim(TITLE_END_POS, 0.0f, XMFLOAT2(GetWidth(g_Texture[TITLE_END]) * 0.9f, GetHeight(g_Texture[TITLE_END]) * 0.7f), 1, 2, g_SelectedMenu == MENU_END ? 1 : 0);
}



