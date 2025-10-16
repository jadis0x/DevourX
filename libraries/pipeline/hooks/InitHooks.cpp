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

std::unordered_map<int32_t, std::vector<app::SteamItemDetails_t>> spoofedResults;
app::Menu* globalMenu;

std::vector<int> spoofedItemIDs = {
	121, 11, 8, 10, 6, 7, 16, 24, 14, 15, 4, 48, 12, 17, 28, 13, 23, 31, 38,
	9, 25, 26, 32, 33, 30, 49, 51, 97, 68, 80, 75, 62, 85, 59, 61, 57, 78,
	50, 104, 65, 69, 72, 73, 70, 117, 116, 105, 106, 108
};

static bool fxApplied = false;

void dDebug_Log(Object* message, MethodInfo* method)
{
	if (settings.bEnableUnityLogs)
		std::cout << "[UNITY]: " << ToString(message) << std::endl;

	app::Debug_2_Log(message, method);
}

void dDebug_LogError(Object* message, MethodInfo* method)
{
	if (settings.bEnableUnityLogs)
		std::cout << "[UNITY]: " << ToString(message) << std::endl;

	app::Debug_2_LogError(message, method);
}

void dDebug_LogException(Exception* exception, MethodInfo* method)
{
	if (settings.bEnableUnityLogs) {
		std::string excMsg = il2cppi_to_string(exception->fields._message);
		std::cout << "[UNITY]: " << excMsg << std::endl;
	}

	app::Debug_2_LogException(exception, method);
}

void dDebug_LogWarning(app::Object* message, MethodInfo* method)
{
	if (settings.bEnableUnityLogs)
		std::cout << "[UNITY]: " << ToString(message) << std::endl;

	app::Debug_2_LogWarning(message, method);
}

bool dCursor_get_visible(MethodInfo* method)
{
	if (settings.bShowMenu)
		return true;

	return app::Cursor_1_get_visible(method);
}

void dCursor_set_visible(bool value, MethodInfo* method)
{
	if (settings.bShowMenu)
		value = true;

	return app::Cursor_1_set_visible(value, method);
}

app::CursorLockMode__Enum dCursor_get_lockState(MethodInfo* method)
{
	if (settings.bShowMenu)
		return app::CursorLockMode__Enum::None;

	return app::Cursor_1_get_lockState(method);
}

void dCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method)
{
	if (settings.bShowMenu)
		value = app::CursorLockMode__Enum::None;

	app::Cursor_1_set_lockState(value, method);
}

float dInput_1_GetAxis(app::String* axisName, MethodInfo* method)
{
	if (settings.bShowMenu)
		return 0.0f;

	return app::Input_1_GetAxis(axisName, method);
}

void dServerConnectToken_Write(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method)
{
	Base::GlobalVar::__g_connectToken = __this;

	auto& f = __this->fields;

	if (settings.bGhostMod)
	{
		f.playerId = nullptr;
		f.uniqueId = nullptr;
		f.username = nullptr;
		f.survivalPrefabIdPreference = nullptr;
	}

	if (settings.bForcePublic)
	{
		f.isFromReconnect = true;
	}

	app::ServerConnectToken_Write(__this, packet, method);
}

void dServerConnectToken_Read(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method)
{
	auto& f = __this->fields;

	if (settings.bGhostMod)
	{
		f.playerId = nullptr;
		f.uniqueId = nullptr;
		f.username = nullptr;
		f.survivalPrefabIdPreference = nullptr; // test
	}

	if (settings.bForcePublic)
	{
		f.isFromReconnect = true;
	}

	app::ServerConnectToken_Read(__this, packet, method);
}

float interactableEspTimer = 0.0f;
float playerEspTimer = 0.0f;
constexpr float espRefreshInterval = 3.0f;

void dMenu_Update(app::Menu* __this, MethodInfo* method)
{
	globalMenu = __this;

	if (settings.bSpoofSteamName) {
		__this->fields.steamName = convert_to_system_string(settings.customSteamname);
	}

	if (settings.bSpoofSteamId) {
		__this->fields.steamID = convert_to_system_string(settings.customSteamIdStr);
	}

	app::Menu_Update(__this, method);
}

void dServerAcceptToken_Read(app::ServerAcceptToken* __this, app::UdpPacket* packet, MethodInfo* method)
{
	if (settings.bSpoofSteamName) {
		auto* newSteamName = convert_to_system_string(settings.customSteamname);

		if (newSteamName)
		{
			__this->fields.username = newSteamName;
		}
	}

	app::ServerAcceptToken_Read(__this, packet, method);
}

void dServerBrowser_Populate(app::ServerBrowser* __this, app::Map_2_System_Guid_UdpSession_* sessions, MethodInfo* method)
{
	// global vars
	Base::GlobalVar::__browser = __this;
	g_ServerList.clear();

	app::ServerBrowser_Populate(__this, sessions, method);

	ServerBrowserTAB::CollectServers(sessions);
}

