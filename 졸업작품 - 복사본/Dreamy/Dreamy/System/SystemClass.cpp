////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SystemClass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Sound = 0;
	//m_FPS = 0;
	//m_Cpu = 0;
	//m_Timer = 0;


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

	//// Sound 객체 생성
	//-------------------------------------------------------------------------------------
	m_Sound = new SoundClass;
	if (!m_Sound) { return false; }

	result = m_Sound->Initialize(m_hwnd);
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Sound object.", L"Error", MB_OK); return false; }
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
	int Mute = 0;
	bool PressedF2 = false;
	bool sibal = false;
	bool Start = false;

	m_Input->SetMousePosition();

	//메인사운드 출력
	result = m_Sound->PlayWaveFile(1);
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the main sound.", L"Error", MB_OK); }

	while (Start==false)
	{
		// 키보드&마우스 상태를 갱신하도록 한다.
		result = m_Input->Frame();
		m_Input->GetMouseLocation(mouseX, mouseY);

		//메인화면 출력
		result = m_Graphics->RenderMainScene(mouseX, mouseY);
		if (!result) { MessageBox(m_hwnd, L"Could not initialize the main scene object.", L"Error", MB_OK); }

		//if (m_Input->IsLeftMouseButtonDown() && (mouseX >= 1050.0f) && (mouseX <= 1160.0f) && (mouseY >= 800.0f) && (mouseY <= 495.0f))
		//{
		//	m_state = STATE::LOADING;
		//	Start = true;
		//}

		if (m_Input->IsF2Pressed() == true)
			PressedF2 = true;
		else
			PressedF2 = false;

		m_Sound->Mute(PressedF2);
		
			
		

		if (m_Input->IsF1Pressed() == true )
		{
			m_state = STATE::LOADING;
			Start = true;
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

	//애니메이션 프레임 객체 생성
	//-------------------------------------------------------------------------------------
	//Frames::Get()->Start();
	m_Frame = Frames::Get();
	m_Frame->Start();
	//-------------------------------------------------------------------------------------


	m_Graphics->Loading(screenWidth, screenHeight, m_hwnd);
	//-------------------------------------------------------------------------------------

	// FPS 객체 생성
	//-------------------------------------------------------------------------------------
//m_FPS = new FpsClass;
//if (!m_FPS) { return false; }
//
//m_FPS->Initialize();
	//-------------------------------------------------------------------------------------

//// CPU 객체 생성
////-------------------------------------------------------------------------------------
//m_Cpu = new CpuClass;
//if (!m_Cpu) { return false; }
//
//m_Cpu->Initialize();
////-------------------------------------------------------------------------------------

	// Timer 객체 생성
////-------------------------------------------------------------------------------------
//m_Timer = new TimerClass;
//if (!m_Timer) { return false; }
//
//result = m_Timer->Initialize();
//if (!result) { MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK); return false; }
////-------------------------------------------------------------------------------------



	m_state = STATE::RUNNING;
	if (m_state = STATE::RUNNING)
	{
		m_Sound->ShutdownMainSound();
		m_Sound->PlayWaveFile(2);
	}
	return true;
}

// 객체를 정리한다. 
void SystemClass::Shutdown()
{

	//SAFE_SHUTDOWN(m_Sound);
	
	m_Frame->Delete();
	SAFE_SHUTDOWN(m_Graphics);
	SAFE_SHUTDOWN(m_Input);

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

			if (m_state == STATE::RUNNING)
			{
				// Otherwise do the frame processing.
				result = Frame();

				if (!result)
				{
					done = true;
				}
			}

			if (m_state == STATE::END)
			{
				End();
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

bool SystemClass::HandleInput()
{
	bool result;


	if(m_Input->IsUpPressed())
		Camera::Get()->MoveForward();
	if (m_Input->IsDownPressed())
		Camera::Get()->MoveBackward();
	if (m_Input->IsLeftTurned())
		Camera::Get()->RotateRight();
	if (m_Input->IsRightTurned())
		Camera::Get()->RotateLeft();
	if (m_Input->IsLookDownTurned())
		Camera::Get()->RotateDown();
	if (m_Input->IsLookUpTurned())
		Camera::Get()->RotateUp();

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

	result = HandleInput();
	if (!result) { return false; }

	// m_Graphics객체를 통해 화면에 그리는 작업을 수행한다.
	m_Frame->Update();

	
	result = m_Graphics->Frame(m_Frame->FramePerSecond(), m_Frame->GetFrameTime(),pos, rot, mouseX, mouseY, Shoot);
	//result = m_Graphics->Frame(m_FPS->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime(), pos, rot, W);
	if (!result){ return false; }
	

	//result = m_Graphics->Render(F1pressed);
	result = m_Graphics->Render();
	if (!result) { return false; }

	//총알발사
	if (m_Input->IsSpacePressed() == true)
		Shoot = true;
	else
		Shoot = false;


	//왼쪽 마우스 클릭시 그래픽들 반응
	if (m_Input->IsLeftMouseButtonDown())
	{
		m_Graphics->click = true;
		m_Graphics->CheckIntersection(mouseX, mouseY, screenWidth, screenHeight);
	}
	else
		m_Graphics->click = false;

	if (m_Graphics->End() == true)
		m_state = STATE::END;

	if (m_Graphics->isEffectSound == true)
	{
		m_Sound->PlayWaveFile(3);
		m_Graphics->isEffectSound = false;
	}

	return true;
}

void SystemClass::End()
{	
	SAFE_SHUTDOWN(m_Sound);
	m_Input->Frame();
	m_Graphics->RenderEndScene();


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
		screenWidth = 1600;
		screenHeight = 900;

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
	ShowCursor(FALSE);


	D3DInfo info;
	info.appName = m_applicationName;
	info.instance = m_hinstance;
	info.isFullScreen = FULL_SCREEN;
	info.isVsync = VSYNC_ENABLED;
	info.mainHandle = m_hwnd;
	info.screenWidth = screenWidth;
	info.screenHeight = screenHeight;
	info.clearColor = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	D3D::SetInfo(info);
	

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