////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;

	a = true;
	
	m_frameTime = 0;
	m_rotationY = 0;
	m_leftTurnSpeed = 0;
	m_rightTurnSpeed = 0;

	m_Speed = 0;
	m_ForwardSpeed = 0;
	m_backwardSpeed = 0;
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
		a = true;
	}
	if (m_keyboardState[DIK_F2] & 0X80)
	{
		a = false;
	}
	
	return a;	
	//  어플리케이션에서 필요한 다른 키들을 확인하는 함수를 만들 수 있다.

}

/*------------------------------------------------------------------------------------------------------------------
이름 : SetFrameTime()
용도 :
- 프레임 속도를 설정하기 위해 사용된다.
- 이 프레임 시간을 사용하여 카메라가 얼마나 빠르게 움직이고 회전하는지 계산한다.
- 카메라가 움직이기 전에 매 프레임의 시작에 불리도록 해야한다.
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}
/*------------------------------------------------------------------------------------------------------------------
이름 : GetRotation()
용도 :
- 카메라의 Y축 회전 정도를 리턴한다.
- 나중에 더 많은 정보를 알려주도록 확장될 수 있다.
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::GetRotation(float& y)
{
	y = m_rotationY;
	return;
}
void InputClass::GetSpeed(float& y)
{
	y = m_Speed;
	return;
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

/*------------------------------------------------------------------------------------------------------------------
이름 : Turn()
용도 :
- 움직임 관련 함수, 매 프레임 마다 호출된다.
- 키가 눌린다면 매 프레임 마다 최고 속도까지 해당 방향으로 가속된다.
- 키를 떼면 매 프레임 마다 속도가 일정한 비율로 부드럽게 줄어들어 0이 된다.
- 프레임간 시간에 기초해 있기 때문에 fps값과 상관없이 동일하게 움직인다(cpu에 종속이 아니다)
--------------------------------------------------------------------------------------------------------------------*/
void InputClass::TurnLeft(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if(keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;
		
		if(m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f; 
		} 
	}
	
	else
	{ 
		m_leftTurnSpeed -= m_frameTime* 0.005f;
	
		if(m_leftTurnSpeed < 0.0f)
		{ 
			m_leftTurnSpeed = 0.0f;
		}
	} 
	
	// Update the rotation using the turning speed.
	m_rotationY -= m_leftTurnSpeed; 
//	
//	if(m_rotationY < 0.0f)
//	{ 
//		m_rotationY += 360.0f;
//	}
//

	return;
}

void InputClass::TurnRight(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns right. If not slow down the turn speed.
	if(keydown) 
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;
		
		if(m_rightTurnSpeed > (m_frameTime * 0.15f))
		
		{ 
			m_rightTurnSpeed = m_frameTime * 0.15f;
		} 
	} 
	
	else
	{ 
		m_rightTurnSpeed -= m_frameTime* 0.005f;
		if(m_rightTurnSpeed < 0.0f)
		{ 
			m_rightTurnSpeed = 0.0f;
		}
	}
	
	// Update the rotation using the turning speed.
	m_rotationY += m_rightTurnSpeed;
//if(m_rotationY > 360.0f)
//{ 
//	m_rotationY -= 360.0f;
//} 
//
	return;
}

void InputClass::GoForward(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if (keydown)
	{
		m_ForwardSpeed += m_frameTime * 0.01f;

		if (m_ForwardSpeed > (m_frameTime * 0.15f))
		{
			m_ForwardSpeed = m_frameTime * 0.15f;
		}
	}

	else
	{
		m_ForwardSpeed -= m_frameTime* 0.005f;

		if (m_ForwardSpeed < 0.0f)
		{
			m_ForwardSpeed = 0.0f;
		}
	}

	m_Speed += m_ForwardSpeed;

	return;
}

void InputClass::GoBackward(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left. If not slow down the turn speed.
	if (keydown)
	{
		m_backwardSpeed += m_frameTime * 0.01f;

		if (m_backwardSpeed  > (m_frameTime * 0.15f))
		{
			m_backwardSpeed = m_frameTime * 0.15f;
		}
	}

	else
	{
		m_backwardSpeed -= m_frameTime* 0.005f;

		if (m_backwardSpeed  < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	m_Speed -= m_backwardSpeed;

	return;
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

