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

#include "D3D.h"
#include "AnimationFrames.h"
#include "Sampler.h"

///////////////////////
// MY CLASS INCLUDES // 
///////////////////////
#include "inputclass.h"
#include "graphicsclass.h"
#include "SoundClass.h"

/////////////////////////




namespace STATE
{
	enum Enum
	{
		INIT,
		MAIN,
		LOADING,
		RUNNING,
		END,
	};
}

/*
namespace + enum����
bool func(STATE::Enum DREAMY)
{
	switch(DREAMY)
	{
		case DREAMY::START:
			break;
		case DREAMY::INIT:
			break;
	}
}
*/

////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	void Main();
	bool Initialize();
	bool Loading();
	void Shutdown();
	void Run();
	void End();

	
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM); // ������κ��� ���� �ý��� �޼������� ���ø����̼��� ���ư��� ���� �����ϵ��� �Ѵ�.

public:
	bool F1pressed;
	int m_state = STATE::INIT;

private:
	bool HandleInput();
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
	//FpsClass* m_FPS;
	//CpuClass* m_Cpu;
	//TimerClass* m_Timer;
	Frames* m_Frame;

	int screenWidth, screenHeight;
	int mouseX, mouseY;
	bool Shoot = false;

	D3DXVECTOR3 pos, rot;
	


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