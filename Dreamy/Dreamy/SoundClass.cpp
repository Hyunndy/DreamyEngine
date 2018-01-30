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
�̸� : Initialize()
�뵵 :
- DirectSound�� �ʱ�ȭ �Ѵ�.
- 1�� ���۸� �ʱ�ȭ �Ѵ�.
- .wav ����� ������ �ε��Ѵ�.
- �ε��� ����� ���Ϸ� 2�� ���۸� �ʱ�ȭ �Ѵ�.
- �ε尡 �Ϸ�Ǹ� �ٷ� .wav������ ����Ѵ�.
---------------------------------------------------------------------------------------------------------------*/
bool SoundClass::Initialize(HWND hwnd)
{
	bool result;

	//DirectSound&1�����۸� �ʱ�ȭ �Ѵ�.
	result = InitializeDirectSound(hwnd);
	if (!result) { return false; }

	//.wav����� ������ �ε��ϰ� �� �����ͷ� 2�����۸� �ʱ�ȭ �Ѵ�.
	result = LoadWaveFile("../Dreamy/sound01.wav", &m_secondaryBuffer1);
	if (!result) { return false; }

	//.wav������ ����Ѵ�.
	// ���߿� �̰� �ٸ����� �ϸ� �ǰ���!
	result = PlayWaveFile();
	if (!result) { return false; }
	
}



/*--------------------------------------------------------------------------------------------------------------
�̸� : InitializeDirectSound()
�뵵 :
- DirectSound API�� 1�� ������ �������̽��� ���� �����͸� ���´�.
- ���⼭ ��� ���� ��ġ�� ��ȸ�Ͽ� �� �� Ư�� ��ġ�� 1�� ���۷� ���� �� �����ͷ� ���� ���� ������,
  ����� �ܼ����� ���� �⺻ ���� ��ġ�� ���� �����͸� ���´�.
- SOUND�� INPUT�� ���������� ���� ����(CooperativeLevel)�� �޾ƿ´�.

- 1�� "����"�� �߿��ϴ�. ���۴� Description�� �ʿ��ϰ�, �װ� ���⼭ ���ش�!

- DirectSound�ʱ�ȭ->DirectSound������ ���� ���� ����-> 1�� ������ desc�ۼ�-> 1�� ���� ���� -> .wav����� ���� ���� ���� -> 1�� ������ ������ .wav���� �������� �ٲ�.
----------------------------------------------------------------------------------------------------------------*/
bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc; // 1�� "����"�� description
	WAVEFORMATEX waveFormat;

	//DirectSound�� �����Ѵ�.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result)) { return false; }

	// 1�� ���� ������ ������ ������ �� �ֵ��� ���� ������ �켱 ������ �����Ѵ�.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);

	/*
	1�� ���ۿ� �����ϱ� ���� Discription�� �ۼ��Ѵ�.
	dwFlags�� ���� �߿��ϴ�.
	*/
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME; //������ ������ �� �ְ� �Ѵ�.
	bufferDesc.dwBufferBytes = 0; 
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL; 
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//1�� ������ Descripiton�� �ۼ��ϸ� 1�� ���۸� �����Ѵ�!
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result)) { return false; }

	/*
	�⺻ ���� ��ġ�� 1�� ���۷� �Ͽ� ������� ����ٸ� �� ��ġ�� ������ �츮�� ����� ���� ����(.wav)�� �ٲ�� �Ѵ�.
	��ǰ���� ���带 ���ϱ� ������ ����� CD����� ǰ���� �����Ѵ�.
	*/
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2; 
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels; 
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0; 
	
	// Ư�� WAV������ 1�������� �������� �������ش�.
	result = m_primaryBuffer->SetFormat(&waveFormat); 
	if(FAILED(result)) { return false; } 
	
	
	return true;

}


