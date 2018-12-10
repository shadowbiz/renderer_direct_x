#ifndef SE_FPS_H
#define SE_FPS_H

#include "globals.h"

class FpsCounter
{
  public:
	FpsCounter();
	FpsCounter(const FpsCounter &other);
	virtual ~FpsCounter();

	void Initialize();
	void Update();
	int GetFps();

  private:
	int m_fps;
	int m_count;
	DWORD m_startTime;
};

#endif // !SE_FPS_H
