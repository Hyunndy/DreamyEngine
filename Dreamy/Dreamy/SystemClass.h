////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES // 빌드 속도를 높이기 위해 잘 사용되지 않는 API를 담고 있는 Win32 헤더를 포함하지 않는다.
///////////////////////////////
#define WIN32_LEAN_AND_MEAN


//////////////
// INCLUDES //
//////////////
#include <windows.h>


///////////////////////
// MY CLASS INCLUDES // 
///////////////////////
#include "inputclass.h"
#include "graphicsclass.h"
#include "SoundClass.h"
#include "FpsClass.h"
#include "CpuClass.h"
#include "TimerClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM); // 윈도우로부터 오는 시스템 메세지들을 어플리케이션이 돌아가는 동안 전달하도록 한다.

public:
	bool F1pressed;

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	SoundClass* m_Sound;
	FpsClass* m_FPS;
	CpuClass* m_Cpu;
	TimerClass* m_Timer;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 윈도우의 시스템메세지들을 messageHandler로 전달할 수 있게 한다.


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;


#endif