void dPhotonRoomProperties_Read(app::PhotonRoomProperties* __this, app::UdpPacket* packet, MethodInfo* method)
{
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

app::CSteamID dSteamUser_GetSteamID(MethodInfo* method)
{
	auto r_steamId = app::SteamUser_GetSteamID(method);

	if (settings.bSpoofSteamId) {
		uint64_t spoofedId = strtoull(settings.customSteamIdStr, nullptr, 10);
		app::CSteamID spoof{ spoofedId };
		app::CSteamID__ctor_1(&spoof, spoofedId, nullptr);

		spoof.m_SteamID = spoofedId;

		return spoof;
	}

	return r_steamId;
}


void dServerBrowser_JoinSession(app::ServerBrowser* __this, app::PhotonSession* photonSession, app::RoomProtocolToken* token, app::String* password, MethodInfo* method)
{
	token->fields.isPasswordRequired = false;

	token->fields.isPrivate = false;
	token->fields.gameInProgress = false;

	photonSession->fields._IsOpen_k__BackingField = true;
	photonSession->fields._IsVisible_k__BackingField = true;

	app::PhotonSession_set_IsVisible(photonSession, true, nullptr);
	app::PhotonSession_set_IsOpen(photonSession, true, nullptr);

	app::ServerBrowser_JoinSession(__this, photonSession, token, password, method);
}

app::String* dSteamFriends_GetPersonaName(MethodInfo* method)
{
	auto steamName = app::SteamFriends_GetPersonaName(method);

	auto steamNameStr = il2cppi_to_string(steamName);

	if (settings.bSpoofSteamName)
	{
		auto* newSteamName = convert_to_system_string(settings.customSteamname);

		if (newSteamName)
		{
			return newSteamName;
		}
		else {
			return steamName;
		}
	}

	return steamName;
}

app::TaskStatus__Enum dShouldCalmDown_OnUpdate(app::ShouldCalmDown* __this, MethodInfo* method)
{
	if (settings.bNeverCalm)
	{
		__this->fields.disableCalmDown->fields._.mValue = true;
	}
	else {
		__this->fields.disableCalmDown->fields._.mValue = false;
	}

	if (settings.bAutoCalmWhenNearby)
	{
		__this->fields.staggerCount->fields._.mValue = 15;
	}

	auto shouldCalmDown = app::ShouldCalmDown_OnUpdate(__this, method);

	return shouldCalmDown;
}

app::TaskStatus__Enum dPlayersFocusingMeWait_OnUpdate(app::PlayersFocusingMeWait* __this, MethodInfo* method)
{
	if (settings.bAutoCalmWhenNearby)
	{
		return app::TaskStatus__Enum::Success;
	}

	return app::PlayersFocusingMeWait_OnUpdate(__this, method);
}

void dSurvivalAzazelBehaviour_AnnaFrying(app::SurvivalAzazelBehaviour* __this, int32_t fryCount, MethodInfo* method)
{
	if (settings.bAutoCalmWhenNearby)
	{
		fryCount = 4;
	}

	app::SurvivalAzazelBehaviour_AnnaFrying(__this, fryCount, method);
}

bool dEnemyState_get_MultipleFocus(app::EnemyState* __this, MethodInfo* method)
{
	if (settings.bAutoCalmWhenNearby)
	{
		return true;
	}

	return app::EnemyState_get_MultipleFocus(__this, method);
}

void dEnemyState_set_MultipleFocus(app::EnemyState* __this, bool value, MethodInfo* method)
{
	if (settings.bAutoCalmWhenNearby)
	{
		value = true;
	}

	app::EnemyState_set_MultipleFocus(__this, value, method);
}



app::UIOutfitSelectionType* dMenu_SetupOutfit(app::Menu* __this, app::CharacterOutfit* outfit, MethodInfo* method)
{
	outfit->fields.isHidden = false;
	outfit->fields.isOwned = true;
	outfit->fields.isSupporter = true;

	return app::Menu_SetupOutfit(__this, outfit, method);
}

bool dSteamInventoryManager_UserHasItem(app::SteamInventoryManager* __this, int32_t steamItemDefID, MethodInfo* method)
{
	return true;
}

app::BoltConfig* dMenu_get_boltConfig(app::Menu* __this, MethodInfo* method)
{
	auto* get_Config = app::Menu_get_boltConfig(__this, method);

	get_Config->fields.serverConnectionAcceptMode = app::BoltConnectionAcceptMode__Enum::Auto;

	return get_Config;
}

void dUIOutfitSelectionType_SetLocked(app::UIOutfitSelectionType* __this, bool locked, MethodInfo* method)
{
	__this->fields.outfitType->fields.isOwned = true;
	__this->fields.outfitType->fields.isSupporter = true;
	__this->fields.outfitType->fields.isHidden = false;

	locked = false;

	app::UIOutfitSelectionType_SetLocked(__this, locked, method);
}

void dCharacterOutfit__ctor(app::CharacterOutfit* __this, MethodInfo* method)
{
	__this->fields.isHidden = false;
	__this->fields.isOwned = true;
	__this->fields.isSupporter = true;

	app::CharacterOutfit__ctor(__this, method);
}

void dNolanBehaviour_FixedUpdate(app::NolanBehaviour* __this, MethodInfo* method)
{
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

	if (settings.bInteractableEsp) {
		interactableEspTimer += app::Time_1_get_deltaTime(nullptr);
		if (interactableEspTimer >= espRefreshInterval) {
			interactableEspTimer = 0.0f;

			auto& esp = ESPManager::GetInteractables();
			esp.Reset();
			esp.Update(nullptr);
			settings.bInteractableEspUpdated = true;
		}
	}
	else {
		interactableEspTimer = 0.0f;
	}

	if (settings.bPlayerEsp) {
		playerEspTimer += app::Time_1_get_deltaTime(nullptr);
		if (playerEspTimer >= espRefreshInterval) {
			playerEspTimer = 0.0f;

			auto& esp = ESPManager::GetPlayers();

			app::Survival* survival = __this->fields.m_Survival;
			if (survival) {
				esp.Reset();
				esp.Update(reinterpret_cast<app::Object_1*>(survival));
			}
		}
	}
	else {
		playerEspTimer = 0.0f;
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

	if (settings.bFreezeAzazel && Base::DevourNet::IsHost())
	{
		app::GameObject* azazelGameObject = __this->fields.m_Survival->fields.m_Azazel;

		if (azazelGameObject) {
			auto component = app::GameObject_GetComponentByName(azazelGameObject, convert_to_system_string("UltimateCharacterLocomotion"), nullptr);

			if (component) {
				app::UltimateCharacterLocomotion* locomotion = reinterpret_cast<app::UltimateCharacterLocomotion*>(component);

				if (locomotion) {

					app::UltimateCharacterLocomotion_set_TimeScale(locomotion, settings.azazelSpeed, NULL);
				}
			}
		}
	}

	return app::NolanBehaviour_FixedUpdate(__this, method);
}

void dNolanBehaviour_Update(app::NolanBehaviour* __this, MethodInfo* method)
{
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

	if (settings.bModifyPlayerSpeed && local_nb->fields.speedChangeAbility)
	{
		local_nb->fields.speedChangeAbility->fields.m_SpeedChangeMultiplier = settings.playerSpeed;
		local_nb->fields.speedChangeAbility->fields.m_MaxSpeedChangeValue = settings.playerSpeed;
	}

	if (settings.bFly)
	{
		app::Transform* transform = app::Component_get_transform((app::Component*)__this, nullptr);
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

		app::GameObject* thisGO = app::Component_get_gameObject((app::Component*)__this, nullptr);
		if (!SafePtr::IsValid(thisGO))
		{
			reset_local_state();
			return app::NolanBehaviour_Update(__this, method);
		}

		if (!SafePtr::IsValid((app::Object_1*)cachedLocomotion))
		{
			cachedLocomotion = app::GameObject_GetComponentByName(thisGO, convert_to_system_string("UltimateCharacterLocomotion"), nullptr);
		}

		if (SafePtr::IsValid((app::Object_1*)cachedLocomotion) && app::UltimateCharacterLocomotion_SetPosition_1)
		{
			app::UltimateCharacterLocomotion_SetPosition_1((app::UltimateCharacterLocomotion*)cachedLocomotion, pos, false, nullptr);
		}
	}

	return app::NolanBehaviour_Update(__this, method);
}

bool dDevourInput_GetLongPress(app::DevourInput* __this, app::String* name, float duration, bool waitForRelease, MethodInfo* method)
{
	if (settings.bDisableLongInteract)
	{
		duration = 0.0f;
	}

	return app::DevourInput_GetLongPress(__this, name, duration, waitForRelease, method);
}

void dNolanBehaviour_OnAttributeUpdateValue(app::NolanBehaviour* __this, app::Attribute* attribute, MethodInfo* method)
{

	if (settings.bModifyFlashlightColor)
	{
		app::Color color = { settings.flashlightColor[0], settings.flashlightColor[1], settings.flashlightColor[2], settings.flashlightColor[3] };

		__this->fields.flashlightUVColor = color;
	}

	if (settings.bUnlimitedUVLight) {


		if (strcmp(il2cppi_to_string(attribute->fields.m_Name).c_str(), "Battery") == 0)
		{
			attribute->fields.m_Value = 100.0f;
		}
	}

	return app::NolanBehaviour_OnAttributeUpdateValue(__this, attribute, method);
}

app::RankHelpers_ExpGainInfo* dRankHelpers_CalculateExpGain(app::RankHelpers* __this, int32_t mapProgress, int32_t numAwards, app::GameConfigToken* gameConfigToken, MethodInfo* method)
{
	if (settings.exp_modifier) {
		app::RankHelpers_ExpGainInfo* gain = app::RankHelpers_CalculateExpGain(__this, mapProgress, numAwards, gameConfigToken, method);

		int32_t bonus = (int32_t)settings.newExp;

		gain->fields.awardsBonus = bonus;
		gain->fields.winBonus = bonus;
		gain->fields.totalExp = bonus;

		return gain;
	}

	return app::RankHelpers_CalculateExpGain(__this, mapProgress, numAwards, gameConfigToken, method);
}

bool dLockedInteractable_CanInteract(app::LockedInteractable* __this, app::GameObject* character, MethodInfo* method)
{
	if (settings.bUnlockDoors) {
		__this->fields.isLocked = false;
		return true;
	}

	return app::LockedInteractable_CanInteract(__this, character, method);
}

bool dOptionsHelpers_IsCharacterUnlocked(app::OptionsHelpers* __this, app::String* prefab, MethodInfo* method)
{
	return true;
}

bool dOptionsHelpers_IsRobeUnlocked(app::OptionsHelpers* __this, app::String* robe, MethodInfo* method)
{
	return true;
}

app::UIPerkSelectionType* dMenu_SetupPerk(app::Menu* __this, app::CharacterPerk* perk, MethodInfo* method)
{
	perk->fields.isHidden = false;
	perk->fields.isOwned = true;
	perk->fields.cost = 0;

	return app::Menu_SetupPerk(__this, perk, method);
}

bool dSurvivalLobbyController_CanReady(app::SurvivalLobbyController* __this, MethodInfo* method)
{
	return true;
}

bool dSurvivalLobbyController_PlayableCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method)
{
	return true;
}

bool dSurvivalLobbyController_UnlockedCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method)
{
	return true;
}

