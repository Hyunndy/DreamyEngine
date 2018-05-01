////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "../Dreamy/System/SystemClass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)

{
	SystemClass* System;
	bool result;


	// �ý��� ��ü �̱������� ����
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// �ý��� ��ü�� �ʱ�ȭ�ϰ� run�� ȣ���Ѵ�.

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



	//��ü�� �����ϰ� �޸𸮸� ��ȯ�Ѵ�.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}