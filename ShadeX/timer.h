#ifndef SE_TIMER_H
#define SE_TIMER_H

#include "globals.h"

class Timer
{
public:
			Timer();
			Timer(const Timer& other);
	virtual	~Timer();

	bool	Initialize();
	void	Update();

	float	GetTime();

private:
	INT64	m_frequency;
	float	m_ticksPerMs;
	INT64	m_startTime;
	float	m_frameTime;
};

#endif // !SE_TIMER_H