void dSurvivalAzazelBehaviour_Update(app::SurvivalAzazelBehaviour* __this, MethodInfo* method)
{
	if (settings.bAzazelEsp)
	{
		ESPManager::GetAzazel().Update(reinterpret_cast<app::Object_1*>(__this));
	}


	app::SurvivalAzazelBehaviour_Update(__this, method);
}

void dAzazelAprilBehaviour_Update(app::AzazelAprilBehaviour* __this, MethodInfo* method)
{
	if (settings.bAzazelEsp)
	{
		ESPManager::GetAzazel().Update(reinterpret_cast<app::Object_1*>(__this));
	}

	app::AzazelAprilBehaviour_Update(__this, method);
}

void dCharacterLoader_Awake(app::CharacterLoader* __this, MethodInfo* method)
{
	app::CharacterLoader_Awake(__this, method);
}

void dCharacterLoader_OnOutfit(app::CharacterLoader* __this, MethodInfo* method)
{
	app::CharacterLoader_OnOutfit(__this, method);
}

//DO_APP_FUNC(0x02FFD5B0, void, SceneManager_Internal_ActiveSceneChanged, (Scene previousActiveScene, Scene newActiveScene, MethodInfo * method));
void dSceneManager_Internal_ActiveSceneChanged(app::Scene previousActiveScene, app::Scene newActiveScene, MethodInfo* method)
{
	ESPManager::ResetAll();
	Players::PlayersManager::Instance().ClearCache();
	LocalPlayer::LocalPlayerManager::Instance().ClearCache();
	settings.bInteractableEspUpdated = false;
	fxApplied = false;

	return app::SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene, method);
}

