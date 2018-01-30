////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <dinput.h>


/*
	18.01.26 카메라를 왼쪽/오른쪽 화살표 키로 조작할 수 있게한다.

*/

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	bool a;

public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed(); // ESC버튼 눌렸는지 안눌렸는지
	bool IsF1Pressed(); // F1버튼 눌렸는지 안눌렸는지
	void GetMouseLocation(int&, int&);

	void SetFrameTime(float);
	void GetRotation(float&);
	void GetSpeed(float&);

	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();

	void TurnLeft(bool keydown);
	void TurnRight(bool keydown);
	void GoForward(bool keydown);
	void GoBackward(bool keydown);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ChangeInputperFrame();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

	float m_frameTime;
	float m_rotationY;
	float m_Speed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_ForwardSpeed, m_backwardSpeed;
};

#endif