#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_


#include <windows.h>

/*
	실행 중 매 프레임 간의 정확한 시간을 재는 고해상도 타이머 클래스.
	주요 용도는 오브젝트의 이동을 위해 프레임의 표준 시간대와의 동기화한다.

	대게 TimerCLass는 프레임간 간격을 1초에 대한 %로 바꿔서 물체를 그만큼 움직이도록 하는 경우에 많이 사용한다.
	프레임 마다 그냥 숫자를 더하면 안되냐고? 안된다! 그럼 cpu에 종속적이게 된다.
*/

////////////////////////////////////////////////////////////////////////////////
// Class name: TimerClass
////////////////////////////////////////////////////////////////////////////////
class TimerClass
{
public:

	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:


	INT64 m_frequency;

	float m_ticksPerMs;

	INT64 m_startTime;

	float m_frameTime;
};

#endif


