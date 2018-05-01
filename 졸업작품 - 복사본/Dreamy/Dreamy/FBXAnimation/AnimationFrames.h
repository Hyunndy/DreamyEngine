#pragma once

#include "stdafx_fbx.h"
class Frames
{
public:
	static Frames* Get();
	static void Delete();


	static float TimeElapsed() { return  timeElapsed; }

	void Update();
	void Print();

	void Start();
	void Stop();

	float FramePerSecond() const { return framePerSecond; }
	float RunningTime() const { return runningTime; }
	float GetFrameTime();
	
private:
	Frames(void);
	~Frames(void);

	static Frames* instance;///< �̱��� ��ü


	static float timeElapsed;///< ���� ���������κ��� ����ð�

	float m_ticksPerms;
	float m_frameTime;

	INT64 ticksPerSecond;///< �ʴ� ƽī��Ʈ
	INT64 currentTime;///< ���� �ð�
	INT64 lastTime;///< �����ð�
	INT64 lastFPSUpdate;///< ������ FPS ������Ʈ �ð�
	INT64 fpsUpdateInterval;///< fps ������Ʈ ����

	UINT frameCount;///< ������ ��
	float runningTime;///< ���� �ð�
	float framePerSecond;///< FPS
};
