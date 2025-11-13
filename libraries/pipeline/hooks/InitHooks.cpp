#include "pch-il2cpp.h"
#define NOMINMAX


#include <Windows.h>
#include "detours/detours.h"
#include "InitHooks.h"
#include <iostream>
#include "DirectX.h"
#include "pipeline/settings.h"
#include <helpers.h>
#include "resolver/il2cpp_resolver.h"
#include "player/player.h"
#include <devour/devourbase.h>
#include <esp/esp.h>
#include <unordered_map>
#include <pipeline/gui/tabs/ServerBrowserTAB.h>

using app::Debug_2_Log;
using app::Debug_2_LogError;
using app::Debug_2_LogException;
using app::Debug_2_LogWarning;

using app::Object;
using app::Exception;

int GetSuperJumpKey()
{
	switch (settings.superJumpKey)
	{
	case 0: return 'Q';           // Q键
	case 1: return VK_XBUTTON1;   // 鼠标侧键1 (前进)
	case 2: return VK_XBUTTON2;   // 鼠标侧键2 (后退)
	case 3: return 0x06;          // 鼠标侧键3
	case 4: return 0x07;          // 鼠标侧键4
	case 5: return 0x08;          // 鼠标侧键5
	default: return 'Q';          // 默认Q键
	}
}


std::unordered_map<int32_t, std::vector<app::SteamItemDetails_t>> spoofedResults;
app::Menu* globalMenu;

std::vector<int> spoofedItemIDs = {
	121, 11, 8, 10, 6, 7, 16, 24, 14, 15, 4, 48, 12, 17, 28, 13, 23, 31, 38,
	9, 25, 26, 32, 33, 30, 49, 51, 97, 68, 80, 75, 62, 85, 59, 61, 57, 78,
	50, 104, 65, 69, 72, 73, 70, 117, 116, 105, 106, 108
};

static bool fxApplied = false;
static bool bMonsterIgnoreMe = false;

void dDebug_Log(Object* message, MethodInfo* method) {
	if (settings.bEnableUnityLogs)
		std::cout << "[UNITY]: " << ToString(message) << std::endl;

	app::Debug_2_Log(message, method);
}

void dDebug_LogError(Object* message, MethodInfo* method) {
	if (settings.bEnableUnityLogs)
		std::cout << "[UNITY]: " << ToString(message) << std::endl;

	app::Debug_2_LogError(message, method);
}
app::String* dMenu_GetPlayerId(app::Menu* __this, MethodInfo* method)
{
	if (settings.bSpoofSteamId) {
		return convert_to_system_string(settings.customSteamIdStr);
	}
	return app::Menu_GetPlayerId(__this, method);
}


void dDebug_LogException(Exception* exception, MethodInfo* method) {
	if (settings.bEnableUnityLogs)
	{
		std::string excMsg = il2cppi_to_string(exception->fields._message);
		std::cout << "[UNITY]: " << excMsg << std::endl;
	}

	app::Debug_2_LogException(exception, method);
}

void dDebug_LogWarning(app::Object* message, MethodInfo* method) {
	if (settings.bEnableUnityLogs)
		std::cout << "[UNITY]: " << ToString(message) << std::endl;

	app::Debug_2_LogWarning(message, method);
}

bool dCursor_get_visible(MethodInfo* method) {
	if (settings.bShowMenu)
		return true;

	return app::Cursor_1_get_visible(method);
}

void dCursor_set_visible(bool value, MethodInfo* method) {
	if (settings.bShowMenu)
		value = true;

	return app::Cursor_1_set_visible(value, method);
}

app::CursorLockMode__Enum dCursor_get_lockState(MethodInfo* method) {
	if (settings.bShowMenu)
		return app::CursorLockMode__Enum::None;

	return app::Cursor_1_get_lockState(method);
}

void dCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method) {
	if (settings.bShowMenu)
		value = app::CursorLockMode__Enum::None;

	app::Cursor_1_set_lockState(value, method);
}

float dInput_1_GetAxis(app::String* axisName, MethodInfo* method) {
	if (settings.bShowMenu)
		return 0.0f;

	return app::Input_1_GetAxis(axisName, method);
}

void dServerConnectToken_Write(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method) {

	if (settings.bSpoofSteamId)
	{
		__this->fields.playerId = convert_to_system_string(settings.customSteamIdStr);
		__this->fields.survivalPrefabIdPreference = nullptr;
		__this->fields.uniqueId = nullptr;
	}

	if (settings.bSpoofSteamName)
	{
		__this->fields.username = convert_to_system_string(settings.customSteamname);
		__this->fields.survivalPrefabIdPreference = nullptr;
		__this->fields.uniqueId = nullptr;
	}

	app::ServerConnectToken_Write(__this, packet, method);
}
void dServerConnectToken_Read(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method) {
	if (settings.bSpoofSteamId)
	{
		__this->fields.playerId = convert_to_system_string(settings.customSteamIdStr);
		__this->fields.survivalPrefabIdPreference = nullptr;
		__this->fields.uniqueId = nullptr;
	}

	if (settings.bSpoofSteamName)
	{
		__this->fields.username = convert_to_system_string(settings.customSteamname);
		__this->fields.survivalPrefabIdPreference = nullptr;
		__this->fields.uniqueId = nullptr;
	}

	app::ServerConnectToken_Read(__this, packet, method);
}

void dServerDisconnectToken_Read(app::ServerDisconnectToken* __this, app::UdpPacket* packet, MethodInfo* method)
{
	return;
}

float interactableEspTimer = 0.0f;
float playerEspTimer = 0.0f;
float demonEspTimer = 0.0f;  // 添加这一行
constexpr float espRefreshInterval = 6.0f;

void dMenu_Update(app::Menu* __this, MethodInfo* method) {
	globalMenu = __this;

	if (settings.bSpoofSteamName)
	{
		__this->fields.steamName = convert_to_system_string(settings.customSteamname);
	}

	if (settings.bSpoofSteamId)
	{
		__this->fields.steamID = convert_to_system_string(settings.customSteamIdStr);
	}

	app::Menu_Update(__this, method);
}

void dServerAcceptToken_Read(app::ServerAcceptToken* __this, app::UdpPacket* packet, MethodInfo* method) {
	if (settings.bSpoofSteamName)
	{
		auto* newSteamName = convert_to_system_string(settings.customSteamname);

		if (newSteamName)
		{
			__this->fields.username = newSteamName;
		}
	}

	app::ServerAcceptToken_Read(__this, packet, method);
}

void dServerBrowser_Populate(app::ServerBrowser* __this, app::Map_2_System_Guid_UdpSession_* sessions, MethodInfo* method) {
	// global vars
	Base::GlobalVar::__browser = __this;
	g_ServerList.clear();

	app::ServerBrowser_Populate(__this, sessions, method);

	ServerBrowserTAB::CollectServers(sessions);
}

void dPhotonRoomProperties_Read(app::PhotonRoomProperties* __this, app::UdpPacket* packet, MethodInfo* method) {
	if (settings.bForcePublic)
	{
		if (__this && __this->klass)
		{
			__this->fields._IsOpen_k__BackingField = true;
			__this->fields._IsVisible_k__BackingField = true;

			app::PhotonRoomProperties_set_IsOpen(__this, true, nullptr);
			app::PhotonRoomProperties_set_IsVisible(__this, true, nullptr);
		}
	}

	app::PhotonRoomProperties_Read(__this, packet, method);
}

// Steam
app::CSteamID dSteamUser_GetSteamID(MethodInfo* method) {
	auto orgFunc = app::SteamUser_GetSteamID(method);

	if (settings.bSpoofSteamId)
	{
		uint64_t spoofedId = strtoull(settings.customSteamIdStr, nullptr, 10);
		app::CSteamID spoof{ spoofedId };
		spoof.m_SteamID = spoofedId;
		app::CSteamID__ctor_1(&spoof, spoofedId, nullptr);

		return spoof;
	}

	return orgFunc;
}

void dServerBrowser_JoinSession(app::ServerBrowser* __this, app::PhotonSession* photonSession, app::RoomProtocolToken* token, app::String* password, MethodInfo* method) {

	if (settings.bForcePublic)
	{
		token->fields.isPasswordRequired = false;
		token->fields.isPrivate = false;
		token->fields.gameInProgress = false;

		/*
		std::string fullUrl = "https://www.steamcommunity.com/profiles/" + std::to_string(token->fields.hostSteamId);
		app::Application_OpenURL(convert_to_system_string(fullUrl.c_str()), nullptr);
		*/

		photonSession->fields._IsOpen_k__BackingField = true;
		photonSession->fields._IsVisible_k__BackingField = true;

		app::PhotonSession_set_IsVisible(photonSession, true, nullptr);
		app::PhotonSession_set_IsOpen(photonSession, true, nullptr);
	}

	app::ServerBrowser_JoinSession(__this, photonSession, token, password, method);
}


app::String* dSteamFriends_GetPersonaName(MethodInfo* method) {
	auto steamName = app::SteamFriends_GetPersonaName(method);

	if (settings.bSpoofSteamName)
	{
		auto* newSteamName = convert_to_system_string(settings.customSteamname);

		if (newSteamName)
		{
			return newSteamName;
		}
	}

	return steamName;
}


app::TaskStatus__Enum dShouldCalmDown_OnUpdate(app::ShouldCalmDown* __this, MethodInfo* method) {
	if (settings.bNeverCalm)
	{
		__this->fields.disableCalmDown->fields._.mValue = true;
	}
	else
	{
		__this->fields.disableCalmDown->fields._.mValue = false;
	}

	if (settings.bAutoCalmWhenNearby)
	{
		__this->fields.staggerCount->fields._.mValue = 15;
	}

	auto shouldCalmDown = app::ShouldCalmDown_OnUpdate(__this, method);

	return shouldCalmDown;
}
// 添加这些函数到你的 InitHooks.cpp 文件中

