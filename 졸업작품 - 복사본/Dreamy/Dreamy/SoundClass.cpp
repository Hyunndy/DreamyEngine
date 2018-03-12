#include "SoundClass.h"



SoundClass::SoundClass()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;
}

SoundClass::SoundClass(const SoundClass&)
{
}

SoundClass::~SoundClass()
{
}

/*--------------------------------------------------------------------------------------------------------------
이름 : Initialize()
용도 :
- DirectSound를 초기화 한다.
- 1차 버퍼를 초기화 한다.
- .wav 오디오 파일을 로드한다.
- 로드한 오디오 파일로 2차 버퍼를 초기화 한다.
- 로드가 완료되면 바로 .wav파일을 재생한다.
---------------------------------------------------------------------------------------------------------------*/
bool SoundClass::Initialize(HWND hwnd)
{
	bool result;

	//DirectSound&1차버퍼를 초기화 한다.
	result = InitializeDirectSound(hwnd);
	if (!result) { return false; }

	//.wav오디오 파일을 로드하고 그 데이터로 2차버퍼를 초기화 한다.
	result = LoadWaveFile("../Dreamy/sound03.wav", &m_secondaryBuffer1);
	if (!result) { return false; }

	//.wav파일을 재생한다.
	// 나중엔 이걸 다른데서 하면 되겠지!
	result = PlayWaveFile();
	if (!result) { return false; }
	
}



/*--------------------------------------------------------------------------------------------------------------
이름 : InitializeDirectSound()
용도 :
- DirectSound API와 1차 버퍼의 인터페이스에 대한 포인터를 얻어온다.
- 여기서 모든 음향 장치를 조회하여 그 중 특정 장치를 1차 버퍼로 정해 그 포인터로 얻어올 수도 있지만,
  현재는 단순함을 위해 기본 음향 장치에 대한 포인터를 얻어온다.
- SOUND도 INPUT과 마찬가지로 협력 레벨(CooperativeLevel)을 받아온다.

- 1차 "버퍼"가 중요하다. 버퍼는 Description이 필요하고, 그걸 여기서 해준다!

- DirectSound초기화->DirectSound에서의 협력 수준 설정-> 1차 버퍼의 desc작성-> 1차 버퍼 생성 -> .wav오디오 파일 포맷 설정 -> 1차 버퍼의 설정을 .wav파일 포맷으로 바꿈.
----------------------------------------------------------------------------------------------------------------*/
bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc; // 1차 "버퍼"의 description
	WAVEFORMATEX waveFormat;

	//DirectSound를 생성한다.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result)) { return false; }

	// 1차 사운드 버퍼의 형식을 수정할 수 있도록 협력 수준을 우선 순위로 설정한다.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);

	/*
	1차 버퍼에 접근하기 위한 Discription을 작성한다.
	dwFlags가 가장 중요하다.
	*/
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME; //볼륨을 조정할 수 있게 한다.
	bufferDesc.dwBufferBytes = 0; 
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL; 
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//1차 버퍼의 Descripiton을 작성하면 1차 버퍼를 생성한다!
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result)) { return false; }

	/*
	기본 음향 장치를 1차 버퍼로 하여 제어권을 얻었다면 이 장치의 포맷을 우리의 오디오 파일 포맷(.wav)로 바꿔야 한다.
	고품질의 사운드를 원하기 때문에 비압축 CD오디오 품질로 설정한다.
	*/
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2; 
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels; 
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0; 
	
	// 특정 WAV포맷을 1차버퍼의 포맷으로 세팅해준다.
	result = m_primaryBuffer->SetFormat(&waveFormat); 
	if(FAILED(result)) { return false; } 
	
	
	return true;

}


