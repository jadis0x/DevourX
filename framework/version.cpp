#include "pch-il2cpp.h"

#define NOMINMAX

#include "main.h"
#include "version.h"
#include <chrono>
#include <filesystem>
#include <thread>

#include "build_info.h"
#include "winhttp_client.h"

#pragma comment(lib, "winhttp.lib")

HMODULE version_dll;

std::filesystem::path getApplicationPath();

namespace
{
	constexpr const wchar_t* kGithubApiHost = L"api.github.com";

	struct ReleaseVersionInfo
	{
		std::string tag;
		std::string modVersion;
		std::string appVersion;
	};

	void ShowNotification(const std::string& message, UINT flags)
	{
		MessageBoxA(nullptr, message.c_str(), "DevourX", flags | MB_SYSTEMMODAL);
		OutputDebugStringA(message.c_str());
	}

	std::optional<ReleaseVersionInfo> ParseReleaseTag(const std::string& tag)
	{
		ReleaseVersionInfo info{};
		info.tag = tag;

		if (info.tag.empty())
		{
			return std::nullopt;
		}

		std::string normalized = info.tag;
		if (!normalized.empty() && (normalized.front() == 'v' || normalized.front() == 'V'))
		{
			normalized.erase(normalized.begin());
		}

		const auto findSeparator = [&normalized]() -> std::optional<size_t>
			{
				const size_t lower = normalized.find("_a");
				if (lower != std::string::npos)
				{
					return lower;
				}

				const size_t upper = normalized.find("_A");
				if (upper != std::string::npos)
				{
					return upper;
				}

				return std::nullopt;
			};

		const auto separator = findSeparator();
		if (!separator.has_value())
		{
			return std::nullopt;
		}

		const size_t splitIndex = separator.value();
		if (splitIndex == 0 || splitIndex + 2 > normalized.size())
		{
			return std::nullopt;
		}

		info.modVersion = normalized.substr(0, splitIndex);
		info.appVersion = normalized.substr(splitIndex + 2);

		if (info.modVersion.empty() || info.appVersion.empty())
		{
			return std::nullopt;
		}

		return info;
	}

	std::optional<ReleaseVersionInfo> FetchLatestReleaseVersion()
	{
		const std::wstring path = WinHttpClient::ToWide(std::string("/repos/") + BuildInfo::kGitHubOwner + "/" + BuildInfo::kGitHubRepo + "/releases/latest");
		const std::wstring userAgent = WinHttpClient::ToWide(std::string("DevourXUpdater/") + BuildInfo::kVersion);

		auto response = WinHttpClient::HttpGet(kGithubApiHost, path, userAgent,
			L"Accept: application/vnd.github+json\r\nX-GitHub-Api-Version: 2022-11-28\r\n");

		if (!response.has_value())
		{
			return std::nullopt;
		}

		const std::string& json = response.value();

		const std::string key = "\"tag_name\"";
		size_t keyPos = json.find(key);
		if (keyPos == std::string::npos)
		{
			return std::nullopt;
		}

		size_t valueStart = json.find('"', json.find(':', keyPos));
		if (valueStart == std::string::npos)
		{
			return std::nullopt;
		}
		++valueStart;
		size_t valueEnd = json.find('"', valueStart);
		if (valueEnd == std::string::npos || valueEnd <= valueStart)
		{
			return std::nullopt;
		}

		std::string version = json.substr(valueStart, valueEnd - valueStart);

		return ParseReleaseTag(version);
	}

	std::string FormatReleaseTag(const std::string& modVersion, const std::string& appVersion)
	{
		return "v" + modVersion + "_a" + appVersion;
	}

	std::vector<int> ParseVersionComponents(const std::string& version)
	{
		std::vector<int> components;
		std::stringstream ss(version);
		std::string part;

		while (std::getline(ss, part, '.'))
		{
			try
			{
				components.push_back(std::stoi(part));
			}
			catch (...)
			{
				components.push_back(0);
			}
		}

		return components;
	}

	int CompareVersions(const std::string& lhs, const std::string& rhs)
	{
		const auto lhsComponents = ParseVersionComponents(lhs);
		const auto rhsComponents = ParseVersionComponents(rhs);

		const size_t maxSize = std::max(lhsComponents.size(), rhsComponents.size());
		for (size_t i = 0; i < maxSize; ++i)
		{
			const int lhsValue = i < lhsComponents.size() ? lhsComponents[i] : 0;
			const int rhsValue = i < rhsComponents.size() ? rhsComponents[i] : 0;

			if (lhsValue < rhsValue)
			{
				return -1;
			}
			if (lhsValue > rhsValue)
			{
				return 1;
			}
		}

		return 0;
	}