void dObjectInteractable_Interact(app::ObjectInteractable* __this, app::GameObject* character, MethodInfo* method)
{
	settings.bInteractableEspUpdated = false;
	__this->fields.m_CanInteract = true;

	app::ObjectInteractable_Interact(__this, character, method);
}

void dObjectInteractable_Awake(app::ObjectInteractable* __this, MethodInfo* method)
{
	__this->fields.m_CanInteract = true;

	app::ObjectInteractable_Awake(__this, method);

	if (settings.bInteractableEsp)
	{
		settings.bInteractableEspUpdated = false;
	}
}

app::BoltEntity* dInGameHelpers_CreateDroppedObject(app::InGameHelpers* __this, app::PrefabId prefabId, app::Vector3 position, app::String* carryObjectName, app::BoltEntity* player, bool fromKnockout, app::BoltEntity* ai, MethodInfo* method)
{
	settings.bInteractableEspUpdated = false;

	return app::InGameHelpers_CreateDroppedObject(__this, prefabId, position, carryObjectName, player, fromKnockout, ai, method);
}

void dSurvivalLobbyController_Update(app::SurvivalLobbyController* __this, MethodInfo* method)
{
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

void dRoomProtocolToken_Write(app::RoomProtocolToken* __this, app::UdpPacket* packet, MethodInfo* method)
{
	if (__this && __this->klass)
	{
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

	}

	app::RoomProtocolToken_Write(__this, packet, method);
}

void dBoltMatchmaking_UpdateSession(app::IProtocolToken* token, MethodInfo* method)
{
	if (token && token->klass)
	{
		const char* className = il2cpp_class_get_name((Il2CppClass*)token->klass);
		const char* classNamespace = il2cpp_class_get_namespace((Il2CppClass*)token->klass);

		if (strcmp(className, "PhotonRoomProperties") == 0)
		{
			auto roomToken = reinterpret_cast<app::PhotonRoomProperties*>(token);


			roomToken->fields._IsOpen_k__BackingField = true;
			roomToken->fields._IsVisible_k__BackingField = true;
		}
	}

	app::BoltMatchmaking_UpdateSession(token, method);
}

bool dSurvivalLobbyController_AllPlayersReady(app::SurvivalLobbyController* __this, MethodInfo* method)
{
	return true;
}

void dMouseFollower_Update(app::MouseFollower* __this, MethodInfo* method)
{
	if (settings.bMouseFollower) {
		if (!__this || !app::Camera_get_main(nullptr))
			return;

		app::Vector3 rawMouse = app::Input_1_get_mousePosition(nullptr);

		float screenWidth = (float)app::Screen_get_width(nullptr);
		float screenHeight = (float)app::Screen_get_height(nullptr);

		float normX = rawMouse.x / screenWidth;
		float normY = rawMouse.y / screenHeight;

		float offsetX = (normX - 0.5f) * 2.0f;
		float offsetY = (normY - 0.5f) * 2.0f;

		// offset
		offsetX *= settings.MF_offsetX;
		offsetY *= settings.MF_offsetY;

		static app::Vector3 basePos = app::Transform_get_position(app::Component_get_transform((app::Component*)__this, nullptr), nullptr);

		app::Vector3 target;
		target.x = basePos.x + offsetX;
		target.y = basePos.y + offsetY;
		target.z = basePos.z;

		app::Transform* transform = app::Component_get_transform((app::Component*)__this, nullptr);
		app::Vector3 current = app::Transform_get_position(transform, nullptr);

		float delta = app::Time_1_get_deltaTime(nullptr);
		float speed = settings.MF_Speed;

		app::Vector3 final = app::Vector3_Lerp(current, target, speed * delta, nullptr);
		app::Transform_set_position(transform, final, nullptr);
	}
	else {
		app::MouseFollower_Update(__this, method);
	}
}

app::IProtocolToken* dProtocolTokenUtils_ReadToken(app::UdpPacket* packet, MethodInfo* method)
{
	auto* token = app::ProtocolTokenUtils_ReadToken(packet, method);

	// test

	return token;
}

bool dSurvivalReviveInteractable_CanInteract(app::SurvivalReviveInteractable* __this, app::GameObject* character, MethodInfo* method) {
	return true;
}

bool dInteractable_CanInteract(app::Interactable* __this, app::GameObject* character, MethodInfo* method) {
	return true;
}

void dNolanBehaviour_StartCarry(app::NolanBehaviour* __this, app::String* objectName, MethodInfo* method) {

	if (PlayerHelper::IsLocalPlayer(__this) && objectName != nullptr)
	{
		std::cout << "carry object name: " << il2cppi_to_string(objectName) << "\n";
	}

	app::NolanBehaviour_StartCarry(__this, objectName, method);
}

void dKeyBehaviour_Attached(app::KeyBehaviour* __this, MethodInfo* method) {

	if (__this)
	{
		auto* nolan = LocalPlayer::GetNolan();

		if (nolan)
		{
			app::NolanBehaviour_StartCarry(nolan, __this->fields.keyName, method);
		}
	}

	app::KeyBehaviour_Attached(__this, method);
}

bool HookFunction(PVOID* ppPointer, PVOID pDetour, const char* functionName)
{
	if (const auto error = DetourAttach(ppPointer, pDetour); error != NO_ERROR) {
		std::cout << "[ERROR]: Failed to hook " << functionName << ", error " << error << std::endl;
		return false;
	}

	//std::cout << "[HOOKED]: " << functionName << std::endl;
	return true;
}

#define HOOKFUNC(n) if (!HookFunction(&(PVOID&)n, d##n, #n)) return;

bool UnhookFunction(PVOID* ppPointer, PVOID pDetour, const char* functionName)
{
	if (const auto error = DetourDetach(ppPointer, pDetour); error != NO_ERROR) {
		std::cout << "[ERROR]: Failed to unhook " << functionName << ", error " << error << std::endl;
		return false;
	}
	//std::cout << "[UNHOOKED]: " << functionName << std::endl;
	return true;
}

#define UNHOOKFUNC(n) if (!UnhookFunction(&(PVOID&)n, d##n, #n)) return;

void DetourInitilization()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	D3D_PRESENT_FUNCTION present = dx11api::GetPresentFunction();
	if (!present) {
		std::cout << "[ERROR]: Unable to retrieve IDXGISwapChain::Present method" << std::endl;
		return;
	}

	oPresent = present;
	if (!oPresent) {
		std::cout << "[ERROR]: oPresent is null!" << std::endl;
		return;
	}

	std::cout << "[INFO]: Attempting to hook oPresent at address: " << oPresent << std::endl;

	if (!HookFunction(&(PVOID&)oPresent, dPresent, "D3D_PRESENT_FUNCTION")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)Debug_2_Log, dDebug_Log, "Debug_2_Log")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)Debug_2_LogError, dDebug_LogError, "Debug_2_LogError")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)Debug_2_LogException, dDebug_LogException, "Debug_2_LogException")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)Debug_2_LogWarning, dDebug_LogWarning, "Debug_2_LogWarning")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::Cursor_1_get_visible, dCursor_get_visible, "Cursor_1_get_visible")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::Cursor_1_set_visible, dCursor_set_visible, "Cursor_1_set_visible")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::Cursor_1_get_lockState, dCursor_get_lockState, "Cursor_1_get_lockState")) {
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&)app::Cursor_1_set_lockState, dCursor_set_lockState, "Cursor_1_set_lockState")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::Input_1_GetAxis, dInput_1_GetAxis, "Input_1_GetAxis")) {
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&)app::NolanBehaviour_FixedUpdate, dNolanBehaviour_FixedUpdate, "NolanBehaviour_FixedUpdate")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::NolanBehaviour_Update, dNolanBehaviour_Update, "NolanBehaviour_Update")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::DevourInput_GetLongPress, dDevourInput_GetLongPress, "DevourInput_GetLongPress")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::NolanBehaviour_OnAttributeUpdateValue, dNolanBehaviour_OnAttributeUpdateValue, "dNolanBehaviour_OnAttributeUpdateValue")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::RankHelpers_CalculateExpGain, dRankHelpers_CalculateExpGain, "RankHelpers_CalculateExpGain")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::LockedInteractable_CanInteract, dLockedInteractable_CanInteract, "LockedInteractable_CanInteract")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::OptionsHelpers_IsCharacterUnlocked, dOptionsHelpers_IsCharacterUnlocked, "OptionsHelpers_IsCharacterUnlocked")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::OptionsHelpers_IsRobeUnlocked, dOptionsHelpers_IsRobeUnlocked, "OptionsHelpers_IsRobeUnlocked")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::Menu_SetupPerk, dMenu_SetupPerk, "Menu_SetupPerk")) {
		DetourTransactionAbort();
		return;
	}



	if (!HookFunction(&(PVOID&)app::SurvivalLobbyController_CanReady, dSurvivalLobbyController_CanReady, "SurvivalLobbyController_CanReady")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SurvivalLobbyController_PlayableCharacterSelected, dSurvivalLobbyController_PlayableCharacterSelected, "SurvivalLobbyController_PlayableCharacterSelected")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SurvivalLobbyController_UnlockedCharacterSelected, dSurvivalLobbyController_UnlockedCharacterSelected, "SurvivalLobbyController_UnlockedCharacterSelected")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SurvivalAzazelBehaviour_Update, dSurvivalAzazelBehaviour_Update, "SurvivalAzazelBehaviour_Update")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::AzazelAprilBehaviour_Update, dAzazelAprilBehaviour_Update, "dAzazelAprilBehaviour_Update")) {
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&)app::CharacterLoader_Awake, dCharacterLoader_Awake, "CharacterLoader_Awake")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::CharacterLoader_OnOutfit, dCharacterLoader_OnOutfit, "CharacterLoader_OnOutfit")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SceneManager_Internal_ActiveSceneChanged, dSceneManager_Internal_ActiveSceneChanged, "SceneManager_Internal_ActiveSceneChanged")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ObjectInteractable_Interact, dObjectInteractable_Interact, "ObjectInteractable_Interact")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::InGameHelpers_CreateDroppedObject, dInGameHelpers_CreateDroppedObject, "InGameHelpers_CreateDroppedObject")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ObjectInteractable_Awake, dObjectInteractable_Awake, "ObjectInteractable_Awake")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SurvivalLobbyController_Update, dSurvivalLobbyController_Update, "SurvivalLobbyController_Update")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::RoomProtocolToken_Write, dRoomProtocolToken_Write, "RoomProtocolToken_Write")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::BoltMatchmaking_UpdateSession, dBoltMatchmaking_UpdateSession, "BoltMatchmaking_UpdateSession")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SurvivalLobbyController_AllPlayersReady, dSurvivalLobbyController_AllPlayersReady, "SurvivalLobbyController_AllPlayersReady")) {
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&)app::MouseFollower_Update, dMouseFollower_Update, "MouseFollower_Update")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ServerConnectToken_Write, dServerConnectToken_Write, "ServerConnectToken_Write")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ServerConnectToken_Read, dServerConnectToken_Read, "dServerConnectToken_Read")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ServerAcceptToken_Read, dServerAcceptToken_Read, "ServerAcceptToken_Read")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ServerBrowser_Populate, dServerBrowser_Populate, "ServerBrowser_Populate")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::PhotonRoomProperties_Read, dPhotonRoomProperties_Read, "dPhotonRoomProperties_Read")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SteamUser_GetSteamID, dSteamUser_GetSteamID, "dSteamUser_GetSteamID")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ServerBrowser_JoinSession, dServerBrowser_JoinSession, "dServerBrowser_JoinSession")) {
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&)app::SteamFriends_GetPersonaName, dSteamFriends_GetPersonaName, "dSteamFriends_GetPersonaName")) {
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&)app::ShouldCalmDown_OnUpdate, dShouldCalmDown_OnUpdate, "dShouldCalmDown_OnUpdate")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::PlayersFocusingMeWait_OnUpdate, dPlayersFocusingMeWait_OnUpdate, "dPlayersFocusingMeWait_OnUpdate")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SurvivalAzazelBehaviour_AnnaFrying, dSurvivalAzazelBehaviour_AnnaFrying, "dSurvivalAzazelBehaviour_AnnaFrying")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::EnemyState_get_MultipleFocus, dEnemyState_get_MultipleFocus, "dEnemyState_get_MultipleFocus")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::EnemyState_set_MultipleFocus, dEnemyState_set_MultipleFocus, "dEnemyState_set_MultipleFocus")) {
		DetourTransactionAbort();
		return;
	}


	if (!HookFunction(&(PVOID&)app::Menu_SetupOutfit, dMenu_SetupOutfit, "dMenu_SetupOutfit")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::SteamInventoryManager_UserHasItem, dSteamInventoryManager_UserHasItem, "dSteamInventoryManager_UserHasItem")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::Menu_get_boltConfig, dMenu_get_boltConfig, "dMenu_get_boltConfig")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::UIOutfitSelectionType_SetLocked, dUIOutfitSelectionType_SetLocked, "dUIOutfitSelectionType_SetLocked")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::CharacterOutfit__ctor, dCharacterOutfit__ctor, "dCharacterOutfit__ctor")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&)app::ProtocolTokenUtils_ReadToken, dProtocolTokenUtils_ReadToken, "dProtocolTokenUtils_ReadToken")) {
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::SurvivalReviveInteractable_CanInteract, dSurvivalReviveInteractable_CanInteract, "dSurvivalReviveInteractable_CanInteract"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::Interactable_CanInteract, dInteractable_CanInteract, "dInteractable_CanInteract"))
	{
		DetourTransactionAbort();
		return;
	}

	if (!HookFunction(&(PVOID&) app::NolanBehaviour_StartCarry, dNolanBehaviour_StartCarry, "dNolanBehaviour_StartCarry"))
	{
		DetourTransactionAbort();
		return;
	}

	DetourTransactionCommit();
}

