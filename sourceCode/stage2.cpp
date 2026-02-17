#include "stage2.h"
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
#include "audio.h"

EnemyProduct* product2;
static int g_Stage2BGM;


void InitStage2() {

	InitializeBG2();

	InitSprite();
	InitializePlayer();
	product2 = new EnemyProduct();
	InitializeBullet();
	InitializeScore();

	product2->LoadStage(2);

	g_Stage2BGM = LoadSound("asset\\BGM\\BGM_STAGEMAP_02.wav");
	PlaySound(g_Stage2BGM, true);
}

void UninitStage2() {
	FinalizePlayer();

	delete product2;
	FinalizeBullet();

	FinalizeBG2();
}

void UpdateStage2() {

	UpdateBG2();

	Player* g_pPlayer = GetPlayer();

	product2->Update();
	UpdateBullet(product2);
	UpdatePlayer(product2);

	//プレイヤーが倒されて落ちたときシーン遷移
	if (g_pPlayer->m_Coordinate.m_Pos.y >= 720.0f + (g_pPlayer->m_Coordinate.m_Size.y * 0.5f)) {
		StopSound(g_Stage2BGM);
		SetScene(SCENE_GAMEOVER);
		UpdateGameOver();
	}

	
	if (Keyboard_IsKeyDownTrigger(KK_M)) {
		StopSound(g_Stage2BGM);
		SetScene(SCENE_RESULT);
	}

	// すべての敵が倒されたら次のステージへ
	if (product2->CheckAllEnemiesDeath()) {
		StopSound(g_Stage2BGM);
		SetScene(SCENE_RESULT);
	}
}

void DrawStage2() {

	DrawBG2();

	DrawBullet();
	product2->Draw();

	DrawPlayer();
	DrawPlayerUI();

	product2->DrawUI();
	DrawScore();
}

EnemyProduct* GetEnemyStage2Product() {
	return product2;
}