app::Byte__Array* dProtocolTokenUtils_ToByteArray(app::IProtocolToken* token, MethodInfo* method)
{
	Il2CppObject* obj = reinterpret_cast<Il2CppObject*>(token);

	Il2CppClass* klass = il2cpp_object_get_class(obj);
	if (!klass) {
		std::cout << "[ToByteArray] il2cpp_object_get_class failed\n";
	}

	const char* ns = il2cpp_class_get_namespace(klass);
	const char* name = il2cpp_class_get_name(klass);
	std::cout << "[ToByteArray] Runtime type: "
		<< (ns ? ns : "(null)") << "."
		<< (name ? name : "(null)") << "\n";

	bool is_value_type = il2cpp_class_is_valuetype(klass) != 0;
	std::cout << "  is_value_type = " << (is_value_type ? "true" : "false") << "\n";

	// Interface listesi
	{
		void* iter = nullptr;
		while (Il2CppClass* ifc = il2cpp_class_get_interfaces(klass, &iter)) {
			std::cout << "  implements: "
				<< (il2cpp_class_get_namespace(ifc) ? il2cpp_class_get_namespace(ifc) : "(null)")
				<< "."
				<< (il2cpp_class_get_name(ifc) ? il2cpp_class_get_name(ifc) : "(null)")
				<< "\n";
		}
	}

	return app::ProtocolTokenUtils_ToByteArray(token, method);
}

void dBoltNetwork_LoadScene_1(app::String* scene, app::IProtocolToken* token, MethodInfo* method)
{
	if (token) {
		const char* className = il2cpp_class_get_name((Il2CppClass*)token->klass);
		const char* classNamespace = il2cpp_class_get_namespace((Il2CppClass*)token->klass);

		printf("Token type: %s.%s\n", classNamespace, className);
	}

	app::BoltNetwork_LoadScene_1(scene, token, method);
}
app::TaskStatus__Enum dPlayersFocusingMeWait_OnUpdate(app::PlayersFocusingMeWait* __this, MethodInfo* method) {
	if (settings.bAutoCalmWhenNearby)
	{
		return app::TaskStatus__Enum::Success;
	}

	return app::PlayersFocusingMeWait_OnUpdate(__this, method);
}

void dSurvivalAzazelBehaviour_AnnaFrying(app::SurvivalAzazelBehaviour* __this, int32_t fryCount, MethodInfo* method) {
	if (settings.bAutoCalmWhenNearby)
	{
		fryCount = 4;
	}

	app::SurvivalAzazelBehaviour_AnnaFrying(__this, fryCount, method);
}

bool dEnemyState_get_MultipleFocus(app::EnemyState* __this, MethodInfo* method) {
	if (settings.bAutoCalmWhenNearby)
	{
		return true;
	}

	return app::EnemyState_get_MultipleFocus(__this, method);
}

void dEnemyState_set_MultipleFocus(app::EnemyState* __this, bool value, MethodInfo* method) {
	if (settings.bAutoCalmWhenNearby)
	{
		value = true;
	}

	app::EnemyState_set_MultipleFocus(__this, value, method);
}



app::UIOutfitSelectionType* dMenu_SetupOutfit(app::Menu* __this, app::CharacterOutfit* outfit, MethodInfo* method) {

	if (settings.bUnlockAll)
	{
		outfit->fields.isHidden = false;
		outfit->fields.isOwned = true;
		outfit->fields.isSupporter = true;
	}

	return app::Menu_SetupOutfit(__this, outfit, method);
}

bool dSteamInventoryManager_UserHasItem(app::SteamInventoryManager* __this, int32_t steamItemDefID, MethodInfo* method) {

	bool orgHasItem = app::SteamInventoryManager_UserHasItem(__this, steamItemDefID, method);

	if (orgHasItem != true && settings.bUnlockAll)
	{
		return true;
	}

	return orgHasItem;
}

app::BoltConfig* dMenu_get_boltConfig(app::Menu* __this, MethodInfo* method) {
	auto* get_Config = app::Menu_get_boltConfig(__this, method);

	// test

	return get_Config;
}

void dUIOutfitSelectionType_SetLocked(app::UIOutfitSelectionType* __this, bool locked, MethodInfo* method) {

	if (settings.bUnlockAll)
	{
		__this->fields.outfitType->fields.isOwned = true;
		__this->fields.outfitType->fields.isSupporter = true;
		__this->fields.outfitType->fields.isHidden = false;

		locked = false;
	}

	app::UIOutfitSelectionType_SetLocked(__this, locked, method);
}

void dCharacterOutfit__ctor(app::CharacterOutfit* __this, MethodInfo* method) {

	if (settings.bUnlockAll)
	{
		__this->fields.isHidden = false;
		__this->fields.isOwned = true;
		__this->fields.isSupporter = true;
	}

	app::CharacterOutfit__ctor(__this, method);
}

void dNolanBehaviour_FixedUpdate(app::NolanBehaviour* __this, MethodInfo* method) {
	if (settings.bPostFX_Override)
	{
		if (!fxApplied && __this && __this->fields.customPostProcessing && __this->fields.colorGradingController && __this->fields.cameraController)
		{
			auto* postProcess = __this->fields.customPostProcessing;
			auto* color = __this->fields.colorGradingController;
			auto* cameraController = __this->fields.cameraController;

			if (cameraController->fields.m_GameObject)
			{	// UltimateBloom

				auto* cameraControllerGO = cameraController->fields.m_GameObject;

				app::String* ultimateBloom = convert_to_system_string("UltimateBloom");

				if (ultimateBloom)
				{
					app::UltimateBloom* ultimateBloomType = reinterpret_cast<app::UltimateBloom*>(app::GameObject_GetComponentByName(cameraControllerGO, ultimateBloom, nullptr));

					if (ultimateBloomType)
					{
						ultimateBloomType->fields.m_BloomIntensity = 1.f;
						ultimateBloomType->fields.m_BloomThreshhold = 1.2f;
						ultimateBloomType->fields.m_ScreenMaxIntensity = 1.0f;

						ultimateBloomType->fields.m_DownscaleCount = 7;
						ultimateBloomType->fields.m_DustIntensity = 4;
						ultimateBloomType->fields.m_DownsamplingQuality = app::UltimateBloom_BloomSamplingQuality__Enum::VeryLargeKernel;
						ultimateBloomType->fields.m_UseBokehFlare = true;
						ultimateBloomType->fields.m_UseLensDust = true;
						ultimateBloomType->fields.m_UseLensFlare = true;
						ultimateBloomType->fields.m_StarFlareIntensity = 0.5f;
						ultimateBloomType->fields.m_FlareTreshold = 3.1f;
						ultimateBloomType->fields.m_StarDownscaleCount = 7;


						ultimateBloomType->fields.m_UpsamplingQuality = app::UltimateBloom_BloomSamplingQuality__Enum::VeryLargeKernel;

					}
				}
			}

			if (postProcess->fields.depthOfField && postProcess->fields.colorGrading)
			{


				postProcess->fields.chromaticAberration->fields.intensity->fields._._.overrideState = true;
				postProcess->fields.chromaticAberration->fields._.active = true;
				postProcess->fields.chromaticAberration->fields.intensity->fields._.value = 0.3f;

				postProcess->fields.depthOfField->fields._.active = true;
				postProcess->fields.depthOfField->fields.aperture->fields._._.overrideState = true;
				postProcess->fields.depthOfField->fields.aperture->fields._.value = 16.f;

				postProcess->fields.depthOfField->fields.focalLength->fields._._.overrideState = true;
				postProcess->fields.depthOfField->fields.focalLength->fields._.value = 70.f;

				postProcess->fields.depthOfField->fields.focusDistance->fields._._.overrideState = true;
				postProcess->fields.depthOfField->fields.focusDistance->fields._.value = 2.0f;

				postProcess->fields.depthOfField->fields.kernelSize->fields._._.overrideState = true;
				postProcess->fields.depthOfField->fields.kernelSize->fields._.value = app::KernelSize__Enum::VeryLarge;

				color->fields.colorGrading->fields._.active = true;
				color->fields.colorGrading->fields.brightness->fields._._.overrideState = true;
				color->fields.colorGrading->fields.brightness->fields._.value = 2.f;

				color->fields.colorGrading->fields.contrast->fields._._.overrideState = true;
				color->fields.colorGrading->fields.contrast->fields._.value = 25.f;

				color->fields.colorGrading->fields.postExposure->fields._._.overrideState = true;
				color->fields.colorGrading->fields.postExposure->fields._.value = 0.5f;

				color->fields.colorGrading->fields.temperature->fields._._.overrideState = true;
				color->fields.colorGrading->fields.temperature->fields._.value = -20.f;

				color->fields.colorGrading->fields.tint->fields._._.overrideState = true;
				color->fields.colorGrading->fields.tint->fields._.value = 1.f;

				fxApplied = true;
				std::cout << "[PostFX] Applied post-processing overrides.\n";
			}
		}
	}

	if (settings.bInteractableEsp)
	{
		interactableEspTimer += app::Time_1_get_deltaTime(nullptr);
		if (interactableEspTimer >= espRefreshInterval)
		{
			interactableEspTimer = 0.0f;

			auto& esp = ESPManager::GetInteractables();
			esp.Reset();
			esp.Update(nullptr);
			settings.bInteractableEspUpdated = true;
		}
	}
	else
	{
		interactableEspTimer = 0.0f;
	}

	if (settings.bPlayerEsp)
	{
		playerEspTimer += app::Time_1_get_deltaTime(nullptr);
		if (playerEspTimer >= espRefreshInterval)
		{
			playerEspTimer = 0.0f;

			auto& esp = ESPManager::GetPlayers();

			app::Survival* survival = __this->fields.m_Survival;
			if (survival)
			{
				esp.Reset();
				esp.Update(reinterpret_cast<app::Object_1*>(survival));
			}
		}
	}
	else
	{
		playerEspTimer = 0.0f;
	}
	// 添加恶魔生物ESP的更新
	if (settings.bSimpleDemonEsp)
	{
		static float demonEspTimer = 0.0f;
		demonEspTimer += app::Time_1_get_deltaTime(nullptr);
		if (demonEspTimer >= espRefreshInterval)
		{
			demonEspTimer = 0.0f;

			auto& esp = ESPManager::GetSimpleDemons();
			esp.Reset();
			esp.Update(nullptr);
		}
	}
	if (PlayerHelper::IsLocalPlayer(__this))
	{
		if (settings.bModifyRank)
		{
			if (app::GameObject* thisGO = __this->fields.gameController)
			{
				Base::Gameplay::setRank(thisGO, settings.newRank);
			}
		}

		if (settings.bFullbright)
		{
			Base::Gameplay::FullBright(__this);
		}
	}
	// 亮度滤镜功能 - 应该放在这里，独立于冻结恶魔
	if (settings.bBrightnessFilter && PlayerHelper::IsLocalPlayer(__this)) {
		if (__this->fields.colorGradingController && __this->fields.colorGradingController->fields.colorGrading) {
			auto* colorGrading = __this->fields.colorGradingController->fields.colorGrading;
			// 基础调整
			colorGrading->fields.brightness->fields._._.overrideState = true;
			colorGrading->fields.brightness->fields._.value = settings.brightnessIntensity;

			colorGrading->fields.contrast->fields._._.overrideState = true;
			colorGrading->fields.contrast->fields._.value = settings.contrast;

			colorGrading->fields.postExposure->fields._._.overrideState = true;
			colorGrading->fields.postExposure->fields._.value = settings.gamma - 1.0f;

			// 色彩调整
			colorGrading->fields.saturation->fields._._.overrideState = true;
			colorGrading->fields.saturation->fields._.value = settings.saturation;

			// 白平衡
			colorGrading->fields.temperature->fields._._.overrideState = true;
			colorGrading->fields.temperature->fields._.value = settings.temperature * 100.0f;

			// 确保滤镜激活
			colorGrading->fields._.active = true;
		}
	}


	if (settings.bFreezeAzazel && Base::DevourNet::IsHost())
	{
		app::GameObject* azazelGameObject = __this->fields.m_Survival->fields.m_Azazel;

		if (azazelGameObject)
		{
			auto component = app::GameObject_GetComponentByName(azazelGameObject, convert_to_system_string("UltimateCharacterLocomotion"), nullptr);

			if (component)
			{
				app::UltimateCharacterLocomotion* locomotion = reinterpret_cast<app::UltimateCharacterLocomotion*>(component);

				if (locomotion)
				{

					app::UltimateCharacterLocomotion_set_TimeScale(locomotion, settings.azazelSpeed, NULL);
				}
			}

		}
	}

	return app::NolanBehaviour_FixedUpdate(__this, method);
}




