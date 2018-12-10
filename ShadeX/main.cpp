////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "system.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int iCmdshow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	System *system = new System;
	if (!system)
	{
		return 1;
	}

	bool result = system->Initialize(hInstance, iCmdshow);
	if (result)
	{
		system->Run();
	}

	SAFE_RELEASE_DELETE(system);
	return 0;
}