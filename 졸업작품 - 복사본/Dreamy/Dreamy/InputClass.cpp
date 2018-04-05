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
이름 : Initialize()
용도 :
- 스크린 너비, 마우스 포인터 위치 초기화
- DirectInput 인터페이스 초기화(Create) -> DirectInput 객체를 얻게 되면 다른 입력장치들을 초기화할 수 있다.

- 1.DirectInput 인터페이스 초기화
- 2.키보드 초기화
- 3.마우스 초기화

- 키보드&마우스 협력 레벨(cooperative level)을 지정: 이 장치가 무엇을 하고, 어떻게 사용될것인지를 결정한다.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// 마우스 커서의 포인터를 사용하기 위해 스크린 너비를 저장한다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = 0;
	m_mouseY = 0;


	// DirectInput의 인터페이스를 초기화한다.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) { return false; }

	// 키보드 초기화
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) { return false; }


	// 키보드의 데이터 포맷을 지정한다.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result)) { return false; }

	/*
	키보드&마우스의 협력레벨(cooperative level)지정
	- 이 장치가 무엇을 하고, 어떻게 사용될것인지를 결정한다.
	- DISCL_EXCLUSIVE : 배제 상태. 다른 프로그램들과 이 장치를 공유하지 않는다. 오로지 이 APP만 입력을 감지할 수 있다.
	- DISCL_NONEXCLUSIVE : 비 배제 상태. print screen key로 스크린 샷을 찍을 수 있고 다른 프로그램에서도 키보드의 입력을 감지할 수 있다.
	- 만일 비 배제 상태이고 풀스크린이 아니라면, 포커스를 잃지 않기 위해 조심해야한다.
	*/
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) { return false; }

	// 키보드가 세팅되면 Acquire함수로 이 포인터로 키보드에 대한 접근을 취득한다.
	result = m_keyboard->Acquire();
	if (FAILED(result)) { return false; }

	// 마우스 초기화
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) { return false; }

	// 마우스의 데이터 포맷을 지정한다.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) { return false; }

	// 마우스 협력레벨 지정
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) { return false; }

	result = m_mouse->Acquire();
	if (FAILED(result)) { return false; }

	return true;
}


/*------------------------------------------------------------------------------------------------------------------
이름 : Shutdown()
용도 :
- 키보드, 마우스, DirectInput을 해제한다
- 장치의 해제는 Unacquire->Release순으로 이루어진다.
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
이름 : Frame()
용도 :
- 장치의 현재 상태를 읽어 앞서 만든 상태 버퍼에 기록한다.
// 현재 키보드와 마우스 상태를 기록하는 변수
unsigned char m_keyboardState[256];
DIMOUSESTATE m_mouseState;
- 각 장치의 상태를 읽은 뒤 변경사항을 처리하게 한다.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::Frame()
{
	bool result;

	// 키보드의 현재 상태를 m_keyboardState에 저장한다.
	result = ReadKeyboard();
	if (!result) { return false; }

	// 마우스의 현재 상태를 m_mouseState에 저장한다.
	result = ReadMouse();
	if (!result) { return false; }

	// 마우스의 물리적 이동을 처리한다.
	ChangeInputperFrame();

	return true;
}
/*------------------------------------------------------------------------------------------------------------------
이름 : ReadKeyboard()
용도 :
- 키보드의 상태를 키보드 상태 버퍼 m_keyboardState변수에 저장한다.
- 이 상태 변수는 모든 키에 대해 눌렸는지 안눌렸는지 보여준다.

- 키보드의 상태를 읽는데 실패한다면 1. 키보드가 포커스를 잃는다. 2. 취득 불가 상태 인 경우 이다.
- 이 경우들을 해결하려면 매 프레임마다 제어권을 돌려받을 때 까지 Acquire()함수를 호출해야 한다.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::ReadKeyboard()
{
	HRESULT result;

	//키보드의 상태를 m_keyboardState에 저장한다.
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
이름 : ReadMouse()
용도 :
- 마우스의 상태를 m_MouseState에 저장한다.
- 마우스는 키보드와 달리 이전 프레임과 달라진 위치(변위)만을 저장한다.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::ReadMouse()
{
	HRESULT result;

	//마우스의 상태를 m_mouseState에 저장한다.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);

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
이름 : ChangeInputperFrame
용도 :
- 이전 프레임에서의 변경사항이 실제로 적용되는 곳.
- 물리적 위치 이동.
- 현재는 마우스의 위치를 갱신하게 한다.
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::ChangeInputperFrame()
{
	// 프레임 마다 마우스의 위치의 변화에 따른 마우스 커서의 위치를 업데이트 시킨다.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 마우스커서가 화면 바깥을 넘어가지 않게 해야한다.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}
/*------------------------------------------------------------------------------------------------------------------
이름 : IsEscapePressed()
용도 :
- esc버튼이 눌렸는지 확인한다.
- 이 함수를 통해 특정 키가 눌렸는지 안눌렸는지 알 수 있다.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::IsEscapePressed()
{
	// 특정 키가 눌렸는지 확인하는 방법.
	if (m_keyboardState[DIK_ESCAPE] & 0X80)
		return true;

	//  어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수 있다.
	return false;
}

/*------------------------------------------------------------------------------------------------------------------
이름 : IsF1Pressed()
용도 :
- F1버튼이 눌렸는지 확인한다.
- 이 함수를 통해 특정 키가 눌렸는지 안눌렸는지 알 수 있다.
--------------------------------------------------------------------------------------------------------------------*/
bool InputClass::IsF1Pressed()
{

	// 특정 키가 눌렸는지 확인하는 방법.
	if (m_keyboardState[DIK_F1] & 0X80)
	{
		return true;
	}
	
	return false;	
	//  어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수 있다.

}


