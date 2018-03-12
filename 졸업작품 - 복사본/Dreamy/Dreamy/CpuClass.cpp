#include "CpuClass.h"



CpuClass::CpuClass(){}

CpuClass::CpuClass(const CpuClass& other){}

CpuClass::~CpuClass(){}


/*------------------------------------------------------------------------------------
이름 : Initialize()
용도 :
- CPU사용량을 질의(QUERY)하기 위해 필요한 핸들을 초기화한다.
- 여기서 설정된 질의(QUERY)는 개개 CPU의 사용량 대신 시스템의 모든 CPU들의 사용량 총합을 합해서 돌려준다.
- 만약 핸들이나 질의를 얻지 못하는 등의 이유로 CPU사용량 정보를 읽을 수 없다면 m_canReadCpu변수가 FALSE로 바뀌어서 기록하는 CPU사용량을 0이 되도록 한다.
- 일부 OS에선 권한 때문에 이 기능을 사용할 수 없을 수 있다.

- 타이머를 돌려서 CPU사용량을 매 초마다 측정하도록 한다.
--------------------------------------------------------------------------------------*/
void CpuClass::Initialize()
{
	//퍼포먼스 데이터 헬퍼
	PDH_STATUS status; 
	
	// cpu사용량을 폴링할 질의 개체를 만든다.
	m_canReadCpu = true; 
	
	// cpu사용량을 재기위한 핸들을 생성한다.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if(status != ERROR_SUCCESS) { m_canReadCpu = false; } 
	
	// 시스템의 모든 CPU를 측정하도록 질의 개체를 설정한다.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if(status != ERROR_SUCCESS) { m_canReadCpu = false; } 

	// 타이머 시작
	m_lastSampleTime = GetTickCount();

	// cpu사용량 초기화
	m_cpuUsage = 0; 
	
	return;
}
/*------------------------------------------------------------------------------------
이름 : ShutDown()
용도 : CPU사용량을 질의하는데 사용한 핸들을 해제한다.
--------------------------------------------------------------------------------------*/
void CpuClass::Shutdown()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	} 
	
	return;
}
/*------------------------------------------------------------------------------------
이름 : Frame()
용도 : 
- 매 프레임 마다 호출되는 함수.
- 질의 횟수를 줄이기 위해서 m_lastSampleTime이라는 변수를 두어 1초 마다 cpu사용량을 샘플링 할 수 있게 한다.
- 매 초 마다 cpu에게 사용량을 물어보고 그 값을 m_cpuUsage에 저장한다.
--------------------------------------------------------------------------------------*/
void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu)
	{
		if ((m_lastSampleTime + 1000) < GetTickCount())
		{
			m_lastSampleTime = GetTickCount();

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}

	return;
}

/*------------------------------------------------------------------------------------
이름 : GetCpuPercentage()
용도 : 외부에서 현재 cpu사용량을 알게 하기 위해 호출될 함수.
--------------------------------------------------------------------------------------*/
int CpuClass::GetCpuPercentage()
{
	int usage;

	if (m_canReadCpu)
	{
		usage = (int)m_cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}

