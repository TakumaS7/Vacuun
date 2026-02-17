#include "bg.h"
#include "texture.h"
#include "sprite.h"

static ID3D11ShaderResourceView* g_Texture[6];
static ID3D11ShaderResourceView* g_Texture2[6];
static ID3D11ShaderResourceView* g_Texture3[6];

//îwåiâ°ÉXÉNÉçÅ[Éã
static float delta_x[6] = { 0 };


void InitializeBG()
{
	g_Texture[0] = InitTexture(L"asset\\texture\\GAME_BG_S1\\SCENE_BG_GAME_STAGE1.png");
	g_Texture[1] = InitTexture(L"asset\\texture\\GAME_BG_S1\\SCENE_BG_GAME_STAGE2.png");
	g_Texture[2] = InitTexture(L"asset\\texture\\GAME_BG_S1\\SCENE_BG_GAME_STAGE3.png");
	g_Texture[3] = InitTexture(L"asset\\texture\\GAME_BG_S1\\SCENE_BG_GAME_STAGE4.png");
	g_Texture[4] = InitTexture(L"asset\\texture\\GAME_BG_S1\\SCENE_BG_GAME_STAGE5.png");
	g_Texture[5] = InitTexture(L"asset\\texture\\GAME_BG_S1\\SCENE_BG_GAME_STAGE6.png");
}

void FinalizeBG()
{
	for (int i = 0; i < 6; i++) {
		UnInitTexture(g_Texture[i]);
	}
}

void UpdateBG()
{
	for (int i = 0; i < 6; i++) {
		delta_x[i] += BACKGROUND_SPEED * pow(0.3, 6 - i - 1);
		if (delta_x[i] > SCREEN_WIDTH) {
			delta_x[i] -= SCREEN_WIDTH;
		}
	}
}

void DrawBG()
{
	for (int i = 0; i < 6; i++) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);
		DrawSpriteAnim(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), 1, 1, 1);
		DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2 - delta_x[i], SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
		DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2 - delta_x[i] + SCREEN_WIDTH, SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
}

void InitializeBG2()
{
	g_Texture2[0] = InitTexture(L"asset\\texture\\GAME_BG_S2\\SCENE_BG_GAME_STAGE1.png");
	g_Texture2[1] = InitTexture(L"asset\\texture\\GAME_BG_S2\\SCENE_BG_GAME_STAGE2.png");
	g_Texture2[2] = InitTexture(L"asset\\texture\\GAME_BG_S2\\SCENE_BG_GAME_STAGE3.png");
	g_Texture2[3] = InitTexture(L"asset\\texture\\GAME_BG_S2\\SCENE_BG_GAME_STAGE4.png");
	g_Texture2[4] = InitTexture(L"asset\\texture\\GAME_BG_S2\\SCENE_BG_GAME_STAGE5.png");
	g_Texture2[5] = InitTexture(L"asset\\texture\\GAME_BG_S2\\SCENE_BG_GAME_STAGE6.png");
}

void FinalizeBG2()
{
	for (int i = 0; i < 6; i++) {
		UnInitTexture(g_Texture2[i]);
	}
}

void UpdateBG2()
{
	for (int i = 0; i < 6; i++) {
		delta_x[i] += BACKGROUND_SPEED * pow(0.3, 6 - i - 1);
		if (delta_x[i] > SCREEN_WIDTH) {
			delta_x[i] -= SCREEN_WIDTH;
		}
	}
}

void DrawBG2()
{
	for (int i = 0; i < 6; i++) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture2[i]);
		DrawSpriteAnim(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), 1, 1, 1);
		DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2 - delta_x[i], SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
		DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2 - delta_x[i] + SCREEN_WIDTH, SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
}

void InitializeBG3()
{
	g_Texture3[0] = InitTexture(L"asset\\texture\\GAME_BG_S3\\SCENE_BG_GAME_STAGE1.png");
	g_Texture3[1] = InitTexture(L"asset\\texture\\GAME_BG_S3\\SCENE_BG_GAME_STAGE2.png");
	g_Texture3[2] = InitTexture(L"asset\\texture\\GAME_BG_S3\\SCENE_BG_GAME_STAGE3.png");
	g_Texture3[3] = InitTexture(L"asset\\texture\\GAME_BG_S3\\SCENE_BG_GAME_STAGE4.png");
	g_Texture3[4] = InitTexture(L"asset\\texture\\GAME_BG_S3\\SCENE_BG_GAME_STAGE5.png");
	g_Texture3[5] = InitTexture(L"asset\\texture\\GAME_BG_S3\\SCENE_BG_GAME_STAGE6.png");
}

void FinalizeBG3()
{
	for (int i = 0; i < 6; i++) {
		UnInitTexture(g_Texture3[i]);
	}
}

void UpdateBG3()
{
	for (int i = 0; i < 6; i++) {
		delta_x[i] += BACKGROUND_SPEED * pow(0.3, 6 - i - 1);
		if (delta_x[i] > SCREEN_WIDTH) {
			delta_x[i] -= SCREEN_WIDTH;
		}
	}
}

void DrawBG3()
{
	for (int i = 0; i < 6; i++) {
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture3[i]);
		DrawSpriteAnim(XMFLOAT2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), 1, 1, 1);
		DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2 - delta_x[i], SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
		DrawSprite(XMFLOAT2(SCREEN_WIDTH / 2 - delta_x[i] + SCREEN_WIDTH, SCREEN_HEIGHT / 2), 0.f, XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
}
