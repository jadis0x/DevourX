#include "pch-il2cpp.h"

#define NOMINMAX

#include "main.h"
#include "version.h"
#include <chrono>
#include <filesystem>
#include <optional>
#include <thread>
#include <cctype>
#include <type_traits>
#include <sstream>
#include <vector>

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

        const std::optional<ReleaseVersionInfo>& GetLatestReleaseInfo()
        {
                static bool fetched = false;
                static std::optional<ReleaseVersionInfo> cachedRelease;

                if (!fetched)
                {
                        cachedRelease = FetchLatestReleaseVersion();
                        fetched = true;
                }

                return cachedRelease;
        }

        std::string NormalizeVersionString(const std::string& version)
        {
                std::string normalized = version;
                while (!normalized.empty() && !std::isdigit(static_cast<unsigned char>(normalized.front())))
                {
                        normalized.erase(normalized.begin());
                }

                return normalized;
        }

        std::string GetLocalModVersion()
        {
                return NormalizeVersionString(BuildInfo::kVersion);
        }

        std::string GetLocalAppVersion()
        {
                return NormalizeVersionString(BuildInfo::kAppVersion);
        }



        void ReportUpdateStatus()
        {
                const auto& latestVersion = GetLatestReleaseInfo();
                if (!latestVersion.has_value())
                {
                        return;
                }

                const ReleaseVersionInfo& releaseInfo = latestVersion.value();

                const std::string currentModVersion = GetLocalModVersion();
                const std::string installedGameVersion = GetLocalAppVersion();

                if (CompareVersions(currentModVersion, releaseInfo.modVersion) < 0)
                {
                        const std::string message =
                                "A new DevourX release (" + releaseInfo.tag + ") is available.\nYou are running version " +
                                FormatReleaseTag(currentModVersion, installedGameVersion) +
                                ").\nPlease update DevourX before launching.";
                        ShowNotification(message, MB_OK | MB_ICONERROR);
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
                                ").\nPlease update Devour before launching DevourX. DevourX will not load until the game is updated.";
                }
                else
                {
                        message = "Devour has been updated to version " + installedGameVersion +
                                ", but the latest DevourX release (" + releaseInfo.tag + ") only supports Devour " + releaseInfo.appVersion +
                                ").\nDevourX will not load until it is updated.";
                }

                ShowNotification(message, MB_OK | MB_ICONERROR);
        }
}

#define DEFINE_VERSION_WRAPPER(name, ret, params, args)                                       \
        using name##_t = ret(WINAPI*) params;                                                \
        static name##_t o##name = nullptr;                                                   \
        extern "C" ret WINAPI _##name params                                                \
        {                                                                                    \
                if (!o##name)                                                                \
                {                                                                            \
                        SetLastError(ERROR_PROC_NOT_FOUND);                                  \
                        if constexpr (std::is_void_v<ret>)                                  \
                        {                                                                    \
                                return;                                                      \
                        }                                                                    \
                        else                                                                 \
                        {                                                                    \
                                return {};                                                   \
                        }                                                                    \
                }                                                                            \
                return o##name args;                                                         \
        }

DEFINE_VERSION_WRAPPER(GetFileVersionInfoA, BOOL,
        (LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData),
        (lptstrFilename, dwHandle, dwLen, lpData));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoByHandle, BOOL,
        (HANDLE hFile, DWORD dwReserved, DWORD dwLen, LPVOID lpData),
        (hFile, dwReserved, dwLen, lpData));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoExW, BOOL,
        (DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData),
        (dwFlags, lpwstrFilename, dwHandle, dwLen, lpData));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoExA, BOOL,
        (DWORD dwFlags, LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData),
        (dwFlags, lptstrFilename, dwHandle, dwLen, lpData));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoSizeA, DWORD,
        (LPCSTR lptstrFilename, LPDWORD lpdwHandle),
        (lptstrFilename, lpdwHandle));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoSizeExA, DWORD,
        (DWORD dwFlags, LPCSTR lptstrFilename, LPDWORD lpdwHandle),
        (dwFlags, lptstrFilename, lpdwHandle));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoSizeExW, DWORD,
        (DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle),
        (dwFlags, lpwstrFilename, lpdwHandle));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoSizeW, DWORD,
        (LPCWSTR lpwstrFilename, LPDWORD lpdwHandle),
        (lpwstrFilename, lpdwHandle));
DEFINE_VERSION_WRAPPER(GetFileVersionInfoW, BOOL,
        (LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData),
        (lpwstrFilename, dwHandle, dwLen, lpData));
DEFINE_VERSION_WRAPPER(VerFindFileA, DWORD,
        (DWORD dwFlags, LPCSTR lpszFileName, LPCSTR lpszWinDir, LPCSTR lpszAppDir, LPSTR lpszCurDir, PUINT lpuCurDirLen,
                LPSTR lpszDestDir, PUINT lpuDestDirLen),
        (dwFlags, lpszFileName, lpszWinDir, lpszAppDir, lpszCurDir, lpuCurDirLen, lpszDestDir, lpuDestDirLen));
