// ===================================================
// collision.h 当たり判定
// 
// 制作者：				日付：2024
// ===================================================
#ifndef _COLLISION_H_
#define _COLLISION_H_

// ===================================================
// プロトタイプ宣言
// ===================================================
//bool CheckBoxCollider(Float2 PosA, Float2 PosB, Float2 SizeA, Float2 SizeB);	// バウンディングボックスの当たり判定
//bool CheckCircleCollider(Float2 PosA, Float2 PosB, float rA, float rB);		// バウンディングサークルの当たり判定

#include "coordinate.h"
bool CheckCircleCollider(XMFLOAT3 PosA, XMFLOAT3 PosB, float rA, float rB);
bool CheckCircleCollider(Coordinate objA, Coordinate objB);

#endif