#include "stage3.h"
#include "sprite.h"
#include "texture.h"
#include "sound.h"
#include "keyboard.h"
#include "player.h"
#include "bullet.h"
#include "enemyProduct.h"
#include "gameOver.h"
#include "math.h"
#include "score.h"
#include "bg.h"
#include "result.h"

EnemyProduct* product3;
int IsClear = false;
bool ToResult = false;
int clearCoutTime;
static int g_Stage3BGM;

void InitStage3()
{
	InitializeBG3();

	InitSprite();
	InitializePlayer();
	product3 = new EnemyProduct();
	InitializeBullet();
	InitializeScore();
	clearCoutTime = 0.0f;
	ToResult = false;
	product3->LoadStage(3);

	g_Stage3BGM = LoadSound("asset\\BGM\\BGM_STAGEMAP_03.wav");
	PlaySound(g_Stage3BGM, true);
}

void UninitStage3()
{
	FinalizePlayer();

	delete product3;
	FinalizeBullet();

	FinalizeBG3();

	SetIsClear(false);
}

void UpdateStage3()
{
	UpdateBG3();

	Player* g_pPlayer = GetPlayer();

	product3->Update();
	UpdateBullet(product3);
	UpdatePlayer(product3);

	//プレイヤーが倒されて落ちたときシーン遷移
	if (g_pPlayer->m_Coordinate.m_Pos.y >= 720.0f + (g_pPlayer->m_Coordinate.m_Size.y * 0.5f)) {
		StopSound(g_Stage3BGM);
		SetScene(SCENE_GAMEOVER);
		UpdateGameOver();
	}

	//クリアしたらボス以外を消す
	if (IsClear) { product3->AllDisableEnemies(); }

	//ボス撃破後からクリアまでのラグ
	if (ToResult) {
		clearCoutTime++;
	}
	if (clearCoutTime > 90) {
		StopSound(g_Stage3BGM);
		SetAllClear(true);
		SetScene(SCENE_RESULT);
	}
}

void DrawStage3()
{
	DrawBG3();

	DrawBullet();
	product3->Draw();

	DrawPlayer();
	DrawPlayerUI();

	product3->DrawUI();
	DrawScore();
}

EnemyProduct* GetEnemyStage3Product() {
	return product3;
}


bool GetIsClear() { return IsClear; }
void SetIsClear(bool b) { IsClear = b; }
void SetToResult(bool b) { ToResult = b; };