#include "fps.h"

FpsCounter::FpsCounter()
{
}

FpsCounter::FpsCounter(const FpsCounter &other)
{
	UNREFERENCED_PARAMETER(other);
}

FpsCounter::~FpsCounter()
{
}

void FpsCounter::Initialize()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
}

void FpsCounter::Update()
{
	m_count++;
	DWORD currentTime = timeGetTime();
	if (currentTime >= m_startTime + 1000)
	{
		m_fps = m_count;
		m_count = 0;
		m_startTime = currentTime;
	}
}

int FpsCounter::GetFps()
{
	return m_fps;
}