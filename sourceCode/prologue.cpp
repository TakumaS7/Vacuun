#include "prologue.h"
#include "texture.h"
#include "sprite.h"
#include "main.h"
#include "keyboard.h"
#include "stage1.h"
#include "controller.h"
#include "videoPlayer.h"
#include "video_texture.h"
#include "tutorial.h"
#include "wiimote/WiimoteManager.h"
#include "sound.h"

static bool g_OldKeystateSpace = false;
static ID3D11ShaderResourceView* g_Texture;
static Controller g_Controller{};
static VideoTexture g_VideoTexture;

/* Wiimote */
static WiimoteManager* m = nullptr;
//Wiiリモコン
static const char* g_PrologueWiiButton{};
bool g_PrologueWiiButtonDown = false;

bool g_PrologueWiiButtonA = false;
bool g_PrologueOldWiiButtonA = false;
bool g_PrologueNextWiiButtonA = false;

static int g_PrologueBGM;
static int countTime = 0;
void InitializePrologue()
{
	VideoTexture::createAPI();  // Video API 初期化
	g_VideoTexture.create("asset\\Movie\\prologue.mp4");
	g_VideoTexture.setLoop(false);

	countTime = 0;

	/* Wiimote初期化 */
	m = new WiimoteManager;
	

	m->onConnection([](Wiimote* wiimote) {

		wiimote->onButton([wiimote](WiimoteButton button, bool down) {
			const char* buttonName = WiimoteButtons::toString(button);

			if (buttonName) {
				//buttonName：ボタンの種類, down：押したかのフラグ
				g_PrologueWiiButton = buttonName;
				g_PrologueWiiButtonDown = down;
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
			}, true);

		wiimote->start(WiimoteFeatures::Buttons | WiimoteFeatures::Accelerometer | WiimoteFeatures::Extension);
		});

	m->onDisconnection([](Wiimote* wiimote) {
		wiimote->clearEvents();
		});

	m->continuousScan();

	g_PrologueBGM = LoadSound("asset\\BGM\\BGM_STAGEMAP_01.wav");
	PlaySound(g_PrologueBGM, true);
}

void FinalizePrologue()
{
	StopSound(g_PrologueBGM);

	delete m;

	UnInitTexture(g_Texture);
	g_VideoTexture.destroy();
	VideoTexture::destroyAPI();  // Video API 解放
}

void UpdatePrologue()
{
	m->update();

	/* Wiiリモコンボタン *************************************/
	if (g_PrologueWiiButton == "A" && g_PrologueWiiButtonDown) {
		g_PrologueOldWiiButtonA = true;
		g_PrologueNextWiiButtonA = true;
	}
	else if (g_PrologueWiiButton == "A" && !g_PrologueWiiButtonDown) {
		g_PrologueOldWiiButtonA = false;
	}

	if (g_PrologueOldWiiButtonA == g_PrologueNextWiiButtonA) {
		g_PrologueWiiButtonA = false;
	}
	else if (g_PrologueOldWiiButtonA != g_PrologueNextWiiButtonA) {
		g_PrologueWiiButtonA = true;
		g_PrologueNextWiiButtonA = false;
	}
	/*********************************************************/

	g_VideoTexture.update(1.0f);

	//スペースキーかBボタン
	bool keystateSpace = Keyboard_IsKeyDown(KK_SPACE);
	if (!g_OldKeystateSpace && keystateSpace || g_Controller.ControllerDownTrigger(BUTTON_B) || g_PrologueWiiButtonA || countTime >= 3000) {
		SetScene(SCENE_STAGE1);
		UpdateStage1();
	}
	g_OldKeystateSpace = keystateSpace;

	countTime++;
}

void DrawPrologue()
{
	TextureForVideo* videoTex = g_VideoTexture.getTexture();

	if (videoTex && videoTex->shader_resource_view) {
		DrawVideoSprite(XMFLOAT2(SCREEN_XCENTER, SCREEN_YCENTER), 0, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), g_VideoTexture);
	}
	else {
		printf("Warning: Video texture is NULL\n");
	}
}