	void ReportUpdateStatus()
	{
		auto latestVersion = FetchLatestReleaseVersion();
		if (!latestVersion.has_value())
		{
			return;
		}

		const ReleaseVersionInfo& releaseInfo = latestVersion.value();

		const std::string currentModVersion = BuildInfo::kVersion;
		const std::string installedGameVersion = BuildInfo::kAppVersion;

		if (CompareVersions(currentModVersion, releaseInfo.modVersion) < 0)
		{
			const std::string message =
				"A new DevourX release (" + releaseInfo.tag + ") is available.\nYou are running version " +
				FormatReleaseTag(currentModVersion, BuildInfo::kAppVersion) +
				").\nVisit the GitHub releases page to download the update.";
			ShowNotification(message, MB_OK | MB_ICONINFORMATION);
		}

		const int compatibility = CompareVersions(installedGameVersion, releaseInfo.appVersion);
		if (compatibility == 0)
		{
			return;
		}

		std::string message;
		if (compatibility < 0)
		{
			message = "DevourX " + releaseInfo.tag + " requires Devour version " + releaseInfo.appVersion +
				").\nYour game reports version " + installedGameVersion +
				").\nPlease update Devour before launching DevourX.";
		}
		else
		{
			message = "Devour has been updated to version " + installedGameVersion +
				", but the latest DevourX release (" + releaseInfo.tag + ") only supports Devour " + releaseInfo.appVersion +
				").\nRunning DevourX may cause crashes until it is updated.";
		}

		ShowNotification(message, MB_OK | MB_ICONERROR);
	}
}

#define WRAPPER_GENFUNC(name) \
    FARPROC o##name; \
    void _##name() \
    { \
        if (o##name) \
            ((void(*)())o##name)(); \
    }

WRAPPER_GENFUNC(GetFileVersionInfoA)
WRAPPER_GENFUNC(GetFileVersionInfoByHandle)
WRAPPER_GENFUNC(GetFileVersionInfoExW)
WRAPPER_GENFUNC(GetFileVersionInfoExA)
WRAPPER_GENFUNC(GetFileVersionInfoSizeA)
WRAPPER_GENFUNC(GetFileVersionInfoSizeExA)
WRAPPER_GENFUNC(GetFileVersionInfoSizeExW)
WRAPPER_GENFUNC(GetFileVersionInfoSizeW)
WRAPPER_GENFUNC(GetFileVersionInfoW)
WRAPPER_GENFUNC(VerFindFileA)
WRAPPER_GENFUNC(VerFindFileW)
WRAPPER_GENFUNC(VerInstallFileA)
WRAPPER_GENFUNC(VerInstallFileW)
WRAPPER_GENFUNC(VerLanguageNameA)
WRAPPER_GENFUNC(VerLanguageNameW)
WRAPPER_GENFUNC(VerQueryValueA)
WRAPPER_GENFUNC(VerQueryValueW)

#define WRAPPER_FUNC(name) o##name = GetProcAddress(version_dll, ###name);

void load_version() {
	char systemPath[MAX_PATH];
	GetSystemDirectoryA(systemPath, MAX_PATH);
	strcat_s(systemPath, "\\version.dll");
	version_dll = LoadLibraryA(systemPath);

	if (!version_dll) {
		std::string message = "Unable to load " + std::string(systemPath);
		MessageBoxA(NULL, message.c_str(), "Title", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}

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

std::filesystem::path getApplicationPath() {
	TCHAR buff[MAX_PATH];
	GetModuleFileName(NULL, buff, MAX_PATH);
	return std::filesystem::path(buff);
}

bool PerformPreInjectionChecks()
{
	ReportUpdateStatus();
	return true;
}

bool EnsureCompatibleGameVersion()
{
	return false;
}

DWORD WINAPI Load(LPVOID lpParam) {

	auto checkDependency = [](const char* dll, const char* runtime) -> bool {
		HMODULE mod = LoadLibraryA(dll);
		if (mod) {
			FreeLibrary(mod);
			return true;
		}
		std::string msg = std::string("Missing dependency: ") + dll +
			"\nPlease install " + runtime + ".";
		MessageBoxA(NULL, msg.c_str(), "Devourx", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		OutputDebugStringA(msg.c_str());
		return false;
		};

	if (!checkDependency("d3d11.dll", "the DirectX 11 runtime") ||
		!checkDependency("dxgi.dll", "the DirectX 11 runtime") ||
		!checkDependency("vcruntime140.dll", "the Microsoft Visual C++ Redistributable") ||
		!checkDependency("vcruntime140_1.dll", "the Microsoft Visual C++ Redistributable") ||
		!checkDependency("msvcp140.dll", "the Microsoft Visual C++ Redistributable")) {
		return 0;
	}

	load_version();
	if (!version_dll)
		return 0;

	if (!PerformPreInjectionChecks())
		return 0;

	std::this_thread::sleep_for(std::chrono::seconds(4));
	Run(lpParam);

	return 0;
}