/*--------------------------------------------------------------------------------------------------------------
�̸� : LoadWaveFile()
�뵵 :
- .wav����� ������ �ε��Ѵ�.
- �ε��� ������ �����͸� 2�� ���ۿ� �����Ѵ�

- �ٸ� �������� �ϱ���Ѵٸ� �� �Լ��� �ٲٰų�...�׷� ����� ���ο� �Լ��� ����� ����Ѵ�.

- 1.wav������ ���� ������ ��� ������ �д´�.
	����� ����� ������ ���� ������ �־ �̸� �̿��Ͽ� �����͸� ���� 2�� ���۸� ������ �� �ִ�.
	����� �������� ũ��, ���� ���� � ���� ������ �ִ�.
	����� ��� �±׵��� �о ����� ������ �ջ���� �ʰ� RIFF, WAVE, FMT, DTA, WAVE_FORMAT_PCM���� �����ߴ��� Ȯ���Ѵ�.
	���� 44.1KHZ ���׷��� 16bit�� ����� �������� Ȯ���Ѵ�.
	���� .wav������ 22.1KHZ�̰ų� 8��Ʈ�̰ų� ����� �ٸ� ����� �� ������ �䱸���׿� ��ġ���� �����Ƿ� �����Ѵ�.
	(�� �䱸������ ���� InitializeDirectSound�� waveformat���� ���Ѱ��̴�)

- 2. .wav������ ����� ����� �����Ǿ��ٸ�, �����͸� ������ 2�� ������ Description�� �ۼ��Ѵ�.
- 3. 2�� ������ description�� ������ �ӽ������� IDirectSoundBuffer ��ü�� �����.
- 4. QueryInterface�Լ��� IID_IDirectSoundBuffer8���ڿ� �Բ� ȣ���Ͽ� �� �ӽ� ���۷� IDirectSoundBuffer8�� 2�� ���۸� �����Ѵ�.
- 5. �ӽ� ���۸� �����ϰ� 2�� ���۸� ����Ѵ�.

- 6. 2�� ���۸� ������ �� ����� ������ ���̺� �����͸� �����´�.
	�켱 �޸� ���ۿ� �ҷ��� �ʿ��ϴٸ� ������ �Ѵ�.
- 7. 2�� ���۸� ��� ���� ������ ��� memcpy�� �����͸� �����ϰ� ����� �����Ѵ�. 2�� ���� �غ� ��!

 ���� ������2���� ����� ���� �ִµ� ���Ҹ��� �𸣰��� ^^; ������ ����� �˷���...?
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

	// .wav������ ����� �д´�.
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

	// ���̺� ������ �����Ѵ�.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 2�� ������ description�� �ۼ��Ѵ�.
	// 1�� ���ۿ� dwFlag, dwBufferBytes�� �ٸ���.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// tempBuffer�̶�� �ӽ� ��ü�� ����� ������ ������ description�� �ִ´�.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ������ �����ϸ� �� �Լ��� ���� �ӽ� ���۷� 2�� ���۸� �����ϰ� �ȴ�.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ӽ� ���۸� �����Ѵ�.
	tempBuffer->Release();
	tempBuffer = 0;

	// ����� ������ ���̺� �����͸� �����ͼ� 2�� ���ۿ� �ִ´�.
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
	����� �� ���� 2�� ���۸� ��� �� �� ���� �����͸� �޾� ���ٴ� ���Դϴ�.
	���۰� �������� �����Ǿ� �ֱ� ������ �߰����� ���۸� ���� �Ǵ� ��� �� ���������� ���� ����� �ƴ� ���� �ʿ��ϱ� �����Դϴ�.
	�׷��� ������ ������ �Ѿ�� �ʰ� ���� �۾��� �� �� �ֽ��ϴ�. 
	�̷� ������ ������� ����ϴ� �Ͱ� ���� �Ͽ� �����մϴ�. 
	�� Ʃ�丮�󿡼��� �۾��� �����ϰ� �ϱ� ���� ����� ���ϰ� ���� ũ���� ���۸� ����� ������ ���ۺ��� ������ ����ϰ� �մϴ�.
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
�̸� : PlayWaveFile()
�뵵 :
- 2�� ���ۿ� ����Ǿ� �ִ� ����� ������ ����Ѵ�.
- Play�Լ����� 1�� ������ ��������� �ͽ��� �ڵ����� ������ �� ����Ѵ�.
- 2�� ������ ���� �κ����� ���� ��ġ�� �����ߴ�. (�ƴϸ� ���� �ֱٿ� ����ߴ� �κк��� �����Ѵ�)
- ������ �����Ѵ�.
----------------------------------------------------------------------------------------------------------------*/
bool SoundClass::PlayWaveFile()
{
	HRESULT result;


	// ���� ������ ���� �κ��� ���Ѵ�.
	// 2�� ������ ���� �κ����� ���� ��ġ�� ����.
	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if(FAILED(result)) { return false; }

	// 1�� ���ۿ��� ������ ��Ʈ���� �� �ְ� description�� �ۼ������� ������ �ִ�� �����.
	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if(FAILED(result)) { return false; } 
	
	result = m_secondaryBuffer1->SetFrequency(1);

	// 1�� ������ ��������� �ͽ��� �����Ѵ�.
	// 3��° ���ڰ� 1�� �����̸� 0, �ݺ� �����̸� 1�̴�.
	result = m_secondaryBuffer1->Play(0, 0, 0);

	if(FAILED(result)) { return false; } 
	
	
	return true;

}

/*--------------------------------------------------------------------------------------------------------------
�̸� : Shutdown()
�뵵 : �����Ѵ�.
----------------------------------------------------------------------------------------------------------------*/
void SoundClass::Shutdown()
{
	// 2�� ���۸� �����Ѵ�.
	ShutdownWaveFile(&m_secondaryBuffer1);

	// DirectSound, 1�� ���۸� �����Ѵ�.
	ShutdownDirectSound();
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// 2�� ���۸� �����ϴ� ���� �Ѵ�.
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


