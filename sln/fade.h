#ifndef _FADE_H
#define _FADE_H

#include "main.h"

enum FadeStep
{
	FADE_NONE,
	FADE_OUT,
	FADE_IN,
	FADE_MAX,
};

void InitFade();
void UninitFade();
void UpdateFade();
void DrawFade();
void FadeIn();
void FadeOut();
bool CheckFadeIn();
bool CheckFadeOut();
void SetFadeScene(SCENE s);

#endif //_FADE_H
