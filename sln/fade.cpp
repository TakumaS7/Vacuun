#include "fade.h"
#include "texture.h"
#include "sprite.h"
#include "main.h"

static ID3D11ShaderResourceView* g_FadeTexture;
static float g_FadeAlpha = 0.0f;
static FadeStep g_FadeStep = FADE_NONE;
static SCENE g_FadeScene = SCENE_MAX;
static bool g_FadeSceneChange = true;

void InitFade()
{
	g_FadeTexture = InitTexture(L"asset\\texture\\white.png");
}

void UninitFade()
{
	UnInitTexture(g_FadeTexture);
}

void UpdateFade()
{
	switch (g_FadeStep)
	{
	case FADE_NONE:
		g_FadeSceneChange = true;
		break;
	case FADE_OUT:
		FadeOut();
		if (CheckFadeOut()) { 
			g_FadeStep = FADE_IN; 
		}
		break;
	case FADE_IN:
		if (g_FadeSceneChange == true){
			SetScene(g_FadeScene); 
			g_FadeSceneChange = false;
		}
		FadeIn();
		if (CheckFadeIn()) { g_FadeStep = FADE_NONE; }
		break;
	case FADE_MAX:
		break;
	default:
		break;
	}
}

void DrawFade()
{
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_FadeTexture);
	DrawSpriteColor({ SCREEN_XCENTER, SCREEN_YCENTER }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 0.0f, 0.0f, 0.0f, g_FadeAlpha });
}

void FadeIn()
{
	g_FadeAlpha += -0.01f;

	if (g_FadeAlpha <= 0.0f) { g_FadeAlpha = 0.0f; }
}

void FadeOut()
{
	g_FadeAlpha += 0.01f;

	if (g_FadeAlpha >= 1.0f) { g_FadeAlpha = 1.0f; }
}

bool CheckFadeIn()
{
	return (g_FadeAlpha <= 0.0f);
}

bool CheckFadeOut()
{
	return (g_FadeAlpha >= 1.0f);
}

void SetFadeScene(SCENE s)
{
	if (g_FadeStep == FADE_NONE) {
		g_FadeStep = FADE_OUT;
		g_FadeScene = s;
	}
}