void DetourUninitialization()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourDetach(&(PVOID&)oPresent, dPresent) != 0) return;

	if (DetourTransactionCommit() == NO_ERROR) {
		DirectX::Shutdown();
		dx11api::Shutdown();
	}

	if (DetourDetach(&(PVOID&)Debug_2_Log, dDebug_Log) != 0) return;
	if (DetourDetach(&(PVOID&)Debug_2_LogError, dDebug_LogError) != 0) return;
	if (DetourDetach(&(PVOID&)Debug_2_LogException, dDebug_LogException) != 0) return;

	if (DetourDetach(&(PVOID&)app::Cursor_1_get_visible, dCursor_get_visible) != 0) return;
	if (DetourDetach(&(PVOID&)app::Cursor_1_set_visible, dCursor_set_visible) != 0) return;
	if (DetourDetach(&(PVOID&)app::Cursor_1_get_lockState, dCursor_get_lockState) != 0) return;
	if (DetourDetach(&(PVOID&)app::Cursor_1_set_lockState, dCursor_set_lockState) != 0) return;
	if (DetourDetach(&(PVOID&)app::Input_1_GetAxis, dInput_1_GetAxis) != 0) return;

	if (DetourDetach(&(PVOID&)app::NolanBehaviour_FixedUpdate, dNolanBehaviour_FixedUpdate) != 0) return;
	if (DetourDetach(&(PVOID&)app::NolanBehaviour_Update, dNolanBehaviour_Update) != 0) return;

	if (DetourDetach(&(PVOID&)app::DevourInput_GetLongPress, dDevourInput_GetLongPress) != 0) return;
	if (DetourDetach(&(PVOID&)app::NolanBehaviour_OnAttributeUpdateValue, dNolanBehaviour_OnAttributeUpdateValue) != 0) return;

	if (DetourDetach(&(PVOID&)app::RankHelpers_CalculateExpGain, dRankHelpers_CalculateExpGain) != 0) return;

	if (DetourDetach(&(PVOID&)app::LockedInteractable_CanInteract, dLockedInteractable_CanInteract) != 0) return;

	if (DetourDetach(&(PVOID&)app::OptionsHelpers_IsCharacterUnlocked, dOptionsHelpers_IsCharacterUnlocked) != 0) return;
	if (DetourDetach(&(PVOID&)app::OptionsHelpers_IsRobeUnlocked, dOptionsHelpers_IsRobeUnlocked) != 0) return;

	if (DetourDetach(&(PVOID&)app::OptionsHelpers_IsRobeUnlocked, dOptionsHelpers_IsRobeUnlocked) != 0) return;
	if (DetourDetach(&(PVOID&)app::OptionsHelpers_IsRobeUnlocked, dOptionsHelpers_IsRobeUnlocked) != 0) return;
	if (DetourDetach(&(PVOID&)app::OptionsHelpers_IsRobeUnlocked, dOptionsHelpers_IsRobeUnlocked) != 0) return;

	if (DetourDetach(&(PVOID&)app::SurvivalLobbyController_CanReady, dSurvivalLobbyController_CanReady) != 0) return;
	if (DetourDetach(&(PVOID&)app::SurvivalLobbyController_PlayableCharacterSelected, dSurvivalLobbyController_PlayableCharacterSelected) != 0) return;
	if (DetourDetach(&(PVOID&)app::SurvivalLobbyController_UnlockedCharacterSelected, dSurvivalLobbyController_UnlockedCharacterSelected) != 0) return;

	if (DetourDetach(&(PVOID&)app::SurvivalAzazelBehaviour_Update, dSurvivalAzazelBehaviour_Update) != 0) return;
	if (DetourDetach(&(PVOID&)app::AzazelAprilBehaviour_Update, dAzazelAprilBehaviour_Update) != 0) return;


	if (DetourDetach(&(PVOID&)app::CharacterLoader_Awake, dCharacterLoader_Awake) != 0) return;
	if (DetourDetach(&(PVOID&)app::CharacterLoader_OnOutfit, dCharacterLoader_OnOutfit) != 0) return;
	if (DetourDetach(&(PVOID&)app::SceneManager_Internal_ActiveSceneChanged, dSceneManager_Internal_ActiveSceneChanged) != 0) return;
	if (DetourDetach(&(PVOID&)app::ObjectInteractable_Interact, dObjectInteractable_Interact) != 0) return;
	if (DetourDetach(&(PVOID&)app::InGameHelpers_CreateDroppedObject, dInGameHelpers_CreateDroppedObject) != 0) return;
	if (DetourDetach(&(PVOID&)app::ObjectInteractable_Awake, dObjectInteractable_Awake) != 0) return;

	if (DetourDetach(&(PVOID&)app::SurvivalLobbyController_Update, dSurvivalLobbyController_Update) != 0) return;
	if (DetourDetach(&(PVOID&)app::RoomProtocolToken_Write, dRoomProtocolToken_Write) != 0) return;
	if (DetourDetach(&(PVOID&)app::BoltMatchmaking_UpdateSession, dBoltMatchmaking_UpdateSession) != 0) return;
	if (DetourDetach(&(PVOID&)app::SurvivalLobbyController_AllPlayersReady, dSurvivalLobbyController_AllPlayersReady) != 0) return;
	if (DetourDetach(&(PVOID&)app::MouseFollower_Update, dMouseFollower_Update) != 0) return;


	if (DetourDetach(&(PVOID&)app::ServerConnectToken_Write, dServerConnectToken_Write) != 0) return;
	if (DetourDetach(&(PVOID&)app::ServerConnectToken_Read, dServerConnectToken_Read) != 0) return;

	if (DetourDetach(&(PVOID&)app::ServerAcceptToken_Read, dServerAcceptToken_Read) != 0) return;
	if (DetourDetach(&(PVOID&)app::ServerBrowser_Populate, dServerBrowser_Populate) != 0) return;
	if (DetourDetach(&(PVOID&)app::PhotonRoomProperties_Read, dPhotonRoomProperties_Read) != 0) return;
	if (DetourDetach(&(PVOID&)app::SteamUser_GetSteamID, dSteamUser_GetSteamID) != 0) return;
	if (DetourDetach(&(PVOID&)app::ServerBrowser_JoinSession, dServerBrowser_JoinSession) != 0) return;
	if (DetourDetach(&(PVOID&)app::SteamFriends_GetPersonaName, dSteamFriends_GetPersonaName) != 0) return;
	if (DetourDetach(&(PVOID&)app::ShouldCalmDown_OnUpdate, dShouldCalmDown_OnUpdate) != 0) return;
	if (DetourDetach(&(PVOID&)app::PlayersFocusingMeWait_OnUpdate, dPlayersFocusingMeWait_OnUpdate) != 0) return;
	if (DetourDetach(&(PVOID&)app::SurvivalAzazelBehaviour_AnnaFrying, dSurvivalAzazelBehaviour_AnnaFrying) != 0) return;
	if (DetourDetach(&(PVOID&)app::EnemyState_get_MultipleFocus, dEnemyState_get_MultipleFocus) != 0) return;
	if (DetourDetach(&(PVOID&)app::EnemyState_set_MultipleFocus, dEnemyState_set_MultipleFocus) != 0) return;
	if (DetourDetach(&(PVOID&)app::Menu_SetupOutfit, dMenu_SetupOutfit) != 0) return;
	if (DetourDetach(&(PVOID&)app::SteamInventoryManager_UserHasItem, dSteamInventoryManager_UserHasItem) != 0) return;
	if (DetourDetach(&(PVOID&)app::Menu_get_boltConfig, dMenu_get_boltConfig) != 0) return;
	if (DetourDetach(&(PVOID&)app::UIOutfitSelectionType_SetLocked, dUIOutfitSelectionType_SetLocked) != 0) return;
	if (DetourDetach(&(PVOID&)app::CharacterOutfit__ctor, dCharacterOutfit__ctor) != 0) return;
	if (DetourDetach(&(PVOID&)app::ProtocolTokenUtils_ReadToken, dProtocolTokenUtils_ReadToken) != 0) return;

	if (DetourDetach(&(PVOID&) app::SurvivalReviveInteractable_CanInteract, dSurvivalReviveInteractable_CanInteract) != 0) return;
	if (DetourDetach(&(PVOID&) app::Interactable_CanInteract, dInteractable_CanInteract) != 0) return;
	if (DetourDetach(&(PVOID&) app::NolanBehaviour_StartCarry, dNolanBehaviour_StartCarry) != 0) return;
}
