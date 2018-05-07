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

	//// Sound ��ü ����
	//-------------------------------------------------------------------------------------
	m_Sound = new SoundClass;
	if (!m_Sound) { return false; }

	result = m_Sound->Initialize(m_hwnd);
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the Sound object.", L"Error", MB_OK); return false; }
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
	int Mute = 0;
	bool PressedF2 = false;
	bool sibal = false;
	bool Start = false;

	m_Input->SetMousePosition();

	//���λ��� ���
	result = m_Sound->PlayWaveFile(1);
	if (!result) { MessageBox(m_hwnd, L"Could not initialize the main sound.", L"Error", MB_OK); }

	while (Start==false)
	{
		// Ű����&���콺 ���¸� �����ϵ��� �Ѵ�.
		result = m_Input->Frame();
		m_Input->GetMouseLocation(mouseX, mouseY);

		//����ȭ�� ���
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
�̸� : Loading()
�뵵 : 
- �ε�ȭ�鿡�� �Ͼ�� �ϵ�.
- Graphics ��ü���� ������ ������ �Ǵ� �ֵ�, FPS, CPU, Timer, Sound ��ü�� �ʱ�ȭ�Ѵ�.
---------------------------------------------------------------------------------------------------------------*/
bool SystemClass::Loading()
{
	bool result;

	//�ִϸ��̼� ������ ��ü ����
	//-------------------------------------------------------------------------------------
	//Frames::Get()->Start();
	m_Frame = Frames::Get();
	m_Frame->Start();
	//-------------------------------------------------------------------------------------


	m_Graphics->Loading(screenWidth, screenHeight, m_hwnd);
	//-------------------------------------------------------------------------------------

	// FPS ��ü ����
	//-------------------------------------------------------------------------------------
//m_FPS = new FpsClass;
//if (!m_FPS) { return false; }
//
//m_FPS->Initialize();
	//-------------------------------------------------------------------------------------

//// CPU ��ü ����
////-------------------------------------------------------------------------------------
//m_Cpu = new CpuClass;
//if (!m_Cpu) { return false; }
//
//m_Cpu->Initialize();
////-------------------------------------------------------------------------------------

	// Timer ��ü ����
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

// ��ü�� �����Ѵ�. 
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
			// ESCŰ�� Ȯ���ϴ� �Լ�
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
// ���ø����̼��� ��� �۾��� �� ���� ���� �Ҹ��� Frame�Լ����� ����ȴ�.
bool SystemClass::Frame()
{
	bool keyDown,result;

	// Ű����&���콺 ���¸� �����ϵ��� �Ѵ�.
	result = m_Input->Frame();
	if (!result) { return false; }

	m_Input->GetMouseLocation(mouseX, mouseY);

	result = HandleInput();
	if (!result) { return false; }

	// m_Graphics��ü�� ���� ȭ�鿡 �׸��� �۾��� �����Ѵ�.
	m_Frame->Update();

	
	result = m_Graphics->Frame(m_Frame->FramePerSecond(), m_Frame->GetFrameTime(),pos, rot, mouseX, mouseY, Shoot);
	//result = m_Graphics->Frame(m_FPS->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime(), pos, rot, W);
	if (!result){ return false; }
	

	//result = m_Graphics->Render(F1pressed);
	result = m_Graphics->Render();
	if (!result) { return false; }

	//�Ѿ˹߻�
	if (m_Input->IsSpacePressed() == true)
		Shoot = true;
	else
		Shoot = false;


	//���� ���콺 Ŭ���� �׷��ȵ� ����
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
		screenWidth = 1600;
		screenHeight = 900;

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