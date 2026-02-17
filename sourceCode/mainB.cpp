#include <Windows.h>
#include "renderer.h"
#include "sprite.h"
#include "sound.h"
#include "keyboard.h"
#include "wind.h"
#include "hanabi.h"
#include "explosion.h"
#include "vanish.h"
#include "score.h"
#include "target.h"


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

	//ポリゴン
	InitSprite();
	InitScore();
	InitWind();
	InitTarget();
	InitHanabi();
	InitExplosion();
	InitVanish();
	
	return S_OK;
}

void Uninit(void)
{
	//ポリゴン
	UninitSprite();
	UnInitWind();
	UninitTarget();
	UnInitHanabi();
	UnInitExplosion();
	UnInitVanish();

	//レンダリングの終了処理
	UninitRenderer();
}
void Update(void)
{
	UpdateWind();
	UpdateTarget();
	UpdateHanabi();
	UpdateExplosion();
	UpdateVanish();
}

void Draw(void)
{
	//バッファクリア
	Clear();

	//2D描画なので深度無効
	SetDepthEnable(false);

	

	//ここに描画処理
	DrawWind();
	DrawTarget();
	DrawHanabi();
	DrawExplosion();
	DrawVanish();

	//バックバッファ、フロントバッファ入れ替え
	Present();
}