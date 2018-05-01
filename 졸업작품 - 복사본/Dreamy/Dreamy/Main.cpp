////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "../Dreamy/System/SystemClass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)

{
	SystemClass* System;
	bool result;


	// 시스템 객체 싱글톤으로 생성
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// 시스템 객체를 초기화하고 run을 호출한다.

	if (System->m_state == STATE::INIT)
	{
		result = System->Initialize();
	}

	if (result && System->m_state == STATE::MAIN)
	{
		System->Main();
	}

	if (System->m_state == STATE::LOADING)
	{
		System->Loading();
	}

	if (System->m_state==STATE::RUNNING || System->m_state==STATE::END)
	{
		System->Run();
	}



	//객체를 종료하고 메모리를 반환한다.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}