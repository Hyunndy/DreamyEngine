////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Sound = 0;
	m_FPS = 0;
	m_Cpu = 0;
	m_Timer = 0;


	//2D이미지 켜고 끄는 BOOL 변수
	F1pressed = false;
	screenHeight = 0;
	screenWidth = 0;

	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mouseX = 0;
	mouseY = 0;
}

//왜 객체들의 포인터를 null로 초기화하냐면 초기화에 실패하면 곧바로 셧다운 함수를 호출하게 되는데 이 함수에서는 객체가 null이 아니라면 이를 올바르게 생성된 객체로 취급하고
//이 객체의 메모리를 정리해야 한다고 판단하고 실제로 정리를 수행하려 한다.

SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}
//파괴자에서 해제를 하지 않고 shutdown함수에서 해제 해준다.

/*--------------------------------------------------------------------------------------------------------------
이름 : Initialize()
용도 :
- 프로그램을 실행하자마자 실행되는 함수.
- Input, Graphics 객체를 초기화 한다.
---------------------------------------------------------------------------------------------------------------*/
bool SystemClass::Initialize()
{

	bool result;


	// 함수에 높이와 너비를 전달하기 전에 변수를 0으로 초기화한다.
	screenWidth = 0;
	screenHeight = 0;

	//윈도우즈 api를 사용하여 초기화한다.
	InitializeWindows(screenWidth, screenHeight);

	// Input객체 생성
	//-------------------------------------------------------------------------------------
	m_Input = new InputClass;
	if (!m_Input) {return false;}

	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if(!result) {MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------
	
	// Graphics 객체 생성
	//-------------------------------------------------------------------------------------
	m_Graphics = new GraphicsClass;
	if (!m_Graphics){return false;}

	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);

	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Grahpics object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// m_state상태 변경
	//--------------------------------------------------------------------------------------
	m_state = STATE::MAIN;

	return true;
}

/*--------------------------------------------------------------------------------------------------------------
이름 : Main()
용도 : 메인화면에서 일어나는 일들.
---------------------------------------------------------------------------------------------------------------*/
void SystemClass::Main()
{
	bool result;
	bool Start = false;

	while (Start==false)
	{
		result = m_Graphics->RenderMainScene();
		if (!result) { MessageBox(m_hwnd, L"Could not initialize the main scene object.", L"Error", MB_OK);  }

		// 키보드&마우스 상태를 갱신하도록 한다.
		result = m_Input->Frame();

		if (m_Input->IsF1Pressed() == true)
		{
			Start = true;
			m_state = STATE::LOADING;

		}

		if (m_Input->IsEscapePressed() == true)
		{
			Start = true;
		}
	}

	return;
}


/*--------------------------------------------------------------------------------------------------------------
이름 : Loading()
용도 : 
- 로딩화면에서 일어나는 일들.
- Graphics 객체에서 실제로 렌더링 되는 애들, FPS, CPU, Timer, Sound 객체를 초기화한다.
---------------------------------------------------------------------------------------------------------------*/
bool SystemClass::Loading()
{
	bool result;

	// Graphics 객체 생성
	//-------------------------------------------------------------------------------------
	result = m_Graphics->Loading(screenWidth, screenHeight, m_hwnd);
	//-------------------------------------------------------------------------------------

	// FPS 객체 생성
	//-------------------------------------------------------------------------------------
	m_FPS = new FpsClass;
	if (!m_FPS) { return false; }

	m_FPS->Initialize();
	//-------------------------------------------------------------------------------------

	// CPU 객체 생성
	//-------------------------------------------------------------------------------------
	m_Cpu = new CpuClass;
	if (!m_Cpu) { return false; }

	m_Cpu->Initialize();
	//-------------------------------------------------------------------------------------

	// Timer 객체 생성
	//-------------------------------------------------------------------------------------
	m_Timer = new TimerClass;
	if (!m_Timer) { return false; }

	m_Timer->Initialize();
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// Sound 객체 생성
	//-------------------------------------------------------------------------------------
	m_Sound = new SoundClass;
	if (!m_Sound) { return false; }

	result = m_Sound->Initialize(m_hwnd);
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Sound object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// move객체 생성
	//-------------------------------------------------------------------------------------
	m_Move = new MoveClass;
	if (!m_Move) { return false; }

	m_Move->SetPosition(946.0f, 56.0f, 464.0f);
	m_Move->SetRotation(0.0f, -50.0f, 0.0f);
	//-------------------------------------------------------------------------------------


	m_Input->SetMousePosition();
	m_state = STATE::RUNNING;

	return true;
}

// 객체를 정리한다. 
void SystemClass::Shutdown()
{
	// Release the timer object.
	if(m_Timer) { delete m_Timer; m_Timer = 0; }
	
	// Release the cpu object.
	if(m_Cpu) { m_Cpu->Shutdown(); delete m_Cpu; m_Cpu = 0; }
	
	// Release the fps object.
	if(m_FPS) { delete m_FPS; m_FPS = 0; } 

	if (m_Move) { delete m_Move; m_Move = 0; }
	if (m_Sound)
	{
		m_Sound->Shutdown();
		delete m_Sound;
		m_Sound = 0;
	}

	// Release the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

// 프로그램이 종료될 때 까지 루프를 돌면서 어플리케이션의 모든 작업을 처리한다.
// while 종료 되지 않는 동안 윈도우의 시스템 메세지를 확인, 메세지 처리, 어플리케이션의 작업, 유저가 작업중 프로그램의 종료를 원하는지 확인
void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();

			if (!result)
			{
				done = true;
			}

			// ESC키를 확인하는 함수
			if (m_Input->IsEscapePressed() == true)
			{
				done = true;
			}

		}
	}

	return;
}