DEFINE_VERSION_WRAPPER(VerFindFileW, DWORD,
        (DWORD dwFlags, LPCWSTR lpwstrFileName, LPCWSTR lpwstrWinDir, LPCWSTR lpwstrAppDir, LPWSTR lpwstrCurDir,
                PUINT lpuCurDirLen, LPWSTR lpwstrDestDir, PUINT lpuDestDirLen),
        (dwFlags, lpwstrFileName, lpwstrWinDir, lpwstrAppDir, lpwstrCurDir, lpuCurDirLen, lpwstrDestDir, lpuDestDirLen));
DEFINE_VERSION_WRAPPER(VerInstallFileA, DWORD,
        (DWORD dwFlags, LPCSTR lpszSrcFileName, LPCSTR lpszDestFileName, LPCSTR lpszSrcDir, LPCSTR lpszDestDir,
                LPCSTR lpszCurDir, LPSTR lpszTmpFile, PUINT lpuTmpFileLen),
        (dwFlags, lpszSrcFileName, lpszDestFileName, lpszSrcDir, lpszDestDir, lpszCurDir, lpszTmpFile, lpuTmpFileLen));
DEFINE_VERSION_WRAPPER(VerInstallFileW, DWORD,
        (DWORD dwFlags, LPCWSTR lpwstrSrcFileName, LPCWSTR lpwstrDestFileName, LPCWSTR lpwstrSrcDir, LPCWSTR lpwstrDestDir,
                LPCWSTR lpwstrCurDir, LPWSTR lpwstrTmpFile, PUINT lpuTmpFileLen),
        (dwFlags, lpwstrSrcFileName, lpwstrDestFileName, lpwstrSrcDir, lpwstrDestDir, lpwstrCurDir, lpwstrTmpFile,
                lpuTmpFileLen));
DEFINE_VERSION_WRAPPER(VerLanguageNameA, DWORD,
        (DWORD wLang, LPSTR szLang, DWORD cchLang),
        (wLang, szLang, cchLang));
DEFINE_VERSION_WRAPPER(VerLanguageNameW, DWORD,
        (DWORD wLang, LPWSTR szLang, DWORD cchLang),
        (wLang, szLang, cchLang));
DEFINE_VERSION_WRAPPER(VerQueryValueA, BOOL,
        (LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen),
        (pBlock, lpSubBlock, lplpBuffer, puLen));
DEFINE_VERSION_WRAPPER(VerQueryValueW, BOOL,
        (LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen),
        (pBlock, lpSubBlock, lplpBuffer, puLen));

#undef DEFINE_VERSION_WRAPPER

template <typename Fn>
void LoadVersionFunction(Fn& function, const char* name)
{
        function = reinterpret_cast<Fn>(GetProcAddress(version_dll, name));
}

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

        LoadVersionFunction(oGetFileVersionInfoA, "GetFileVersionInfoA");
        LoadVersionFunction(oGetFileVersionInfoByHandle, "GetFileVersionInfoByHandle");
        LoadVersionFunction(oGetFileVersionInfoExW, "GetFileVersionInfoExW");
        LoadVersionFunction(oGetFileVersionInfoExA, "GetFileVersionInfoExA");
        LoadVersionFunction(oGetFileVersionInfoSizeA, "GetFileVersionInfoSizeA");
        LoadVersionFunction(oGetFileVersionInfoSizeExW, "GetFileVersionInfoSizeExW");
        LoadVersionFunction(oGetFileVersionInfoSizeExA, "GetFileVersionInfoSizeExA");
        LoadVersionFunction(oGetFileVersionInfoSizeW, "GetFileVersionInfoSizeW");
        LoadVersionFunction(oGetFileVersionInfoW, "GetFileVersionInfoW");
        LoadVersionFunction(oVerFindFileA, "VerFindFileA");
        LoadVersionFunction(oVerFindFileW, "VerFindFileW");
        LoadVersionFunction(oVerInstallFileA, "VerInstallFileA");
        LoadVersionFunction(oVerInstallFileW, "VerInstallFileW");
        LoadVersionFunction(oVerLanguageNameA, "VerLanguageNameA");
        LoadVersionFunction(oVerLanguageNameW, "VerLanguageNameW");
        LoadVersionFunction(oVerQueryValueA, "VerQueryValueA");
        LoadVersionFunction(oVerQueryValueW, "VerQueryValueW");
}

std::filesystem::path getApplicationPath() {
	TCHAR buff[MAX_PATH];
	GetModuleFileName(NULL, buff, MAX_PATH);
	return std::filesystem::path(buff);
}

bool PerformPreInjectionChecks()
{
        ReportUpdateStatus();

        const auto& latestVersion = GetLatestReleaseInfo();
        if (!latestVersion.has_value())
        {
                return true;
        }

        const std::string currentModVersion = GetLocalModVersion();
        const ReleaseVersionInfo& releaseInfo = latestVersion.value();

        if (CompareVersions(currentModVersion, releaseInfo.modVersion) < 0)
        {
                return false;
        }

        return EnsureCompatibleGameVersion();
}

bool EnsureCompatibleGameVersion()
{
        const auto& latestVersion = GetLatestReleaseInfo();
        if (!latestVersion.has_value())
        {
                return true;
        }

        const std::string installedGameVersion = GetLocalAppVersion();
        const ReleaseVersionInfo& releaseInfo = latestVersion.value();

        return CompareVersions(installedGameVersion, releaseInfo.appVersion) == 0;
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
