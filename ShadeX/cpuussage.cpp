#include "cpuussage.h"

CpuUssage::CpuUssage()
{
}

CpuUssage::CpuUssage(const CpuUssage &other)
{
	UNREFERENCED_PARAMETER(other);
}

CpuUssage::~CpuUssage()
{
}

void CpuUssage::Initialize()
{
	m_canReadCpu = true;
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}
	m_lastSampleTime = GetTickCount();
	m_cpuUsage = 0;
}

void CpuUssage::Release()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
		m_canReadCpu = false;
	}
}

void CpuUssage::Update()
{
	if (m_canReadCpu)
	{
		long currentTick = GetTickCount();
		if ((m_lastSampleTime + 1000) < static_cast<DWORD>(currentTick))
		{
			m_lastSampleTime = currentTick;
			PdhCollectQueryData(m_queryHandle);
			PDH_FMT_COUNTERVALUE value;
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);
			m_cpuUsage = value.longValue;
		}
	}
}

int CpuUssage::GetCpuPercentage()
{
	return m_canReadCpu ? static_cast<int>(m_cpuUsage) : 0;
}