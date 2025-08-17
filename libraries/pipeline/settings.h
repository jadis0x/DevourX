#pragma once

#include "keybinds.h"

class Settings {
public:
	KeyBinds::Config KeyBinds = {
		VK_TAB
	};

	int selectedRegion = 0;
	int lobbySize = 32;
	bool isPrivateLobby = false;

	const char* regions[7] = { "TR", "EU", "US", "ASIA", "RU", "JP", "AU" };

	bool ImGuiInitialized = false;
	bool bShowMenu = false;
	bool bEnableUnityLogs = false;

	bool bSteamInitialized = true;

	bool bMouseFollower = false;
	bool bPostFX_Override = false;

	bool bFly = false;
	float flySpeed = 10.f;

	bool exp_modifier = false;
	float newExp = 3000.f;

	bool bAutoCalmWhenNearby = false;
	bool bNeverCalm = false;
	bool bFreezeAzazel = false;
	float azazelSpeed = 0.0f;

	bool bModifyRank = false;
	int32_t newRank = 0;
	bool bUnlockDoors = false;
	bool bFullbright = false;
	bool bUnlimitedUVLight = false;
	bool bDisableLongInteract = false;
	bool bModifyPlayerSpeed = false;
	float playerSpeed = 1.0f;

	bool bConnectionId = false;
	bool bChangeRoomName = false;
	bool bForcePublic = false;
	bool bFakeGameInProgress = false;
	char customRoomName[64] = "-";

	bool bSpoofSteamName = false;
	char customSteamname[64] = "neck hurt";

	bool bSpoofSteamId = false;
	char customSteamIdStr[32] = "";

	int32_t progress = 0;

	bool bAzazelEsp = false;
	bool bAzazelLine = false;
	bool bInteractableEsp = false;
	bool bInteractableEspUpdated = false;

	bool bPlayerLine = false;
	bool bPlayerEsp = false;

	bool bModifyFlashlightColor = false;
	bool bAutoDrop = false;


	float flashlightColor[4] = { 1.0f, 1.f, 1.f, 1.f };

	float MF_offsetX = -5.0f;
	float MF_offsetY = 5.f;
	float MF_Speed = 1.f;
};

extern Settings settings;