void dNolanBehaviour_Update(app::NolanBehaviour* __this, MethodInfo* method) {
	static app::NolanBehaviour* local_nb = nullptr;
	static app::Component* cachedLocomotion = nullptr;

	auto reset_local_state = [&]()
	{
		local_nb = nullptr;
		cachedLocomotion = nullptr;
	};

	if (local_nb && !SafePtr::IsValid(reinterpret_cast<app::Object_1*>(local_nb)))
	{
		reset_local_state();
	}

	const bool isLocalPlayer = PlayerHelper::IsLocalPlayer(__this);

	if (!isLocalPlayer)
	{
		return app::NolanBehaviour_Update(__this, method);
	}

	if (local_nb != __this)
	{
		reset_local_state();
		local_nb = __this;
	}


	// 运物宏功能
	static bool lastCarryKeyPressed = false;
	static bool carryMacroActive = false;
	static float carryMacroTimer = 0.0f;

	if (settings.bCarryMacro) {
		bool currentCarryKeyPressed = false;

		// 根据设置检测不同的按键
		if (settings.carryMacroKey == 0) {
			// 鼠标左键
			currentCarryKeyPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
		}
		else {
			// E键
			currentCarryKeyPressed = (GetAsyncKeyState('E') & 0x8000) != 0;
		}

		// 检测按键按下
		if (currentCarryKeyPressed && !lastCarryKeyPressed) {
			carryMacroActive = true;
			carryMacroTimer = 0.0f;
			std::cout << "[CarryMacro] 开始计时，将在0.2秒后放下物品" << std::endl;
		}

		// 如果运物宏激活，计时
		if (carryMacroActive) {
			carryMacroTimer += app::Time_1_get_deltaTime(nullptr);

			// 0.3秒后自动按下G键
			if (carryMacroTimer >= 0.22f) {
				// 模拟按下G键放下物品
				keybd_event('G', 0, 0, 0); // 按下G键
				keybd_event('G', 0, KEYEVENTF_KEYUP, 0); // 释放G键

				carryMacroActive = false;
				std::cout << "[CarryMacro] 自动放下物品" << std::endl;
			}
		}
		lastCarryKeyPressed = currentCarryKeyPressed;
	}
	// 自定义传送功能

	if (settings.bTriggerCustomTeleport) {
		settings.bTriggerCustomTeleport = false;

		if (settings.selectedTeleportIndex >= 0 &&
			settings.selectedTeleportIndex < settings.customTeleportLocations.size()) {

			auto& location = settings.customTeleportLocations[settings.selectedTeleportIndex];
			if (location.isValid) {
				// 使用 NolanBehaviour_TeleportTo 进行传送
				app::NolanBehaviour_TeleportTo(__this, location.position, app::Quaternion_get_identity(nullptr), false, nullptr);

				std::cout << "[Teleport] 已传送到: " << location.name << std::endl;
			}
		}
	}
	// 修改玩家速度（带恢复功能）
	static float originalSpeedMultiplier = 1.0f;
	static float originalMaxSpeedValue = 1.0f;
	static bool speedOriginalValueStored = false;

	if (settings.bModifyPlayerSpeed && local_nb->fields.speedChangeAbility)
	{
		if (!speedOriginalValueStored)
		{
			originalSpeedMultiplier = local_nb->fields.speedChangeAbility->fields.m_SpeedChangeMultiplier;
			originalMaxSpeedValue = local_nb->fields.speedChangeAbility->fields.m_MaxSpeedChangeValue;
			speedOriginalValueStored = true;
		}

		local_nb->fields.speedChangeAbility->fields.m_SpeedChangeMultiplier = settings.playerSpeed;
		local_nb->fields.speedChangeAbility->fields.m_MaxSpeedChangeValue = settings.playerSpeed;
	}
	else
	{
		if (speedOriginalValueStored && local_nb->fields.speedChangeAbility)
		{
			local_nb->fields.speedChangeAbility->fields.m_SpeedChangeMultiplier = originalSpeedMultiplier;
			local_nb->fields.speedChangeAbility->fields.m_MaxSpeedChangeValue = originalMaxSpeedValue;
			speedOriginalValueStored = false;
		}
	}


	// === 修改跳跃高度 ===
	static float originalJumpForce = 8.0f; // 存储原始跳跃力
	static bool originalValueStored = false;

	if (settings.bModifyJumpHeight)
	{
		app::GameObject* playerGO = app::Component_get_gameObject((app::Component*)local_nb, nullptr);
		if (playerGO)
		{
			app::NolanVoiceOvers* voiceOvers = reinterpret_cast<app::NolanVoiceOvers*>(
				app::GameObject_GetComponentByName(playerGO, convert_to_system_string("NolanVoiceOvers"), nullptr)
				);

			if (voiceOvers && voiceOvers->fields.jumpAbility)
			{
				// 如果是第一次启用，保存原始值
				if (!originalValueStored)
				{
					originalJumpForce = voiceOvers->fields.jumpAbility->fields.m_Force;
					originalValueStored = true;
					std::cout << "[Jump] 保存原始跳跃力: " << originalJumpForce << std::endl;
				}

				// 设置新的跳跃力
				voiceOvers->fields.jumpAbility->fields.m_Force = settings.jumpHeight;
			}
		}
	}
	else
	{
		// 取消勾选时恢复原始值
		if (originalValueStored)
		{
			app::GameObject* playerGO = app::Component_get_gameObject((app::Component*)local_nb, nullptr);
			if (playerGO)
			{
				app::NolanVoiceOvers* voiceOvers = reinterpret_cast<app::NolanVoiceOvers*>(
					app::GameObject_GetComponentByName(playerGO, convert_to_system_string("NolanVoiceOvers"), nullptr)
					);

				if (voiceOvers && voiceOvers->fields.jumpAbility)
				{
					voiceOvers->fields.jumpAbility->fields.m_Force = originalJumpForce;
					std::cout << "[Jump] 恢复原始跳跃力: " << originalJumpForce << std::endl;
				}
			}
			originalValueStored = false; // 重置标志
		}
	}
	// === 跳跃高度修改结束 ===

	// === Q键高速连续跳跃 ===


	if (settings.bModifyJumpHeight)
	{
		app::GameObject* playerGO = app::Component_get_gameObject((app::Component*)local_nb, nullptr);
		if (playerGO)
		{
			app::NolanVoiceOvers* voiceOvers = reinterpret_cast<app::NolanVoiceOvers*>(
				app::GameObject_GetComponentByName(playerGO, convert_to_system_string("NolanVoiceOvers"), nullptr)
				);

			if (voiceOvers && voiceOvers->fields.jumpAbility)
			{
				// 如果是第一次启用，保存原始值
				if (!originalValueStored)
				{
					originalJumpForce = voiceOvers->fields.jumpAbility->fields.m_Force;
					originalValueStored = true;
					std::cout << "[Jump] 保存原始跳跃力: " << originalJumpForce << std::endl;
				}

				// 设置新的跳跃力
				voiceOvers->fields.jumpAbility->fields.m_Force = settings.jumpHeight;
			}
		}
	}
	else
	{
		// 取消勾选时恢复原始值
		if (originalValueStored)
		{
			app::GameObject* playerGO = app::Component_get_gameObject((app::Component*)local_nb, nullptr);
			if (playerGO)
			{
				app::NolanVoiceOvers* voiceOvers = reinterpret_cast<app::NolanVoiceOvers*>(
					app::GameObject_GetComponentByName(playerGO, convert_to_system_string("NolanVoiceOvers"), nullptr)
					);

				if (voiceOvers && voiceOvers->fields.jumpAbility)
				{
					voiceOvers->fields.jumpAbility->fields.m_Force = originalJumpForce;
					std::cout << "[Jump] 恢复原始跳跃力: " << originalJumpForce << std::endl;
				}
			}
			originalValueStored = false; // 重置标志
		}
	}
	// === 跳跃高度修改结束 ===

	// === 高速连续跳跃 ===
	static bool jumpKeyPressed = false;
	static float jumpTimer = 0.0f;
	const float jumpInterval = 0.05f; // 更短的跳跃间隔（0.05秒 = 每秒20次）

	if (settings.bSuperJump && (GetAsyncKeyState(GetSuperJumpKey()) & 0x8000))
	{
		jumpKeyPressed = true;
		jumpTimer += app::Time_1_get_deltaTime(nullptr);

		if (jumpTimer >= jumpInterval)
		{
			jumpTimer = 0.0f;

			app::GameObject* playerGO = app::Component_get_gameObject((app::Component*)local_nb, nullptr);
			if (playerGO)
			{
				app::NolanVoiceOvers* voiceOvers = reinterpret_cast<app::NolanVoiceOvers*>(
					app::GameObject_GetComponentByName(playerGO, convert_to_system_string("NolanVoiceOvers"), nullptr)
					);

				if (voiceOvers && voiceOvers->fields.jumpAbility)
				{
					float jumpMultiplier = settings.superJumpMultiplier;
					float originalForce = voiceOvers->fields.jumpAbility->fields.m_Force;
					voiceOvers->fields.jumpAbility->fields.m_Force = originalForce * jumpMultiplier;
					app::Ability_StartAbility((app::Ability*)voiceOvers->fields.jumpAbility, nullptr);
					voiceOvers->fields.jumpAbility->fields.m_Force = originalForce;
				}
				else
				{
					app::Rigidbody* rigidbody = reinterpret_cast<app::Rigidbody*>(
						app::GameObject_GetComponentByName(playerGO, convert_to_system_string("Rigidbody"), nullptr)
						);

					if (rigidbody)
					{
						float jumpForceValue = 8.0f * settings.superJumpMultiplier;
						app::Vector3 jumpForce = { 0, jumpForceValue, 0 };
						app::Rigidbody_AddForce(rigidbody, jumpForce, app::ForceMode__Enum::Impulse, nullptr);
					}
				}
			}
		}
	}
	else
	{
		jumpKeyPressed = false;
		jumpTimer = 0.0f;
	}
	// === 高速连续跳跃结束 ===

	// ========== 添加怪物无视逻辑 - V键直接控制勾选框 ==========
	static bool lastVKeyPressed = false;

	// V键处理
	bool currentVKeyPressed = (GetAsyncKeyState('V') & 0x8000) != 0;
	bool vKeyJustPressed = currentVKeyPressed && !lastVKeyPressed;
	lastVKeyPressed = currentVKeyPressed;

	// 根据V键模式直接控制勾选框状态
	if (settings.monsterIgnoreVKeyMode == 1) { // 切换模式
		if (vKeyJustPressed) {
			settings.bMonsterIgnoreMe = !settings.bMonsterIgnoreMe;
			std::cout << "[MonsterIgnore] Toggle: " << (settings.bMonsterIgnoreMe ? "ON" : "OFF") << std::endl;
		}
	}
	else if (settings.monsterIgnoreVKeyMode == 2) { // 按住模式
		settings.bMonsterIgnoreMe = currentVKeyPressed;
	}
	// 模式0（关闭）不做任何事，由用户手动勾选

	// 应用隐身状态
	static int originalLayer = -1;
	static bool lastMonsterIgnoreState = false;

	if (settings.bMonsterIgnoreMe != lastMonsterIgnoreState) {
		lastMonsterIgnoreState = settings.bMonsterIgnoreMe;

		app::GameObject* playerGO = app::Component_get_gameObject((app::Component*)__this, nullptr);
		if (playerGO) {
			if (settings.bMonsterIgnoreMe) {
				// 保存原始layer
				if (originalLayer == -1) {
					originalLayer = app::GameObject_get_layer(playerGO, nullptr);
					std::cout << "[MonsterIgnore] Original layer: " << originalLayer << std::endl;
				}

				// 设置为第9层
				app::GameObject_set_layer(playerGO, 9, nullptr);
				int currentLayer = app::GameObject_get_layer(playerGO, nullptr);
				std::cout << "[MonsterIgnore] Invisible ON - Layer: " << currentLayer << std::endl;

			}
			else {
				// 恢复原始layer
				if (originalLayer != -1) {
					app::GameObject_set_layer(playerGO, originalLayer, nullptr);
					int restoredLayer = app::GameObject_get_layer(playerGO, nullptr);
					std::cout << "[MonsterIgnore] Invisible OFF - Layer: " << restoredLayer << std::endl;
					originalLayer = -1;
				}
			}
		}
	}
	// ========== 添加结束 ==========

	if (settings.bFly)
	{
		app::Transform* transform = app::Component_get_transform((app::Component*) __this, nullptr);
		if (!transform)
		{
			return app::NolanBehaviour_Update(__this, method);
		}

		float deltaTime = app::Time_1_get_deltaTime(nullptr);
		float speed = settings.flySpeed;

		const app::Vector3 forward = app::Transform_get_forward(transform, nullptr);
		const app::Vector3 right = app::Transform_get_right(transform, nullptr);
		const app::Vector3 up = app::Transform_get_up(transform, nullptr);
		app::Vector3 pos = app::Transform_get_position(transform, nullptr);

		if (GetAsyncKeyState('W') & 0x8000) pos = pos + (forward * speed * deltaTime);
		if (GetAsyncKeyState('S') & 0x8000) pos = pos - (forward * speed * deltaTime);
		if (GetAsyncKeyState('D') & 0x8000) pos = pos + (right * speed * deltaTime);
		if (GetAsyncKeyState('A') & 0x8000) pos = pos - (right * speed * deltaTime);
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) pos = pos + (up * speed * deltaTime);
		if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) pos = pos - (up * speed * deltaTime);

		app::GameObject* thisGO = app::Component_get_gameObject((app::Component*) __this, nullptr);
		if (!SafePtr::IsValid(thisGO))
		{
			reset_local_state();
			return app::NolanBehaviour_Update(__this, method);
		}

		if (!SafePtr::IsValid((app::Object_1*) cachedLocomotion))
		{
			cachedLocomotion = app::GameObject_GetComponentByName(thisGO, convert_to_system_string("UltimateCharacterLocomotion"), nullptr);
		}

		if (SafePtr::IsValid((app::Object_1*) cachedLocomotion) && app::UltimateCharacterLocomotion_SetPosition_1)
		{
			app::UltimateCharacterLocomotion_SetPosition_1((app::UltimateCharacterLocomotion*) cachedLocomotion, pos, false, nullptr);
		}
	}

	return app::NolanBehaviour_Update(__this, method);
}

