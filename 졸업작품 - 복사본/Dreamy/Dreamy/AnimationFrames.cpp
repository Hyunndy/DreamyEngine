
#include "AnimationFrames.h"

Frames* Frames::instance = NULL;


float Frames::timeElapsed = 0.0f;

Frames::Frames(void) :
	ticksPerSecond(0), currentTime(0), lastTime(0), lastFPSUpdate(0), fpsUpdateInterval(0),
	frameCount(0), runningTime(0), framePerSecond(0)
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond);
	fpsUpdateInterval = ticksPerSecond >> 1;

	m_ticksPerms = (float)(ticksPerSecond / 1000);
	//TwBar* bar = TweakBar::Get()->GetBar();
	//TwAddVarRO(bar, "Frames", TW_TYPE_FLOAT, &framePerSecond, "");
}

Frames::~Frames(void)
{

}

Frames* Frames::Get()
{
	if (instance == NULL)
		instance = new Frames();

	return instance;
}

void Frames::Delete()
{
	SAFE_DELETE(instance);
}

void Frames::Update()
{


	//1. ����ð��� ������ �ð� ���� �� ���� �ð��� ����Ѵ�.
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	timeElapsed = (float)(currentTime - lastTime) / (float)ticksPerSecond;
	runningTime += timeElapsed;
	m_frameTime = (float)(currentTime - lastTime) / m_ticksPerms;

	//2. FPS Update
	frameCount++;
	if (currentTime - lastFPSUpdate >= fpsUpdateInterval)
	{
		float tempCurrentTime = (float)currentTime / (float)ticksPerSecond;
		float tempLastTime = (float)lastFPSUpdate / (float)ticksPerSecond;
		framePerSecond = (float)frameCount / (tempCurrentTime - tempLastTime);

		lastFPSUpdate = (INT64)currentTime;
		frameCount = 0;
	}

	lastTime = currentTime;
}

float Frames::GetFrameTime()
{
	return  m_frameTime;
}

void Frames::Print()
{

}

void Frames::Start()
{
	//if(!isTimerStopped)
	//assert(false);

	QueryPerformanceCounter((LARGE_INTEGER *)&lastTime);

}

//void Frames::Stop()
//{
//	//if(isTimerStopped)
//		//assert(false);
//    
//    INT64 stopTime = 0;
//    QueryPerformanceCounter((LARGE_INTEGER *)&stopTime);
//    runningTime += (float)(stopTime - lastTime) / (float)ticksPerSecond;
//    isTimerStopped = true;
//}