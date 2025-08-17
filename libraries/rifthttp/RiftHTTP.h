#ifndef RIFT_HTTP_H
#define RIFT_HTTP_H

#include <windows.h>
#include <wininet.h>

#include <string>
#include <map>

namespace Rift {

    enum class HttpMethod {
        GET,
        POST,
        PUT,
        DEL
    };

    struct HttpResponse {
        long statusCode;
        std::string body;
        std::map<std::string, std::string> headers;
        std::string errorMessage;
    };

    class RiftHTTP {
    public:
        RiftHTTP();
        ~RiftHTTP();

        bool SendHTTPRequest(
            HttpMethod method,
            const std::string& url,
            const std::map<std::string, std::string>& headers,
            const std::map<std::string, std::string>& params,
            const std::string& body,
            HttpResponse& response
        );

        void SetTimeout(short int timeoutMs);

        void SetProxy(const std::string& proxy);

        void SetRetryCount(int retryCount);

        void SetRetryDelay(int retryDelayMs);

    private:
        short int m_timeoutMs;
        std::string m_proxy;
        int m_retryCount;
        int m_retryDelayMs;

        bool ParseURL(const std::string& url, std::string& host, std::string& path, short& port, bool& isSecure);

        bool ValidateResponseCert(HINTERNET hRequest);

        std::string PrepareHeaders(const std::map<std::string, std::string>& headers);

        std::string BuildQueryString(const std::map<std::string, std::string>& params);

        std::string UrlEncode(const std::string& value);

        void LogRequestDetails(
            HttpMethod method,
            const std::string& url,
            const std::map<std::string, std::string>& headers,
            const std::map<std::string, std::string>& params,
            const std::string& body
        );

        void LogResponseDetails(const HttpResponse& response);
    };

}

#endif // RIFT_HTTP_H