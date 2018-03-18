////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;

	a = false;

}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}

/*------------------------------------------------------------------------------------------------------------------
�̸� : Initialize()
�뵵 :
- ��ũ�� �ʺ�, ���콺 ������ ��ġ �ʱ�ȭ
- DirectInput �������̽� �ʱ�ȭ(Create) -> DirectInput ��ü�� ��� �Ǹ� �ٸ� �Է���ġ���� �ʱ�ȭ�� �� �ִ�.

- 1.DirectInput �������̽� �ʱ�ȭ
- 2.Ű���� �ʱ�ȭ
- 3.���콺 �ʱ�ȭ

- Ű����&���콺 ���� ����(cooperative level)�� ����: �� ��ġ�� ������ �ϰ�, ��� ���ɰ������� �����Ѵ�.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// ���콺 Ŀ���� �����͸� ����ϱ� ���� ��ũ�� �ʺ� �����Ѵ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = 800;
	m_mouseY = 450;

	// DirectInput�� �������̽��� �ʱ�ȭ�Ѵ�.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) { return false; }

	// Ű���� �ʱ�ȭ
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) { return false; }


	// Ű������ ������ ������ �����Ѵ�.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result)) { return false; }

	/*
	Ű����&���콺�� ���·���(cooperative level)����
	- �� ��ġ�� ������ �ϰ�, ��� ���ɰ������� �����Ѵ�.
	- DISCL_EXCLUSIVE : ���� ����. �ٸ� ���α׷���� �� ��ġ�� �������� �ʴ´�. ������ �� APP�� �Է��� ������ �� �ִ�.
	- DISCL_NONEXCLUSIVE : �� ���� ����. print screen key�� ��ũ�� ���� ���� �� �ְ� �ٸ� ���α׷������� Ű������ �Է��� ������ �� �ִ�.
	- ���� �� ���� �����̰� Ǯ��ũ���� �ƴ϶��, ��Ŀ���� ���� �ʱ� ���� �����ؾ��Ѵ�.
	*/
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) { return false; }

	// Ű���尡 ���õǸ� Acquire�Լ��� �� �����ͷ� Ű���忡 ���� ������ ����Ѵ�.
	result = m_keyboard->Acquire();
	if (FAILED(result)) { return false; }

	// ���콺 �ʱ�ȭ
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) { return false; }

	// ���콺�� ������ ������ �����Ѵ�.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) { return false; }

	// ���콺 ���·��� ����
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) { return false; }

	result = m_mouse->Acquire();
	if (FAILED(result)) { return false; }

	return true;
}


