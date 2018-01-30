#pragma once
#ifndef _CPUCLASS_H_
#define _CPUCLASS_H_


#pragma comment(lib, "pdh.lib")
#include <pdh.h>

/*
	PDH = Performance Data Helper
	매 초 마다 cpu 사용량의 총량을 재는 데 사용된다.
*/

class CpuClass
{
public: 

	CpuClass();
	CpuClass(const CpuClass&);
	~CpuClass();
	
	void Initialize();
	void Shutdown(); 
	void Frame();
	int GetCpuPercentage(); 


private:

	bool m_canReadCpu;

	HQUERY m_queryHandle;

	HCOUNTER m_counterHandle;

	unsigned long m_lastSampleTime;

	long m_cpuUsage;

};
#endif

