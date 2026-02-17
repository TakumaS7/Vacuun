//==================================================================
//	チュートリアル [tutorial.cpp]
//
// Author :　大岡優剛		Date	: 2025/02/07
//==================================================================
#include "tutorial.h"
#include "frameTime.h"
#include "texture.h"
#include "sprite.h"
#include "keyboard.h"
#include "title.h"
#include "stage1.h"
#include "controller.h"
#include "player.h"
#include "bullet.h"
#include "enemyWeak.h"
#include "bg.h"
#include "wiimote/WiimoteManager.h"
#include "video_texture.h"
#include "score.h"

enum Tutorial_type
{
	TUTORIAL_START,
	TUTORIAL_MOVE,
	TUTORIAL_NEXT,
	TUTORIAL_ABRORBED_01,
	TUTORIAL_ABRORBED_02,
	TUTORIAL_GARD_01,
	TUTORIAL_GARD_02,
	TUTORIAL_SHOT_01,
	TUTORIAL_SHOT_02,
	TUTORIAL_SHOT_DO,
	TUTORIAL_NICE,
    TUTORIAL_ULT,
	TUTORIAL_END,
	TUTORIAL_MAX,
};

static ID3D11ShaderResourceView* g_Texture[TUTORIAL_MAX];
static ID3D11ShaderResourceView* g_PushTexture[3];
static ID3D11ShaderResourceView* g_TextureBlack;
static ID3D11ShaderResourceView* g_OtherTexture[3];

EnemyProduct* tEnemy;

//背景横スクロール
static float delta_x[6] = { 0 };

static bool IsTutorial ;
static float move_timer;
static bool player_moved;

static int now_tutorial;

static bool IsBlack;
static bool IsStopProgress;
static bool enemyCreated = false;

static int g_Time;
static float lastEnemyCreateTime = 0.0f;



/* Wiimote */
static WiimoteManager* m = nullptr;
//Wiiリモコン
static const char* g_TutorialWiiButton{};
bool g_TutorialWiiButtonDown = false;

bool g_TutorialWiiButtonA = false;
bool g_TutorialOldWiiButtonA = false;
bool g_TutorialNextWiiButtonA = false;
static VideoTexture g_VideoTexture;

// チュートリアル遷移用のタイマーとフラグ
static float tutorialTransitionTime = -1.0f;
static bool isTransitioning = false;

