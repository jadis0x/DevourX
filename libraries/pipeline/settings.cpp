#include "pch-il2cpp.h"

#include "settings.h"
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include "nlohmann/json.hpp"
#include "main.h"

Settings settings;

namespace
{
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    fs::path gConfigPath;

    fs::path ResolveConfigPath()
    {
        if (!gConfigPath.empty())
        {
            return gConfigPath;
        }

        char modulePath[MAX_PATH] = { 0 };
        if (GetModuleFileNameA(hModule, modulePath, MAX_PATH) == 0)
        {
            return {};
        }

        gConfigPath = fs::path(modulePath).parent_path() / "config.json";
        return gConfigPath;
    }
}

bool LoadSettingsFromConfig()
{
    const fs::path configPath = ResolveConfigPath();
    if (configPath.empty())
    {
        return false;
    }

    std::ifstream stream(configPath);
    if (!stream.is_open())
    {
        return false;
    }

    try
    {
        json document;
        stream >> document;

        if (const auto it = document.find("localizationCulture"); it != document.end() && it->is_string())
        {
            settings.localizationCulture = it->get<std::string>();
        }

        if (const auto it = document.find("showLanguagePromptOnStart"); it != document.end() && it->is_boolean())
        {
            settings.showLanguagePromptOnStart = it->get<bool>();
        }

        if (const auto it = document.find("enable_RenderHook"); it != document.end() && it->is_boolean())
        {
            settings.bEnableRenderHook = it->get<bool>();
        }

        if (const auto it = document.find("render_QualityMode"); it != document.end() && it->is_number_integer())
        {
            settings.renderQualityMode = std::clamp(it->get<int>(), 0, 3);
        }

        if (const auto it = document.find("render_Sharpness"); it != document.end() && it->is_number())
        {
            settings.dSharpness = std::clamp(it->get<float>(), 0.0f, 1.0f);
        }

    }
    catch (const std::exception&)
    {
        return false;
    }

    if (settings.localizationCulture.empty())
    {
        settings.localizationCulture = "en-US";
    }

    return true;
}

bool SaveSettingsToConfig()
{
    const fs::path configPath = ResolveConfigPath();
    if (configPath.empty())
    {
        return false;
    }

    std::error_code directoryError;
    const fs::path directory = configPath.parent_path();
    if (!directory.empty())
    {
        fs::create_directories(directory, directoryError);
    }

    std::ofstream stream(configPath, std::ios::trunc);
    if (!stream.is_open())
    {
        return false;
    }

    json document = {
            { "localizationCulture", settings.localizationCulture },
            { "showLanguagePromptOnStart", settings.showLanguagePromptOnStart },
            { "showLanguagePromptOnStart", settings.showLanguagePromptOnStart },
            { "enable_RenderHook", settings.bEnableRenderHook },
            { "render_QualityMode", settings.renderQualityMode },
            { "render_Sharpness", settings.dSharpness }
    };

    stream << document.dump(4);
    return true;
}