bool dDevourInput_GetLongPress(app::DevourInput* __this, app::String* name, float duration, bool waitForRelease, MethodInfo* method) {
	if (settings.bDisableLongInteract)
	{
		duration = 0.0f;
	}

	return app::DevourInput_GetLongPress(__this, name, duration, waitForRelease, method);
}

void dNolanBehaviour_OnAttributeUpdateValue(app::NolanBehaviour* __this, app::Attribute* attribute, MethodInfo* method) {

	if (settings.bModifyFlashlightColor)
	{
		app::Color color = { settings.flashlightColor[0], settings.flashlightColor[1], settings.flashlightColor[2], settings.flashlightColor[3] };

		__this->fields.flashlightUVColor = color;
	}

	if (settings.bUnlimitedUVLight)
	{


		if (strcmp(il2cppi_to_string(attribute->fields.m_Name).c_str(), "Battery") == 0)
		{
			attribute->fields.m_Value = 100.0f;
		}
	}

	return app::NolanBehaviour_OnAttributeUpdateValue(__this, attribute, method);
}

app::RankHelpers_ExpGainInfo* dRankHelpers_CalculateExpGain(app::RankHelpers* __this, int32_t mapProgress, int32_t numAwards, app::GameConfigToken* gameConfigToken, MethodInfo* method) {
	if (settings.exp_modifier)
	{
		app::RankHelpers_ExpGainInfo* gain = app::RankHelpers_CalculateExpGain(__this, mapProgress, numAwards, gameConfigToken, method);

		int32_t bonus = (int32_t) settings.newExp;

		gain->fields.awardsBonus = bonus;
		gain->fields.winBonus = bonus;
		gain->fields.totalExp = bonus;

		return gain;
	}

	return app::RankHelpers_CalculateExpGain(__this, mapProgress, numAwards, gameConfigToken, method);
}

bool dLockedInteractable_CanInteract(app::LockedInteractable* __this, app::GameObject* character, MethodInfo* method) {
	if (settings.bUnlockDoors)
	{
		__this->fields.isLocked = false;
		return true;
	}

	return app::LockedInteractable_CanInteract(__this, character, method);
}

bool dOptionsHelpers_IsCharacterUnlocked(app::OptionsHelpers* __this, app::String* prefab, MethodInfo* method) {

	bool orgCharUnlocked = app::OptionsHelpers_IsCharacterUnlocked(__this, prefab, method);

	if (orgCharUnlocked != true && settings.bUnlockAll)
	{
		return true;
	}

	return orgCharUnlocked;
}

bool dOptionsHelpers_IsRobeUnlocked(app::OptionsHelpers* __this, app::String* robe, app::String* character, MethodInfo* method) {

	bool orgRobeUnlocked = app::OptionsHelpers_IsRobeUnlocked(__this, robe, character, method);

	if (orgRobeUnlocked != true && settings.bUnlockAll)
	{
		return true;
	}

	return orgRobeUnlocked;
}

