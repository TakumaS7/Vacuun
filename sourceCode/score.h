#pragma once

#include "main.h"

class Score
{
private:
	int m_Score = 000000;

public:
	void SetScore(int score) { m_Score = score; }
	int GetScore(void) { return m_Score; }

};

void InitializeScore();
void DrawScore();

void AddScore(int add);

Score* pGetScore();