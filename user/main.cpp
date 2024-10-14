// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Custom injected code entry point

#include "pch-il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include "il2cpp-appdata.h"
#include "helpers.h"
#include <filesystem>
#include "main.h"
#include <il2cpp-init.h>

// custom headers
#include <ClientHelper.h>
#include <players/players.h>
#include <UnityEngine/Object.h>
#include "color.hpp"
#include <chrono>
#include <thread>

#include "json.hpp"
#include <hooks/hooks.hpp>

// Set the name of your log file here
extern const LPCWSTR LOG_FILE = L"DevourX-log.txt";

using std::cout;
using std::endl;
using json = nlohmann::json;

HMODULE myhModule = NULL;
HMODULE version_dll = NULL;

#define WRAPPER_FUNC(name) o##name = GetProcAddress(version_dll, #name)

#ifdef _M_AMD64
#pragma warning (disable: 4081)
#define STRINGIFY(name) #name
#define EXPORT_FUNCTION comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
#define WRAPPER_GENFUNC(name) \
    FARPROC o##name; \
    __declspec(dllexport) void WINAPI _##name() \
    { \
        __pragma(STRINGIFY(EXPORT_FUNCTION)); \
        o##name(); \
    }
#else
#define WRAPPER_GENFUNC(name) \
	FARPROC o##name; \
	__declspec(naked) void _##name() \
	{ \
		__asm jmp[o##name] \
	}
#endif

WRAPPER_GENFUNC(GetFileVersionInfoA);
WRAPPER_GENFUNC(GetFileVersionInfoByHandle);
WRAPPER_GENFUNC(GetFileVersionInfoExW);
WRAPPER_GENFUNC(GetFileVersionInfoExA);
WRAPPER_GENFUNC(GetFileVersionInfoSizeA);
WRAPPER_GENFUNC(GetFileVersionInfoSizeExA);
WRAPPER_GENFUNC(GetFileVersionInfoSizeExW);
WRAPPER_GENFUNC(GetFileVersionInfoSizeW);
WRAPPER_GENFUNC(GetFileVersionInfoW);
WRAPPER_GENFUNC(VerFindFileA);
WRAPPER_GENFUNC(VerFindFileW);
WRAPPER_GENFUNC(VerInstallFileA);
WRAPPER_GENFUNC(VerInstallFileW);
WRAPPER_GENFUNC(VerLanguageNameA);
WRAPPER_GENFUNC(VerLanguageNameW);
WRAPPER_GENFUNC(VerQueryValueA);
WRAPPER_GENFUNC(VerQueryValueW);

void load_version() {
	char systemPath[MAX_PATH];
	GetSystemDirectoryA(systemPath, MAX_PATH);
	strcat_s(systemPath, "\\version.dll");
	version_dll = LoadLibraryA(systemPath);

	if (!version_dll) return;

	WRAPPER_FUNC(GetFileVersionInfoA);
	WRAPPER_FUNC(GetFileVersionInfoByHandle);
	WRAPPER_FUNC(GetFileVersionInfoExW);
	WRAPPER_FUNC(GetFileVersionInfoExA);
	WRAPPER_FUNC(GetFileVersionInfoSizeA);
	WRAPPER_FUNC(GetFileVersionInfoSizeExW);
	WRAPPER_FUNC(GetFileVersionInfoSizeExA);
	WRAPPER_FUNC(GetFileVersionInfoSizeW);
	WRAPPER_FUNC(GetFileVersionInfoW);
	WRAPPER_FUNC(VerFindFileA);
	WRAPPER_FUNC(VerFindFileW);
	WRAPPER_FUNC(VerInstallFileA);
	WRAPPER_FUNC(VerInstallFileW);
	WRAPPER_FUNC(VerLanguageNameA);
	WRAPPER_FUNC(VerLanguageNameW);
	WRAPPER_FUNC(VerQueryValueA);
	WRAPPER_FUNC(VerQueryValueW);
}

#pragma execution_character_set("utf-8");

void EnableNolanBehaviour();

void Load(LPVOID lpParam) {
	load_version();
	if (!version_dll) {
		MessageBox(NULL, L"Version.dll failed to load!", L"Rift Loader", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
		return;
	}

	Sleep(9000);
	init_il2cpp();
	Il2CppDomain* domain = NULL;
	while (!domain) {
		domain = il2cpp_domain_get();
		Sleep(1000); 
	}

	il2cpp_thread_attach(domain);
	il2cppi_new_console();

	std::cout << dye::red("\n\tDevourX\n\t") << __DATE__ << " - " << __TIME__ << std::endl;

	std::cout << "\tDevour Version ";
	if (app::Application_get_version != nullptr)
		std::cout << dye::aqua("v") << dye::aqua(il2cppi_to_string(app::Application_get_version(NULL))) << "\n\n";
	else
		std::cout << "-\n\n";

	std::cout << dye::green("\tMade with <3 by ALittlePatate and Jadis0x.\n\n");
	std::cout << "[DevourX]: " << dye::white("GitHub: https://github.com/jadis0x/DevourX") << "\n";


	app::CSteamID steamUserID = app::SteamUser_GetSteamID(nullptr);
	std::string steamName = il2cppi_to_string(app::SteamFriends_GetPersonaName(nullptr));

	std::cout << "[DevourX]: Logged in as " << dye::green(steamName) << " (" << steamUserID.m_SteamID << ")\n\n";

	std::cout << "[DevourX]: " << dye::yellow("Initializing..\n");
	il2cppi_log_write("Initializing..");

	if (InitializeHooks()) {
		il2cppi_log_write("Hooks initialized");
		std::cout << "[DevourX]: " << dye::yellow("Hooks initialized.\n");
	}
	else {
		il2cppi_log_write("MH_Initialize failed!");
		std::cout << "[DevourX]: " << dye::yellow("MH_Initialize failed!\n");
		return;
	}

	CreateHooks();

	if (HookDX11()) {
		il2cppi_log_write("DirectX11 hooked");
		std::cout << "[DevourX]: " << dye::yellow("DirectX11 hooked.\n");
	}
	else {
		il2cppi_log_write("DirectX11 hook failed");
		std::cout << "[DevourX]: " << dye::yellow("DirectX11 hook failed!\n");
		return;
	}

	std::cout << "[DevourX]: " << dye::yellow("Done! Press F1 to open Menu\n\n");

	std::thread nolanThread(EnableNolanBehaviour);

	if (nolanThread.joinable()) {
		nolanThread.join();
	}
}

void EnableNolanBehaviour()
{
	if (SceneName() == std::string("Menu") && Players::GetLocalPlayer() != nullptr) {
		app::NolanBehaviour* nolan = Players::GetNolan();
		if (nolan != nullptr && !Object::Enabled(nolan)) {
			Object::Enabled(nolan, true);
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // Check every 5 seconds
}