app::UIPerkSelectionType* dMenu_SetupPerk(app::Menu* __this, app::CharacterPerk* perk, MethodInfo* method) {

	auto perkSelectionType = app::Menu_SetupPerk(__this, perk, method);

	if (settings.bUnlockAll)
	{
		if (!perk->fields.isOwned)
		{
			perk->fields.isHidden = false;
			perk->fields.isOwned = true;
		}
	}

	return perkSelectionType;
}



bool dSurvivalLobbyController_CanReady(app::SurvivalLobbyController* __this, MethodInfo* method) {
	return true;
}

bool dSurvivalLobbyController_PlayableCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method) {

	bool orgPlayableSelected = app::SurvivalLobbyController_PlayableCharacterSelected(__this, method);

	if (orgPlayableSelected != true && settings.bUnlockAll)
	{
		return true;
	}

	return orgPlayableSelected;
}

bool dSurvivalLobbyController_UnlockedCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method) {

	if (settings.bUnlockAll)
	{
		return true;
	}

	return app::SurvivalLobbyController_UnlockedCharacterSelected(__this, method);
}

void dSurvivalAzazelBehaviour_Update(app::SurvivalAzazelBehaviour* __this, MethodInfo* method) {
	if (settings.bAzazelEsp)
	{
		ESPManager::GetAzazel().Update(reinterpret_cast<app::Object_1*>(__this));
	}


	app::SurvivalAzazelBehaviour_Update(__this, method);
}


//蜘蛛拿蛋
void (*original_SpiderBehaviour_OnEggCarried)(app::SpiderBehaviour* __this, app::GameObject* egg, MethodInfo* method) = nullptr;

void dSpiderBehaviour_OnEggCarried(app::SpiderBehaviour* __this, app::GameObject* egg, MethodInfo* method)
{
	// 获取蜘蛛的GameObject
	app::GameObject* spiderGO = app::Component_get_gameObject(reinterpret_cast<app::Component*>(__this), nullptr);
	if (spiderGO && SafePtr::IsValid(spiderGO)) {
		// 标记蜘蛛为持蛋状态
		ESPManager::GetSimpleDemons().SetSpiderCarryingEgg(spiderGO, true);
	}

	// 调用原始方法
	if (original_SpiderBehaviour_OnEggCarried) {
		original_SpiderBehaviour_OnEggCarried(__this, egg, method);
	}
}

// 蜘蛛死亡协程的 MoveNext Hook
bool (*original_SpiderBehaviour_Die_d_60_MoveNext)(app::SpiderBehaviour_Die_d_60* __this, MethodInfo* method) = nullptr;

bool dSpiderBehaviour_Die_d_60_MoveNext(app::SpiderBehaviour_Die_d_60* __this, MethodInfo* method)
{
	il2cppi_log_write("[SpiderDieMoveNext] MoveNext 被调用");

	// 备用方案：如果原始方法指针为空，尝试直接调用函数
	bool result = false;
	if (original_SpiderBehaviour_Die_d_60_MoveNext) {
		result = original_SpiderBehaviour_Die_d_60_MoveNext(__this, method);
	}
	else {
		// 如果原始指针为空，直接调用原始函数
		result = app::SpiderBehaviour_Die_d_60_MoveNext(__this, method);
		il2cppi_log_write("[SpiderDieMoveNext] 使用备用方案调用原始方法");
	}

	il2cppi_log_write(("[SpiderDieMoveNext] 方法执行完成，结果: " + std::to_string(result)).c_str());

	// 处理状态更新
	app::SpiderBehaviour* spiderBehaviour = __this->fields.__4__this;
	if (spiderBehaviour) {
		app::GameObject* spiderGO = app::Component_get_gameObject(reinterpret_cast<app::Component*>(spiderBehaviour), nullptr);
		if (spiderGO && SafePtr::IsValid(spiderGO)) {
			ESPManager::GetSimpleDemons().SetSpiderCarryingEgg(spiderGO, false);
			il2cppi_log_write("[SpiderDieMoveNext] 已设置持蛋状态为 false");
		}
	}

	return result;
}

// ========== 幽灵相关 Hook ==========

// 幽灵拿书
void (*original_GhostBehaviour_OnRitualBookCarried)(app::GhostBehaviour* __this, app::GameObject* ritualBook, MethodInfo* method) = nullptr;

void dGhostBehaviour_OnRitualBookCarried(app::GhostBehaviour* __this, app::GameObject* ritualBook, MethodInfo* method)
{
	// 获取幽灵的GameObject
	app::GameObject* ghostGO = app::Component_get_gameObject(reinterpret_cast<app::Component*>(__this), nullptr);
	if (ghostGO && SafePtr::IsValid(ghostGO)) {
		// 标记幽灵为持书状态
		ESPManager::GetSimpleDemons().SetGhostCarryingBook(ghostGO, true);
	}

	// 调用原始方法
	if (original_GhostBehaviour_OnRitualBookCarried) {
		original_GhostBehaviour_OnRitualBookCarried(__this, ritualBook, method);
	}
}

bool (*original_GhostBehaviour_Die_d_84_MoveNext)(app::GhostBehaviour_Die_d_84* __this, MethodInfo* method) = nullptr;

bool dGhostBehaviour_Die_d_84_MoveNext(app::GhostBehaviour_Die_d_84* __this, MethodInfo* method)
{
	il2cppi_log_write("[GhostDieMoveNext] MoveNext 被调用");

	bool result = false;
	if (original_GhostBehaviour_Die_d_84_MoveNext) {
		result = original_GhostBehaviour_Die_d_84_MoveNext(__this, method);
	}
	else {
		result = app::GhostBehaviour_Die_d_84_MoveNext(__this, method);
		il2cppi_log_write("[GhostDieMoveNext] 使用备用方案调用原始方法");
	}

	il2cppi_log_write(("[GhostDieMoveNext] 方法执行完成，结果: " + std::to_string(result)).c_str());

	// 处理状态更新
	app::GhostBehaviour* ghostBehaviour = __this->fields.__4__this;
	if (ghostBehaviour) {
		app::GameObject* ghostGO = app::Component_get_gameObject(reinterpret_cast<app::Component*>(ghostBehaviour), nullptr);
		if (ghostGO && SafePtr::IsValid(ghostGO)) {
			ESPManager::GetSimpleDemons().SetGhostCarryingBook(ghostGO, false);
			il2cppi_log_write("[GhostDieMoveNext] 已设置持书状态为 false");
		}
	}

	return result;
}

// ========== 乌鸦相关 Hook ==========

// 乌鸦拿头
void (*original_CrowBehaviour_OnHeadCarried)(app::CrowBehaviour* __this, app::GameObject* head, MethodInfo* method) = nullptr;

void dCrowBehaviour_OnHeadCarried(app::CrowBehaviour* __this, app::GameObject* head, MethodInfo* method)
{
	// 获取乌鸦的GameObject
	app::GameObject* crowGO = app::Component_get_gameObject(reinterpret_cast<app::Component*>(__this), nullptr);
	if (crowGO && SafePtr::IsValid(crowGO)) {
		// 标记乌鸦为持头状态
		ESPManager::GetSimpleDemons().SetCrowCarryingHead(crowGO, true);
	}

	// 调用原始方法
	if (original_CrowBehaviour_OnHeadCarried) {
		original_CrowBehaviour_OnHeadCarried(__this, head, method);
	}
}

bool (*original_CrowBehaviour_Die_d_72_MoveNext)(app::CrowBehaviour_Die_d_72* __this, MethodInfo* method) = nullptr;

bool dCrowBehaviour_Die_d_72_MoveNext(app::CrowBehaviour_Die_d_72* __this, MethodInfo* method)
{
	il2cppi_log_write("[CrowDieMoveNext] MoveNext 被调用");

	bool result = false;
	if (original_CrowBehaviour_Die_d_72_MoveNext) {
		result = original_CrowBehaviour_Die_d_72_MoveNext(__this, method);
	}
	else {
		result = app::CrowBehaviour_Die_d_72_MoveNext(__this, method);
		il2cppi_log_write("[CrowDieMoveNext] 使用备用方案调用原始方法");
	}

	il2cppi_log_write(("[CrowDieMoveNext] 方法执行完成，结果: " + std::to_string(result)).c_str());

	// 处理状态更新
	app::CrowBehaviour* crowBehaviour = __this->fields.__4__this;
	if (crowBehaviour) {
		app::GameObject* crowGO = app::Component_get_gameObject(reinterpret_cast<app::Component*>(crowBehaviour), nullptr);
		if (crowGO && SafePtr::IsValid(crowGO)) {
			ESPManager::GetSimpleDemons().SetCrowCarryingHead(crowGO, false);
			il2cppi_log_write("[CrowDieMoveNext] 已设置持头状态为 false");
		}
	}

	return result;
}



void dAzazelAprilBehaviour_Update(app::AzazelAprilBehaviour* __this, MethodInfo* method) {
	if (settings.bAzazelEsp)
	{
		ESPManager::GetAzazel().Update(reinterpret_cast<app::Object_1*>(__this));
	}

	app::AzazelAprilBehaviour_Update(__this, method);
}

void dCharacterLoader_Awake(app::CharacterLoader* __this, MethodInfo* method) {
	app::CharacterLoader_Awake(__this, method);
}

void dCharacterLoader_OnOutfit(app::CharacterLoader* __this, MethodInfo* method) {
	app::CharacterLoader_OnOutfit(__this, method);
}

//DO_APP_FUNC(0x02FFD5B0, void, SceneManager_Internal_ActiveSceneChanged, (Scene previousActiveScene, Scene newActiveScene, MethodInfo * method));
void dSceneManager_Internal_ActiveSceneChanged(app::Scene previousActiveScene, app::Scene newActiveScene, MethodInfo* method) {
	ESPManager::ResetAll();
	Players::PlayersManager::Instance().ClearCache();
	LocalPlayer::LocalPlayerManager::Instance().ClearCache();
	settings.bInteractableEspUpdated = false;
	fxApplied = false;

	return app::SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene, method);
}

