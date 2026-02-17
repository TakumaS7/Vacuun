#include "frametime.h"

float GameFrameTime::deltaTime = 0;

void GameFrameTime::SetElapsedTime(float time) {
	deltaTime = time;
}

float GameFrameTime::GetElapsedTime() {
	return deltaTime;
}