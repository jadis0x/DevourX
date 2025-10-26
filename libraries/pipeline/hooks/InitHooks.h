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

void dNolanBehaviour_Update(app::NolanBehaviour* __this, MethodInfo* method);
void dNolanBehaviour_FixedUpdate(app::NolanBehaviour* __this, MethodInfo* method);

bool dDevourInput_GetLongPress(app::DevourInput* __this, app::String* name, float duration, bool waitForRelease, MethodInfo* method);

// DO_APP_FUNC(0x0060A950, void, NolanBehaviour_OnAttributeUpdateValue, (NolanBehaviour * __this, Attribute * attribute, MethodInfo * method));
void dNolanBehaviour_OnAttributeUpdateValue(app::NolanBehaviour* __this, app::Attribute* attribute, MethodInfo* method);

app::RankHelpers_ExpGainInfo* dRankHelpers_CalculateExpGain(app::RankHelpers* __this, int32_t mapProgress, int32_t numAwards, app::GameConfigToken* gameConfigToken, MethodInfo* method);
bool dLockedInteractable_CanInteract(app::LockedInteractable* __this, app::GameObject* character, MethodInfo* method);

bool dOptionsHelpers_IsCharacterUnlocked(app::OptionsHelpers* __this, app::String* prefab, MethodInfo* method);
bool dOptionsHelpers_IsRobeUnlocked(app::OptionsHelpers* __this, app::String* robe, MethodInfo* method);

bool dSurvivalLobbyController_CanReady(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_PlayableCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_UnlockedCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method);

//DO_APP_FUNC(0x0067A550, void, SurvivalAzazelBehaviour_Update, (SurvivalAzazelBehaviour * __this, MethodInfo * method));
void dSurvivalAzazelBehaviour_Update(app::SurvivalAzazelBehaviour* __this, MethodInfo* method);

// DO_APP_FUNC(0x004B6C30, void, AzazelAprilBehaviour_Update, (AzazelAprilBehaviour * __this, MethodInfo * method));
void dAzazelAprilBehaviour_Update(app::AzazelAprilBehaviour* __this, MethodInfo* method);

//DO_APP_FUNC(0x02FFD5B0, void, SceneManager_Internal_ActiveSceneChanged, (Scene previousActiveScene, Scene newActiveScene, MethodInfo * method));
void dSceneManager_Internal_ActiveSceneChanged(app::Scene previousActiveScene, app::Scene newActiveScene, MethodInfo* method);

// DO_APP_FUNC(0x00657A10, void, ObjectInteractable_Interact, (ObjectInteractable * __this, GameObject * character, MethodInfo * method));
void dObjectInteractable_Interact(app::ObjectInteractable* __this, app::GameObject* character, MethodInfo* method);

//DO_APP_FUNC(0x00656D20, void, ObjectInteractable_Awake, (ObjectInteractable * __this, MethodInfo * method));
void dObjectInteractable_Awake(app::ObjectInteractable* __this, MethodInfo* method);


//DO_APP_FUNC(0x00763190, BoltEntity *, InGameHelpers_CreateDroppedObject, (InGameHelpers * __this, PrefabId prefabId, Vector3 position, String * carryObjectName, BoltEntity * player, bool fromKnockout, BoltEntity * ai, MethodInfo * method));
app::BoltEntity* dInGameHelpers_CreateDroppedObject(app::InGameHelpers* __this, app::PrefabId prefabId, app::Vector3 position, app::String* carryObjectName, app::BoltEntity* player, bool fromKnockout, app::BoltEntity* ai, MethodInfo* method);

// DO_APP_FUNC(0x005F2A60, void, SurvivalLobbyController_Update, (SurvivalLobbyController * __this, MethodInfo * method));
void dSurvivalLobbyController_Update(app::SurvivalLobbyController* __this, MethodInfo* method);

// DO_APP_FUNC(0x00746AC0, void, RoomProtocolToken_Write, (RoomProtocolToken * __this, UdpPacket * packet, MethodInfo * method));
void dRoomProtocolToken_Write(app::RoomProtocolToken* __this, app::UdpPacket* packet, MethodInfo* method);