void dObjectInteractable_Interact(app::ObjectInteractable* __this, app::GameObject* character, MethodInfo* method) {
	settings.bInteractableEspUpdated = false;
	__this->fields.m_CanInteract = true;

	app::ObjectInteractable_Interact(__this, character, method);
}

void dObjectInteractable_Awake(app::ObjectInteractable* __this, MethodInfo* method) {
	__this->fields.m_CanInteract = true;

	app::ObjectInteractable_Awake(__this, method);

	if (settings.bInteractableEsp)
	{
		settings.bInteractableEspUpdated = false;
	}
}

app::BoltEntity* dInGameHelpers_CreateDroppedObject(app::InGameHelpers* __this, app::PrefabId prefabId, app::Vector3 position, app::String* carryObjectName, app::BoltEntity* player, bool fromKnockout, app::BoltEntity* ai, MethodInfo* method) {
	settings.bInteractableEspUpdated = false;

	return app::InGameHelpers_CreateDroppedObject(__this, prefabId, position, carryObjectName, player, fromKnockout, ai, method);
}

void dSurvivalLobbyController_Update(app::SurvivalLobbyController* __this, MethodInfo* method) {
	__this->fields.m_ready = true;
	__this->fields.m_Menu->fields.gameStarted = false;

	if (settings.bModifyRank)
	{
		if (app::GameObject* thisGO = __this->fields._m_SurvivalPlayer_k__BackingField)
		{
			Base::Gameplay::setRank(thisGO, settings.newRank);
		}
	}

	return app::SurvivalLobbyController_Update(__this, method);
}

void dRoomProtocolToken_Write(app::RoomProtocolToken* __this, app::UdpPacket* packet, MethodInfo* method) {

	if (settings.bForcePublic)
	{
		__this->fields.isPrivate = false;
	}

	if (settings.bChangeRoomName)
	{
		__this->fields.serverName = convert_to_system_string(settings.customRoomName);
	}


	if (settings.bFakeGameInProgress)
	{
		__this->fields.gameInProgress = false;
	}

	app::RoomProtocolToken_Write(__this, packet, method);
}

void dBoltMatchmaking_UpdateSession(app::IProtocolToken* token, MethodInfo* method) {

	if (token && token->klass)
	{
		const char* className = il2cpp_class_get_name((Il2CppClass*)token->klass);
		const char* classNamespace = il2cpp_class_get_namespace((Il2CppClass*)token->klass);

		if (strcmp(className, "PhotonRoomProperties") == 0)
		{
			auto roomToken = reinterpret_cast<app::PhotonRoomProperties*>(token);

			if (settings.bForcePublic)
			{
				roomToken->fields._IsOpen_k__BackingField = true;
			}

			if (settings.bFakeGameInProgress)
			{
				roomToken->fields._IsVisible_k__BackingField = true;
			}
		}
	}

	app::BoltMatchmaking_UpdateSession(token, method);
}

void dMouseFollower_Update(app::MouseFollower* __this, MethodInfo* method) {
	if (settings.bMouseFollower)
	{
		if (!__this || !app::Camera_get_main(nullptr))
			return;

		app::Vector3 rawMouse = app::Input_1_get_mousePosition(nullptr);

		float screenWidth = (float) app::Screen_get_width(nullptr);
		float screenHeight = (float) app::Screen_get_height(nullptr);

		float normX = rawMouse.x / screenWidth;
		float normY = rawMouse.y / screenHeight;

		float offsetX = (normX - 0.5f) * 2.0f;
		float offsetY = (normY - 0.5f) * 2.0f;

		// offset
		offsetX *= settings.MF_offsetX;
		offsetY *= settings.MF_offsetY;

		static app::Vector3 basePos = app::Transform_get_position(app::Component_get_transform((app::Component*) __this, nullptr), nullptr);

		app::Vector3 target;
		target.x = basePos.x + offsetX;
		target.y = basePos.y + offsetY;
		target.z = basePos.z;

		app::Transform* transform = app::Component_get_transform((app::Component*) __this, nullptr);
		app::Vector3 current = app::Transform_get_position(transform, nullptr);

		float delta = app::Time_1_get_deltaTime(nullptr);
		float speed = settings.MF_Speed;

		app::Vector3 final = app::Vector3_Lerp(current, target, speed * delta, nullptr);
		app::Transform_set_position(transform, final, nullptr);
	}
	else
	{
		app::MouseFollower_Update(__this, method);
	}
}

app::IProtocolToken* dProtocolTokenUtils_ReadToken(app::UdpPacket* packet, MethodInfo* method) {
	auto* token = app::ProtocolTokenUtils_ReadToken(packet, method);

	// test

	return token;
}

bool dSurvivalReviveInteractable_CanInteract(app::SurvivalReviveInteractable* __this, app::GameObject* character, MethodInfo* method) {
	// 恢复默认行为 - 需要急救包才能救助
	return app::SurvivalReviveInteractable_CanInteract(__this, character, method);
}

bool dInteractable_CanInteract(app::Interactable* __this, app::GameObject* character, MethodInfo* method) {
	// 恢复默认行为 - 遵守游戏原有的交互规则
	return app::Interactable_CanInteract(__this, character, method);
}

void dNolanBehaviour_StartCarry(app::NolanBehaviour* __this, app::String* objectName, MethodInfo* method) {

	if (PlayerHelper::IsLocalPlayer(__this) && objectName != nullptr)
	{
		std::cout << "carry object name: " << il2cppi_to_string(objectName) << "\n";
	}

	app::NolanBehaviour_StartCarry(__this, objectName, method);
}

bool dKeyHelpers_IsKeyInArray(app::KeyHelpers* __this, app::String* state, app::String* name, MethodInfo* method) {

	return true;
}

bool dDoorBehaviour_IsLocked(app::DoorBehaviour* __this, MethodInfo* method) {
	if (settings.bUnlockDoors)
	{
		__this->fields.lockedInteractable->fields.isLocked = false;
		return false;
	}

	return app::DoorBehaviour_IsLocked(__this, method);
}

bool HookFunction(PVOID* ppPointer, PVOID pDetour, const char* functionName) {
	if (const auto error = DetourAttach(ppPointer, pDetour); error != NO_ERROR)
	{
		std::cout << "[ERROR]: Failed to hook " << functionName << ", error " << error << std::endl;
		return false;
	}

	//std::cout << "[HOOKED]: " << functionName << std::endl;
	return true;
}

#define HOOKFUNC(n) if (!HookFunction(&(PVOID&)n, d##n, #n)) return;

bool dSurvivalLobbyController_AllPlayersReady(app::SurvivalLobbyController* __this, MethodInfo* method) {
	return app::SurvivalLobbyController_AllPlayersReady(__this, method);  // 返回游戏原始值
}

bool UnhookFunction(PVOID* ppPointer, PVOID pDetour, const char* functionName) {
	if (const auto error = DetourDetach(ppPointer, pDetour); error != NO_ERROR)
	{
		std::cout << "[ERROR]: Failed to unhook " << functionName << ", error " << error << std::endl;
		return false;
	}
	//std::cout << "[UNHOOKED]: " << functionName << std::endl;
	return true;
}

#define UNHOOKFUNC(n) if (!UnhookFunction(&(PVOID&)n, d##n, #n)) return;

