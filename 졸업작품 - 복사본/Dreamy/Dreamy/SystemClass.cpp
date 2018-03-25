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


	//2D�̹��� �Ѱ� ���� BOOL ����
	F1pressed = false;
	screenHeight = 0;
	screenWidth = 0;

	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mouseX = 0;
	mouseY = 0;
}

//�� ��ü���� �����͸� null�� �ʱ�ȭ�ϳĸ� �ʱ�ȭ�� �����ϸ� ��ٷ� �˴ٿ� �Լ��� ȣ���ϰ� �Ǵµ� �� �Լ������� ��ü�� null�� �ƴ϶�� �̸� �ùٸ��� ������ ��ü�� ����ϰ�
//�� ��ü�� �޸𸮸� �����ؾ� �Ѵٰ� �Ǵ��ϰ� ������ ������ �����Ϸ� �Ѵ�.

SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}
//�ı��ڿ��� ������ ���� �ʰ� shutdown�Լ����� ���� ���ش�.

/*--------------------------------------------------------------------------------------------------------------
�̸� : Initialize()
�뵵 :
- ���α׷��� �������ڸ��� ����Ǵ� �Լ�.
- Input, Graphics ��ü�� �ʱ�ȭ �Ѵ�.
---------------------------------------------------------------------------------------------------------------*/
bool SystemClass::Initialize()
{

	bool result;


	// �Լ��� ���̿� �ʺ� �����ϱ� ���� ������ 0���� �ʱ�ȭ�Ѵ�.
	screenWidth = 0;
	screenHeight = 0;

	//�������� api�� ����Ͽ� �ʱ�ȭ�Ѵ�.
	InitializeWindows(screenWidth, screenHeight);

	// Input��ü ����
	//-------------------------------------------------------------------------------------
	m_Input = new InputClass;
	if (!m_Input) {return false;}

	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if(!result) {MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------
	
	// Graphics ��ü ����
	//-------------------------------------------------------------------------------------
	m_Graphics = new GraphicsClass;
	if (!m_Graphics){return false;}

	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);

	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Grahpics object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// m_state���� ����
	//--------------------------------------------------------------------------------------
	m_state = STATE::MAIN;

	return true;
}

