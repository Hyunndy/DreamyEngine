#include "CpuClass.h"



CpuClass::CpuClass(){}

CpuClass::CpuClass(const CpuClass& other){}

CpuClass::~CpuClass(){}


/*------------------------------------------------------------------------------------
�̸� : Initialize()
�뵵 :
- CPU��뷮�� ����(QUERY)�ϱ� ���� �ʿ��� �ڵ��� �ʱ�ȭ�Ѵ�.
- ���⼭ ������ ����(QUERY)�� ���� CPU�� ��뷮 ��� �ý����� ��� CPU���� ��뷮 ������ ���ؼ� �����ش�.
- ���� �ڵ��̳� ���Ǹ� ���� ���ϴ� ���� ������ CPU��뷮 ������ ���� �� ���ٸ� m_canReadCpu������ FALSE�� �ٲ� ����ϴ� CPU��뷮�� 0�� �ǵ��� �Ѵ�.
- �Ϻ� OS���� ���� ������ �� ����� ����� �� ���� �� �ִ�.

- Ÿ�̸Ӹ� ������ CPU��뷮�� �� �ʸ��� �����ϵ��� �Ѵ�.
--------------------------------------------------------------------------------------*/
void CpuClass::Initialize()
{
	//�����ս� ������ ����
	PDH_STATUS status; 
	
	// cpu��뷮�� ������ ���� ��ü�� �����.
	m_canReadCpu = true; 
	
	// cpu��뷮�� ������� �ڵ��� �����Ѵ�.
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if(status != ERROR_SUCCESS) { m_canReadCpu = false; } 
	
	// �ý����� ��� CPU�� �����ϵ��� ���� ��ü�� �����Ѵ�.
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if(status != ERROR_SUCCESS) { m_canReadCpu = false; } 

	// Ÿ�̸� ����
	m_lastSampleTime = GetTickCount();

	// cpu��뷮 �ʱ�ȭ
	m_cpuUsage = 0; 
	
	return;
}
/*------------------------------------------------------------------------------------
�̸� : ShutDown()
�뵵 : CPU��뷮�� �����ϴµ� ����� �ڵ��� �����Ѵ�.
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
�̸� : Frame()
�뵵 : 
- �� ������ ���� ȣ��Ǵ� �Լ�.
- ���� Ƚ���� ���̱� ���ؼ� m_lastSampleTime�̶�� ������ �ξ� 1�� ���� cpu��뷮�� ���ø� �� �� �ְ� �Ѵ�.
- �� �� ���� cpu���� ��뷮�� ����� �� ���� m_cpuUsage�� �����Ѵ�.
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
�̸� : GetCpuPercentage()
�뵵 : �ܺο��� ���� cpu��뷮�� �˰� �ϱ� ���� ȣ��� �Լ�.
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

