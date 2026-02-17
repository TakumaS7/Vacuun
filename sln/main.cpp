#include <Windows.h>

/*-------------*/
#include "main.h"
#include "title.h"
#include "prologue.h"
#include "tutorial.h"
#include "stage1.h"
#include "stage2.h"
#include "stage3.h"
#include "result.h"
#include "gameOver.h"
#include "wiiWarning.h"

#include "renderer.h"
#include "sprite.h"
#include "sound.h"
#include "keyboard.h"
#include "frametime.h"
#include "enemyProduct.h"
#include "fade.h"
/*-------------*/


//ライブラリのリンク
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "winmm.lib")


//マクロ定義
#define CLASS_NAME "GameWindow"
#define WINDOW_CAPTION "このウィンドウを消すとプログラム終了"

//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//グローバル変数
#ifdef _DEBUG
int g_CountFPS;
char g_DebugStr[2048] = WINDOW_CAPTION;
#endif//_DEBUG

SCENE scene;

float DeltaTime = 0.0f;
int g_StageNumber = 0;

//メイン
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	//ウィンドウクラスの登録
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	RegisterClass(&wc);

	RECT rc = { 0, 0, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_EX_OVERLAPPEDWINDOW, FALSE);

	//ウィンドウの作成
	HWND hWnd = CreateWindow(
		CLASS_NAME,         //使用するウィンドウクラス
		WINDOW_CAPTION,     //ウィンドウの名前
		WS_OVERLAPPEDWINDOW,//
		CW_USEDEFAULT,      //x
		CW_USEDEFAULT,      //y
		rc.right - rc.left,  //幅
		rc.bottom - rc.top, //高さ
		NULL,               //ウィンドウの親
		NULL,               //ウィンドウメニューの設定
		hInstance,          //アプリケーション本体の変数
		NULL                //
	);

	//DirectXの初期化（ウィンドウを作成した後に行う）
	if (FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}

	//時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	//フレームカウント初期化
	timeBeginPeriod(1);
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;

	//ウィンドウ表示(Init()の後に呼ばないとダメ)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//メッセージループ
	MSG    msg;

	
	while (1)
	{
    		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			DeltaTime = (dwCurrentTime - dwExecLastTime) / 1000.0f;
			GameFrameTime::SetElapsedTime(DeltaTime);

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;

				wsprintf(g_DebugStr, WINDOW_CAPTION);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], "FPS:%d", g_CountFPS);
				SetWindowText(hWnd, g_DebugStr);
#endif
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if ((dwCurrentTime - dwExecLastTime) >= ((float)1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				
				Update();
				Draw();

				keycopy();

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);

	//終了
	Uninit();
	return (int)msg.wParam;
}

//プロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	case WM_ACTIVATEAPP:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard_ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);//ウィンドウ消去
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);//アプリケーション終了
		return 0;
	};

	//その他のメッセージはwindowsに任せる
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	//レンダリング処理の初期化
	InitRenderer(hInstance, hWnd, bWindow);

	//サウンドの初期化
	InitSound(hWnd);

	//ポリゴンの初期化
	InitSprite();

	//フェードの初期化
	InitFade();

	// 起動時のシーンをセット
	scene = SCENE_WII_WARNING;

	// 各シーンによって初期化の処理
	switch (scene) {
	case SCENE_TITLE:
		InitTitle();
		break;
	case SCENE_PROLOGUE:
		InitializePrologue();
		break;
	case SCENE_STAGE1:
		InitStage1();
		break;
	case SCENE_STAGE2:
		InitStage2();
		break;
	case SCENE_STAGE3:
		InitStage3();
		break;
	case SCENE_RESULT:
		InitResult();
		break;
	case SCENE_GAMEOVER:
		InitGameOver();
		break;
	case SCENE_WII_WARNING:
		InitWiiWarning();
		break;
	default:
		break;
	}

	
	return S_OK;
}

void Uninit(void)
{
	//ポリゴン
	UninitSprite();

	switch (scene) {
	case SCENE_TITLE:
		UninitTitle();
		break;
	case SCENE_PROLOGUE:
		FinalizePrologue();
		break;
	case SCENE_STAGE1:
		UninitStage1();
		break;
	case SCENE_STAGE2:
		UninitStage2();
		break;
	case SCENE_STAGE3:
		UninitStage3();
		break;
	case SCENE_RESULT:
		UninitResult();
		break;
	case SCENE_GAMEOVER:
		UninitGameOver();
		break;
	case SCENE_WII_WARNING:
		UninitWiiWarning();
		break;
	default:
		break;
	}

	//フェードの終了処理
	UninitFade();

	//サウンドの終了処理
	UninitSound();

	//レンダリングの終了処理
	UninitRenderer();
}
void Update(void)
{
	UpdateFade();

 	switch (scene) {
	case SCENE_TITLE:
		UpdateTitle();
		break;
	case SCENE_PROLOGUE:
		UpdatePrologue();
		break;
	case SCENE_STAGE1:
		UpdateStage1();
		break;
	case SCENE_STAGE2:
		UpdateStage2();
		break;
	case SCENE_STAGE3:
		UpdateStage3();
		break;
	case SCENE_RESULT:
		UpdateResult();
		break;
	case SCENE_GAMEOVER:
		UpdateGameOver();
		break;
	case SCENE_WII_WARNING:
		UpdateWiiWarning();
		break;
	default:
		break;
	}

}

