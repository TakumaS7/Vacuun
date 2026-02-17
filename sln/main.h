#ifndef _MAIN_H
#define _MAIN_H
/*==============================================================================

   共通ヘッダー [main.h]
                                                         Author :
                                                         Date   :
--------------------------------------------------------------------------------

==============================================================================*/


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)



#include <DirectXMath.h>
using namespace DirectX;

#include "DirectXTex.h"

#if _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex_Release.lib")
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCREEN_WIDTH	(1280)				// ウインドウの幅
#define SCREEN_HEIGHT	(720)				// ウインドウの高さ
#define SCREEN_XCENTER  (640)
#define SCREEN_YCENTER  (360)

#define HANABI_SIZE_HEIGHT (200)
#define HANABI_SIZE_WIDTH (200)

#define WIND_SPEED (0.15f)

#define HANABI_FIRST_SPEED (3.f)

#define SAFE_RELEASE(a) if (a) { (a)->Release(); a = NULL; }

#define EFFECT_MAX (10)//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//シーン管理用
enum SCENE {
	SCENE_TITLE = 0,
	SCENE_PROLOGUE,
	SCENE_STAGE1,
	SCENE_STAGE2,
	SCENE_STAGE3,
	SCENE_RESULT,
	SCENE_GAMEOVER,
	SCENE_WII_WARNING,

	SCENE_MAX,
};

class EnemyProduct;

SCENE GetScene(void);
void SetScene(SCENE s);

void UninitStage();
void UpdateStage();
void DrawStage();
SCENE GetStage();
SCENE GetNextStage();
EnemyProduct* GetEnemyProduct();
#endif //_MAIN_H