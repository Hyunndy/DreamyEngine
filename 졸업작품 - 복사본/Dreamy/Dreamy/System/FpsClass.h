#pragma once


#pragma comment(lib, "winmm.lib")

#include <windows.h> 
#include <mmsystem.h>

/*
	단순한 타이머가 있는 카운터.
	1초의 시간 동안 얼만큼의 프레임들이 그려지는지 세고 정기적으로 이 숫자를 갱신한다.
*/
class FpsClass
{
public:
	FpsClass();
	FpsClass(const FpsClass&);
	~FpsClass();


	void Initialize();
	void Frame();
	int GetFps();
	int GetFrameCount();
	int GetTime();
	void Start();

private: 

	int m_fps, m_count;
	unsigned long m_startTime;
	int m_time;


};