/*--------------------------------------------------------------------------------------------------------------
이름 : LoadWaveFile()
용도 :
- .wav오디오 파일을 로드한다.
- 로드한 파일의 데이터를 2차 버퍼에 복사한다

- 다른 포맷으로 하기원한다면 이 함수를 바꾸거나...그런 기능의 새로운 함수를 만들어 사용한다.

- 1.wav파일을 열고 파일의 헤더 정보를 읽는다.
	헤더엔 오디오 파일의 각종 정보가 있어서 이를 이용하여 데이터를 담을 2차 버퍼를 생성할 수 있다.
	헤더엔 데이터의 크기, 시작 지점 등에 대한 정보가 있다.
	헤더의 모든 태그들을 읽어서 오디오 파일이 손상되지 않고 RIFF, WAVE, FMT, DTA, WAVE_FORMAT_PCM들을 포함했는지 확인한다.
	또한 44.1KHZ 스테레오 16bit의 오디오 파일인지 확인한다.
	만일 .wav파일이 22.1KHZ이거나 8비트이거나 등등의 다른 경우라면 이 파일은 요구사항에 일치하지 않으므로 실패한다.
	(이 요구사항은 위의 InitializeDirectSound의 waveformat에서 정한것이다)

- 2. .wav파일의 헤더가 충분히 검증되었다면, 데이터를 복사할 2차 버퍼의 Description을 작성한다.
- 3. 2차 버퍼의 description을 가지고 임시적으로 IDirectSoundBuffer 객체를 만든다.
- 4. QueryInterface함수를 IID_IDirectSoundBuffer8인자와 함께 호출하여 이 임시 버퍼로 IDirectSoundBuffer8의 2차 버퍼를 생성한다.
- 5. 임시 버퍼를 해제하고 2차 버퍼를 사용한다.

- 6. 2차 버퍼를 생성한 후 오디오 파일의 웨이브 데이터를 가져온다.
	우선 메모리 버퍼에 불러서 필요하다면 수정을 한다.
- 7. 2차 버퍼를 잠궈 접근 권한을 얻고 memcpy로 데이터를 복사하고 잠금을 해제한다. 2차 버퍼 준비 끝!

 무슨 포인터2개를 쓰라는 말이 있는데 뭔소린지 모르겠음 ^^; 버퍼의 사이즈를 알려고...?
----------------------------------------------------------------------------------------------------------------*/
bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;

	unsigned char* waveData; 
	unsigned char *bufferPtr;
	unsigned long bufferSize;

	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// .wav파일의 헤더를 읽는다.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// 웨이브 포맷을 설정한다.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 2차 버퍼의 description을 작성한다.
	// 1차 버퍼와 dwFlag, dwBufferBytes는 다르다.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// tempBuffer이라는 임시 객체를 만들어 위에서 설정한 description을 넣는다.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 위의 과정이 성공하면 이 함수를 통해 임시 버퍼로 2차 버퍼를 생성하게 된다.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 임시 버퍼를 해제한다.
	tempBuffer->Release();
	tempBuffer = 0;

	// 오디오 파일의 웨이브 데이터를 가져와서 2차 버퍼에 넣는다.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	/*
	기억해 둘 것은 2차 버퍼를 잠글 때 두 개의 포인터를 받아 쓴다는 것입니다.
	버퍼가 원형으로 구성되어 있기 때문에 중간부터 버퍼를 쓰게 되는 경우 그 지점에서의 버퍼 사이즈를 아는 것이 필요하기 때문입니다.
	그래야 버퍼의 범위를 넘어가지 않게 쓰기 작업을 할 수 있습니다. 
	이런 구조는 오디오를 재생하는 것과 같은 일에 유용합니다. 
	이 튜토리얼에서는 작업을 간단하게 하기 위해 오디오 파일과 같은 크기의 버퍼를 만들고 버퍼의 시작부터 파일을 기록하게 합니다.
	*/
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	return true;
}

/*--------------------------------------------------------------------------------------------------------------
이름 : PlayWaveFile()
용도 :
- 2차 버퍼에 저장되어 있는 오디오 파일을 재생한다.
- Play함수에서 1차 버퍼의 오디오와의 믹싱을 자동으로 수행한 뒤 재생한다.
- 2차 버퍼의 시작 부분으로 시작 위치를 지정했다. (아니면 가장 최근에 재생했던 부분부터 시작한다)
- 볼륨을 세팅한다.
----------------------------------------------------------------------------------------------------------------*/
bool SoundClass::PlayWaveFile()
{
	HRESULT result;


	// 사운드 버퍼의 시작 부분을 정한다.
	// 2차 버퍼의 시작 부분으로 시작 위치를 정함.
	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if(FAILED(result)) { return false; }

	// 1차 버퍼에서 볼륨을 컨트롤할 수 있게 description을 작성했으니 볼륨을 최대로 맞춘다.
	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if(FAILED(result)) { return false; } 
	
	result = m_secondaryBuffer1->SetFrequency(1);

	// 1차 버퍼의 오디오와의 믹싱을 수행한다.
	// 3번째 인자가 1번 실행이면 0, 반복 싱행이면 1이다.
	result = m_secondaryBuffer1->Play(0, 0, 1);

	if(FAILED(result)) { return false; } 
	
	
	return true;

}

/*--------------------------------------------------------------------------------------------------------------
이름 : Shutdown()
용도 : 해제한다.
----------------------------------------------------------------------------------------------------------------*/
void SoundClass::Shutdown()
{
	// 2차 버퍼를 해제한다.
	ShutdownWaveFile(&m_secondaryBuffer1);

	// DirectSound, 1차 버퍼를 해제한다.
	ShutdownDirectSound();
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// 2차 버퍼를 해제하는 일을 한다.
	if(*secondaryBuffer) 
	{ 
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0; } 
	
	return;


}

void SoundClass::ShutdownDirectSound()
{
	// Release the primary sound buffer pointer.
	if(m_primaryBuffer) { m_primaryBuffer->Release(); m_primaryBuffer = 0; }
	
	// Release the direct sound interface pointer.
	if(m_DirectSound) { m_DirectSound->Release(); m_DirectSound = 0; } 
	
	return;
}


