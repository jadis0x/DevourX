#include "pch-il2cpp.h"

#include "settings.h"
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include "json/json.hpp"
#include "main.h"
#include <algorithm>
#include <pipeline/localization/LocalizationManager.h>

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

	bool configUpdated = false;
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
	}
	catch (const std::exception&)
	{
		return false;
	}

	stream.close();

	if (settings.localizationCulture.empty())
	{
		settings.localizationCulture = Localization::GetFallbackCulture();
		configUpdated = true;
	}
	else
	{
		const auto availableCultures = Localization::GetAvailableCultures();
		const bool cultureKnown = std::find(availableCultures.begin(), availableCultures.end(), settings.localizationCulture) != availableCultures.end();
		if (!cultureKnown)
		{
			settings.localizationCulture = Localization::GetFallbackCulture();
			configUpdated = true;
		}
	}

	if (configUpdated)
	{
		SaveSettingsToConfig();
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
			{ "showLanguagePromptOnStart", settings.showLanguagePromptOnStart }
	};

	stream << document.dump(4);
	return true;
}