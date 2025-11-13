#pragma once

#include "keybinds.h"
#include "pch-il2cpp.h"
#include <vector>
#include <string>


struct CustomTeleportLocation {
	std::string name;
	app::Vector3 position;
	bool isValid = false;
};
class Settings {
public:
	KeyBinds::Config KeyBinds = {
		VK_TAB
	};

	int selectedRegion = 0;
	const char* regions[7] = { "TR", "EU", "US", "ASIA", "RU", "JP", "AU" };
	int lobbySize = 64;
	bool isPrivateLobby = false;



	std::vector<CustomTeleportLocation> customTeleportLocations;
	int selectedTeleportIndex = -1;
	bool bTriggerCustomTeleport = false;

	int32_t newRank = 0;
	int32_t progress = 0;

	// 运物宏设置
	bool bCarryMacro = false;
	int carryMacroKey = 0; // 0: 鼠标左键, 1: E键
	// 传送点保存/加载
	void SaveTeleportLocations();
	void LoadTeleportLocations();
	// 滤镜设置保存/加载
	void SaveFilterSettings();
	void LoadFilterSettings();
	// 新增滤镜设置
	// 高级滤镜设置
	bool bBrightnessFilter = false;
	float brightnessIntensity = 2.0f;
	float contrast = 1.2f;
	float gamma = 1.8f;

	// 新增核心参数
	float saturation = 1.0f;        // 饱和度

	float temperature = 0.0f;       // 色温

	bool bMonsterIgnoreMe = false;           // 怪物无视我
	int monsterIgnoreLayer = 9;              // 固定第9层
	int monsterIgnoreVKeyMode = 1;           // V键模式: 0=关闭, 1=切换, 2=按住


	float flySpeed = 10.f;
	float newExp = 3000.f;
	float azazelSpeed = 0.0f;
	float playerSpeed = 1.0f;
	float flashlightColor[4] = { 1.0f, 1.f, 1.f, 1.f };
	//跳跃设置
	bool bSuperJump = false;
	float superJumpMultiplier = 3.0f;
	int superJumpKey = 0; // 0=Q键, 1=侧键1, 2=侧键2, 3=侧键3, 4=侧键4, 5=侧键5

	float MF_offsetX = -5.0f;
	float MF_offsetY = 5.f;
	float MF_Speed = 1.f;
	bool bModifyJumpHeight = false;
	float jumpHeight = 1.0f;  // 默认跳跃高度倍数
	bool bSimpleDemonEsp = false;
	bool ImGuiInitialized = false;
	bool bShowMenu = false;
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
	//bool bGhostMod = false;
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
	bool bUnlockAll = false;
	char customRoomName[64] = "-";
	char customSteamname[64] = "hotdogcat";
	char customSteamIdStr[32] = "";

};

extern Settings settings;
