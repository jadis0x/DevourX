// Generated C++ file by Il2CppInspectorPro - https://github.com/jadis0x
// DLL entry point

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "version.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Load, hModule, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		if (version_dll)
		{
			FreeLibrary(version_dll);
			version_dll = nullptr;
		}
		break;
	default:
		break;
	}
	return TRUE;
}