/*****************************************************************/
//
// 
// controller.cpp（コントローラー処理）
//
//
//
//                    2024/11/01　製作者名：佐藤琢磨
//
/*****************************************************************/
#include "main.h"
#include "controller.h"

void Controller::GetControllor()
{
	//ゲームパッド情報を取得
	XINPUT_STATE state{};
	XInputGetState(0, &state);
	int iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
	int iPad_LeftShoulder = 0, iPad_RightShoulder = 0;
	int iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)		iPad_left = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)		iPad_right = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)		iPad_up = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)		iPad_down = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)	iPad_LeftShoulder = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)	iPad_RightShoulder = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)				iPad_A = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)				iPad_B = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)				iPad_X = 1;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)				iPad_Y = 1;
}

//右スティックのX軸の傾き度合を返す値（-32768～32767）
FLOAT Controller::RStickX()
{
	//ゲームパッド情報を取得
	XINPUT_STATE state{};
	XInputGetState(0, &state);

	//ゲームパッド右アナログスティックのデッドゾーン処理
	if (state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state.Gamepad.sThumbRX = 0;
	}

	//右スティックのX軸の数値をFLOATで代入
	Controller::RightX = static_cast <FLOAT>(state.Gamepad.sThumbRX); //static_cast：型を変えるもの
	

	return Controller::RightX;
}

//右スティックのY軸の傾き度合を返す値（-32768～32767）
FLOAT Controller::RStickY()
{
	//ゲームパッド情報を取得
	XINPUT_STATE state{};
	XInputGetState(0, &state);

	//ゲームパッド右アナログスティックのデッドゾーン処理
	if (state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state.Gamepad.sThumbRY = 0;
	}

	//右スティックのY軸の数値をFLOATで代入
	Controller::RightY = static_cast <FLOAT>(state.Gamepad.sThumbRY);

	return Controller::RightY;
}

//左スティックのX軸の傾き度合を返す値（-32768～32767）
FLOAT Controller::LStickX()
{
	//ゲームパッド情報を取得
	XINPUT_STATE state{};
	XInputGetState(0, &state);

	//ゲームパッド左アナログスティックXのデッドゾーン処理
	if (state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		state.Gamepad.sThumbLX = 0;
	}

	//左スティックのX軸の数値をFLOATで代入
	Controller::LeftX = static_cast <FLOAT>(state.Gamepad.sThumbLX);

	return Controller::LeftX;
}

//左スティックのY軸の傾き度合を返す値（-32768～32767）
FLOAT Controller::LStickY()
{
	//ゲームパッド情報を取得
	XINPUT_STATE state{};
	XInputGetState(0, &state);

	//ゲームパッド左アナログスティックYのデッドゾーン処理
	if (state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		state.Gamepad.sThumbLY = 0;
	}

	//左スティックのY軸の数値をFLOATで代入
	Controller::LeftY = static_cast <FLOAT>(state.Gamepad.sThumbLY);

	return Controller::LeftY;
}

bool Controller::ControllerDownTrigger(int button)
{
	//ゲームパッド情報を取得
	XINPUT_STATE state{};
	XInputGetState(0, &state);

	ButtonCopy(); //OldStateに前回の入力を保存する

	Controller::m_state = 0; //一回ボタンを押した後に初期化する

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)		Controller::m_state = BUTTON_LEFT;				// ゲームパッド十字キー左
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)		Controller::m_state = BUTTON_RIGHT;				// ゲームパッド十字キー右
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)		Controller::m_state = BUTTON_UP;				// ゲームパッド十字キー上
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)		Controller::m_state = BUTTON_DOWN;				// ゲームパッド十字キー下
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)	Controller::m_state = BUTTON_LEFT_SHOULDER;		// ゲームパッドLトリガー
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)	Controller::m_state = BUTTON_RIGHT_SHOULDER;	// ゲームパッドRトリガー
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)				Controller::m_state = BUTTON_A;					// ゲームパッドA
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)				Controller::m_state = BUTTON_B;					// ゲームパッドB
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)				Controller::m_state = BUTTON_X;					// ゲームパッドX
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)				Controller::m_state = BUTTON_Y;					// ゲームパッドY

	if (button == Controller::m_state && 
		Controller::m_state != Controller::m_OldState) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool Controller::ControllerDown(int button)
{
	//ゲームパッド情報を取得
	XINPUT_STATE state{};
	XInputGetState(0, &state);

	Controller::m_state = 0; //一回ボタンを押した後に初期化する

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)		Controller::m_state = BUTTON_LEFT;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)		Controller::m_state = BUTTON_RIGHT;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)		Controller::m_state = BUTTON_UP;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)		Controller::m_state = BUTTON_DOWN;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)	Controller::m_state = BUTTON_LEFT_SHOULDER;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)	Controller::m_state = BUTTON_RIGHT_SHOULDER;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)				Controller::m_state = BUTTON_A;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)				Controller::m_state = BUTTON_B;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)				Controller::m_state = BUTTON_X;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)				Controller::m_state = BUTTON_Y;

	if (button == Controller::m_state)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Controller::ButtonCopy()
{
	Controller::m_OldState = Controller::m_state; //前回の入力を保存
}


