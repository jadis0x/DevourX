#pragma once

#define SYS_STR_LOG(x) \
    do { \
        if (x) std::cout << il2cppi_to_string(x) << "\n"; \
        else std::cout << "<null string>\n"; \
    } while (0)

void DetourInitilization();
void DetourUninitialization();

// ============================================
// Unity Debug Logging
// ============================================
void dDebug_Log(app::Object* message, MethodInfo* method);
void dDebug_LogError(app::Object* message, MethodInfo* method);
void dDebug_LogException(app::Exception* exception, MethodInfo* method);
void dDebug_LogWarning(app::Object* message, MethodInfo* method);

// ============================================
// Input & Cursor
// ============================================
bool dCursor_get_visible(MethodInfo* method);
void dCursor_set_visible(bool value, MethodInfo* method);
app::CursorLockMode__Enum dCursor_get_lockState(MethodInfo* method);
void dCursor_set_lockState(app::CursorLockMode__Enum value, MethodInfo* method);
float dInput_1_GetAxis(app::String* axisName, MethodInfo* method);
bool dDevourInput_GetLongPress(app::DevourInput* __this, app::String* name, float duration, bool waitForRelease, MethodInfo* method);
void dMouseFollower_Update(app::MouseFollower* __this, MethodInfo* method);
bool dKeyHelpers_IsKeyInArray(app::KeyHelpers* __this, app::String* state, app::String* name, MethodInfo* method);

// ============================================
// Player & Gameplay
// ============================================
void dNolanBehaviour_Update(app::NolanBehaviour* __this, MethodInfo* method);
void dNolanBehaviour_FixedUpdate(app::NolanBehaviour* __this, MethodInfo* method);
void dNolanBehaviour_OnAttributeUpdateValue(app::NolanBehaviour* __this, app::Attribute* attribute, MethodInfo* method);
void dNolanBehaviour_StartCarry(app::NolanBehaviour* __this, app::String* objectName, MethodInfo* method);
app::RankHelpers_ExpGainInfo* dRankHelpers_CalculateExpGain(app::RankHelpers* __this, int32_t mapProgress, int32_t numAwards, app::GameConfigToken* gameConfigToken, MethodInfo* method);

// ============================================
// Steam & Network (Steam伪装相关)
// ============================================
// Steam身份伪装
app::String* dSteamFriends_GetPersonaName(MethodInfo* method);
app::CSteamID dSteamUser_GetSteamID(MethodInfo* method);
app::String* dMenu_GetPlayerId(app::Menu* __this, MethodInfo* method);
void dMenu_Update(app::Menu* __this, MethodInfo* method);

