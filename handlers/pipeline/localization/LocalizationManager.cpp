#include "pch-il2cpp.h"

#include "LocalizationManager.h"

#include <Windows.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"
#include "main.h"

namespace
{
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    constexpr const char* kLocalizationDirectory = "localization";
    const std::string kFallbackCultureCode = "en-US";

    std::mutex gLocalizationMutex;
    bool gInitialized = false;
    fs::path gLocalizationRoot;
    std::unordered_map<std::string, std::string> gFallbackStrings;
    std::unordered_map<std::string, std::string> gCurrentStrings;
    std::unordered_map<std::string, std::string> gMissingStrings;
    std::string gCurrentCulture = kFallbackCultureCode;

    void FlattenJson(const json& node, const std::string& prefix, std::unordered_map<std::string, std::string>& out)
    {
        if (!node.is_object())
        {
            return;
        }

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            const std::string newKey = prefix.empty() ? it.key() : prefix + "." + it.key();
            if (it->is_object())
            {
                FlattenJson(*it, newKey, out);
            }
            else if (it->is_string())
            {
                out[newKey] = it->get<std::string>();
            }
            else
            {
                out[newKey] = it->dump();
            }
        }
    }

    fs::path ResolveLocalizationRoot()
    {
        if (!gLocalizationRoot.empty())
        {
            return gLocalizationRoot;
        }

        char modulePath[MAX_PATH] = { 0 };
        if (GetModuleFileNameA(hModule, modulePath, MAX_PATH) == 0)
        {
            return {};
        }

        gLocalizationRoot = fs::path(modulePath).parent_path() / kLocalizationDirectory;
        return gLocalizationRoot;
    }

    std::unordered_map<std::string, std::string> LoadCultureFile(const std::string& culture)
    {
        std::unordered_map<std::string, std::string> entries;

        const fs::path root = ResolveLocalizationRoot();
        if (root.empty())
        {
            return entries;
        }

        const fs::path filePath = root / (culture + ".json");
        std::ifstream fileStream(filePath);
        if (!fileStream.is_open())
        {
            return entries;
        }

        try
        {
            json document;
            fileStream >> document;
            FlattenJson(document, "", entries);
        }
        catch (const std::exception&)
        {
            entries.clear();
        }

        return entries;
    }

    void EnsureInitializedLocked()
    {
        if (gInitialized)
        {
            return;
        }

        gFallbackStrings = LoadCultureFile(kFallbackCultureCode);
        gCurrentStrings = gFallbackStrings;
        gCurrentCulture = kFallbackCultureCode;
        gInitialized = true;
    }

    bool SetCultureInternal(const std::string& culture)
    {
        if (culture.empty() || culture == gCurrentCulture)
        {
            return true;
        }

        auto loaded = LoadCultureFile(culture);
        if (loaded.empty())
        {
            return false;
        }

        gCurrentCulture = culture;
        gCurrentStrings = std::move(loaded);
        return true;
    }
}

namespace Localization
{
    void Initialize(const std::string& preferredCulture)
    {
        std::scoped_lock lock(gLocalizationMutex);
        EnsureInitializedLocked();

        if (!preferredCulture.empty())
        {
            if (!SetCultureInternal(preferredCulture))
            {
                gCurrentStrings = gFallbackStrings;
                gCurrentCulture = kFallbackCultureCode;
            }
        }
    }

    bool SetCulture(const std::string& culture)
    {
        std::scoped_lock lock(gLocalizationMutex);
        EnsureInitializedLocked();

        const bool success = SetCultureInternal(culture);
        if (!success)
        {
            gCurrentStrings = gFallbackStrings;
            gCurrentCulture = kFallbackCultureCode;
        }
        return success;
    }

    const std::string& Get(const std::string& key)
    {
        std::scoped_lock lock(gLocalizationMutex);
        EnsureInitializedLocked();

        if (const auto currentIt = gCurrentStrings.find(key); currentIt != gCurrentStrings.end())
        {
            return currentIt->second;
        }

        if (const auto fallbackIt = gFallbackStrings.find(key); fallbackIt != gFallbackStrings.end())
        {
            return fallbackIt->second;
        }

        auto [missingIt, _] = gMissingStrings.emplace(key, key);
        return missingIt->second;
    }

    const std::string& GetCurrentCulture()
    {
        std::scoped_lock lock(gLocalizationMutex);
        EnsureInitializedLocked();
        return gCurrentCulture;
    }

    const std::string& GetFallbackCulture()
    {
        return kFallbackCultureCode;
    }

    std::vector<std::string> GetAvailableCultures()
    {
        std::scoped_lock lock(gLocalizationMutex);
        ResolveLocalizationRoot();

        std::vector<std::string> cultures;
        if (gLocalizationRoot.empty())
        {
            return cultures;
        }

        if (!fs::exists(gLocalizationRoot) || !fs::is_directory(gLocalizationRoot))
        {
            return cultures;
        }

        std::error_code iteratorError;
        for (const auto& entry : fs::directory_iterator(gLocalizationRoot, iteratorError))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            if (entry.path().extension() != ".json")
            {
                continue;
            }

            const std::string stem = entry.path().stem().string();
            if (stem.empty())
            {
                continue;
            }

            if (stem == "config" || stem.find('.') != std::string::npos)
            {
                continue;
            }

            cultures.emplace_back(stem);
        }

        if (iteratorError)
        {
            cultures.clear();
            return cultures;
        }

        std::sort(cultures.begin(), cultures.end());
        return cultures;
    }

    std::string GetDisplayName(const std::string& culture)
    {
        return Get("languages." + culture);
    }
}