//-------------------------------------------------------------
// bulletType.h
// 製作：笠原凜　日付：2024/11/01
//-------------------------------------------------------------


//--------------------------概要-------------------------------
// 弾の素材になれるやつらが持つ、属性値です。
// 仮なのでＡＢＣという名称があります。それぞれの区別はないので、好きに判別してください。
// 
// 弾の素材はこの数値を内部に持ちます。
//-------------------------------------------------------------


#ifndef _BULLETTYPE_H
#define _BULLETTYPE_H

#include "main.h"

class BulletType {
public:
	float typeA = 0.0f;
	float typeB = 0.0f;
	float typeC = 0.0f;

	BulletType(){}

	BulletType(float a, float b, float c)
	{
		typeA = a;
		typeB = b;
		typeC = c;
	}

	~BulletType() {}

	BulletType operator+(const BulletType& r)
	{
		return{ typeA + r.typeA,typeB + r.typeB,typeC + r.typeC };
	}

	BulletType operator*(const BulletType& r)
	{
		return{ typeA * r.typeA,typeB * r.typeB,typeC * r.typeC };
	}
};


#endif // !_BULLETTYPE_H_