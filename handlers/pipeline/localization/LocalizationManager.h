#pragma once

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

namespace Localization
{
    void Initialize(const std::string& preferredCulture = "");
    bool SetCulture(const std::string& culture);
    [[nodiscard]] const std::string& Get(const std::string& key);

    template<typename... Args>
    [[nodiscard]] std::string Format(const std::string& key, Args&&... args)
    {
        const std::string& formatString = Get(key);
        if (formatString.empty())
        {
            return formatString;
        }

        const int required = std::snprintf(nullptr, 0, formatString.c_str(), std::forward<Args>(args)...);
        if (required <= 0)
        {
            return formatString;
        }

        std::vector<char> buffer(static_cast<size_t>(required) + 1, '\0');
        std::snprintf(buffer.data(), buffer.size(), formatString.c_str(), std::forward<Args>(args)...);
        return std::string(buffer.data());
    }

    [[nodiscard]] const std::string& GetCurrentCulture();
    [[nodiscard]] const std::string& GetFallbackCulture();
    [[nodiscard]] std::vector<std::string> GetAvailableCultures();
    [[nodiscard]] std::string GetDisplayName(const std::string& culture);
}