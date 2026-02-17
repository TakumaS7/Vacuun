#include "stage1.h"
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
#include "tutorial.h"

EnemyProduct* product;
static int g_Stage1BGM;


void InitStage1() {

	InitializeBG();

	InitSprite();
	InitializePlayer();
	product = new EnemyProduct();
	InitializeBullet();
	InitializeScore();
	InitTutorial();
	product->LoadStage(1);

	g_Stage1BGM = LoadSound("asset\\BGM\\BGM_STAGEMAP_01.wav");
	PlaySound(g_Stage1BGM, true);
}

void UninitStage1() {
	FinalizePlayer();

	delete product;
	FinalizeBullet();
	UninitTutorial();
	FinalizeBG();
}

void UpdateStage1()
{
	Player* g_pPlayer = GetPlayer();

	//プレイヤーが倒されて落ちたときシーン遷移
	if (g_pPlayer->m_Coordinate.m_Pos.y >= 720.0f + (g_pPlayer->m_Coordinate.m_Size.y * 0.5f)) {
		StopSound(g_Stage1BGM);
		SetScene(SCENE_GAMEOVER);
		UpdateGameOver();
	}

	if (Keyboard_IsKeyDownTrigger(KK_M)) {
		SetIsTutorial(false);
		StopSound(g_Stage1BGM);
		SetScene(SCENE_RESULT);
	}
	//クリア判定
	if (product->CheckAllEnemiesDeath()) {
		StopSound(g_Stage1BGM);
		SetScene(SCENE_RESULT);
	}

	UpdateBG();

	UpdateTutorial();

	if (GetStopProgress()) { return; }

	product->Update();
	UpdateBullet(product);
	UpdatePlayer(product);

}

void DrawStage1() {

	DrawBG();

	Player* g_pPlayer = GetPlayer();


	DrawBullet();
	product->Draw();
	
	DrawPlayer();
	DrawPlayerUI();

	product->DrawUI();

	DrawScore();

	DrawTutorial();

	if (g_pPlayer->m_Coordinate.m_Pos.y >= 720.0f + (g_pPlayer->m_Coordinate.m_Size.y * 0.5f)) {
		//SetScene(SCENE_GAMEOVER);
		DrawGameOver();
	}
}

EnemyProduct* GetEnemyStage1Product() {
	return product;
}

