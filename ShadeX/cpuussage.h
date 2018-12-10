#ifndef SE_CPU_USSAGE_H
#define SE_CPU_USSAGE_H

#include "globals.h"

class CpuUssage
{
  public:
	CpuUssage();
	CpuUssage(const CpuUssage &other);
	virtual ~CpuUssage();

	void Initialize();
	void Release();
	void Update();
	int GetCpuPercentage();

  private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	DWORD m_lastSampleTime;
	long m_cpuUsage;
};

#endif // !SE_CPU_USSAGE_H