// DO_APP_FUNC(0x00B956A0, void, BoltMatchmaking_UpdateSession, (IProtocolToken * token, MethodInfo * method));
void dBoltMatchmaking_UpdateSession(app::IProtocolToken* token, MethodInfo* method);


// DO_APP_FUNC(0x005EF860, bool, SurvivalLobbyController_AllPlayersReady, (SurvivalLobbyController * __this, MethodInfo * method));
bool dSurvivalLobbyController_AllPlayersReady(app::SurvivalLobbyController* __this, MethodInfo* method);

// DO_APP_FUNC(0x006285B0, void, MouseFollower_Update, (MouseFollower * __this, MethodInfo * method));
void dMouseFollower_Update(app::MouseFollower* __this, MethodInfo* method);

// Cursor
bool dCursor_get_visible(MethodInfo* method);
void dCursor_set_visible(bool value, MethodInfo* method);
app::CursorLockMode__Enum dCursor_get_lockState(MethodInfo* method);
void dCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method);

// DO_APP_FUNC(0x0304C300, float, Input_1_GetAxis, (String * axisName, MethodInfo * method));
float dInput_1_GetAxis(app::String* axisName, MethodInfo* method);

//DO_APP_FUNC(0x00745ED0, void, ServerConnectToken_Write, (ServerConnectToken * __this, UdpPacket * packet, MethodInfo * method));
void dServerConnectToken_Write(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method);

// DO_APP_FUNC(0x00745E70, void, ServerConnectToken_Read, (ServerConnectToken * __this, UdpPacket * packet, MethodInfo * method));
void dServerConnectToken_Read(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method);

// DO_APP_FUNC(0x01E2F6F0, void, Menu_Update, (Menu * __this, MethodInfo * method));
void dMenu_Update(app::Menu* __this, MethodInfo* method);


// DO_APP_FUNC(0x00746170, void, ServerDisconnectToken_Read, (ServerDisconnectToken * __this, UdpPacket * packet, MethodInfo * method));
//void dServerDisconnectToken_Read(app::ServerDisconnectToken* __this, app::UdpPacket* packet, MethodInfo* method);

// DO_APP_FUNC(0x00746020, void, ServerAcceptToken_Read, (ServerAcceptToken * __this, UdpPacket * packet, MethodInfo * method));
void dServerAcceptToken_Read(app::ServerAcceptToken* __this, app::UdpPacket* packet, MethodInfo* method);

// DO_APP_FUNC(0x007453D0, void, RoomProtocolToken_Read, (RoomProtocolToken * __this, UdpPacket * packet, MethodInfo * method));
void dPhotonRoomProperties_Read(app::PhotonRoomProperties* __this, app::UdpPacket* packet, MethodInfo* method);

// DO_APP_FUNC(0x00C95A30, CSteamID, SteamUser_GetSteamID, (MethodInfo * method));
app::CSteamID dSteamUser_GetSteamID(MethodInfo* method);

// DO_APP_FUNC(0x0074A140, void, ServerBrowser_JoinSession, (ServerBrowser * __this, PhotonSession * photonSession, RoomProtocolToken * token, String * password, MethodInfo * method));
void dServerBrowser_JoinSession(app::ServerBrowser* __this, app::PhotonSession* photonSession, app::RoomProtocolToken* token, app::String* password, MethodInfo* method);

// DO_APP_FUNC(0x00C934E0, String *, SteamFriends_GetPersonaName, (MethodInfo * method));
app::String* dSteamFriends_GetPersonaName(MethodInfo* method);

// DO_APP_FUNC(0x008C5BF0, TaskStatus__Enum, ShouldCalmDown_OnUpdate, (ShouldCalmDown * __this, MethodInfo * method));
app::TaskStatus__Enum dShouldCalmDown_OnUpdate(app::ShouldCalmDown* __this, MethodInfo* method);

// DO_APP_FUNC(0x008C53C0, TaskStatus__Enum, PlayersFocusingMeWait_OnUpdate, (PlayersFocusingMeWait * __this, MethodInfo * method));
app::TaskStatus__Enum dPlayersFocusingMeWait_OnUpdate(app::PlayersFocusingMeWait* __this, MethodInfo* method);

