#ifndef SE_SYSTEMDX_H
#define SE_SYSTEMDX_H

#include "globals.h"
#include "input.h"
#include "graphics.h"
#include "fps.h"
#include "cpuussage.h"
#include "timer.h"

class System
{
  public:
	System();
	System(const System &);
	~System();

	bool Initialize(HINSTANCE, int);
	void Release();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	static double GetTime();

  private:
	bool Frame();
	bool Update();
	bool Render();
	void InitializeWindows(HINSTANCE, int, int &, int &);
	void ReleaseWindows();

  private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input *m_input;
	Graphics *m_graphics;

	FpsCounter *m_fps;
	CpuUssage *m_cpu;
	Timer *m_timer;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static System *ApplicationHandle = 0;

#endif