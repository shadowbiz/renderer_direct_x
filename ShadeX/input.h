#ifndef SE_INPUTDX_H
#define SE_INPUTDX_H

#include "globals.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class Input
{
  public:
	Input();
	Input(const Input &);
	~Input();

	bool Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight);
	void Release();
	bool Update();
	bool IsEscPressed() const;
	void GetMousePosition(int &x, int &y) const;
	void GetMouseDelta(float &x, float &y) const;
	void GetKeyboardDelta(float &x, float &y) const;

  private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

  private:
	IDirectInput8 *m_directInput;
	IDirectInputDevice8 *m_keyboard;
	IDirectInputDevice8 *m_mouse;

	unsigned char m_keyboardState[256];

	DIMOUSESTATE m_mouseState;
	bool m_pressedButtons[4];

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

	float m_keyDeltaX, m_keyDeltaY;
};

#endif