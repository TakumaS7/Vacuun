#include "score.h"
#include "texture.h"
#include "sprite.h"
#include "player.h"

static ID3D11ShaderResourceView* g_Texture;
Score g_Score;


void InitializeScore()
{
	g_Texture = InitTexture(L"asset\\texture\\UI_NUMBER_SCORE_01.png");
	g_Score.SetScore(0);
}

void DrawScore()
{
	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

	//プレイヤーと重なったときに透過
	if (GetPlayer()->m_Coordinate.m_Pos.x > SCREEN_WIDTH - 400 &&
		GetPlayer()->m_Coordinate.m_Pos.y < 100)
	{
		color = { 1.0f,1.0f,1.0f,0.5f };
	}

	//スコアカンスト
	if (g_Score.GetScore() > 999999) { g_Score.SetScore(999999); }

	int score = g_Score.GetScore();

	for (int i = 0; i < 6; i++)
	{
		int Num = g_Score.GetScore() % 10;

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);
		DrawSpriteAnimColor({ (SCREEN_WIDTH - 40.0f - (60.0f * i)), 40.0f }, 0.0f, { 60.0f, 60.0f }, 8, 2, Num, color);

		g_Score.SetScore(g_Score.GetScore() / 10);
	}

	g_Score.SetScore(score);
}

void AddScore(int add)
{
       	g_Score.SetScore(g_Score.GetScore() + add);
}

Score* pGetScore()
{
	return &g_Score;
}


