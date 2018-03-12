#pragma once
#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

//라이브러리
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

//포함
#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

/*
	현재 .wav파일만 재생 가능
*/


class SoundClass
{
private:

	//.wav파일 포맷을 위한 구조체.
	//다른 오디오 파일 포맷을 사용한다면 다른 헤더 형식으로 바꿀 수 있다.
	struct WaveHeaderType
	{ 
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize; 
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};



public:
	SoundClass ();
	SoundClass(const SoundClass&);
	~SoundClass();


	bool Initialize(HWND);
	void Shutdown();

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();


	bool LoadWaveFile(char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);


	bool PlayWaveFile(); 


private:
	IDirectSound8* m_DirectSound;

	IDirectSoundBuffer* m_primaryBuffer;

	//사운드 갯수 만큼 2차 버퍼를 생성해야 한다.
	IDirectSoundBuffer8* m_secondaryBuffer1;
};

#endif