/*--------------------------------------------------------------------------------------------------------------
�̸� : Main()
�뵵 : ����ȭ�鿡�� �Ͼ�� �ϵ�.
---------------------------------------------------------------------------------------------------------------*/
void SystemClass::Main()
{
	bool result;
	bool Start = false;

	while (Start==false)
	{
		result = m_Graphics->RenderMainScene();
		if (!result) { MessageBox(m_hwnd, L"Could not initialize the main scene object.", L"Error", MB_OK);  }

		// Ű����&���콺 ���¸� �����ϵ��� �Ѵ�.
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
�̸� : Loading()
�뵵 : 
- �ε�ȭ�鿡�� �Ͼ�� �ϵ�.
- Graphics ��ü���� ������ ������ �Ǵ� �ֵ�, FPS, CPU, Timer, Sound ��ü�� �ʱ�ȭ�Ѵ�.
---------------------------------------------------------------------------------------------------------------*/
bool SystemClass::Loading()
{
	bool result;

	// Graphics ��ü ����
	//-------------------------------------------------------------------------------------
	result = m_Graphics->Loading(screenWidth, screenHeight, m_hwnd);
	//-------------------------------------------------------------------------------------

	// FPS ��ü ����
	//-------------------------------------------------------------------------------------
	m_FPS = new FpsClass;
	if (!m_FPS) { return false; }

	m_FPS->Initialize();
	//-------------------------------------------------------------------------------------

	// CPU ��ü ����
	//-------------------------------------------------------------------------------------
	m_Cpu = new CpuClass;
	if (!m_Cpu) { return false; }

	m_Cpu->Initialize();
	//-------------------------------------------------------------------------------------

	// Timer ��ü ����
	//-------------------------------------------------------------------------------------
	m_Timer = new TimerClass;
	if (!m_Timer) { return false; }

	m_Timer->Initialize();
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// Sound ��ü ����
	//-------------------------------------------------------------------------------------
	m_Sound = new SoundClass;
	if (!m_Sound) { return false; }

	result = m_Sound->Initialize(m_hwnd);
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Sound object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// move��ü ����
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

// ��ü�� �����Ѵ�. 
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

// ���α׷��� ����� �� ���� ������ ���鼭 ���ø����̼��� ��� �۾��� ó���Ѵ�.
// while ���� ���� �ʴ� ���� �������� �ý��� �޼����� Ȯ��, �޼��� ó��, ���ø����̼��� �۾�, ������ �۾��� ���α׷��� ���Ḧ ���ϴ��� Ȯ��
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

			// ESCŰ�� Ȯ���ϴ� �Լ�
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



	//�� ������ ���� ������ �ð��� �����Ѵ�.
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
// ���ø����̼��� ��� �۾��� �� ���� ���� �Ҹ��� Frame�Լ����� ����ȴ�.
bool SystemClass::Frame()
{
	bool keyDown,result;

	// Ű����&���콺 ���¸� �����ϵ��� �Ѵ�.
	result = m_Input->Frame();
	if (!result) { return false; }

	m_Input->GetMouseLocation(mouseX, mouseY);

	//�ý��� ������ ������Ʈ �Ѵ�.
	m_Timer->Frame();
	m_FPS->Frame();
	m_Cpu->Frame();

	result = HandleInput(m_Timer->GetTime());
	if (!result) { return false; }

	// m_Graphics��ü�� ���� ȭ�鿡 �׸��� �۾��� �����Ѵ�.

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

// �������� �ý��� �޼����� ���޵Ǵ� ��.
// ���� ���� Ű�� �����ִ���, ����������� �� �� �ְ� �� ������ m_Input��ü�� �����Ѵ�.

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
}

// InitializeWindows �Լ����� �츮�� �������� �ϰ� �� �����츦 ����� �ڵ尡 ����.
// �� �Լ��� ȣ���� �Լ����Է� screenWidth�� screenHeight ���� �ٽ� �ǵ����ֹǷ� �� ���� ���ø����̼ǿ��� Ȱ���� �� �ִ�.

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// �ܺ� �����͸� �� ��ü�� �����Ѵ�.	
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� �����´�.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// ������ Ŭ������ �⺻ �������� �����.
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

	// ������ Ŭ������ ����Ѵ�.
	RegisterClassEx(&wc);

	//����� ȭ���� �ػ󵵸� �˾ƿ´�.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Ǯ��ũ�� ��� ������ ���� ���� ȭ�鼳���� �Ѵ�.
	if (FULL_SCREEN)
	{
		// ���� Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũž ũ�⿡ ���߰� ������ 32bit�� �Ѵ�.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ���� �´� ���÷��� ������ �մϴ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// �������� ��ġ�� ȭ���� ���� ���� �����.
		posX = posY = 0;
	}
	else
	{
		// ������ ����� 800x600���� �����.
		screenWidth = 1280;
		screenHeight = 800;

		// â�� ȭ���� �߰��� �����Ѵ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// ������ ���� ������ â�� ����� �� �ڵ��� �����´�.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ش�.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// ���콺 Ŀ���� ǥ�� ��/��
	ShowCursor(false);

	return;
}

// ȭ�� ������ �ǵ����� ������� �� �ڵ��� ��ȯ�Ѵ�.

void SystemClass::ShutdownWindows()
{
	// ���콺 Ŀ���� ǥ���Ѵ�.
	ShowCursor(true);

	// Ǯ��ũ�� ��带 �������� �� ���÷��� ������ �ٲ۴�.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����Ѵ�.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���ø����̼� �ν��Ͻ��� �����Ѵ�.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �� Ŭ������ ���� �ܺ� ������ ������ �����Ѵ�.
	ApplicationHandle = NULL;

	return;
}

// WndProc �Լ��� ������ý��ۿ��� �޼����� ������ ���̴�.
// MessageHandler���� ó���� �޼����� �޾ƾ��ϱ� ������ WndProc�Լ��� �� Ŭ���� ���Ͼȿ� �����ߴ�.
// SystemClass���� �������� �޼����� ����ä�°��� ������ �Ѵ�.

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Ű���尡 Ű�� ���ȴ��� Ȯ���Ѵ�.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �����찡 �������� Ȯ���Ѵ�.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �ٸ� ���޼������� system Ŭ������ �޼��� ó���⿡ �����Ѵ�.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}