void Draw(void)
{
	//バッファクリア
	Clear();

	//2D描画なので深度無効
	SetDepthEnable(false);


	switch (scene) {
	case SCENE_TITLE:
		DrawTitle();
		break;
	case SCENE_PROLOGUE:
		DrawPrologue();
		break;
	case SCENE_STAGE1:
		DrawStage1();
		break;
	case SCENE_STAGE2:
		DrawStage2();
		break;
	case SCENE_STAGE3:
		DrawStage3();
		break;
	case SCENE_RESULT:
		DrawStage();
		DrawResult();
		break;
	case SCENE_GAMEOVER:
		DrawStage();
		DrawGameOver();
		break;
	case SCENE_WII_WARNING:
		DrawWiiWarning();
		break;
	default:
		break;
	}

	//フェードの描画処理
	DrawFade();

	//ここに描画処理 -------------> game.cppに移植
	//DrawWind();

	//バックバッファ、フロントバッファ入れ替え
	Present();
}



// ===============================================================
// シーン取得
// ===============================================================
SCENE GetScene(void) {
	return scene;
}


// ===============================================================
// シーン遷移処理
// ===============================================================
void SetScene(SCENE s) {

	// 現在のシーンの終了
	switch (scene) {
	case SCENE_TITLE:
		UninitTitle();
		break;
	case SCENE_PROLOGUE:
		FinalizePrologue();
		break;
	case SCENE_STAGE1:
		break;
	case SCENE_STAGE2:
		break;
	case SCENE_STAGE3:
		break;
	case SCENE_RESULT:
		UninitStage();
		UninitResult();
		break;
	case SCENE_GAMEOVER:
		UninitStage();
		UninitGameOver();
		break;
	case SCENE_WII_WARNING:
		UninitWiiWarning();
		break;
	default:
		break;
	}

	//前のシーンを記録
	float beforeScene = scene;
	// 次のシーンに更新
	scene = s;

	
	// 次のシーンの初期化
	switch (scene) {
	case SCENE_TITLE:
		InitTitle();
		break;
	case SCENE_PROLOGUE:
		InitializePrologue();
		break;
	case SCENE_STAGE1:
		InitStage1();
		g_StageNumber = SCENE_STAGE1;

		//ゲームオーバー後ならチュートリアル無効
		if (beforeScene == SCENE_GAMEOVER) { SetInvalidTutorial(); }
		break;
	case SCENE_STAGE2:
		InitStage2();
		g_StageNumber = SCENE_STAGE2;
		break;
	case SCENE_STAGE3:
		InitStage3();
		g_StageNumber = SCENE_STAGE3;
		break;
	case SCENE_RESULT:
		InitResult();
		break;
	case SCENE_GAMEOVER:
		InitGameOver();
		break;
	case SCENE_WII_WARNING:
		InitWiiWarning();
		break;
	default:
		break;
	}
}

void UninitStage()
{
	switch (g_StageNumber) {
	case SCENE_STAGE1:
		UninitStage1();
		break;
	case SCENE_STAGE2:
		UninitStage2();
		break;
	case SCENE_STAGE3:
		UninitStage3();
		break;
	}
}

void UpdateStage()
{
	switch (g_StageNumber) {
	case SCENE_STAGE1:
		UpdateStage1();
		break;
	case SCENE_STAGE2:
		UpdateStage2();
		break;
	case SCENE_STAGE3:
		UpdateStage3();
		break;
	}
}

void DrawStage()
{
	switch (g_StageNumber) {
	case SCENE_STAGE1:
		DrawStage1();
		break;
	case SCENE_STAGE2:
		DrawStage2();
		break;
	case SCENE_STAGE3:
		DrawStage3();
		break;
	}
}

SCENE GetStage()
{
	return (SCENE)g_StageNumber;
}

SCENE GetNextStage()
{
	return (SCENE)(g_StageNumber + 1);
}

EnemyProduct* GetEnemyProduct() {
	switch (g_StageNumber) {
	case SCENE_STAGE1:
		return GetEnemyStage1Product();
		break;
	case SCENE_STAGE2:
		return GetEnemyStage2Product();
		break;
	case SCENE_STAGE3:
		return GetEnemyStage3Product();
		break;
	}
}