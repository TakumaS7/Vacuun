#ifndef _STAGE3_H_
#define _STAGE3_H
#include "enemyProduct.h"

void InitStage3();
void UninitStage3();
void UpdateStage3();
void DrawStage3();

void SetIsClear(bool b);
bool GetIsClear();
EnemyProduct* GetEnemyStage3Product();
void SetToResult(bool b);

#endif //!_STAGE3_H_
