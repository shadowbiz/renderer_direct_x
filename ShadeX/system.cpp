#include "system.h"

System::System()
{
	m_input = NULL;
	m_graphics = NULL;
	m_fps = NULL;
	m_timer = NULL;
	m_cpu = NULL;
}

System::System(const System &other)
{
	UNREFERENCED_PARAMETER(other);
}

System::~System()
{
}

bool System::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	ofstream os("Loading.txt");
	double startTime = GetTime();

	os << "Start\n";

	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	InitializeWindows(hInstance, nCmdShow, screenWidth, screenHeight);

	os << "Windows initialized in " << GetTime() - startTime << " sec \n";

	m_input = new Input;
	if (!m_input)
	{
		return false;
	}
	result = m_input->Initialize(hInstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	os << "Input initialized in " << GetTime() - startTime << " sec\n";

	m_graphics = new Graphics;
	if (!m_graphics)
	{
		return false;
	}
	result = m_graphics->Initialize(m_hwnd);
	if (!result)
	{
		return false;
	}

	os << "Graphics initialized in " << GetTime() - startTime << " sec\n";

	m_fps = new FpsCounter;
	if (!m_fps)
	{
		return false;
	}
	m_fps->Initialize();

	m_cpu = new CpuUssage;
	if (!m_cpu)
	{
		return false;
	}
	m_cpu->Initialize();

	m_timer = new Timer;
	if (!m_timer)
	{
		return false;
	}
	result = m_timer->Initialize();
	if (!result)
	{
		return false;
	}

	os << "Done\n";
	os.close();

	return true;
}

void System::Release()
{
	SAFE_RELEASE_DELETE(m_graphics);
	SAFE_RELEASE_DELETE(m_input);
	SAFE_DELETE(m_fps);
	SAFE_DELETE(m_timer);
	SAFE_RELEASE_DELETE(m_cpu);
	ReleaseWindows();
}

void System::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

double System::GetTime()
{
	static clock_t start = clock();
	clock_t current = clock();
	double result = (current - start) / (double)CLOCKS_PER_SEC;
	return result;
}

bool System::Update()
{
	static double simulationTime = 0.0;
	simulationTime += m_timer->GetTime();
	if (simulationTime < DELTA_TIME)
	{
		return true;
	}
	simulationTime = 0.0;
	bool result = m_graphics->Update(static_cast<float>(DELTA_TIME), m_input, m_fps->GetFps(), m_cpu->GetCpuPercentage());
	if (!result)
	{
		return false;
	}
	return true;
#if 0
	double realTime = GetTime();
	while (simulationTime < realTime) {
		simulationTime += DELTA_TIME;
		result = m_graphics->Update(static_cast<float>(DELTA_TIME), m_input, m_fps->GetFps(), m_cpu->GetCpuPercentage());
		if(!result) {
			return false;
		}
	}
	return true;
#endif
}

bool System::Render()
{
	bool result = m_graphics->Render();
	if (!result)
	{
		return false;
	}
	return true;
}

bool System::Frame()
{
	m_timer->Update();
	m_fps->Update();
	m_cpu->Update();
	m_input->Update();

	if (m_input->IsEscPressed())
	{
		return false;
	}

	bool result = Update();
	if (!result)
	{
		return false;
	}
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void System::InitializeWindows(HINSTANCE hInstance, int nCmdShow, int &screenWidth, int &screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;
	m_hinstance = hInstance;

	m_applicationName = L"Shade";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DWORD style = WS_MINIMIZEBOX | WS_SYSMENU;

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
		posX = posY = 0;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, style,
							posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, nCmdShow);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
	ShowCursor(false);
}

void System::ReleaseWindows()
{

	ShowCursor(true);
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}
	DestroyWindow(m_hwnd);
	UnregisterClass(m_applicationName, m_hinstance);
	m_hwnd = NULL;
	m_hinstance = NULL;
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}