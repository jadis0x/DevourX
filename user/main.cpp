// Generated C++ file by Il2CppInspectorPro - https://github.com/jadis0x
// Custom injected code entry point

#include "pch-il2cpp.h"

#define WIN32_LEAN_AND_MEAN

#include "main.h"
#include <Windows.h>
#include <iostream>
#include "il2cpp-appdata.h"
#include "il2cpp-init.h"
#include <cstdio>
#include "helpers.h"

#include <devour/devourbase.h>
#include <pipeline/hooks/InitHooks.h>
#include <pipeline/localization/LocalizationManager.h>
#include <pipeline/settings.h>

// Set the name of your log file here
extern const LPCWSTR LOG_FILE = L"Logs.txt";

namespace
{
	class ScopedHandle
	{
	public:
		ScopedHandle() = default;
		explicit ScopedHandle(HANDLE handle) noexcept : handle_(handle) {}
		~ScopedHandle() { reset(); }

		ScopedHandle(const ScopedHandle&) = delete;
		ScopedHandle& operator=(const ScopedHandle&) = delete;

		ScopedHandle(ScopedHandle&& other) noexcept : handle_(other.release()) {}
		ScopedHandle& operator=(ScopedHandle&& other) noexcept
		{
			if (this != &other)
			{
				reset(other.release());
			}
			return *this;
		}

		void reset(HANDLE newHandle = nullptr) noexcept
		{
			if (handle_ && handle_ != INVALID_HANDLE_VALUE)
			{
				CloseHandle(handle_);
			}
			handle_ = newHandle;
		}

		[[nodiscard]] HANDLE get() const noexcept { return handle_; }
		[[nodiscard]] bool valid() const noexcept { return handle_ && handle_ != INVALID_HANDLE_VALUE; }
		HANDLE release() noexcept
		{
			HANDLE tmp = handle_;
			handle_ = nullptr;
			return tmp;
		}

	private:
		HANDLE handle_ = nullptr;
	};

	void ShowWarning(const char* title, const std::string& message)
	{
		MessageBoxA(nullptr, message.c_str(), title, MB_OK | MB_ICONWARNING);
	}

	void LogAndShow(const char* logMessage, const char* boxTitle, const std::string& userMessage)
	{
		if (logMessage && *logMessage)
		{
			il2cppi_log_write(logMessage);
		}
		if (!userMessage.empty())
		{
			ShowWarning(boxTitle, userMessage);
		}
	}

	[[nodiscard]] bool AttachIl2Cpp(Il2CppDomain*& outDomain, Il2CppThread*& outThread)
	{
		outDomain = il2cpp_domain_get();
		if (!outDomain)
		{
			LogAndShow("IL2CPP Domain Not Found!", "DevourX", "[ERROR] IL2CPP Domain Not Found!");
			return false;
		}

		outThread = il2cpp_thread_attach(outDomain);
		if (!outThread)
		{
			LogAndShow("Failed to Attach IL2CPP Thread!", "DevourX", "[ERROR] Failed to Attach IL2CPP Thread!");
			return false;
		}

		return true;
	}

} // namespace

HMODULE hModule;
HANDLE hUnloadEvent;

DWORD WINAPI UnloadWatcherThread(LPVOID lpParam)
{
	const HANDLE eventHandle = static_cast<HANDLE>(lpParam);
	if (!eventHandle)
	{
		return 0;
	}

	if (WaitForSingleObject(eventHandle, INFINITE) == WAIT_OBJECT_0)
	{
		std::cout << "[WARNING] Unload signal received.." << std::endl;
		DetourUninitialization();
		fclose(stdout);
		FreeConsole();
		if (hUnloadEvent)
		{
			CloseHandle(hUnloadEvent);
			hUnloadEvent = nullptr;
		}
		FreeLibraryAndExitThread(hModule, 0);
	}

	return 0;
}

void Run(LPVOID lpParam)
{
	hModule = static_cast<HMODULE>(lpParam);

	il2cppi_log_write("Initializing..");
	Localization::Initialize(settings.localizationCulture);

#ifdef _DEBUG
	il2cppi_new_console();
	SetConsoleTitleA("DevourX");
#endif
	init_il2cpp();

	Il2CppDomain* domain = nullptr;
	Il2CppThread* thread = nullptr;
	if (!AttachIl2Cpp(domain, thread))
	{
		return;
	}

	il2cppi_log_write("IL2CPP Thread Attached Successfully.");

	DetourInitilization();

	const auto userId = Base::Steam::GetUserID();
	const std::string message = "[INFO] Logged in as " + std::to_string(userId);
	std::cout << message << std::endl;
	il2cppi_log_write(message.c_str());

	hUnloadEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!hUnloadEvent)
	{
		const DWORD error = GetLastError();
		std::cout << "Failed to create unload event! Error code: " << error << std::endl;
		il2cppi_log_write("Failed to create unload event!");
		return;
	}

	ScopedHandle watcherThread{ CreateThread(nullptr, 0, UnloadWatcherThread, hUnloadEvent, 0, nullptr) };
	if (!watcherThread.valid())
	{
		const DWORD error = GetLastError();
		std::cout << "[ERROR] Unable to create unload monitor thread! Error code: " << error << std::endl;
		il2cppi_log_write("Unable to create unload monitor thread!");
		return;
	}
}

