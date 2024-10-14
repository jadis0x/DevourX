#pragma once

#include <string>

#include <wininet.h>
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

    class RiftNet {
    public:
        RiftNet();
        ~RiftNet();

        // HTTP isteði gönderme fonksiyonu (Asenkron destekli)
        bool SendHTTPRequest(
            HttpMethod method,
            const std::string& url,
            const std::map<std::string, std::string>& headers,
            const std::map<std::string, std::string>& params,
            const std::string& body,
            HttpResponse& response
        );

        // Zaman aþýmý ayarlama
        void SetTimeout(short int timeoutMs);

        // Proxy ayarlama
        void SetProxy(const std::string& proxy);

        // Retry mekanizmasý için deneme sayýsýný ayarlama
        void SetRetryCount(int retryCount);

        // Retry mekanizmasý için gecikme süresini ayarlama
        void SetRetryDelay(int retryDelayMs);

    private:
        short int m_timeoutMs;
        std::string m_proxy;
        int m_retryCount;
        int m_retryDelayMs;

        // URL'yi host ve path olarak ayýrma
        bool ParseURL(const std::string& url, std::string& host, std::string& path, short& port, bool& isSecure);

        // SSL sertifikasýný doðrulama
        bool ValidateResponseCert(HINTERNET hRequest);

        // Baþlýklarý hazýrlama
        std::string PrepareHeaders(const std::map<std::string, std::string>& headers);

        // Parametreleri URL'ye ekleme
        std::string BuildQueryString(const std::map<std::string, std::string>& params);

        // URL encode fonksiyonu
        std::string UrlEncode(const std::string& value);

        // Ýstek detaylarýný yazdýrma
        void LogRequestDetails(
            HttpMethod method,
            const std::string& url,
            const std::map<std::string, std::string>& headers,
            const std::map<std::string, std::string>& params,
            const std::string& body
        );

        // Yanýt detaylarýný yazdýrma
        void LogResponseDetails(const HttpResponse& response);
    };

}