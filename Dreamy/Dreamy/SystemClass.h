////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES // ���� �ӵ��� ���̱� ���� �� ������ �ʴ� API�� ��� �ִ� Win32 ����� �������� �ʴ´�.
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

	
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM); // ������κ��� ���� �ý��� �޼������� ���ø����̼��� ���ư��� ���� �����ϵ��� �Ѵ�.

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
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // �������� �ý��۸޼������� messageHandler�� ������ �� �ְ� �Ѵ�.


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;


#endif