void DetourInitilization() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	D3D_PRESENT_FUNCTION present = dx11api::GetPresentFunction();
	if (!present)
	{
		std::cout << "[ERROR]: Unable to retrieve IDXGISwapChain::Present method" << std::endl;
		return;
	}

	oPresent = present;
	if (!oPresent)
	{
		std::cout << "[ERROR]: oPresent is null!" << std::endl;
		return;
	}

	std::cout << "[INFO]: Attempting to hook oPresent at address: " << oPresent << std::endl;

	if (!HookFunction(&(PVOID&) oPresent, dPresent, "D3D_PRESENT_FUNCTION"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) Debug_2_Log, dDebug_Log, "Debug_2_Log"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) Debug_2_LogError, dDebug_LogError, "Debug_2_LogError"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) Debug_2_LogException, dDebug_LogException, "Debug_2_LogException"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) Debug_2_LogWarning, dDebug_LogWarning, "Debug_2_LogWarning"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::Cursor_1_get_visible, dCursor_get_visible, "Cursor_1_get_visible"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::Cursor_1_set_visible, dCursor_set_visible, "Cursor_1_set_visible"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::Cursor_1_get_lockState, dCursor_get_lockState, "Cursor_1_get_lockState"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::Cursor_1_set_lockState, dCursor_set_lockState, "Cursor_1_set_lockState"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::Input_1_GetAxis, dInput_1_GetAxis, "Input_1_GetAxis"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::NolanBehaviour_FixedUpdate, dNolanBehaviour_FixedUpdate, "NolanBehaviour_FixedUpdate"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::NolanBehaviour_Update, dNolanBehaviour_Update, "NolanBehaviour_Update"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::DevourInput_GetLongPress, dDevourInput_GetLongPress, "DevourInput_GetLongPress"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::NolanBehaviour_OnAttributeUpdateValue, dNolanBehaviour_OnAttributeUpdateValue, "dNolanBehaviour_OnAttributeUpdateValue"))
	{
		DetourTransactionAbort();
		return;
	}

	// 蜘蛛拿蛋
	if (!HookFunction(&(PVOID&)app::SpiderBehaviour_OnEggCarried, dSpiderBehaviour_OnEggCarried, "SpiderBehaviour_OnEggCarried"))
	{
		DetourTransactionAbort();
		return;
	}

	// Hook 蜘蛛死亡协程的 MoveNext 方法
	if (!HookFunction(&(PVOID&)app::SpiderBehaviour_Die_d_60_MoveNext, dSpiderBehaviour_Die_d_60_MoveNext, "SpiderBehaviour_Die_d_60_MoveNext"))
	{
		std::cout << "[Hook] SpiderBehaviour_Die_d_60_MoveNext Hook 安装失败" << std::endl;
	}
	else {
		std::cout << "[Hook] SpiderBehaviour_Die_d_60_MoveNext Hook 安装成功" << std::endl;
		il2cppi_log_write("[Hook] SpiderBehaviour_Die_d_60_MoveNext Hook 安装成功");
	}

	// 幽灵相关 Hook
	if (!HookFunction(&(PVOID&)app::GhostBehaviour_OnRitualBookCarried, dGhostBehaviour_OnRitualBookCarried, "GhostBehaviour_OnRitualBookCarried"))
	{
		DetourTransactionAbort();
		return;
	}
	// Hook 幽灵死亡协程
	if (!HookFunction(&(PVOID&)app::GhostBehaviour_Die_d_84_MoveNext, dGhostBehaviour_Die_d_84_MoveNext, "GhostBehaviour_Die_d_84_MoveNext"))
	{
		il2cppi_log_write("[Hook] GhostBehaviour_Die_d_84_MoveNext Hook 安装失败");
	}
	else {
		il2cppi_log_write("[Hook] GhostBehaviour_Die_d_84_MoveNext Hook 安装成功");
	}


	// 乌鸦相关 Hook
	if (!HookFunction(&(PVOID&)app::CrowBehaviour_OnHeadCarried, dCrowBehaviour_OnHeadCarried, "CrowBehaviour_OnHeadCarried"))
	{
		DetourTransactionAbort();
		return;
	}

	// Hook 乌鸦死亡协程
	if (!HookFunction(&(PVOID&)app::CrowBehaviour_Die_d_72_MoveNext, dCrowBehaviour_Die_d_72_MoveNext, "CrowBehaviour_Die_d_72_MoveNext"))
	{
		il2cppi_log_write("[Hook] CrowBehaviour_Die_d_72_MoveNext Hook 安装失败");
	}
	else {
		il2cppi_log_write("[Hook] CrowBehaviour_Die_d_72_MoveNext Hook 安装成功");
	}




	if (!HookFunction(&(PVOID&) app::RankHelpers_CalculateExpGain, dRankHelpers_CalculateExpGain, "RankHelpers_CalculateExpGain"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::LockedInteractable_CanInteract, dLockedInteractable_CanInteract, "LockedInteractable_CanInteract"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::OptionsHelpers_IsCharacterUnlocked, dOptionsHelpers_IsCharacterUnlocked, "OptionsHelpers_IsCharacterUnlocked"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::OptionsHelpers_IsRobeUnlocked, dOptionsHelpers_IsRobeUnlocked, "OptionsHelpers_IsRobeUnlocked"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::Menu_SetupPerk, dMenu_SetupPerk, "Menu_SetupPerk"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ServerDisconnectToken_Read, dServerDisconnectToken_Read, "dServerDisconnectToken_Read"))
	{
		DetourTransactionAbort();
		return;
	}

	//if (!HookFunction(&(PVOID&) app::SurvivalLobbyController_CanReady, dSurvivalLobbyController_CanReady, "SurvivalLobbyController_CanReady"))
	//{
	//	DetourTransactionAbort();
	//	return;
	//}

	if (!HookFunction(&(PVOID&) app::SurvivalLobbyController_PlayableCharacterSelected, dSurvivalLobbyController_PlayableCharacterSelected, "SurvivalLobbyController_PlayableCharacterSelected"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::SurvivalLobbyController_UnlockedCharacterSelected, dSurvivalLobbyController_UnlockedCharacterSelected, "SurvivalLobbyController_UnlockedCharacterSelected"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::SurvivalAzazelBehaviour_Update, dSurvivalAzazelBehaviour_Update, "SurvivalAzazelBehaviour_Update"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::AzazelAprilBehaviour_Update, dAzazelAprilBehaviour_Update, "dAzazelAprilBehaviour_Update"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::CharacterLoader_Awake, dCharacterLoader_Awake, "CharacterLoader_Awake"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::CharacterLoader_OnOutfit, dCharacterLoader_OnOutfit, "CharacterLoader_OnOutfit"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::SceneManager_Internal_ActiveSceneChanged, dSceneManager_Internal_ActiveSceneChanged, "SceneManager_Internal_ActiveSceneChanged"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ObjectInteractable_Interact, dObjectInteractable_Interact, "dObjectInteractable_Interact"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::InGameHelpers_CreateDroppedObject, dInGameHelpers_CreateDroppedObject, "dInGameHelpers_CreateDroppedObject"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ObjectInteractable_Awake, dObjectInteractable_Awake, "dObjectInteractable_Awake"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::SurvivalLobbyController_Update, dSurvivalLobbyController_Update, "SurvivalLobbyController_Update"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::RoomProtocolToken_Write, dRoomProtocolToken_Write, "RoomProtocolToken_Write"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::BoltMatchmaking_UpdateSession, dBoltMatchmaking_UpdateSession, "BoltMatchmaking_UpdateSession"))
	{
		DetourTransactionAbort();
		return;
	}

	//if (!HookFunction(&(PVOID&) app::SurvivalLobbyController_AllPlayersReady, dSurvivalLobbyController_AllPlayersReady, "SurvivalLobbyController_AllPlayersReady"))
	//{
	//	DetourTransactionAbort();
	//	return;
	//}


	if (!HookFunction(&(PVOID&) app::MouseFollower_Update, dMouseFollower_Update, "MouseFollower_Update"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ServerConnectToken_Write, dServerConnectToken_Write, "ServerConnectToken_Write"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ServerConnectToken_Read, dServerConnectToken_Read, "dServerConnectToken_Read"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ServerAcceptToken_Read, dServerAcceptToken_Read, "ServerAcceptToken_Read"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ServerBrowser_Populate, dServerBrowser_Populate, "ServerBrowser_Populate"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::PhotonRoomProperties_Read, dPhotonRoomProperties_Read, "dPhotonRoomProperties_Read"))
	{
		DetourTransactionAbort();
		return;
	}
	// 添加 Menu_GetPlayerId 的hook
	if (!HookFunction(&(PVOID&)app::Menu_GetPlayerId, dMenu_GetPlayerId, "dMenu_GetPlayerId"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::SteamUser_GetSteamID, dSteamUser_GetSteamID, "dSteamUser_GetSteamID"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ServerBrowser_JoinSession, dServerBrowser_JoinSession, "dServerBrowser_JoinSession"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::SteamFriends_GetPersonaName, dSteamFriends_GetPersonaName, "dSteamFriends_GetPersonaName"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::ShouldCalmDown_OnUpdate, dShouldCalmDown_OnUpdate, "dShouldCalmDown_OnUpdate"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::PlayersFocusingMeWait_OnUpdate, dPlayersFocusingMeWait_OnUpdate, "dPlayersFocusingMeWait_OnUpdate"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::SurvivalAzazelBehaviour_AnnaFrying, dSurvivalAzazelBehaviour_AnnaFrying, "dSurvivalAzazelBehaviour_AnnaFrying"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::EnemyState_get_MultipleFocus, dEnemyState_get_MultipleFocus, "dEnemyState_get_MultipleFocus"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::EnemyState_set_MultipleFocus, dEnemyState_set_MultipleFocus, "dEnemyState_set_MultipleFocus"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::Menu_SetupOutfit, dMenu_SetupOutfit, "dMenu_SetupOutfit"))
	{
		DetourTransactionAbort();
		return;
	}
	// 在 DetourInitilization 中添加这些挂钩

	if (!HookFunction(&(PVOID&)app::ProtocolTokenUtils_ToByteArray, dProtocolTokenUtils_ToByteArray, "ProtocolTokenUtils_ToByteArray"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::BoltNetwork_LoadScene_1, dBoltNetwork_LoadScene_1, "BoltNetwork_LoadScene_1"))
	{
		DetourTransactionAbort();
		return;
	}
	if (!HookFunction(&(PVOID&) app::SteamInventoryManager_UserHasItem, dSteamInventoryManager_UserHasItem, "dSteamInventoryManager_UserHasItem"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::Menu_get_boltConfig, dMenu_get_boltConfig, "dMenu_get_boltConfig"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::UIOutfitSelectionType_SetLocked, dUIOutfitSelectionType_SetLocked, "dUIOutfitSelectionType_SetLocked"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::CharacterOutfit__ctor, dCharacterOutfit__ctor, "dCharacterOutfit__ctor"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::ProtocolTokenUtils_ReadToken, dProtocolTokenUtils_ReadToken, "dProtocolTokenUtils_ReadToken"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::SurvivalReviveInteractable_CanInteract, dSurvivalReviveInteractable_CanInteract, "dSurvivalReviveInteractable_CanInteract"))
	{
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&) app::NolanBehaviour_StartCarry, dNolanBehaviour_StartCarry, "dNolanBehaviour_StartCarry"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::KeyHelpers_IsKeyInArray, dKeyHelpers_IsKeyInArray, "dKeyHelpers_IsKeyInArray"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::DoorBehaviour_IsLocked, dDoorBehaviour_IsLocked, "dDoorBehaviour_IsLocked"))
	{
		DetourTransactionAbort();
		return;
	}

	DetourTransactionCommit();
}

