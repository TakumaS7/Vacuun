#ifndef _GAME_H_
#define _GAME_H

#include "enemyProduct.h"

void InitGame();
void UninitGame();
void UpdateGame();
void DrawGame();

//void InitStage2();
//void UninitStage2();
//void UpdateStage2();
//void DrawStage2();

EnemyProduct* GetEnemyGameProduct();
void SetIsPogress(bool b);
#endif // !_GAME_H_