//チェックタイマー
static float checkTime = -1.0f;
void InitTutorial()
{
    tutorialTransitionTime = -1.0f;
    checkTime = -1.0f;
	// テクスチャの読み込み
	g_Texture[TUTORIAL_START]	   	= InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_01.png");
	g_Texture[TUTORIAL_MOVE]	    = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_02.png");
	g_Texture[TUTORIAL_NEXT]	    = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_03.png");
	g_Texture[TUTORIAL_ABRORBED_01] = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_04.png");
	g_Texture[TUTORIAL_ABRORBED_02] = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_05.png");
	g_Texture[TUTORIAL_GARD_01]     = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_06.png");
	g_Texture[TUTORIAL_GARD_02]     = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_08_01.png");
	g_Texture[TUTORIAL_SHOT_01]     = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_07.png");
	g_Texture[TUTORIAL_SHOT_02]     = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_08_02.png");
	g_Texture[TUTORIAL_SHOT_DO]     = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_08.png");
	g_Texture[TUTORIAL_NICE]	    = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_09.png");
    g_Texture[TUTORIAL_ULT]         = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_12.png");
	g_Texture[TUTORIAL_END]			= InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_10.png");
	g_TextureBlack = InitTexture(L"asset\\texture\\tutorial\\brack.png");

    g_PushTexture[0] = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_02_01.png");
    g_PushTexture[1] = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_05_01.png");
    g_PushTexture[2] = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_05_02.png");

    g_OtherTexture[0] = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_08_01.png");
    g_OtherTexture[1] = InitTexture(L"asset\\texture\\tutorial\\UI_TUTORIAL_08_02.png");
    g_OtherTexture[2] = InitTexture(L"asset\\texture\\tutorial\\CHECK.png");

	IsStopProgress = false;

	IsTutorial = true;
	move_timer = 0.0f;
	player_moved = false;
	now_tutorial = TUTORIAL_START;
	IsBlack = false;
	g_Time = 0;
	GetEnemyStage1Product()->SetTimeStop(true);

    VideoTexture::createAPI();  // Video API 初期化
    g_VideoTexture.create("asset\\Movie\\Wii_Explanation_07.mp4");

    m = new WiimoteManager;

    m->onConnection([](Wiimote* wiimote) {

        wiimote->onButton([wiimote](WiimoteButton button, bool down) {
            const char* buttonName = WiimoteButtons::toString(button);

            if (buttonName) {
                //buttonName：ボタンの種類, down：押したかのフラグ
                g_TutorialWiiButton = buttonName;
                g_TutorialWiiButtonDown = down;
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
}

void UninitTutorial()
{
    delete m;

	GetEnemyStage1Product()->SetTimeStop(false);

	//テクスチャの解放
	for (int i = 0; i < TUTORIAL_MAX; ++i) {
		UnInitTexture(g_Texture[i]);
	}
	UnInitTexture(g_TextureBlack);

    g_VideoTexture.destroy();
    VideoTexture::destroyAPI();  // Video API 解放
}

void UpdateTutorial()
{
    m->update();

    /* Wiiリモコンボタン *************************************/
    if (g_TutorialWiiButton == "A" && g_TutorialWiiButtonDown) {
        g_TutorialOldWiiButtonA = true;
        g_TutorialNextWiiButtonA = true;
    }
    else if (g_TutorialWiiButton == "A" && !g_TutorialWiiButtonDown) {
        g_TutorialOldWiiButtonA = false;
    }

    if (g_TutorialOldWiiButtonA == g_TutorialNextWiiButtonA) {
        g_TutorialWiiButtonA = false;
    }
    else if (g_TutorialOldWiiButtonA != g_TutorialNextWiiButtonA) {
        g_TutorialWiiButtonA = true;
        g_TutorialNextWiiButtonA = false;
    }
    /*********************************************************/

    if (!IsTutorial) { return; }

    UpdateBG();

    Player* g_pPlayer = GetPlayer();
    vector<Enemy*>* enemies = GetEnemyStage1Product()->GetEnemy();

    if (Keyboard_IsKeyDownTrigger(KK_G) || (g_TutorialWiiButton == "Plus" && !g_TutorialWiiButtonDown)) {
        IsTutorial = false;
        IsStopProgress = false;
        GetEnemyStage1Product()->SetTimeStop(false);
        return;
    }

    switch (now_tutorial) {
    case TUTORIAL_START:
        if (Keyboard_IsKeyDownTrigger(KK_ENTER) || g_TutorialWiiButtonA) {
            now_tutorial = TUTORIAL_MOVE;
        }
        break;

    case TUTORIAL_MOVE:
        if ((g_pPlayer->m_Coordinate.m_Vel.x != 0 || g_pPlayer->m_Coordinate.m_Vel.y != 0)) {
            move_timer++; 
        }
     
        if (move_timer > 90.0f) {
            now_tutorial = TUTORIAL_NEXT;
        }
        
        break;

    case TUTORIAL_NEXT:
        if (Keyboard_IsKeyDownTrigger(KK_ENTER) || g_TutorialWiiButtonA) {
            now_tutorial = TUTORIAL_ABRORBED_01;
            IsStopProgress = true;
            IsBlack = true;
            GetPlayer()->SetState(PLAYERSTATE_DEFAULT);
        }
        break;

    case TUTORIAL_ABRORBED_01:
        if (Keyboard_IsKeyDownTrigger(KK_ENTER) || g_TutorialWiiButtonA) {
            now_tutorial = TUTORIAL_ABRORBED_02;
            IsStopProgress = false;
            IsBlack = false;
            g_Time = 0;
            enemyCreated = false; // 敵の生成フラグをリセット
            
        }
        break;

    case TUTORIAL_ABRORBED_02:
        g_VideoTexture.update(1.0f);

        if (!enemyCreated) {
            CreateTutorialEnemy();
            enemyCreated = true;
        }

        if (CheckTutorialAbsorbed() && !isTransitioning) {
            tutorialTransitionTime = g_Time; 
            isTransitioning = true;        
        }

        if (isTransitioning && g_Time - tutorialTransitionTime >= 30) {
            now_tutorial = TUTORIAL_GARD_01;
            g_Time = 0;
            enemyCreated = false;
            tutorialTransitionTime = -1.0f; 
            isTransitioning = false;
        }
        break;

    case TUTORIAL_GARD_01:
        if (!enemyCreated) {
            CreateTutorialEnemy();
            enemyCreated = true;
        }

        if (CheckTutorialGard() && !isTransitioning) {
            tutorialTransitionTime = g_Time;
            isTransitioning = true;
            checkTime = 0.0f;
        }

        if (isTransitioning && g_Time - tutorialTransitionTime >= 60) {
            now_tutorial = TUTORIAL_SHOT_01;
            g_Time = 0;
            enemyCreated = false;
            tutorialTransitionTime = -1.0f;
            isTransitioning = false;
            checkTime = -1.0f;
        }
        break;

    case TUTORIAL_SHOT_01:
        if (!enemyCreated) {
            CreateTutorialEnemy();
            enemyCreated = true;
        }

        if (CheckTutorialShot() && !isTransitioning) {
            tutorialTransitionTime = g_Time;
            isTransitioning = true;
            checkTime = 0.0f;
        }

        if (isTransitioning && g_Time - tutorialTransitionTime >= 60) {
            now_tutorial = TUTORIAL_NICE;
            enemyCreated = false;
            tutorialTransitionTime = -1.0f;
            isTransitioning = false;
            checkTime = -1.0f;
        }
        break;     

    case TUTORIAL_NICE:
        if (Keyboard_IsKeyDownTrigger(KK_ENTER) || g_TutorialWiiButtonA) {
            now_tutorial = TUTORIAL_ULT;
            IsBlack = true;
            IsStopProgress = true;
        }
        break;
    case TUTORIAL_ULT:
        if (Keyboard_IsKeyDownTrigger(KK_ENTER) || g_TutorialWiiButtonA) {
            now_tutorial = TUTORIAL_END;
            IsStopProgress = false;
            IsBlack = false;
            g_Time = 0;
            enemyCreated = false; // 敵の生成フラグをリセット

        }
        break;
    case TUTORIAL_END:
        if (Keyboard_IsKeyDownTrigger(KK_ENTER) || g_TutorialWiiButtonA) {
            IsTutorial = false;
            IsStopProgress = false;
            GetEnemyStage1Product()->SetTimeStop(false);
        }
    }



    // 生成した敵が x < 0 になったら再生成
    if (enemyCreated && g_Time > 300) {
        CreateTutorialEnemy();
        g_Time = 0;
    }

    //チェックタイマーの加算
    if (checkTime >= 21.0f) {
        checkTime = 21.0f;
    } else if (checkTime >= 0.0f) {
        checkTime++;
    }
    g_Time++;
}


void DrawTutorial()
{
    if (!IsTutorial) { return; }

	if (IsBlack) {

		XMFLOAT2 center = GetPlayer()->GetAimingCenter();
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_TextureBlack);
		DrawSpriteColor(XMFLOAT2(center.x + 760.0f, center.y + 175.0f), 0.0f, XMFLOAT2(SCREEN_WIDTH * 11, SCREEN_HEIGHT * 11), { 1.0f, 1.0f,1.0f,0.5f });


        if (now_tutorial == TUTORIAL_ULT) {
            DrawPlayerUI();
            DrawScore();
        }
	}


	if (!IsTutorial) { return; }


    if (now_tutorial == TUTORIAL_ULT) {
        GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[now_tutorial]);
        DrawSprite(XMFLOAT2(SCREEN_XCENTER, SCREEN_YCENTER - 35), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_Texture[now_tutorial])) * 0.8f, static_cast<float>(GetHeight(g_Texture[now_tutorial])) * 0.8f));
        return;
    }

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[now_tutorial]); 
	DrawSprite(XMFLOAT2(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 160), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_Texture[now_tutorial])) * 0.3f, static_cast<float>(GetHeight(g_Texture[now_tutorial])) * 0.3f));

    
    if (now_tutorial == TUTORIAL_MOVE) {
        GetDeviceContext()->PSSetShaderResources(0, 1, &g_PushTexture[0]);
        DrawSpriteAnim(XMFLOAT2(250, SCREEN_HEIGHT - 100), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_PushTexture[0])) * 0.4f, static_cast<float>(GetHeight(g_PushTexture[0])) / 4 * 0.4f ), 1, 4, static_cast<int>(g_Time * 0.0625) % 4);
        return;
    }

    if (now_tutorial == TUTORIAL_ABRORBED_02) {
        GetDeviceContext()->PSSetShaderResources(0, 1, &g_PushTexture[1]);
        DrawSpriteAnim(XMFLOAT2(250, SCREEN_HEIGHT - 100), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_PushTexture[1])) * 0.4f, static_cast<float>(GetHeight(g_PushTexture[1])) / 8 * 0.4f ), 1, 8, static_cast<int>(g_Time * 0.125) % 8);
    
        TextureForVideo* videoTex = g_VideoTexture.getTexture();

        //動画再生
        if (videoTex && videoTex->shader_resource_view) {
            DrawVideoSprite(XMFLOAT2(200, 500), 0, XMFLOAT2(300, 150), g_VideoTexture);
        }
    }
    else if (now_tutorial == TUTORIAL_SHOT_01) {
        GetDeviceContext()->PSSetShaderResources(0, 1, &g_PushTexture[2]);
        DrawSpriteAnim(XMFLOAT2(200, SCREEN_HEIGHT - 100), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_PushTexture[2])) * 0.4f, static_cast<float>(GetHeight(g_PushTexture[2])) / 8 * 0.4f), 1, 8, static_cast<int>(g_Time * 0.125) % 8);

        GetDeviceContext()->PSSetShaderResources(0, 1, &g_OtherTexture[1]);
        DrawSprite(XMFLOAT2(SCREEN_WIDTH - 200, 100), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_OtherTexture[2])) * 0.37f, static_cast<float>(GetHeight(g_OtherTexture[2])) * 0.37f));
      
    }
    else if (now_tutorial == TUTORIAL_GARD_01) {
        GetDeviceContext()->PSSetShaderResources(0, 1, &g_OtherTexture[0]);
        DrawSprite(XMFLOAT2(SCREEN_WIDTH - 200, 100), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_OtherTexture[1])) * 0.37f, static_cast<float>(GetHeight(g_OtherTexture[1])) * 0.37f));
       
    }

    if (checkTime >= 0.0f) {
        GetDeviceContext()->PSSetShaderResources(0, 1, &g_OtherTexture[2]);
        DrawSpriteAnim(XMFLOAT2(SCREEN_WIDTH - 40, 100), 0.0f, XMFLOAT2(static_cast<float>(GetWidth(g_OtherTexture[2])) / 8 * 0.4f, static_cast<float>(GetHeight(g_OtherTexture[2]))  * 0.4f), 8, 1, static_cast<int>(checkTime / 3));

    }

}


