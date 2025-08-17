// Generated C++ file by Il2CppInspectorPro - https://github.com/jadis0x
// Custom injected code entry point

#include "pch-il2cpp.h"

#define WIN32_LEAN_AND_MEAN

#include "main.h"
#include <Windows.h>
#include <iostream>
#include "il2cpp-appdata.h"
#include <il2cpp-init.h>
#include "helpers.h"
#include <pipeline/hooks/InitHooks.h>
#include "devour/devourbase.h"


// Set the name of your log file here
extern const LPCWSTR LOG_FILE = L"Logs.txt";

HMODULE hModule;
HANDLE hUnloadEvent;

DWORD WINAPI UnloadWatcherThread(LPVOID lpParam)
{
	HANDLE hEvent = static_cast<HANDLE>(lpParam);

	if (WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0)
	{
		std::cout << "[WARNING] Unload signal received.." << std::endl;
		DetourUninitialization();
		fclose(stdout);
		FreeConsole();
		CloseHandle(hUnloadEvent);
		FreeLibraryAndExitThread(hModule, 0);
	}
	return 0;
}

// Custom injected code entry point
void Run(LPVOID lpParam)
{
	hModule = (HMODULE)lpParam;

	Il2CppDomain* _domain;
	Il2CppThread* _thread;

	// If you would like to write to a log file, specify the name above and use il2cppi_log_write()
	il2cppi_log_write("Initializing");


#ifdef _DEBUG
	il2cppi_new_console();
	SetConsoleTitleA("DevourX by Jadis0x");
#endif

	init_il2cpp();

	// Initialize thread data - DO NOT REMOVE
	_domain = il2cpp_domain_get();
	_thread = nullptr;

	// Check if IL2CPP domain is found

	if (!_domain)
	{
		il2cppi_log_write("IL2CPP Domain Not Found!");
		std::cout << "[ERROR] IL2CPP Domain Not Found!" << std::endl;
		MessageBoxA(
			NULL,
			"[ERROR] IL2CPP Domain Not Found!",
			"RiftLab",
			MB_OK | MB_ICONWARNING
		);
		return;
	}

	il2cppi_log_write("IL2CPP Domain Found.");

	_thread = il2cpp_thread_attach(_domain);

	if (!_thread) {
		il2cppi_log_write("Failed to Attach IL2CPP Thread!");
		MessageBoxA(
			NULL,
			"[ERROR] Failed to Attach IL2CPP Thread!",
			"RiftLab",
			MB_OK | MB_ICONWARNING
		);
		return;
	}

	il2cppi_log_write("IL2CPP Thread Attached Successfully.");


	DetourInitilization();

	std::string msg = "[INFO] Logged in as " + std::to_string(Base::Steam::GetUserID());
	std::cout << msg << std::endl;
	il2cppi_log_write(msg);

	hUnloadEvent = CreateEvent(nullptr, FALSE, FALSE, NULL);
	if (hUnloadEvent == nullptr) {
		std::cout << "Failed to create unload event! Error code: " << GetLastError() << std::endl;
		il2cppi_log_write("Failed to create unload event!");
		return;
	}

	DWORD dwThreadId;
	HANDLE hWatcherThread = CreateThread(nullptr, 0, UnloadWatcherThread, hUnloadEvent, 0, &dwThreadId);
	if (hWatcherThread != nullptr)
	{
		CloseHandle(hWatcherThread);
	}
	else
	{
		std::cout << "[ERROR] Unable to create unload monitor thread! Error code: " << GetLastError() << std::endl;
		il2cppi_log_write("Unable to create unload monitor thread!");
		return;
	}
}

