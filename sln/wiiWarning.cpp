#include "wiiWarning.h"
#include "texture.h"
#include "sprite.h"
#include "main.h"
#include "fade.h"

static ID3D11ShaderResourceView* g_Texture[2];
static float g_WarningAlpha = 1.0f;
static unsigned int g_WarningTime = 0;

void InitWiiWarning()
{
	g_Texture[0] = InitTexture(L"asset\\texture\\Wii_Warning.jpg");
	g_Texture[1] = InitTexture(L"asset\\texture\\white.png");

	g_WarningAlpha = 1.0f;
	g_WarningTime = 0;
}

void UninitWiiWarning()
{
	for (int i = 0; i < 2; ++i) {
		UnInitTexture(g_Texture[i]);
	}
}

void UpdateWiiWarning()
{
	g_WarningAlpha += -0.01f;
	if (g_WarningAlpha <= 0.0f) { g_WarningAlpha = 0.0f; }

	if (g_WarningTime >= 360) {
		SetFadeScene(SCENE_TITLE);
	}

	g_WarningTime++;
}

void DrawWiiWarning()
{
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
	DrawSprite({ SCREEN_XCENTER, SCREEN_YCENTER }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT });

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);
	DrawSpriteColor({ SCREEN_XCENTER, SCREEN_YCENTER }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, {0.0f, 0.0f, 0.0f, g_WarningAlpha });
}