// 网络协议令牌
void dServerConnectToken_Write(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dServerConnectToken_Read(app::ServerConnectToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dServerAcceptToken_Read(app::ServerAcceptToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dServerDisconnectToken_Read(app::ServerDisconnectToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dRoomProtocolToken_Write(app::RoomProtocolToken* __this, app::UdpPacket* packet, MethodInfo* method);
void dPhotonRoomProperties_Read(app::PhotonRoomProperties* __this, app::UdpPacket* packet, MethodInfo* method);

// 网络匹配与连接
void dBoltMatchmaking_UpdateSession(app::IProtocolToken* token, MethodInfo* method);
void dServerBrowser_JoinSession(app::ServerBrowser* __this, app::PhotonSession* photonSession, app::RoomProtocolToken* token, app::String* password, MethodInfo* method);
void dServerBrowser_Populate(app::ServerBrowser* __this, app::Map_2_System_Guid_UdpSession_* sessions, MethodInfo* method);
app::BoltConfig* dMenu_get_boltConfig(app::Menu* __this, MethodInfo* method);
void dBoltNetwork_LoadScene_1(app::String * scene, app::IProtocolToken * token, MethodInfo * method);
// 协议工具
app::Byte__Array* dProtocolTokenUtils_ToByteArray(app::IProtocolToken* token, MethodInfo* method);
app::IProtocolToken* dProtocolTokenUtils_ReadToken(app::UdpPacket* packet, MethodInfo* method);

//透视
//蜘蛛拿蛋
extern void (*original_SpiderBehaviour_OnEggCarried)(app::SpiderBehaviour * __this, app::GameObject * egg, MethodInfo * method);
void dSpiderBehaviour_OnEggCarried(app::SpiderBehaviour * __this, app::GameObject * egg, MethodInfo * method);
extern bool (*original_SpiderBehaviour_Die_d_60_MoveNext)(app::SpiderBehaviour_Die_d_60 * __this, MethodInfo * method);
bool dSpiderBehaviour_Die_d_60_MoveNext(app::SpiderBehaviour_Die_d_60 * __this, MethodInfo * method);
// ========== 幽灵相关 Hook 声明 ==========

// 原始函数指针
extern void (*original_GhostBehaviour_OnRitualBookCarried)(app::GhostBehaviour * __this, app::GameObject * ritualBook, MethodInfo * method);
// Hook函数声明
void dGhostBehaviour_OnRitualBookCarried(app::GhostBehaviour * __this, app::GameObject * ritualBook, MethodInfo * method);
// 幽灵死亡协程 Hook
extern bool (*original_GhostBehaviour_Die_d_84_MoveNext)(app::GhostBehaviour_Die_d_84 * __this, MethodInfo * method);
bool dGhostBehaviour_Die_d_84_MoveNext(app::GhostBehaviour_Die_d_84 * __this, MethodInfo * method);

// ========== 乌鸦相关 Hook 声明 ==========

// 原始函数指针
extern void (*original_CrowBehaviour_OnHeadCarried)(app::CrowBehaviour * __this, app::GameObject * head, MethodInfo * method);

// Hook函数声明
void dCrowBehaviour_OnHeadCarried(app::CrowBehaviour * __this, app::GameObject * head, MethodInfo * method);

// 乌鸦死亡协程 Hook
extern bool (*original_CrowBehaviour_Die_d_72_MoveNext)(app::CrowBehaviour_Die_d_72 * __this, MethodInfo * method);
bool dCrowBehaviour_Die_d_72_MoveNext(app::CrowBehaviour_Die_d_72 * __this, MethodInfo * method);
// ============================================
// Lobby & Menu
// ============================================
void dSurvivalLobbyController_Update(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_CanReady(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_PlayableCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_UnlockedCharacterSelected(app::SurvivalLobbyController* __this, MethodInfo* method);
bool dSurvivalLobbyController_AllPlayersReady(app::SurvivalLobbyController* __this, MethodInfo* method);

// ============================================
// AI & Enemies
// ============================================
void dSurvivalAzazelBehaviour_Update(app::SurvivalAzazelBehaviour* __this, MethodInfo* method);
void dAzazelAprilBehaviour_Update(app::AzazelAprilBehaviour* __this, MethodInfo* method);
void dSurvivalAzazelBehaviour_AnnaFrying(app::SurvivalAzazelBehaviour* __this, int32_t fryCount, MethodInfo* method);
app::TaskStatus__Enum dShouldCalmDown_OnUpdate(app::ShouldCalmDown* __this, MethodInfo* method);
app::TaskStatus__Enum dPlayersFocusingMeWait_OnUpdate(app::PlayersFocusingMeWait* __this, MethodInfo* method);
bool dEnemyState_get_MultipleFocus(app::EnemyState* __this, MethodInfo* method);
void dEnemyState_set_MultipleFocus(app::EnemyState* __this, bool value, MethodInfo* method);

// ============================================
// Interactables & Objects
// ============================================
void dObjectInteractable_Interact(app::ObjectInteractable* __this, app::GameObject* character, MethodInfo* method);
void dObjectInteractable_Awake(app::ObjectInteractable* __this, MethodInfo* method);
bool dLockedInteractable_CanInteract(app::LockedInteractable* __this, app::GameObject* character, MethodInfo* method);
bool dDoorBehaviour_IsLocked(app::DoorBehaviour* __this, MethodInfo* method);
bool dSurvivalReviveInteractable_CanInteract(app::SurvivalReviveInteractable* __this, app::GameObject* character, MethodInfo* method);
bool dInteractable_CanInteract(app::Interactable* __this, app::GameObject* character, MethodInfo* method);
app::BoltEntity* dInGameHelpers_CreateDroppedObject(app::InGameHelpers* __this, app::PrefabId prefabId, app::Vector3 position, app::String* carryObjectName, app::BoltEntity* player, bool fromKnockout, app::BoltEntity* ai, MethodInfo* method);

// ============================================
// Character & Outfits
// ============================================
void dCharacterLoader_Awake(app::CharacterLoader* __this, MethodInfo* method);
void dCharacterLoader_OnOutfit(app::CharacterLoader* __this, MethodInfo* method);
app::UIOutfitSelectionType* dMenu_SetupOutfit(app::Menu* __this, app::CharacterOutfit* outfit, MethodInfo* method);
void dUIOutfitSelectionType_SetLocked(app::UIOutfitSelectionType* __this, bool locked, MethodInfo* method);
void dCharacterOutfit__ctor(app::CharacterOutfit* __this, MethodInfo* method);

// ============================================
// Unlock & Inventory
// ============================================
bool dOptionsHelpers_IsCharacterUnlocked(app::OptionsHelpers* __this, app::String* prefab, MethodInfo* method);
bool dOptionsHelpers_IsRobeUnlocked(app::OptionsHelpers* __this, app::String* robe, app::String* character, MethodInfo* method);
app::UIPerkSelectionType* dMenu_SetupPerk(app::Menu* __this, app::CharacterPerk* perk, MethodInfo* method);
bool dSteamInventoryManager_UserHasItem(app::SteamInventoryManager* __this, int32_t steamItemDefID, MethodInfo* method);

// ============================================
// Scene & Events
// ============================================
void dSceneManager_Internal_ActiveSceneChanged(app::Scene previousActiveScene, app::Scene newActiveScene, MethodInfo* method);