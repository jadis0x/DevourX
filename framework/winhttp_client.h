#pragma once

#include <optional>
#include <string>
#include <windows.h>
#include <winhttp.h>

namespace WinHttpClient
{
    class Handle
    {
    public:
        Handle() = default;
        explicit Handle(HINTERNET handle) noexcept : handle_(handle) {}
        ~Handle() { reset(); }

        Handle(const Handle&) = delete;
        Handle& operator=(const Handle&) = delete;

        Handle(Handle&& other) noexcept : handle_(other.release()) {}
        Handle& operator=(Handle&& other) noexcept
        {
            if (this != &other)
            {
                reset(other.release());
            }
            return *this;
        }

        void reset(HINTERNET newHandle = nullptr) noexcept
        {
            if (handle_)
            {
                WinHttpCloseHandle(handle_);
            }
            handle_ = newHandle;
        }

        [[nodiscard]] HINTERNET get() const noexcept { return handle_; }
        [[nodiscard]] bool valid() const noexcept { return handle_ != nullptr; }
        HINTERNET release() noexcept
        {
            HINTERNET tmp = handle_;
            handle_ = nullptr;
            return tmp;
        }

    private:
        HINTERNET handle_ = nullptr;
    };

    inline std::wstring ToWide(const char* value)
    {
        if (!value)
        {
            return {};
        }

        std::wstring result;
        while (*value)
        {
            result.push_back(static_cast<wchar_t>(*value++));
        }

        return result;
    }

    inline std::wstring ToWide(const std::string& value)
    {
        return ToWide(value.c_str());
    }

    inline std::optional<std::string> HttpGet(const std::wstring& host,
        const std::wstring& path,
        const std::wstring& userAgent,
        const std::wstring& headers = {})
    {
        Handle session{ WinHttpOpen(userAgent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
                WINHTTP_NO_PROXY_BYPASS, 0) };
        if (!session.valid())
        {
            return std::nullopt;
        }

        Handle connection{ WinHttpConnect(session.get(), host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0) };
        if (!connection.valid())
        {
            return std::nullopt;
        }

        Handle request{ WinHttpOpenRequest(connection.get(), L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE) };
        if (!request.valid())
        {
            return std::nullopt;
        }

        std::wstring headerBlock;
        if (!userAgent.empty())
        {
            headerBlock += L"User-Agent: ";
            headerBlock += userAgent;
            headerBlock += L"\r\n";
        }
        if (!headers.empty())
        {
            headerBlock += headers;
        }

        if (!headerBlock.empty())
        {
            WinHttpAddRequestHeaders(request.get(), headerBlock.c_str(), static_cast<DWORD>(headerBlock.length()),
                WINHTTP_ADDREQ_FLAG_ADD);
        }

        if (!WinHttpSendRequest(request.get(), WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
            !WinHttpReceiveResponse(request.get(), nullptr))
        {
            return std::nullopt;
        }

        std::string response;
        DWORD available = 0;
        do
        {
            if (!WinHttpQueryDataAvailable(request.get(), &available) || available == 0)
            {
                break;
            }

            std::string buffer(available, '\0');
            DWORD downloaded = 0;
            if (!WinHttpReadData(request.get(), buffer.data(), available, &downloaded))
            {
                break;
            }

            buffer.resize(downloaded);
            response.append(buffer);
        } while (available > 0);

        if (response.empty())
        {
            return std::nullopt;
        }

        return response;
    }
}