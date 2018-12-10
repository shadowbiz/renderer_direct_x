#include "timer.h"

Timer::Timer()
{
}

Timer::Timer(const Timer &other)
{
	UNREFERENCED_PARAMETER(other);
}

Timer::~Timer()
{
}

bool Timer::Initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}
	m_ticksPerMs = static_cast<float>(m_frequency / 1000);
	QueryPerformanceCounter((LARGE_INTEGER *)&m_startTime);
	return true;
}

void Timer::Update()
{
	INT64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	float timeDifference = static_cast<float>(currentTime - m_startTime);
	m_frameTime = timeDifference / m_ticksPerMs;
	m_startTime = currentTime;
}

float Timer::GetTime()
{
	return m_frameTime;
}