///////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "timerclass.h"


TimerClass::TimerClass(){}


TimerClass::TimerClass(const TimerClass& other){}


TimerClass::~TimerClass(){}

/*---------------------------------------------------------------------------
이름 : Initialize()
용도 : 
- 1. 시스템이 고해상도 타이머를 지원하는지 알아본다.
- 2. 만약 그 타이머의 주기를 받으면 이 값을 가지고 각 밀리초마다 카운터에서 몇번이나 틱이 일어나는지 계산한다.
- 3. 그 값으로 프레임간 시간을 계산한다.
- 4. 이 함수의 마지막에서 첫 프레임 간격을 물어봐서 타이머를 시작한다.
-----------------------------------------------------------------------------*/
bool TimerClass::Initialize()
{
	// 고해상도 타이머를 지원하는지 묻는다.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0){	return false;}

	// 밀리초 마다 틱이 얼마나 일어나는지 알아 낸다.
	// Tick = 시간 단위
	m_ticksPerMs = (float)(m_frequency / 1000);

	//첫 프레임 간격을 물어봐서 타이머를 시작한다.
	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

/*---------------------------------------------------------------------------
이름 : Frame()
용도 :
- 메인 프로그램에서 매 루프 마다 호출되는 함수.
- 매 루프 마다 시간의 차이를 계산해 프레임 간의 시간 간격을 알 수 있다.

- Initialize()에서 마지막에 QueryPerformanceCounter()으로 첫 프레임을 물어봤으니까
  CurrentTime의 시간을 물어서 그 둘의 차이로 프레임 간의 시간 간격을 알 수 있다.

- 질의하고, 계산한 뒤 그 값을 m_frame에 저장해 이를 호출하는 다른 오브젝트들이 동기화될 수 있게 한다.
- 그리고 현재 시간을 다음 프레임의 시작 시간으로 저장한다.
-----------------------------------------------------------------------------*/
void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;

	// 현재 프레임의 시간을 잰다.
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	// 현재 프레임 - 전 프레임의 시간 간격을 잰다.
	timeDifference = (float)(currentTime - m_startTime);

	// 프레임 간의 간격 = 시간차이/밀리초 마다 틱이 일어나는 횟수
	m_frameTime = timeDifference / m_ticksPerMs;

	// 다음 프레임-다다음 프레임의 시간을 알기위해 시작 시간을 현재 시간으로 초기화한다.
	m_startTime = currentTime;

	return;
}

/*---------------------------------------------------------------------------
이름 : GetTime()
용도 : 외부의 함수들에게 가장 최근에 계산된 프레임 시간을 리턴한다.
-----------------------------------------------------------------------------*/
float TimerClass::GetTime()
{
	return m_frameTime;
}