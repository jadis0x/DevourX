#pragma once

#include "keybinds.h"
#include <string>

class Settings {
public:
	KeyBinds::Config KeyBinds = {
		VK_TAB
	};

	std::string localizationCulture = "en-US";
	bool showLanguagePromptOnStart = true;
	bool queueLanguageRestartNotification = false;

	int selectedRegion = 0;
	const char* regions[7] = { "TR", "EU", "US", "ASIA", "RU", "JP", "AU" };
	int lobbySize = 64;
	bool isPrivateLobby = false;

	int32_t newRank = 0;
	int32_t progress = 0;

	float flySpeed = 10.f;
	float newExp = 3000.f;
	float azazelSpeed = 0.0f;
	float playerSpeed = 1.0f;
	float flashlightColor[4] = { 1.0f, 1.f, 1.f, 1.f };

	float MF_offsetX = -5.0f;
	float MF_offsetY = 5.f;
	float MF_Speed = 1.f;

	bool ImGuiInitialized = false;
	bool bShowMenu = false;
	bool bPendingFontReload = false;
	bool bEnableUnityLogs = false;
	bool bSteamInitialized = true;
	bool bMouseFollower = false;
	bool bPostFX_Override = false;
	bool bFly = false;
	bool exp_modifier = false;
	bool bAutoCalmWhenNearby = false;
	bool bNeverCalm = false;
	bool bFreezeAzazel = false;
	bool bModifyRank = false;
	bool bUnlockDoors = false;
	bool bFullbright = false;
	bool bUnlimitedUVLight = false;
	bool bDisableLongInteract = false;
	bool bModifyPlayerSpeed = false;
	bool bGhostMod = false;
	bool bChangeRoomName = false;
	bool bForcePublic = false;
	bool bFakeGameInProgress = false;
	bool bSpoofSteamName = false;
	bool bSpoofSteamId = false;
	bool bAzazelEsp = false;
	bool bAzazelLine = false;
	bool bInteractableEsp = false;
	bool bInteractableEspUpdated = false;
	bool bPlayerLine = false;
	bool bPlayerEsp = false;
	bool bModifyFlashlightColor = false;

	char customRoomName[64] = "-";
	char customSteamname[64] = "hotdogcat";
	char customSteamIdStr[32] = "";

	bool bEnableRenderHook = false;
	int renderQualityMode = 1;
	float dSharpness = 0.2f;
};

extern Settings settings;

bool LoadSettingsFromConfig();
bool SaveSettingsToConfig();