#pragma once

#include <xaudio2.h>

struct AUDIO
{
	IXAudio2SourceVoice* SourceVoice{};
	BYTE* SoundData{};

	int						Length{};
	int						PlayLength{};
};

void InitAudio();
void UninitAudio();

int LoadAudio(const char* FileName);
void UnloadAudio(int Index);
void PlayAudio(int Index, bool Loop = false);
