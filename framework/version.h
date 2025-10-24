#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern HMODULE version_dll;

bool PerformPreInjectionChecks();
DWORD WINAPI Load(LPVOID lpParam);