// DO_APP_FUNC(0x006862D0, void, SurvivalAzazelBehaviour_AnnaFrying, (SurvivalAzazelBehaviour * __this, int32_t fryCount, MethodInfo * method));
void dSurvivalAzazelBehaviour_AnnaFrying(app::SurvivalAzazelBehaviour* __this, int32_t fryCount, MethodInfo* method);

// DO_APP_FUNC(0x00B437D0, bool, EnemyState_get_MultipleFocus, (EnemyState * __this, MethodInfo * method));
// DO_APP_FUNC(0x00B43830, void, EnemyState_set_MultipleFocus, (EnemyState* __this, bool value, MethodInfo* method));
bool dEnemyState_get_MultipleFocus(app::EnemyState* __this, MethodInfo* method);
void dEnemyState_set_MultipleFocus(app::EnemyState* __this, bool value, MethodInfo* method);

// DO_APP_FUNC(0x006E1120, UIOutfitSelectionType *, Menu_SetupOutfit, (Menu * __this, CharacterOutfit * outfit, MethodInfo * method));
app::UIOutfitSelectionType* dMenu_SetupOutfit(app::Menu* __this, app::CharacterOutfit* outfit, MethodInfo* method);

// DO_APP_FUNC(0x005B87A0, bool, SteamInventoryManager_UserHasItem, (SteamInventoryManager * __this, int32_t steamItemDefID, MethodInfo * method));
bool dSteamInventoryManager_UserHasItem(app::SteamInventoryManager* __this, int32_t steamItemDefID, MethodInfo* method);

// DO_APP_FUNC(0x006D8ED0, BoltConfig *, Menu_get_boltConfig, (Menu * __this, MethodInfo * method));
app::BoltConfig* dMenu_get_boltConfig(app::Menu* __this, MethodInfo* method);

// DO_APP_FUNC(0x0063AB50, void, UIOutfitSelectionType_SetLocked, (UIOutfitSelectionType * __this, bool locked, MethodInfo * method));
void dUIOutfitSelectionType_SetLocked(app::UIOutfitSelectionType* __this, bool locked, MethodInfo* method);

// DO_APP_FUNC(0x006B8F00, void, CharacterOutfit__ctor, (CharacterOutfit * __this, MethodInfo * method));
void dCharacterOutfit__ctor(app::CharacterOutfit* __this, MethodInfo* method);

// DO_APP_FUNC(0x0074C340, bool, SurvivalReviveInteractable_CanInteract, (SurvivalReviveInteractable * __this, GameObject * character, MethodInfo * method));
bool dSurvivalReviveInteractable_CanInteract(app::SurvivalReviveInteractable* __this, app::GameObject* character, MethodInfo* method);

// DO_APP_FUNC(0x005FDEB0, void, NolanBehaviour_StartCarry, (NolanBehaviour * __this, String * objectName, MethodInfo * method));
void dNolanBehaviour_StartCarry(app::NolanBehaviour* __this, app::String* objectName, MethodInfo* method);

// DO_APP_FUNC(0x0076A460, bool, KeyHelpers_IsKeyInArray, (KeyHelpers * __this, String * state, String * name, MethodInfo * method));
bool dKeyHelpers_IsKeyInArray(app::KeyHelpers* __this, app::String* state, app::String* name, MethodInfo* method);

// DO_APP_FUNC(0x007F6CB0, bool, DoorBehaviour_IsLocked, (DoorBehaviour * __this, MethodInfo * method));
bool dDoorBehaviour_IsLocked(app::DoorBehaviour* __this, MethodInfo* method);

// DO_APP_FUNC(0x007D48D0, bool, Menu_CanPlayMode, (Menu * __this, DevourGameMode__Enum gameMode, DevourMap__Enum map, MethodInfo * method));
bool dMenu_CanPlayMode(app::Menu* __this, app::DevourGameMode__Enum gameMode, app::DevourMap__Enum map, MethodInfo* method);

// DO_APP_FUNC(0x00BBC870, void, BoltNetwork_LoadScene_1, (String * scene, IProtocolToken * token, MethodInfo * method));
void dBoltNetwork_LoadScene_1(app::String* scene, app::IProtocolToken* token, MethodInfo* method);

// DO_APP_FUNC(0x007DBF90, String *, Menu_GetPlayerId, (Menu * __this, MethodInfo * method));
app::String* dMenu_GetPlayerId(app::Menu* __this, MethodInfo* method);