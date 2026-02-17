//-------------------------------------------------------------
// coordinate.h
// 製作：笠原凜　日付：2024/11/01
//-------------------------------------------------------------

//--------------------------概要-------------------------------
// 座標クラスです。
// 座標そのもの、描画に必要なサイズ等が含まれます。
// これに対応した描画の関数も後で考えます
// 
// 座標等が3Dですが、演出のために深度をつけるだけなので基本2Dの認識で大丈夫です
// この辺は更新予定
//-------------------------------------------------------------

#ifndef _COORDINATE_H
#define _COORDINATE_H

#include "main.h"



class Coordinate {
	//一応全部アクセスできたほうが良いというか
	//支障のない形にした……
public:
	XMFLOAT3 m_Pos = {};							//座標
	XMFLOAT3 m_OldPos = {};
	XMFLOAT3 m_Vel = {};							//加速度
	XMFLOAT4 m_Color = { 1.0f,1.0f,1.0f,1.0f };		//色
	float m_Rotation = 0.0f;						//角度
	XMFLOAT2 m_Size = { 1.0f,1.0f };				//大きさ
	XMFLOAT2 m_Scale = { 1.0f,1.0f };				//大きさの倍率

	Coordinate() {};
	Coordinate(XMFLOAT3 pos, float rotation, XMFLOAT2 size, XMFLOAT2 scale)
	{
		m_Pos = m_OldPos = pos;
		m_Rotation = rotation;
		m_Size = size;
		m_Scale = scale;
	}
};



#endif // !_COORDINATE_H_