
#include "RiftHTTP.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "wininet.lib")

namespace Rift {

    RiftHTTP::RiftHTTP() : m_timeoutMs(30000), m_proxy(""), m_retryCount(3), m_retryDelayMs(2000) {}

    RiftHTTP::~RiftHTTP() {}

    void RiftHTTP::SetTimeout(short int timeoutMs) {
        this->m_timeoutMs = timeoutMs;
    }

    void RiftHTTP::SetProxy(const std::string& proxy) {
        m_proxy = proxy;
    }

    void RiftHTTP::SetRetryCount(int retryCount) {
        m_retryCount = retryCount;
    }

    void RiftHTTP::SetRetryDelay(int retryDelayMs) {
        m_retryDelayMs = retryDelayMs;
    }

    bool RiftHTTP::ParseURL(const std::string& url, std::string& host, std::string& path, short& port, bool& isSecure) {
        if (url.empty() || url.find("://") == std::string::npos) {
            std::cerr << "[RiftNet]: Invalid URL provided.\n";
            return false;
        }

        isSecure = false;
        size_t protocolEnd = url.find("://");
        if (protocolEnd == std::string::npos) {
            return false;
        }

        std::string protocol = url.substr(0, protocolEnd);
        if (protocol == "https") {
            isSecure = true;
            port = 443;
        }
        else if (protocol == "http") {
            port = 80;
        }
        else {
            return false;
        }

        size_t hostStart = protocolEnd + 3;
        size_t pathStart = url.find('/', hostStart);
        if (pathStart == std::string::npos) {
            host = url.substr(hostStart);
            path = "/";
        }
        else {
            host = url.substr(hostStart, pathStart - hostStart);
            path = url.substr(pathStart);
        }

        size_t portPos = host.find(':');
        if (portPos != std::string::npos) {
            port = static_cast<INTERNET_PORT>(std::stoi(host.substr(portPos + 1)));
            host = host.substr(0, portPos);
        }

        return true;
    }

    bool RiftHTTP::ValidateResponseCert(HINTERNET hRequest) {
        DWORD flags = 0;
        DWORD flagsSize = sizeof(flags);

        if (InternetQueryOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &flags, &flagsSize)) {
            if (!(flags & SECURITY_FLAG_IGNORE_REVOCATION) && !(flags & SECURITY_FLAG_IGNORE_UNKNOWN_CA)) {
                std::cout << "[RiftNet]: SSL Certificate is valid.\n";
                return true;
            }
        }