bool SystemClass::HandleInput(float frametime)
{
	bool result;



	//매 프레임 마다 프레임 시간을 갱신한다.
	m_Move->SetFrameTime(frametime);





	//m_Move->TurnLeft(m_Input->IsLeftPressed());
	//m_Move->TurnRight(m_Input->IsRightPressed());
	m_Move->TurnLeft(m_Input->IsLeftTurned());
	m_Move->TurnRight(m_Input->IsRightTurned());
	m_Move->LookUpward(m_Input->IsLookUpTurned());
	m_Move->LookDownward(m_Input->IsLookDownTurned());
	m_Move->GoForward(m_Input->IsUpPressed());
	m_Move->GoBackward(m_Input->IsDownPressed());
	//m_Move->LookUpward(m_Input->IsLookUpPressed());
	//m_Move->LookDownward(m_Input->IsLookDownPressed());

	

	m_Move->GetPosition(pos);
	m_Move->GetRotation(rot);

	

	return true;
}
// 어플리케이션의 모든 작업은 매 루프 마다 불리는 Frame함수에서 수행된더.
bool SystemClass::Frame()
{
	bool keyDown,result;

	// 키보드&마우스 상태를 갱신하도록 한다.
	result = m_Input->Frame();
	if (!result) { return false; }

	m_Input->GetMouseLocation(mouseX, mouseY);

	//시스템 스탯을 업데이트 한다.
	m_Timer->Frame();
	m_FPS->Frame();
	m_Cpu->Frame();

	result = HandleInput(m_Timer->GetTime());
	if (!result) { return false; }

	// m_Graphics객체를 통해 화면에 그리는 작업을 수행한다.

	result = m_Graphics->Frame(m_FPS->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime(),pos, rot, mouseX, mouseY);
	//result = m_Graphics->Frame(m_FPS->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime(), pos, rot, W);
	if (!result){ return false; }
	
	//result = m_Graphics->Render(F1pressed);
	result = m_Graphics->Render(F1pressed, m_FPS->GetFps());
	if (!result) { return false; }

	result = m_Input->IsLeftMouseButtonDown();
	if (result == true)
	{	
		m_Graphics->CheckIntersection(mouseX, mouseY, screenWidth, screenHeight);
		//m_Graphics->TestIntersection(mouseX, mouseY, screenWidth, screenHeight);
	}



	return true;
}

// 윈도우의 시스템 메세지가 전달되는 곳.
// 현재 단지 키가 눌려있는지, 뗴어지는지를 알 수 있고 이 정보를 m_Input객체에 전달한다.

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
}

// InitializeWindows 함수에는 우리가 렌더링을 하게 될 윈도우를 만드는 코드가 들어간다.
// 이 함수는 호출한 함수에게로 screenWidth와 screenHeight 값을 다시 되돌려주므로 이 값을 어플리케이션에서 활용할 수 있다.

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// 외부 포인터를 이 객체로 설정한다.	
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스를 가져온다.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// 윈도우 클래스를 기본 설정으로 맞춘다.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스를 등록한다.
	RegisterClassEx(&wc);

	//모니터 화면의 해상도를 알아온다.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 풀스크린 모드 변수의 값에 따라 화면설정을 한다.
	if (FULL_SCREEN)
	{
		// 만약 풀스크린 모드라면 화면 크기를 데스크탑 크기에 맞추고 색상을 32bit로 한다.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린에 맞는 디스플레이 설정을 합니다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 윈도우의 위치를 화면의 왼쪽 위로 맞춘다.
		posX = posY = 0;
	}
	else
	{
		// 윈도우 모드라면 800x600으로 맞춘다.
		screenWidth = 1280;
		screenHeight = 800;

		// 창을 화면의 중간에 오게한다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 설정한 것을 가지고 창을 만들고 그 핸들을 가져온다.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 윈도우를 화면에 표시하고 포커스를 준다.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서를 표시 유/무
	ShowCursor(false);

	return;
}

// 화면 설정을 되돌리고 윈도우와 그 핸들을 반환한다.

void SystemClass::ShutdownWindows()
{
	// 마우스 커서를 표시한다.
	ShowCursor(true);

	// 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꾼다.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거한다.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 어플리케이션 인스턴스를 제거한다.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 이 클래스에 대한 외부 포인터 참조를 제거한다.
	ApplicationHandle = NULL;

	return;
}

// WndProc 함수는 윈도우시스템에서 메세지를 보내는 곳이다.
// MessageHandler에서 처리할 메세지를 받아야하기 때문에 WndProc함수도 이 클래스 파일안에 선언했다.
// SystemClass에서 윈도우의 메세지를 가로채는것을 가능케 한다.

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 키보드가 키가 눌렸는지 확인한다.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 윈도우가 닫히는지 확인한다.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 다른 모든메세지들은 system 클래스의 메세지 처리기에 전달한다.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}