void DetourUninitialization() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourDetach(&(PVOID&) oPresent, dPresent) != 0) return;

	if (DetourTransactionCommit() == NO_ERROR)
	{
		DirectX::Shutdown();
		dx11api::Shutdown();
	}

	if (DetourDetach(&(PVOID&) Debug_2_Log, dDebug_Log) != 0) return;
	if (DetourDetach(&(PVOID&) Debug_2_LogError, dDebug_LogError) != 0) return;
	if (DetourDetach(&(PVOID&) Debug_2_LogException, dDebug_LogException) != 0) return;

	if (DetourDetach(&(PVOID&) app::Cursor_1_get_visible, dCursor_get_visible) != 0) return;
	if (DetourDetach(&(PVOID&) app::Cursor_1_set_visible, dCursor_set_visible) != 0) return;
	if (DetourDetach(&(PVOID&) app::Cursor_1_get_lockState, dCursor_get_lockState) != 0) return;
	if (DetourDetach(&(PVOID&) app::Cursor_1_set_lockState, dCursor_set_lockState) != 0) return;
	if (DetourDetach(&(PVOID&) app::Input_1_GetAxis, dInput_1_GetAxis) != 0) return;

	if (DetourDetach(&(PVOID&) app::NolanBehaviour_FixedUpdate, dNolanBehaviour_FixedUpdate) != 0) return;
	if (DetourDetach(&(PVOID&) app::NolanBehaviour_Update, dNolanBehaviour_Update) != 0) return;
	if (DetourDetach(&(PVOID&)app::ProtocolTokenUtils_ToByteArray, dProtocolTokenUtils_ToByteArray) != 0) return;
	if (DetourDetach(&(PVOID&)app::BoltNetwork_LoadScene_1, dBoltNetwork_LoadScene_1) != 0) return;
	if (DetourDetach(&(PVOID&) app::DevourInput_GetLongPress, dDevourInput_GetLongPress) != 0) return;
	if (DetourDetach(&(PVOID&) app::NolanBehaviour_OnAttributeUpdateValue, dNolanBehaviour_OnAttributeUpdateValue) != 0) return;

	if (DetourDetach(&(PVOID&) app::RankHelpers_CalculateExpGain, dRankHelpers_CalculateExpGain) != 0) return;

	if (DetourDetach(&(PVOID&) app::LockedInteractable_CanInteract, dLockedInteractable_CanInteract) != 0) return;

	if (DetourDetach(&(PVOID&) app::OptionsHelpers_IsCharacterUnlocked, dOptionsHelpers_IsCharacterUnlocked) != 0) return;
	if (DetourDetach(&(PVOID&)app::OptionsHelpers_IsRobeUnlocked, dOptionsHelpers_IsRobeUnlocked) != 0) return;

	if (DetourDetach(&(PVOID&)app::ServerDisconnectToken_Read, dServerDisconnectToken_Read) != 0) return;
	if (DetourDetach(&(PVOID&) app::SurvivalLobbyController_CanReady, dSurvivalLobbyController_CanReady) != 0) return;
	if (DetourDetach(&(PVOID&) app::SurvivalLobbyController_PlayableCharacterSelected, dSurvivalLobbyController_PlayableCharacterSelected) != 0) return;
	if (DetourDetach(&(PVOID&) app::SurvivalLobbyController_UnlockedCharacterSelected, dSurvivalLobbyController_UnlockedCharacterSelected) != 0) return;

	if (DetourDetach(&(PVOID&) app::SurvivalAzazelBehaviour_Update, dSurvivalAzazelBehaviour_Update) != 0) return;
	if (DetourDetach(&(PVOID&) app::AzazelAprilBehaviour_Update, dAzazelAprilBehaviour_Update) != 0) return;


	if (DetourDetach(&(PVOID&) app::CharacterLoader_Awake, dCharacterLoader_Awake) != 0) return;
	if (DetourDetach(&(PVOID&) app::CharacterLoader_OnOutfit, dCharacterLoader_OnOutfit) != 0) return;
	if (DetourDetach(&(PVOID&) app::SceneManager_Internal_ActiveSceneChanged, dSceneManager_Internal_ActiveSceneChanged) != 0) return;
	if (DetourDetach(&(PVOID&) app::ObjectInteractable_Interact, dObjectInteractable_Interact) != 0) return;
	if (DetourDetach(&(PVOID&) app::InGameHelpers_CreateDroppedObject, dInGameHelpers_CreateDroppedObject) != 0) return;
	if (DetourDetach(&(PVOID&) app::ObjectInteractable_Awake, dObjectInteractable_Awake) != 0) return;

	if (DetourDetach(&(PVOID&) app::SurvivalLobbyController_Update, dSurvivalLobbyController_Update) != 0) return;
	if (DetourDetach(&(PVOID&) app::RoomProtocolToken_Write, dRoomProtocolToken_Write) != 0) return;
	if (DetourDetach(&(PVOID&) app::BoltMatchmaking_UpdateSession, dBoltMatchmaking_UpdateSession) != 0) return;
	if (DetourDetach(&(PVOID&) app::SurvivalLobbyController_AllPlayersReady, dSurvivalLobbyController_AllPlayersReady) != 0) return;
	if (DetourDetach(&(PVOID&) app::MouseFollower_Update, dMouseFollower_Update) != 0) return;
	//蜘蛛拿蛋和掉蛋
	if (DetourDetach(&(PVOID&)app::SpiderBehaviour_OnEggCarried, dSpiderBehaviour_OnEggCarried) != 0) return;
	if (DetourDetach(&(PVOID&)app::SpiderBehaviour_Die_d_60_MoveNext, dSpiderBehaviour_Die_d_60_MoveNext) != 0) {
		std::cout << "[Unhook] SpiderBehaviour_Die_d_60_MoveNext 卸载失败" << std::endl;
	}
	else {
		std::cout << "[Unhook] SpiderBehaviour_Die_d_60_MoveNext 卸载成功" << std::endl;
	}
	// 移除幽灵相关 Hook
	if (DetourDetach(&(PVOID&)app::GhostBehaviour_OnRitualBookCarried, dGhostBehaviour_OnRitualBookCarried) != 0) return;
	// 卸载幽灵死亡协程 Hook
	if (DetourDetach(&(PVOID&)app::GhostBehaviour_Die_d_84_MoveNext, dGhostBehaviour_Die_d_84_MoveNext) != 0) {
		il2cppi_log_write("[Unhook] GhostBehaviour_Die_d_84_MoveNext 卸载失败");
	}
	else {
		il2cppi_log_write("[Unhook] GhostBehaviour_Die_d_84_MoveNext 卸载成功");
	}

	// 移除乌鸦相关 Hook
	if (DetourDetach(&(PVOID&)app::CrowBehaviour_OnHeadCarried, dCrowBehaviour_OnHeadCarried) != 0) return;
	// 卸载乌鸦死亡协程 Hook
	if (DetourDetach(&(PVOID&)app::CrowBehaviour_Die_d_72_MoveNext, dCrowBehaviour_Die_d_72_MoveNext) != 0) {
		il2cppi_log_write("[Unhook] CrowBehaviour_Die_d_72_MoveNext 卸载失败");
	}
	else {
		il2cppi_log_write("[Unhook] CrowBehaviour_Die_d_72_MoveNext 卸载成功");
	}

	if (DetourDetach(&(PVOID&) app::ServerConnectToken_Write, dServerConnectToken_Write) != 0) return;
	if (DetourDetach(&(PVOID&) app::ServerConnectToken_Read, dServerConnectToken_Read) != 0) return;

	if (DetourDetach(&(PVOID&) app::ServerAcceptToken_Read, dServerAcceptToken_Read) != 0) return;
	if (DetourDetach(&(PVOID&) app::ServerBrowser_Populate, dServerBrowser_Populate) != 0) return;
	if (DetourDetach(&(PVOID&) app::PhotonRoomProperties_Read, dPhotonRoomProperties_Read) != 0) return;
	if (DetourDetach(&(PVOID&) app::SteamUser_GetSteamID, dSteamUser_GetSteamID) != 0) return;
	if (DetourDetach(&(PVOID&) app::ServerBrowser_JoinSession, dServerBrowser_JoinSession) != 0) return;
	if (DetourDetach(&(PVOID&) app::SteamFriends_GetPersonaName, dSteamFriends_GetPersonaName) != 0) return;
	if (DetourDetach(&(PVOID&) app::ShouldCalmDown_OnUpdate, dShouldCalmDown_OnUpdate) != 0) return;
	if (DetourDetach(&(PVOID&) app::PlayersFocusingMeWait_OnUpdate, dPlayersFocusingMeWait_OnUpdate) != 0) return;
	if (DetourDetach(&(PVOID&) app::SurvivalAzazelBehaviour_AnnaFrying, dSurvivalAzazelBehaviour_AnnaFrying) != 0) return;
	if (DetourDetach(&(PVOID&) app::EnemyState_get_MultipleFocus, dEnemyState_get_MultipleFocus) != 0) return;
	if (DetourDetach(&(PVOID&) app::EnemyState_set_MultipleFocus, dEnemyState_set_MultipleFocus) != 0) return;
	if (DetourDetach(&(PVOID&) app::Menu_SetupOutfit, dMenu_SetupOutfit) != 0) return;
	if (DetourDetach(&(PVOID&) app::SteamInventoryManager_UserHasItem, dSteamInventoryManager_UserHasItem) != 0) return;
	if (DetourDetach(&(PVOID&) app::Menu_get_boltConfig, dMenu_get_boltConfig) != 0) return;
	if (DetourDetach(&(PVOID&) app::UIOutfitSelectionType_SetLocked, dUIOutfitSelectionType_SetLocked) != 0) return;
	if (DetourDetach(&(PVOID&) app::CharacterOutfit__ctor, dCharacterOutfit__ctor) != 0) return;
	if (DetourDetach(&(PVOID&) app::ProtocolTokenUtils_ReadToken, dProtocolTokenUtils_ReadToken) != 0) return;
	if (DetourDetach(&(PVOID&) app::Menu_GetPlayerId, dMenu_GetPlayerId) != 0) return;
	if (DetourDetach(&(PVOID&) app::SurvivalReviveInteractable_CanInteract, dSurvivalReviveInteractable_CanInteract) != 0) return;
	if (DetourDetach(&(PVOID&) app::NolanBehaviour_StartCarry, dNolanBehaviour_StartCarry) != 0) return;
	if (DetourDetach(&(PVOID&) app::KeyHelpers_IsKeyInArray, dKeyHelpers_IsKeyInArray) != 0) return;
	if (DetourDetach(&(PVOID&) app::DoorBehaviour_IsLocked, dDoorBehaviour_IsLocked) != 0) return;
}
