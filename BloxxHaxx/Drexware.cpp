#include <Windows.h>
#include <string>
#include <iostream>

#include "DrexwareUI.hpp"


BOOL APIENTRY DllMain(HMODULE Module, DWORD Reason, void* Reserved) {
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(Module);
		DrexwareMod = Module;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitGUI, NULL, NULL, NULL);
		break;
	}
	case DLL_PROCESS_DETACH: {
		exit(0);
	}
	default: {
		break;
	}
	}
	return TRUE;
}