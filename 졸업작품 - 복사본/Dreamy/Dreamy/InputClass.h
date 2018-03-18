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
#include <d3d11.h>
#include <d3dx10math.h>
#include <dinput.h>


/*
	18.01.26 ī�޶� ����/������ ȭ��ǥ Ű�� ������ �� �ְ��Ѵ�.

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

	bool IsEscapePressed(); // ESC��ư ���ȴ��� �ȴ��ȴ���
	bool IsF1Pressed(); // F1��ư ���ȴ��� �ȴ��ȴ���
	void GetMouseLocation(int&, int&);


	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsLookUpPressed();
	bool IsLookDownPressed();

	bool IsLeftTurned();
	bool IsRightTurned();
	bool IsLookUpTurned();
	bool IsLookDownTurned();

	bool IsLeftMouseButtonDown();


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
	D3DXVECTOR2 Mousepos;

};

#endif