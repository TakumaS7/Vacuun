#pragma once
/*****************************************************************/
//
// 
// controller.h（コントローラー処理）
//
//
//
//                    2024/11/01　製作者名：佐藤琢磨
//
/*****************************************************************/
#pragma comment(lib,"xinput.lib")
#include <Xinput.h>
#include "main.h"

enum
{
	BOTTAN_MAX = 0,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT_SHOULDER,
	BUTTON_RIGHT_SHOULDER,
	BUTTON_A,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y
};

class Controller
{
private:
	FLOAT RightX = 0.0f;
	FLOAT RightY = 0.0f;
	FLOAT LeftX = 0.0f;
	FLOAT LeftY = 0.0f;

	int m_state = 0;
	int m_OldState = 0;

public:
	Controller() {};
	Controller(FLOAT rx, FLOAT ry, FLOAT lx, FLOAT ly, int s, int os)
		: RightX(rx), RightY(ry), LeftX(lx), LeftY(ly), m_state(s), m_OldState(os) {};
	~Controller() {};

	void GetControllor(); //使っていない

	FLOAT RStickX();
	FLOAT RStickY();
	FLOAT LStickX();
	FLOAT LStickY();

	bool ControllerDownTrigger(int button);
	bool ControllerDown(int button);
	void ButtonCopy();
};