bool GetIsTutorial()
{
	return IsTutorial;
}

void SetIsTutorial(bool b)
{
    IsTutorial = b;
}

bool CheckTutorialAbsorbed()
{
	if (GetPlayer()->GetState() != PLAYERSTATE_HOLD) { return false; }

	
	vector<Enemy*>* enemies = GetEnemyStage1Product()->GetEnemy();

	for (Enemy* enemy : *enemies){

		if (!enemy->GetUse()) { continue; }

		if (enemy->GetState() == ENEMYSTATE_KNOCKOUT) { return true; }
	}

	return false;
}

bool CheckTutorialGard()
{
	vector<Enemy*>* enemies = GetEnemyStage1Product()->GetEnemy();

	GetPlayer()->SetHoldingTimeLimit(GetPlayer()->GetHoldingTimeLimit() + 10);

	for (Enemy* enemy : *enemies) {

		if (!enemy->GetUse()) { continue; }

		if (enemy->GetIsHit()) { return true; }

	}
	return false;
}

bool CheckTutorialShot()
{
	vector<Enemy*>* enemies = GetEnemyStage1Product()->GetEnemy();

	GetPlayer()->SetHoldingTimeLimit(GetPlayer()->GetHoldingTimeLimit() + 10);

	for (Enemy* enemy : *enemies) {

		if (!enemy->GetUse()) { continue; }

		if (enemy->GetIsHit() && enemy->GetIsShot()) { return true; }

	}
	return false;
}

void CreateTutorialEnemy()
{
    XMFLOAT3 pos = GetPlayer()->m_Coordinate.m_Pos;
    GetEnemyStage1Product()->Create(0, ENEMYTYPE_STONE, SCREEN_WIDTH + 30, pos.y + 10, 1);
    GetEnemyStage1Product()->Create(0, ENEMYTYPE_STONE, SCREEN_WIDTH + 20, pos.y, 1);
    GetEnemyStage1Product()->Create(0, ENEMYTYPE_STONE, SCREEN_WIDTH + 30, pos.y - 10, 1);

    enemyCreated = true; 
}


const bool GetStopProgress() 
{
	return IsStopProgress;
}

void SetIsStopProgress(bool b)
{
    IsStopProgress = b;
}

void SetInvalidTutorial()
{
    SetIsTutorial(false);
    SetIsStopProgress(false);
    GetEnemyStage1Product()->SetTimeStop(false);
}