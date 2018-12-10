#include "input.h"

Input::Input()
{
	m_directInput = NULL;
	m_keyboard = NULL;
	m_mouse = NULL;
}

Input::Input(const Input &other)
{
	UNREFERENCED_PARAMETER(other);
}
Input::~Input() {}

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	HRESULT result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE); //DISCL_NONEXCLUSIVE
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	POINT pt;

	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);

	m_mouseX = pt.x;
	m_mouseY = pt.y;

	return true;
}

void Input::Release()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = NULL;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = NULL;
	}
	SAFE_RELEASE(m_directInput);
}

bool Input::Update()
{
	bool result;
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	ProcessInput();

	return true;
}

bool Input::ReadKeyboard()
{
	HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool Input::ReadMouse()
{
	HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

void Input::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if (m_mouseX < 0)
	{
		m_mouseX = 0;
	}
	else if (m_mouseX > m_screenWidth)
	{
		m_mouseX = m_screenWidth;
	}

	if (m_mouseY < 0)
	{
		m_mouseY = 0;
	}
	else if (m_mouseY > m_screenHeight)
	{
		m_mouseY = m_screenHeight;
	}

	for (int i = 0; i < 4; i++)
	{
		m_pressedButtons[i] = m_mouseState.rgbButtons[i] & 0x80;
	}

	m_keyDeltaX = 0;
	m_keyDeltaY = 0;

	if (m_keyboardState[DIK_W] & 0x80)
	{
		m_keyDeltaY = 25;
	}
	else if (m_keyboardState[DIK_S] & 0x80)
	{
		m_keyDeltaY = -25;
	}
	if (m_keyboardState[DIK_A] & 0x80)
	{
		m_keyDeltaX = -25;
	}
	else if (m_keyboardState[DIK_D] & 0x80)
	{
		m_keyDeltaX = 25;
	}
}

bool Input::IsEscPressed() const
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}
	return false;
}

void Input::GetMouseDelta(float &mouseX, float &mouseY) const
{
	mouseX = (float)m_mouseState.lX;
	mouseY = (float)m_mouseState.lY;
}

void Input::GetMousePosition(int &mouseX, int &mouseY) const
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}

void Input::GetKeyboardDelta(float &x, float &y) const
{
	x = m_keyDeltaX;
	y = m_keyDeltaY;
}
