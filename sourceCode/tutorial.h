//==================================================================
//
//	チュートリアル [tutorial.h]
//
//
// Author :　大岡優剛		Date	: 2025/02/07
//==================================================================
#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#define BACKGROUND_SPEED (5.0f)

#include "enemyProduct.h"

void InitTutorial();
void UninitTutorial();
void UpdateTutorial();
void DrawTutorial();

bool CheckTutorialAbsorbed();
bool CheckTutorialGard();
bool GetIsTutorial();
void CreateTutorialEnemy();
bool CheckTutorialShot();
const bool GetStopProgress();
void SetIsTutorial(bool b);
void SetIsStopProgress(bool b);
void SetInvalidTutorial();

#endif // _TUTORIAL_H_