/*------------------------------------------------------------------------------------------------------------------
�̸� : Shutdown()
�뵵 :
- Ű����, ���콺, DirectInput�� �����Ѵ�
- ��ġ�� ������ Unacquire->Release������ �̷������.
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

/*------------------------------------------------------------------------------------------------------------------
�̸� : Frame()
�뵵 :
- ��ġ�� ���� ���¸� �о� �ռ� ���� ���� ���ۿ� ����Ѵ�.
// ���� Ű����� ���콺 ���¸� ����ϴ� ����
unsigned char m_keyboardState[256];
DIMOUSESTATE m_mouseState;
- �� ��ġ�� ���¸� ���� �� ��������� ó���ϰ� �Ѵ�.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::Frame()
{
	bool result;

	// Ű������ ���� ���¸� m_keyboardState�� �����Ѵ�.
	result = ReadKeyboard();
	if (!result) { return false; }

	// ���콺�� ���� ���¸� m_mouseState�� �����Ѵ�.
	result = ReadMouse();
	if (!result) { return false; }

	// ���콺�� ������ �̵��� ó���Ѵ�.
	ChangeInputperFrame();

	return true;
}
/*------------------------------------------------------------------------------------------------------------------
�̸� : ReadKeyboard()
�뵵 :
- Ű������ ���¸� Ű���� ���� ���� m_keyboardState������ �����Ѵ�.
- �� ���� ������ ��� Ű�� ���� ���ȴ��� �ȴ��ȴ��� �����ش�.

- Ű������ ���¸� �дµ� �����Ѵٸ� 1. Ű���尡 ��Ŀ���� �Ҵ´�. 2. ��� �Ұ� ���� �� ��� �̴�.
- �� ������ �ذ��Ϸ��� �� �����Ӹ��� ������� �������� �� ���� Acquire()�Լ��� ȣ���ؾ� �Ѵ�.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::ReadKeyboard()
{
	HRESULT result;

	//Ű������ ���¸� m_keyboardState�� �����Ѵ�.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_keyboard->Acquire();
		else
			return false;
	}

	return true;
}

/*------------------------------------------------------------------------------------------------------------------
�̸� : ReadMouse()
�뵵 :
- ���콺�� ���¸� m_MouseState�� �����Ѵ�.
- ���콺�� Ű����� �޸� ���� �����Ӱ� �޶��� ��ġ(����)���� �����Ѵ�.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::ReadMouse()
{
	HRESULT result;

	//���콺�� ���¸� m_mouseState�� �����Ѵ�.
	result = m_mouse->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_mouse->Acquire();
		else
			return false;
	}

	return true;
}

/*------------------------------------------------------------------------------------------------------------------
�̸� : ChangeInputperFrame
�뵵 :
- ���� �����ӿ����� ��������� ������ ����Ǵ� ��.
- ������ ��ġ �̵�.
- ����� ���콺�� ��ġ�� �����ϰ� �Ѵ�.
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::ChangeInputperFrame()
{
	// ������ ���� ���콺�� ��ġ�� ��ȭ�� ���� ���콺 Ŀ���� ��ġ�� ������Ʈ ��Ų��.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// ���콺Ŀ���� ȭ�� �ٱ��� �Ѿ�� �ʰ� �ؾ��Ѵ�.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}
/*------------------------------------------------------------------------------------------------------------------
�̸� : IsEscapePressed()
�뵵 :
- esc��ư�� ���ȴ��� Ȯ���Ѵ�.
- �� �Լ��� ���� Ư�� Ű�� ���ȴ��� �ȴ��ȴ��� �� �� �ִ�.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::IsEscapePressed()
{
	// Ư�� Ű�� ���ȴ��� Ȯ���ϴ� ���.
	if (m_keyboardState[DIK_ESCAPE] & 0X80)
		return true;

	//  ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� �� �ִ�.
	return false;
}

/*------------------------------------------------------------------------------------------------------------------
�̸� : IsF1Pressed()
�뵵 :
- F1��ư�� ���ȴ��� Ȯ���Ѵ�.
- �� �Լ��� ���� Ư�� Ű�� ���ȴ��� �ȴ��ȴ��� �� �� �ִ�.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::IsF1Pressed()
{

	// Ư�� Ű�� ���ȴ��� Ȯ���ϴ� ���.
	if (m_keyboardState[DIK_F1] & 0X80)
	{
		return true;
	}
	
	return false;	
	//  ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� �� �ִ�.

}


bool InputClass::IsLeftPressed()
{
	// Ư�� Ű�� ���ȴ��� Ȯ���ϴ� ���.
	if (m_keyboardState[DIK_LEFT] & 0X80 || m_keyboardState[DIK_A] & 0X80)
		return true;
		
	//  ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� �� �ִ�.
	return false;
}
bool InputClass::IsRightPressed()
{
	// Ư�� Ű�� ���ȴ��� Ȯ���ϴ� ���.
	if (m_keyboardState[DIK_RIGHT] & 0X80 || m_keyboardState[DIK_D] & 0X80)
		return true;

	//  ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� �� �ִ�.
	return false;
}

bool InputClass::IsUpPressed()
{
	// Ư�� Ű�� ���ȴ��� Ȯ���ϴ� ���.
	if (m_keyboardState[DIK_UP] & 0X80 || m_keyboardState[DIK_W] & 0X80)
		return true;

	//  ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� �� �ִ�.
	return false;
}

bool InputClass::IsDownPressed()
{
	// Ư�� Ű�� ���ȴ��� Ȯ���ϴ� ���.
	if (m_keyboardState[DIK_DOWN] & 0X80 || m_keyboardState[DIK_S] & 0X80)
		return true;

	//  ���ø����̼ǿ��� �ʿ��� �ٸ� Ű���� Ȯ���ϴ� �Լ��� ���� �� �ִ�.
	return false;
}

bool InputClass::IsLookUpPressed()
{
	if (m_keyboardState[DIK_ADD] & 0X80)
		return true;
	return false;
}

bool InputClass::IsLookDownPressed()
{
	if (m_keyboardState[DIK_SUBTRACT] & 0X80)
		return true;
	return false;
}

/*------------------------------------------------------------------------------------------------------------------
�̸� : GetMouseLocation()
�뵵 :
- ���� ���콺�� ��ġ�� �˷��ش�.
- GraphicsClass�� �� ������ ���� TextClass���� ���콺�� x,y��ǥ�� �׸��� �Ѵ�.
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

bool InputClass::IsLeftTurned()
{
	//if (m_mouseX >= 0 && m_mouseX < (m_screenWidth/4))
	if(m_mouseX <=0)
		return true;
	else
		return false;
}

bool InputClass::IsRightTurned()
{
	//if (m_mouseX > (m_screenWidth/4)*3 && m_mouseX <= m_screenWidth)
	if(m_mouseX>=m_screenWidth)
		return true;
	else
		return false;
}

bool InputClass::IsLookUpTurned()
{
	if (m_mouseY == 0)
		return true;
	else
		return false;
}

bool InputClass::IsLookDownTurned()
{
	if (m_mouseY == m_screenHeight)
		return true;
	else
		return false;
}



bool InputClass::IsLeftMouseButtonDown()
{
	// Check if the left mouse button is currently pressed.
	if (m_mouseState.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}