bool InputClass::IsLeftPressed()
{
	// 특정 키가 눌렸는지 확인하는 방법.
	if (m_keyboardState[DIK_LEFT] & 0X80 || m_keyboardState[DIK_A] & 0X80)
		return true;
		
	//  어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수 있다.
	return false;
}
bool InputClass::IsRightPressed()
{
	// 특정 키가 눌렸는지 확인하는 방법.
	if (m_keyboardState[DIK_RIGHT] & 0X80 || m_keyboardState[DIK_D] & 0X80)
		return true;

	//  어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수 있다.
	return false;
}

bool InputClass::IsUpPressed()
{
	// 특정 키가 눌렸는지 확인하는 방법.
	if (m_keyboardState[DIK_UP] & 0X80 || m_keyboardState[DIK_W] & 0X80)
		return true;

	//  어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수 있다.
	return false;
}

bool InputClass::IsDownPressed()
{
	// 특정 키가 눌렸는지 확인하는 방법.
	if (m_keyboardState[DIK_DOWN] & 0X80 || m_keyboardState[DIK_S] & 0X80)
		return true;

	//  어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수 있다.
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
이름 : GetMouseLocation()
용도 :
- 현재 마우스의 위치를 알려준다.
- GraphicsClass는 이 정보를 통해 TextClass에게 마우스의 x,y좌표를 그리게 한다.
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

void InputClass::SetMousePosition()
{
	m_mouseX = 640;
	m_mouseY = 400;
}

bool InputClass::IsLeftTurned()
{
	//if (m_mouseX >= 0 && m_mouseX < (m_screenWidth/4))
	//if (m_mouseX <= 440.0f) 800
	if(m_mouseX <= (m_screenWidth/3))
		return true;
	else
		return false;
}

bool InputClass::IsRightTurned()
{
	//if (m_mouseX > (m_screenWidth/4)*3 && m_mouseX <= m_screenWidth)
	if(m_mouseX >= (m_screenWidth / 3)*2)
		return true;
	else
		return false;
}

bool InputClass::IsLookUpTurned()
{
	if (m_mouseY <= (m_screenHeight / 4))
		return true;
	else
		return false;
}

bool InputClass::IsLookDownTurned()
{
	
	if (m_mouseY >= (m_screenHeight/4)*3)
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