        std::cerr << "[RiftNet]: Invalid SSL Certificate.\n";
        return false;
    }

    std::string RiftHTTP::UrlEncode(const std::string& value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (auto c : value) {
            if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            }
            else {
                escaped << '%' << std::setw(2) << int((unsigned char)c);
            }
        }

        return escaped.str();
    }

    std::string RiftHTTP::BuildQueryString(const std::map<std::string, std::string>& params) {
        std::ostringstream oss;
        bool first = true;
        for (const auto& [key, value] : params) {
            if (!first) {
                oss << "&";
            }
            first = false;
            oss << UrlEncode(key) << "=" << UrlEncode(value);
        }
        return oss.str();
    }

    std::string RiftHTTP::PrepareHeaders(const std::map<std::string, std::string>& headers) {
        std::ostringstream oss;
        for (const auto& [key, value] : headers) {
            oss << key << ": " << value << "\r\n";
        }
        return oss.str();
    }

    void RiftHTTP::LogRequestDetails(
        HttpMethod method,
        const std::string& url,
        const std::map<std::string, std::string>& headers,
        const std::map<std::string, std::string>& params,
        const std::string& body
    ) {
        std::string methodStr;
        switch (method) {
        case HttpMethod::GET: methodStr = "GET"; break;
        case HttpMethod::POST: methodStr = "POST"; break;
        case HttpMethod::PUT: methodStr = "PUT"; break;
        case HttpMethod::DEL: methodStr = "DELETE"; break;
        default: methodStr = "GET";
        }

        std::cout << "[RiftNet]: Sending " << methodStr << " request to URL: " << url << "\n";

        if (!headers.empty()) {
            std::cout << "[RiftNet]: Headers:\n";
            for (const auto& [key, value] : headers) {
                std::cout << "  " << key << ": " << value << "\n";
            }
        }

        if (!params.empty()) {
            std::cout << "[RiftNet]: Parameters:\n";
            for (const auto& [key, value] : params) {
                std::cout << "  " << key << ": " << value << "\n";
            }
        }

        if (!body.empty()) {
            std::cout << "[RiftNet]: Body:\n" << body << "\n";
        }
    }

    void RiftHTTP::LogResponseDetails(const Rift::HttpResponse& response) {
        std::cout << "[RiftNet]: Response Status Code: " << response.statusCode << "\n";

        if (!response.headers.empty()) {
            std::cout << "[RiftNet]: Response Headers:\n";
            for (const auto& [key, value] : response.headers) {
                std::cout << "  " << key << ": " << value << "\n";
            }
        }

        if (!response.body.empty()) {
            std::cout << "[RiftNet]: Response Body:\n" << response.body << "\n";
        }

        if (!response.errorMessage.empty()) {
            std::cout << "[RiftNet]: Error Message: " << response.errorMessage << "\n";
        }
    }


    bool RiftHTTP::SendHTTPRequest(
        HttpMethod method,
        const std::string& url,
        const std::map<std::string, std::string>& headers,
        const std::map<std::string, std::string>& params,
        const std::string& body,
        HttpResponse& response
    ) {
        HINTERNET hSession = InternetOpenA("RiftNet_Client", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hSession) {
            std::cerr << "[RiftNet]: Failed to open internet session.\n";
            response.errorMessage = "Failed to open internet session.";
            return false;
        }

        // Timeout settings
        InternetSetOption(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &m_timeoutMs, sizeof(m_timeoutMs));
        InternetSetOption(hSession, INTERNET_OPTION_SEND_TIMEOUT, &m_timeoutMs, sizeof(m_timeoutMs));
        InternetSetOption(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &m_timeoutMs, sizeof(m_timeoutMs));

        if (!m_proxy.empty()) {
            INTERNET_PROXY_INFO proxyInfo;
            std::wstring proxyW(m_proxy.begin(), m_proxy.end());
            proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
            proxyInfo.lpszProxy = proxyW.c_str();
            proxyInfo.lpszProxyBypass = NULL;
            InternetSetOption(hSession, INTERNET_OPTION_PROXY, &proxyInfo, sizeof(proxyInfo));
        }

        std::string host, path;
        bool isSecure;
        short port = 80;
        if (!ParseURL(url, host, path, port, isSecure)) {
            std::cerr << "[RiftNet]: Invalid URL format.\n";
            response.errorMessage = "Invalid URL format.";
            InternetCloseHandle(hSession);
            return false;
        }

        std::string queryString = BuildQueryString(params);
        if (!queryString.empty()) {
            path += '?' + queryString;
        }

        HINTERNET hConnect = InternetConnectA(hSession, host.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) {
            std::cerr << "[RiftNet]: Failed to connect to server.\n";
            InternetCloseHandle(hSession);
            return false;
        }

        const char* methodStr;
        switch (method) {
        case HttpMethod::GET:
            methodStr = "GET";
            break;
        case HttpMethod::POST:
            methodStr = "POST";
            break;
        case HttpMethod::PUT:
            methodStr = "PUT";
            break;
        case HttpMethod::DEL:
            methodStr = "DELETE";
            break;
        default:
            methodStr = "GET";
        }

        DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
        if (isSecure) {
            flags |= INTERNET_FLAG_SECURE;
        }

        HINTERNET hRequest = HttpOpenRequestA(hConnect, methodStr, path.c_str(), NULL, NULL, NULL, flags, 0);
        if (!hRequest) {
            std::cerr << "[RiftNet]: Failed to open request.\n";
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hSession);
            return false;
        }

        std::string headerStr = PrepareHeaders(headers);

        std::string postData = (method == HttpMethod::POST || method == HttpMethod::PUT) ? body : "";

        BOOL result = HttpSendRequestA(
            hRequest,
            headerStr.c_str(),
            static_cast<DWORD>(headerStr.length()),
            (method == HttpMethod::POST || method == HttpMethod::PUT) ? (LPVOID)postData.c_str() : NULL,
            (method == HttpMethod::POST || method == HttpMethod::PUT) ? static_cast<DWORD>(postData.length()) : 0
        );

        if (!result) {
            std::cerr << "[RiftNet]: Failed to send request.\n";
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hSession);
            return false;
        }

        char buffer[4096];
        DWORD bytesRead;
        response.body.clear();
        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead != 0) {
            buffer[bytesRead] = '\0';
            response.body += buffer;
        }

        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(statusCode);
        if (HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL)) {
            response.statusCode = statusCode;
        }

        char headerBuffer[4096];
        DWORD headerBufferSize = sizeof(headerBuffer);
        if (HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, headerBuffer, &headerBufferSize, NULL)) {
            response.headers["RawHeaders"] = std::string(headerBuffer, headerBufferSize);
        }

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hSession);

        return true;
    }
}