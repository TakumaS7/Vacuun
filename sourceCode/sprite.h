#ifndef _SPRITE_H
#define _SPRITE_H

#include "main.h"
#include "renderer.h"
#include "coordinate.h"
#include "video_texture.h"

//マクロ定義

//関数定義
void InitSprite(void);
void UninitSprite(void);
void DrawSprite(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale);
void DrawSpriteColor(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, XMFLOAT4 Color);
void DrawSprite(Coordinate c);
void DrawSpriteAnim(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, int AnimCols, int AnimRows, int AnimPattern);
void DrawSpriteAnim(Coordinate c, int AnimCols, int AnimRows, int AnimPattern);
void DrawSpriteAnimColor(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, int AnimCols, int AnimRows, int AnimPattern, XMFLOAT4 Color);
void SetIsShake(bool b);
void SetShake(float t);
void DrawVideoSprite(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, VideoTexture video);
void DrawVideoSpriteChromaKey(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, VideoTexture video);

#endif//_SPRITE_H