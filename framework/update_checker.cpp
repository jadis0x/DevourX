#include "pch-il2cpp.h"

#define NOMINMAX
#include "build_info.h"
#include "helpers.h"
#include "update_checker.h"
#include "winhttp_client.h"

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "winhttp.lib")

namespace
{
    std::vector<int> ParseVersionParts(const std::string& version)
    {
        std::vector<int> parts;
        std::stringstream stream(version);
        std::string token;
        while (std::getline(stream, token, '.'))
        {
            try
            {
                parts.push_back(std::stoi(token));
            }
            catch (...)
            {
                return {};
            }
        }
        return parts;
    }

    int CompareVersions(const std::vector<int>& lhs, const std::vector<int>& rhs)
    {
        const size_t maxSize = std::max(lhs.size(), rhs.size());
        for (size_t i = 0; i < maxSize; ++i)
        {
            const int left = i < lhs.size() ? lhs[i] : 0;
            const int right = i < rhs.size() ? rhs[i] : 0;
            if (left != right)
            {
                return (left < right) ? -1 : 1;
            }
        }
        return 0;
    }

    bool NotifyUser(const std::string& message, NotifyType type = NotifyType::Info)
    {
        UINT flags = MB_SYSTEMMODAL | MB_SETFOREGROUND | MB_TOPMOST;

        AllowSetForegroundWindow(ASFW_ANY);

        HWND owner = GetForegroundWindow();
        const DWORD currentThread = GetCurrentThreadId();
        DWORD ownerThread = 0;
        if (owner)
        {
            ownerThread = GetWindowThreadProcessId(owner, nullptr);
        }

        bool attachedToOwner = false;
        if (owner && ownerThread != 0 && ownerThread != currentThread)
        {
            attachedToOwner = AttachThreadInput(ownerThread, currentThread, TRUE);
        }

        if (owner)
        {
            SetForegroundWindow(owner);
            BringWindowToTop(owner);
        }

        switch (type)
        {
        case NotifyType::Info:
            flags |= MB_ICONINFORMATION | MB_OK;
            break;
        case NotifyType::Warning:
            flags |= MB_ICONWARNING | MB_OK;
            break;
        case NotifyType::Error:
            flags |= MB_ICONERROR | MB_OK;
            break;
        case NotifyType::Confirm:
            flags |= MB_ICONQUESTION | MB_OKCANCEL;
            break;
        }

        il2cppi_log_write(message);

        int result = MessageBoxA(owner ? owner : nullptr, message.c_str(), "DevourX", flags);

        if (attachedToOwner)
        {
            AttachThreadInput(ownerThread, currentThread, FALSE);
        }

        return (result == IDOK);
    }

    bool NotifyUser(const std::string& message)
    {
        return NotifyUser(message, NotifyType::Warning);
    }
}

namespace UpdateChecker
{
    void CheckAndNotify()
    {
        const std::wstring host = L"api.github.com";
        const std::string pathNarrow = std::string("/repos/") + BuildInfo::kGitHubOwner + "/" + BuildInfo::kGitHubRepo + "/releases/latest";
        const std::wstring path = WinHttpClient::ToWide(pathNarrow);
        const std::wstring userAgent = WinHttpClient::ToWide("DevourX-Updater");
        const std::wstring headers = WinHttpClient::ToWide("Accept: application/vnd.github+json\r\n");

        std::optional<std::string> response = WinHttpClient::HttpGet(host, path, userAgent, headers);
        if (!response)
        {
            return;
        }

        const std::string tagToken = "\"tag_name\":\"";
        const size_t tagPos = response->find(tagToken);
        if (tagPos == std::string::npos)
        {
            return;
        }

        const size_t valueStart = tagPos + tagToken.length();
        const size_t valueEnd = response->find('"', valueStart);
        if (valueEnd == std::string::npos)
        {
            return;
        }

        const std::string tag = response->substr(valueStart, valueEnd - valueStart);
        const size_t separatorPos = tag.find('_');
        if (separatorPos == std::string::npos)
        {
            return;
        }

        const std::string remoteModTag = tag.substr(0, separatorPos);
        const std::string remoteAppTag = tag.substr(separatorPos + 1);
        if (remoteModTag.empty() || remoteAppTag.empty())
        {
            return;
        }

        if (remoteModTag.front() != 'v' || remoteAppTag.front() != 'a')
        {
            return;
        }

        const std::string remoteModVersion = remoteModTag.substr(1);
        const std::string remoteAppVersion = remoteAppTag.substr(1);

        const std::string localModVersion = BuildInfo::kVersion[0] == 'v' ? std::string(BuildInfo::kVersion + 1) : std::string(BuildInfo::kVersion);
        const std::string localAppVersion = BuildInfo::kAppVersion[0] == 'a' ? std::string(BuildInfo::kAppVersion + 1) : std::string(BuildInfo::kAppVersion);

        const std::vector<int> remoteModParts = ParseVersionParts(remoteModVersion);
        const std::vector<int> remoteAppParts = ParseVersionParts(remoteAppVersion);
        const std::vector<int> localModParts = ParseVersionParts(localModVersion);
        const std::vector<int> localAppParts = ParseVersionParts(localAppVersion);

        if (remoteModParts.empty() || remoteAppParts.empty() || localModParts.empty() || localAppParts.empty())
        {
            return;
        }

        if (CompareVersions(localModParts, remoteModParts) < 0)
        {
            if (NotifyUser(
                "A new mod update is available!\n\n"
                "Latest version: " + remoteModVersion +
                "\nYour version: " + localModVersion +
                "\n\nWould you like to open the GitHub releases page?",
                NotifyType::Confirm))
            {
                ShellExecuteA(nullptr, "open", "https://github.com/Jadis0x/DevourX/releases", nullptr, nullptr, SW_SHOWNORMAL);
            }
        }

        if (CompareVersions(localAppParts, remoteAppParts) < 0)
        {
            NotifyUser(
                "A new game update has been detected!\n\n"
                "Game version: " + remoteAppVersion +
                "\nYour version: " + localAppVersion +
                "\n\nThe mod may no longer work properly until memory addresses are updated.",
                NotifyType::Error
            );
        }
    }
}