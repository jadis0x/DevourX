#pragma once

#include <vector>
#include <string>

namespace neo {
	extern std::vector<int> spoofedItemIDs;
	extern app::Menu* globalMenu;
}

namespace esp_manager {
	extern float interactableEspTimer;
	extern float playerEspTimer;
	constexpr float espRefreshInterval = 3.0f;
}

namespace UnityLogger {
	void HandleUnityLog(const char* level, const std::string& message);
}

void DetourInitilization();
void DetourUninitialization();

// Unity.Debug
void dDebug_Log(app::Object* message, MethodInfo* method);
void dDebug_LogError(app::Object* message, MethodInfo* method);
void dDebug_LogException(app::Exception* exception, MethodInfo* method);
void dDebug_LogWarning(app::Object* message, MethodInfo* method);

// Input
bool dCursor_get_visible(MethodInfo* method);
void dCursor_set_visible(bool value, MethodInfo* method);
app::CursorLockMode__Enum dCursor_get_lockState(MethodInfo* method);
void dCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method);
float dInput_1_GetAxis(app::String* axisName, MethodInfo* method);
bool dDevourInput_GetLongPress(app::DevourInput* __this, app::String* name, float duration, bool waitForRelease, MethodInfo* method);
void dMouseFollower_Update(app::MouseFollower* __this, MethodInfo* method);

// Network
void dServerConnectToken_Write(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dServerConnectToken_Read(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dServerAcceptToken_Read(app::ServerAcceptToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dServerDisconnectToken_Read(app::ServerDisconnectToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dRoomProtocolToken_Write(app::RoomProtocolToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dPhotonRoomProperties_Read(app::PhotonRoomProperties* __this, app::UdpPacket* packet, MethodInfo* method);
void dBoltMatchmaking_UpdateSession(app::IProtocolToken* token, MethodInfo* method);
void dServerBrowser_JoinSession(app::ServerBrowser* __this, app::PhotonSession* photonSession, app::RoomProtocolToken* token, app::String* password, MethodInfo* method);
app::Byte__Array* dProtocolTokenUtils_ToByteArray(app::IProtocolToken* token, MethodInfo* method);
app::BoltConfig* dMenu_get_boltConfig(app::Menu* __this, MethodInfo* method);
void dBoltNetwork_LoadScene_1(app::String* scene, app::IProtocolToken* token, MethodInfo* method);

// StunManager__ctor


// Steam
app::CSteamID dSteamUser_GetSteamID(MethodInfo* method);
app::String* dSteamFriends_GetPersonaName(MethodInfo* method);
app::String* dMenu_GetPlayerId(app::Menu* __this, MethodInfo* method);

// AI
void dSurvivalAzazelBehaviour_Update(app::SurvivalAzazelBehaviour* __this, MethodInfo* method);
void dAzazelAprilBehaviour_Update(app::AzazelAprilBehaviour* __this, MethodInfo* method);
app::TaskStatus__Enum dShouldCalmDown_OnUpdate(app::ShouldCalmDown* __this, MethodInfo* method);
app::TaskStatus__Enum dPlayersFocusingMeWait_OnUpdate(app::PlayersFocusingMeWait* __this, MethodInfo* method);
void dSurvivalAzazelBehaviour_AnnaFrying(app::SurvivalAzazelBehaviour* __this, int32_t fryCount, MethodInfo* method);
bool dEnemyState_get_MultipleFocus(app::EnemyState* __this, MethodInfo* method);
void dEnemyState_set_MultipleFocus(app::EnemyState* __this, bool value, MethodInfo* method);

// Outfit etc.
bool dOptionsHelpers_IsCharacterUnlocked(app::OptionsHelpers* __this, app::String* prefab, MethodInfo* method);
bool dOptionsHelpers_IsRobeUnlocked(app::OptionsHelpers* __this, app::String* robe, app::String* character, MethodInfo* method);
bool dOptionsHelpers_HasOutfit(app::OptionsHelpers* __this, app::String* outfitName, app::String* characterName, MethodInfo* method);
app::UIOutfitSelectionType* dMenu_SetupOutfit(app::Menu* __this, app::CharacterOutfit* outfit, MethodInfo* method);
app::UIPerkSelectionType* dMenu_SetupPerk(app::Menu* __this, app::CharacterPerk* perk, MethodInfo* method);
bool dSteamInventoryManager_UserHasItem(app::SteamInventoryManager* __this, int32_t steamItemDefID, MethodInfo* method);
void dUIOutfitSelectionType_SetLocked(app::UIOutfitSelectionType* __this, bool locked, MethodInfo* method);
void dUIPerkSelectionType_SetLocked(app::UIPerkSelectionType* __this, bool locked, int32_t cost, MethodInfo* method);
void dCharacterOutfit__ctor(app::CharacterOutfit* __this, MethodInfo* method);

// Player
void dNolanBehaviour_Update(app::NolanBehaviour* __this, MethodInfo* method);
void dNolanBehaviour_FixedUpdate(app::NolanBehaviour* __this, MethodInfo* method);
void dNolanBehaviour_OnAttributeUpdateValue(app::NolanBehaviour* __this, app::Attribute* attribute, MethodInfo* method);
app::RankHelpers_ExpGainInfo* dRankHelpers_CalculateExpGain(app::RankHelpers* __this, int32_t mapProgress, int32_t numAwards, app::GameConfigToken* gameConfigToken, MethodInfo* method);
void dNolanBehaviour_StartCarry(app::NolanBehaviour* __this, app::String* objectName, MethodInfo* method);

// Lobby
bool dSurvivalLobbyController_CanReady(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_PlayableCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_UnlockedCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method);
void dSurvivalLobbyController_Update(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_AllPlayersReady(app::SurvivalLobbyController* __this, MethodInfo* method);
int32_t dSurvivalLobbyController_PlayerPrefabsAttached(app::SurvivalLobbyController* __this, MethodInfo* method);


// Event
void dSceneManager_Internal_ActiveSceneChanged(app::Scene previousActiveScene, app::Scene newActiveScene, MethodInfo* method);

// Misc
void dMenu_Update(app::Menu* __this, MethodInfo* method);
bool dMenu_CanPlayMode(app::Menu* __this, app::DevourGameMode__Enum gameMode, app::DevourMap__Enum map, MethodInfo* method);

// Interact
void dObjectInteractable_Interact(app::ObjectInteractable* __this, app::GameObject* character, MethodInfo* method);
void dObjectInteractable_Awake(app::ObjectInteractable* __this, MethodInfo* method);
app::BoltEntity* dInGameHelpers_CreateDroppedObject(app::InGameHelpers* __this, app::PrefabId prefabId, app::Vector3 position, app::String* carryObjectName, app::BoltEntity* player, bool fromKnockout, app::BoltEntity* ai, MethodInfo* method);
bool dSurvivalReviveInteractable_CanInteract(app::SurvivalReviveInteractable* __this, app::GameObject* character, MethodInfo* method);
bool dKeyHelpers_IsKeyInArray(app::KeyHelpers* __this, app::String* state, app::String* name, MethodInfo* method);
bool dDoorBehaviour_IsLocked(app::DoorBehaviour* __this, MethodInfo* method);
bool dLockedInteractable_CanInteract(app::LockedInteractable* __this, app::GameObject* character, MethodInfo* method);



