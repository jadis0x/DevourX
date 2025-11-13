
#include "pch-il2cpp.h"
#include <esp/esp.h>
#include "devourbase.h"
#include <map>
#include <player/player.h>
#include <resolver/il2cpp_resolver.h>
#include <random>

app::ServerBrowser* Base::GlobalVar::__browser = nullptr;
app::ServerConnectToken* Base::GlobalVar::__g_connectToken = nullptr;

uint64_t Base::Steam::GetUserID() {
	if (app::SteamUser_GetSteamID != nullptr)
	{
		if (const auto id = app::SteamUser_GetSteamID(nullptr); id.m_SteamID)
			return id.m_SteamID;
	}
	return 0;
}

bool Base::Game::IsInGame() {
	app::OptionsHelpers* helper = app::OptionsHelpers_get_singleton(nullptr);

	if (helper == nullptr) return false;

	return app::OptionsHelpers_get_inGame(helper, nullptr);
}

void Base::Gameplay::FullBright(app::NolanBehaviour* localPlayer_nolanBehaviour) {
	if (localPlayer_nolanBehaviour != nullptr)
	{
		if (app::Light* flashlight = localPlayer_nolanBehaviour->fields.flashlightSpot; flashlight != nullptr)
		{
			app::Light_set_intensity(flashlight, 1.0f, nullptr);
			app::Light_set_range(flashlight, 90.f, nullptr);
			app::Light_set_spotAngle(flashlight, 178.f, nullptr);

			app::Light_set_shadows(flashlight, app::LightShadows__Enum::None, nullptr);
		}
	}
}


bool Base::DevourNet::IsHost() {
	return app::BoltNetwork_get_IsServer(nullptr);
}

float Base::DevourNet::GetPing() {
	if (app::BoltConnection* connection = app::BoltNetwork_get_Server(nullptr))
	{
		const float ping_seconds = app::BoltConnection_get_PingNetwork(connection, nullptr);
		return (roundf(ping_seconds * 1000.0f));
	}

	return 0.0;
}

uint32_t Base::DevourNet::GetConnectionID() {
	return app::NetworkIdAllocator_get_LocalConnectionId(nullptr);
}

void Base::Gameplay::ForceLobbyStart() {
	if (DevourNet::IsHost())
	{
		static Il2cppData data;
		data.SetImage(ASSEMBLY_CSHARP);
		data.FindClass("Horror", "Menu");

		if (!data.Get()) return;

		UObject<app::Menu> finder;
		finder.FindAllObjects(data, true);

		if (app::Menu* menu = finder.FindByName("MenuController", data))
		{
			if (app::Menu_OnLobbyStartButtonClick != nullptr)
			{
				app::Menu_OnLobbyStartButtonClick(menu, nullptr);
			}
		}
	}
}


void Base::Gameplay::SetProgressTo(int32_t progress) {
	const std::string scene = Base::Gameplay::GetSceneName();

	if (scene == "Menu")
	{
		return;
	}

	if (!Base::DevourNet::IsHost())
	{
		return;
	}

	if (scene == "Inn" || scene == "Manor")
	{
		app::MapController* map_controller = Func::FindObject<app::MapController>(
			ASSEMBLY_CSHARP, "", "MapController");

		if (!map_controller)
		{
			return;
		}

		if (app::MapController_SetProgressTo != nullptr)
		{
			app::MapController_SetProgressTo(map_controller, progress, nullptr);
		}
	}
	else if (scene == "Slaughterhouse")
	{
		app::SlaughterhouseAltarController* altar_controller = Func::FindObject<app::SlaughterhouseAltarController>(
			ASSEMBLY_CSHARP, "", "SlaughterhouseAltarController");

		if (!altar_controller)
		{
			return;
		}

		if (app::SlaughterhouseAltarController_SkipToGoat != nullptr)
		{
			app::SlaughterhouseAltarController_SkipToGoat(altar_controller, progress, nullptr);
		}
	}
	else
	{
		app::SurvivalObjectBurnController* burn_controller = Func::FindObject<app::SurvivalObjectBurnController>(
			ASSEMBLY_CSHARP, "", "SurvivalObjectBurnController");

		if (!burn_controller)
		{
			return;
		}

		if (app::SurvivalObjectBurnController_SkipToGoat != nullptr)
		{
			app::SurvivalObjectBurnController_SkipToGoat(burn_controller, progress, nullptr);
		}
	}
}
void Base::Gameplay::CreateLobby(const char* region_code, const int lobby_limit, const bool is_private) {
	if (Base::Game::IsInGame() || LocalPlayer::GetLocalPlayer())
	{
		return;
	}

	// 这里是你之前提供的 CreateLobby 函数实现
	auto* image = Il2CppInvoker::LoadImageFromAssembly("udpkit.platform.photon.dll");
	auto* klass = Il2CppInvoker::FindClass(image, "UdpKit.Platform.Photon", "PhotonRegion");
	auto* method = Il2CppInvoker::FindMethod(klass, "GetRegion", 1);

	Il2CppString* region_str = il2cpp_string_new(region_code);
	void* args[1] = { region_str };

	Il2CppObject* result = Il2CppInvoker::InvokeStatic(method, args);
	app::PhotonRegion* region = result ? reinterpret_cast<app::PhotonRegion*>(result) : nullptr;

	Il2cppData menu_data;
	menu_data.SetImage(ASSEMBLY_CSHARP);
	menu_data.FindClass("Horror", "Menu");

	if (!menu_data.Get()) return;

	UObject<app::Menu> finder;
	finder.FindAllObjects(menu_data, true);

	if (app::Menu* menu = finder.FindByName("MenuController", menu_data); menu && region)
	{
		app::Menu_SetUDPPlatform(menu, region, nullptr);
		std::cout << "[SUCCESS] Server created in region: " << il2cppi_to_string(region->fields._City_k__BackingField) << "\n";

		if (app::BoltConfig* config = app::Menu_get_boltConfig(menu, nullptr))
		{
			config->fields.serverConnectionLimit = lobby_limit;
			std::cout << "[SUCCESS] New Connection Limit: " << lobby_limit << "\n";

			if (app::Toggle* toggle = menu->fields.hostPrivateServer)
				app::Toggle_Set(toggle, is_private, true, nullptr);

			app::BoltLauncher_StartServer_1(config, nullptr, nullptr);

			app::Menu_ShowCanvasGroup(menu, menu->fields.loadingCanvasGroup, true, nullptr);
			app::Menu_ShowCanvasGroup(menu, menu->fields.hostCanvasGroup, false, nullptr);
			app::Menu_ShowCanvasGroup(menu, menu->fields.mainMenuCanvasGroup, false, nullptr);
		}
	}
}
void Base::Gameplay::StartCarryItem(const char* itemName) {
	if (Base::Gameplay::GetSceneName() == "Menu")
	{
		return;
	}
	// 特殊处理玩偶头 - 使用和其他物品相同的安全调用方式
	if (strcmp(itemName, "SurvivalDollHead") == 0)
	{
		app::NolanBehaviour* nolan = LocalPlayer::GetNolan();
		if (!nolan) return;

		SafePtr::safe_call([&]()
			{
				il2cpp_thread_attach(il2cpp_domain_get());

				Il2CppClass* k = il2cpp_object_get_class(reinterpret_cast<Il2CppObject*>(nolan));
				il2cpp_runtime_class_init(k);

				const MethodInfo* baseMi = il2cpp_class_get_method_from_name(k, "StartCarry", 1);
				if (!baseMi) {
					std::printf("StartCarry could NOT be resolved (base)\n");
					return;
				}

				const MethodInfo* mi = il2cpp_object_get_virtual_method(reinterpret_cast<Il2CppObject*>(nolan), baseMi);
				if (!mi) {
					std::printf("StartCarry could NOT be resolved (virtual)\n");
					return;
				}

				Il2CppString* il2str = il2cpp_string_new("SurvivalDollHead");
				if (!il2str) {
					std::printf("string_new null\n");
					return;
				}

				void* args[1] = { il2str };
				Il2CppException* exc = nullptr;
				il2cpp_runtime_invoke(mi, nolan, args, &exc);

				if (exc)
				{
					char msg[1024] = {};
					char st[2048] = {};
					il2cpp_format_exception(exc, msg, sizeof(msg) - 1);
					il2cpp_format_stack_trace(exc, st, sizeof(st) - 1);
					std::printf("[StartCarry] EXC: %s\nSTACK:\n%s\n", msg, st);
				}
				else
				{
					std::printf("[StartCarry] Successfully carried: SurvivalDollHead\n");
				}
			});
		return;
	}
	static const std::map<std::string, std::string> itemMap = {
		{"Hay", "SurvivalHay"},
		{"First aid", "SurvivalFirstAid"},
		{"Spade", "SurvivalSpade"},
		{"Cake", "SurvivalCake"},
		{"Bone", "SurvivalBone"},
		{"Battery", "SurvivalBattery"},
		{"Gasoline", "SurvivalGasoline"},
		{"Fuse", "SurvivalFuse"},
		{"Food", "SurvivalRottenFood"},
		{"Bleach", "SurvivalBleach"},
		{"Salt", "SurvivalSalt"},
		{"Ritual Book (in-active)", "RitualBook-InActive-1"},
		{"Ritual Book (active)", "RitualBook-Active-1"},
		{"Matchbox", "Matchbox-3"},
		{"Egg-1", "Egg-Clean-1"}, {"Egg-2", "Egg-Clean-2"}, {"Egg-3", "Egg-Clean-3"},
		{"Egg-4", "Egg-Clean-4"}, {"Egg-5", "Egg-Clean-5"}, {"Egg-6", "Egg-Clean-6"},
		{"Egg-7", "Egg-Clean-7"}, {"Egg-8", "Egg-Clean-8"}, {"Egg-9", "Egg-Clean-9"},
		{"Egg-10", "Egg-Clean-10"},
		{"Head (Dirty/Gardener)", "Head-Dirty-Gardener"},
		{"Head (Clean/Gardener)", "Head-Clean-Gardener"},
		{"Head (Dirty/Butler)", "Head-Dirty-Butler"},
		{"Head (Clean/Butler)", "Head-Clean-Butler"},
		{"Head (Dirty/Bridesmaid_01)", "Head-Dirty-Bridesmaid_01"},
		{"Head (Clean/Bridesmaid_01)", "Head-Clean-Bridesmaid_01"},
		{"Head (Dirty/Bridesmaid_02)", "Head-Dirty-Bridesmaid_02"},
		{"Head (Clean/Bridesmaid_02)", "Head-Clean-Bridesmaid_02"},
		{"Head (Dirty/Cook)", "Head-Dirty-Cook"},
		{"Head (Clean/Cook)", "Head-Clean-Cook"},
		{"Head (Dirty/Groomsman_01)", "Head-Dirty-Groomsman_01"},
		{"Head (Clean/Groomsman_01)", "Head-Clean-Groomsman_01"},
		{"Head (Dirty/Groomsman_02)", "Head-Dirty-Groomsman_02"},
		{"Head (Clean/Groomsman_02)", "Head-Clean-Groomsman_02"},
		{"Head (Dirty/Maid)", "Head-Dirty-Maid"},
		{"Head (Clean/Maid)", "Head-Clean-Maid"},
		{"Head (Dirty/Photographer)", "Head-Dirty-Photographer"},
		{"Head (Clean/Photographer)", "Head-Clean-Photographer"},
		{"Head (Dirty/Priest)", "Head-Dirty-Priest"},
		{"Head (Clean/Priest)", "Head-Clean-Priest"},
		{"Coin", "SurvivalCoin"},
		{"Music Box (Idle)", "MusicBox-Idle"},
		{"Music Box (Armed)", "MusicBox-Armed"},
	};

	const auto it = itemMap.find(itemName);
	if (it == itemMap.end())
	{
		std::printf("[StartCarryItem] Item not found: %s\n", itemName);
		return;
	}

	app::GameObject* local_player = LocalPlayer::GetLocalPlayer();
	if (!SafePtr::IsValid(local_player))
	{
		return;
	}

	app::NolanBehaviour* nolan = LocalPlayer::GetNolan();
	if (!nolan)
	{
		return;
	}

	const std::string internal_name = it->second;

	SafePtr::safe_call([&]()
		{
			il2cpp_thread_attach(il2cpp_domain_get());

			Il2CppClass* k = il2cpp_object_get_class(reinterpret_cast<Il2CppObject*>(nolan));
			il2cpp_runtime_class_init(k);

			// 对于其他物品，使用反射方式调用StartCarry
			const MethodInfo* baseMi = il2cpp_class_get_method_from_name(k, "StartCarry", 1);
			if (!baseMi) {
				std::printf("StartCarry could NOT be resolved (base)\n");
				return;
			}

			const MethodInfo* mi = il2cpp_object_get_virtual_method(reinterpret_cast<Il2CppObject*>(nolan), baseMi);
			if (!mi) {
				std::printf("StartCarry could NOT be resolved (virtual)\n");
				return;
			}

			Il2CppString* il2str = il2cpp_string_new(internal_name.c_str());
			if (!il2str) {
				std::printf("string_new null\n");
				return;
			}

			void* args[1] = { il2str };
			Il2CppException* exc = nullptr;
			il2cpp_runtime_invoke(mi, nolan, args, &exc);

			if (exc)
			{
				char msg[1024] = {};
				char st[2048] = {};
				il2cpp_format_exception(exc, msg, sizeof(msg) - 1);
				il2cpp_format_stack_trace(exc, st, sizeof(st) - 1);
				std::printf("[StartCarry] EXC: %s\nSTACK:\n%s\n", msg, st);
			}
			else
			{
				std::printf("[StartCarry] Successfully carried: %s -> %s\n", itemName, internal_name.c_str());
			}
		});
}

void Base::Gameplay::StartCarryAnimal(const char* animal_name) {
	if (Base::Gameplay::GetSceneName() == "Menu")
	{
		return;
	}

	std::map<std::string, std::string> animal_map = {
		{"Rat", "SurvivalRat"},
		{"Goat", "SurvivalGoat"},
		{"Pig", "SurvivalPig"},
		{"DollHead", "SurvivalDollHead"}
	};

	const std::string carry_animal = animal_map[animal_name];

	if (!carry_animal.empty() && LocalPlayer::GetLocalPlayer())
	{
		app::NolanBehaviour_StartCarry(LocalPlayer::GetNolan(), convert_to_system_string(carry_animal.c_str()), nullptr);
	}
}

void Base::Gameplay::InstantiatePrefab(const char* prefabName, app::PrefabId app::BoltPrefabs__StaticFields::* prefabField)
{
	if (!prefabField || !Base::DevourNet::IsHost())
	{
		return;
	}

	auto* typeInfo = (*app::BoltPrefabs__TypeInfo);
	if (!typeInfo || !typeInfo->static_fields)
	{
		std::cout << "[-] BoltPrefabs static_fields not found.\n";
		return;
	}

	il2cpp_thread_attach(il2cpp_domain_get());

	Il2CppClass* boltKlass = reinterpret_cast<Il2CppClass*>((*app::BoltNetwork__TypeInfo));
	il2cpp_runtime_class_init(boltKlass);

	auto* playerGO = LocalPlayer::GetLocalPlayer();
	if (!playerGO) return;

	auto* transform = app::GameObject_get_transform(playerGO, nullptr);
	if (!transform) return;

	app::Vector3 pos = app::Transform_get_position(transform, nullptr);
	app::Vector3 forward = app::Transform_get_forward(transform, nullptr);

	pos.x += forward.x * 1.5f;
	pos.y += forward.y * 1.5f;
	pos.z += forward.z * 1.5f;

	app::Quaternion rot = app::Transform_get_rotation(transform, nullptr);

	const app::PrefabId prefabId = typeInfo->static_fields->*prefabField;

	SafePtr::safe_call([&]() {
		auto* spawned = app::BoltNetwork_Instantiate_6(prefabId, pos, rot, nullptr);

		if (spawned && spawned->klass)
		{
			std::cout << "[+] Spawn successful: "
				<< (prefabName ? prefabName : "<unknown>") << " -> " << spawned << std::endl;

			auto* gameObject = app::Component_get_gameObject(reinterpret_cast<app::Component*>(spawned), nullptr);
			if (!gameObject) return;

			app::String* compName = convert_to_system_string("SurvivalAzazelBehaviour");
			auto* behaviourComp = app::GameObject_GetComponentByName(gameObject, compName, nullptr);

			if (behaviourComp)
			{
				std::cout << "[+] Found SurvivalAzazelBehaviour, calling Spawn()\n";

				Il2CppClass* behaviourKlass = il2cpp_object_get_class(reinterpret_cast<Il2CppObject*>(behaviourComp));
				il2cpp_runtime_class_init(behaviourKlass);

				const MethodInfo* spawnMethod = il2cpp_class_get_method_from_name(behaviourKlass, "Spawn", 0);
				if (spawnMethod)
				{
					Il2CppException* exc = nullptr;
					il2cpp_runtime_invoke(spawnMethod, behaviourComp, nullptr, &exc);

					if (exc)
					{
						char msg[512];
						il2cpp_format_exception(exc, msg, sizeof(msg));
						std::cout << "[-] Exception calling Spawn(): " << msg << std::endl;
					}
					else
					{
						std::cout << "[+] AI Spawn() called successfully.\n";
					}
				}
				else
				{
					std::cout << "[-] Spawn() method not found on SurvivalAzazelBehaviour.\n";
				}
			}
			else
			{
				std::cout << "[-] SurvivalAzazelBehaviour component not found on prefab.\n";
			}
		}
		else
		{
			std::cout << "[-] Spawn nullptr or invalid: "
				<< (prefabName ? prefabName : "<unknown>") << std::endl;
		}
		});
}


std::string Base::Gameplay::GetSceneName() {
	app::Scene scene = app::SceneManager_GetActiveScene(nullptr);
	app::String* name = app::Scene_get_name(&scene, nullptr);

	return name ? il2cppi_to_string(name) : "Unknown";
}

bool Base::Gameplay::IsPlayerCrawling(app::NolanBehaviour* nolan) {
	if (!nolan) return false;

	return app::NolanBehaviour_IsCrawling(nolan, nullptr);
}

bool Base::Gameplay::IsSequencePlaying() {
	static app::InGameHelpers* cached_helper = nullptr;

	if (!cached_helper)
		cached_helper = app::InGameHelpers_get_singleton(nullptr);

	if (!cached_helper)
		return false;

	auto* survival = cached_helper->fields.m_Survival;
	if (!survival)
		return false;

	return app::Survival_IsEndingPlaying(survival, nullptr)
		|| app::Survival_IsJumpScarePlaying(survival, nullptr)
		|| app::Survival_StartingToPlayFailEnding(survival, nullptr);
}

void Base::Gameplay::Knockout(app::GameObject* target_player) {
	if (!SafePtr::IsValid(target_player))
	{
		return;
	}
	if (!Base::DevourNet::IsHost())
	{
		return;
	}
	if (Base::Gameplay::GetSceneName() == "Menu")
	{
		return;
	}

	app::Survival* survival = Func::FindObject<app::Survival>(ASSEMBLY_CSHARP, "", "Survival");

	if (!survival)
	{
		return;
	}

	app::GameObject* azazel_game_object = app::Survival_GetAzazel(survival, nullptr);

	if (!azazel_game_object)
	{
		return;
	}

	app::SurvivalAzazelBehaviour* azazel_behaviour = Func::FindObject<app::SurvivalAzazelBehaviour>(ASSEMBLY_CSHARP, "", "SurvivalAzazelBehaviour");

	if (!azazel_behaviour)
	{
		return;
	}


	if (app::SurvivalAzazelBehaviour_OnKnockout != nullptr)
	{
		app::SurvivalAzazelBehaviour_OnKnockout(azazel_behaviour, azazel_game_object, target_player, nullptr);
	}
}

void Base::Gameplay::Revive(app::GameObject* target_player) {
	if (!Base::Game::IsInGame()) return;

	if (!SafePtr::IsValid(target_player))
	{
		return;
	}
	if (!Base::DevourNet::IsHost())
	{
		return;
	}

	UComponent u_nolan;
	u_nolan.Attach(target_player);
	app::Component* nb_component = u_nolan.GetComponentByName("NolanBehaviour");
	if (!nb_component)
	{
		return;
	}

	const auto nb = reinterpret_cast<app::NolanBehaviour*>(nb_component);
	if (!nb)
	{
		return;
	}
	if (!Base::Gameplay::IsPlayerCrawling(nb))
	{
		return;
	}

	if (app::SurvivalReviveInteractable* revive_interactable = Func::FindObject<app::SurvivalReviveInteractable>(ASSEMBLY_CSHARP, "", "SurvivalReviveInteractable"); !revive_interactable)
	{
		if (app::Quaternion_get_identity && app::NolanBehaviour_TeleportTo)
		{
			constexpr app::Vector3 vec { 0.f, -150.f, 0.f };
			app::NolanBehaviour_TeleportTo(nb, vec, app::Quaternion_get_identity(nullptr), false, nullptr);
		}
	}
	else
	{
		app::SurvivalReviveInteractable_Interact(revive_interactable, target_player, nullptr);
	}
}

void Base::Gameplay::Jumpscare(app::GameObject* target_player) {
	if (!Base::Game::IsInGame()) return;

	if (!SafePtr::IsValid(target_player))
	{
		return;
	}
	if (!Base::DevourNet::IsHost())
	{
		return;
	}
	if (Base::Gameplay::GetSceneName() == "Menu")
	{
		return;
	}

	app::Survival* survival = Func::FindObject<app::Survival>(ASSEMBLY_CSHARP, "", "Survival");
	if (!survival)
	{
		return;
	}

	app::GameObject* azazel_game_object = app::Survival_GetAzazel(survival, nullptr);
	if (!azazel_game_object)
	{
		return;
	}

	app::SurvivalAzazelBehaviour* azazel_behaviour = Func::FindObject<app::SurvivalAzazelBehaviour>(ASSEMBLY_CSHARP, "", "SurvivalAzazelBehaviour");
	if (!azazel_behaviour)
	{
		return;
	}

	UComponent u_nolan;
	u_nolan.Attach(target_player);
	app::Component* nb_component = u_nolan.GetComponentByName("NolanBehaviour");
	if (!nb_component)
	{
		return;
	}

	if (const auto nb = reinterpret_cast<app::NolanBehaviour*>(nb_component); !nb)
	{
		return;
	}

	if (app::SurvivalAzazelBehaviour_OnPickedUpPlayer)
	{
		app::SurvivalAzazelBehaviour_OnPickedUpPlayer(azazel_behaviour, azazel_game_object, target_player, true, nullptr);
	}
}

void Base::Gameplay::TP(app::GameObject* target_player) {
	if (!Base::Game::IsInGame()) return;

	if (!target_player) return;

	app::NolanBehaviour* local_player = LocalPlayer::GetNolan();

	if (!local_player) return;

	app::Transform* target_transform = app::GameObject_get_transform(target_player, nullptr);

	if (!target_transform) return;

	const app::Vector3 target_pos = app::Transform_get_localPosition(target_transform, nullptr);

	app::NolanBehaviour_TeleportTo(local_player, target_pos, app::Quaternion_get_identity(nullptr), false, nullptr);
}

void Base::Gameplay::TPAzazel(app::GameObject* target_player) {
	if (!Base::Game::IsInGame()) return;

	if (!target_player) return;

	static UGameObject azazel_go;
	app::GameObject* azazel = azazel_go.FindWithTag("Azazel");

	if (!azazel) return;

	app::String* sys_str = convert_to_system_string("UltimateCharacterLocomotion");

	static UComponent azazel_locomotion_comp;
	azazel_locomotion_comp.Attach(azazel);

	app::Component* comp = azazel_locomotion_comp.GetComponentByName("UltimateCharacterLocomotion");

	if (!comp) return;

	if (const auto locomotion = reinterpret_cast<app::UltimateCharacterLocomotion*>(comp))
	{
		app::Transform* target_transform = app::GameObject_get_transform(target_player, nullptr);

		if (!target_transform) return;

		const app::Vector3 target_pos = app::Transform_get_localPosition(target_transform, nullptr);

		app::UltimateCharacterLocomotion_SetPosition_1(locomotion, target_pos, false, nullptr);
	}
}

void Base::Gameplay::Shoot(app::GameObject* target_player) {
	if (!Base::DevourNet::IsHost())
	{
		return;
	}

	if (!target_player)
	{
		return;
	}

	app::AzazelSamBehaviour* azazel_behaviour = Func::FindObject<app::AzazelSamBehaviour>(ASSEMBLY_CSHARP, "", "AzazelSamBehaviour");

	if (!azazel_behaviour)
	{
		return;
	}

	app::AzazelSamBehaviour_OnShootPlayer(azazel_behaviour, target_player, true, nullptr);
}

void Base::Gameplay::setRank(app::GameObject* local_player, int32_t new_rank) {
	if (!SafePtr::IsValid(local_player)) return;

	const auto component = app::GameObject_GetComponentByName(local_player, convert_to_system_string("NolanRankController"), nullptr);

	if (!component) return;

	if (const auto rank_controller = reinterpret_cast<app::NolanRankController*>(component))
	{
		app::NolanRankController_SetRank(rank_controller, new_rank, nullptr);
	}
}


std::string Base::System::generate_random_unique_id(int length) {
	static const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	static std::mt19937 rng(std::random_device {}());
	static std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

	std::string id;
	for (int i = 0; i < length; ++i)
	{
		id += charset[dist(rng)];
	}
	return id;
}

uint64_t Base::System::GenerateRandomSteamID64() {
	const uint64_t universe = 1ull << 56;
	const uint64_t accountType = 1ull << 52;
	const uint64_t instance = 1ull << 32;

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dist(1, 999999999);

	uint64_t accountID = dist(gen);

	return universe | accountType | instance | accountID;
}
// 在 devourbase.cpp 文件末尾添加以下代码

#include <random>
#include <algorithm>

std::vector<app::SurvivalInteractable*> Base::Gameplay::GetSurvivalInteractables()
{
	std::vector<app::SurvivalInteractable*> interactables;

	try {
		// 使用 ESP 中已经实现的 InteractableEsp 方法来获取物品
		auto& esp = ESPManager::GetInteractables();

		// 强制重置并更新ESP数据
		esp.Reset();
		esp.Update(nullptr);

		// 由于没有 GetEntities() 方法，我们需要直接访问内部数据
		// 这里我们重新实现 ESP 中的物品搜索逻辑
		UObject<app::ObjectInteractable> interactSearch;
		Il2cppData il2cppData;

		il2cppData.SetImage(ASSEMBLY_CSHARP);
		il2cppData.FindClass(NS_EMPTY, "ObjectInteractable");

		auto& rawObjects = interactSearch.FindAllObjects(il2cppData);
		if (rawObjects.empty()) return interactables;

		for (auto* obj : rawObjects)
		{
			if (!SafePtr::IsValid(reinterpret_cast<app::Object_1*>(obj))) continue;

			app::GameObject* go = app::Component_get_gameObject(reinterpret_cast<app::Component*>(obj), nullptr);
			if (!go || !SafePtr::IsValid(go)) continue;

			auto* tf = app::Component_get_transform(reinterpret_cast<app::Component*>(obj), nullptr);
			if (!tf) continue;

			// 检查游戏对象是否活跃
			if (!app::GameObject_get_activeInHierarchy(go, nullptr)) continue;

			// 获取 SurvivalInteractable 组件
			auto* survivalInteractable = app::GameObject_GetComponentByName(
				go,
				convert_to_system_string("SurvivalInteractable"),
				nullptr
			);

			if (survivalInteractable) {
				interactables.push_back(reinterpret_cast<app::SurvivalInteractable*>(survivalInteractable));
			}
		}

	}
	catch (...) {
		// 静默处理所有异常
	}

	return interactables;
}

void Base::Gameplay::TPItems()
{
	try {
		auto* localPlayer = LocalPlayer::GetNolan();
		if (!localPlayer) return;

		auto* playerTransform = app::Component_get_transform(reinterpret_cast<app::Component*>(localPlayer), nullptr);
		if (!playerTransform) return;

		auto playerPosition = app::Transform_get_position(playerTransform, nullptr);
		auto playerForward = app::Transform_get_forward(playerTransform, nullptr);

		auto items = Base::Gameplay::GetSurvivalInteractables();
		if (items.empty()) return;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(1.0f, 3.0f);

		for (auto* item : items) {
			if (!item) continue;

			auto* itemTransform = app::Component_get_transform(reinterpret_cast<app::Component*>(item), nullptr);
			if (!itemTransform) continue;

			float randomOffset = dist(gen);
			app::Vector3 targetPosition = {
				playerPosition.x + playerForward.x * randomOffset,
				playerPosition.y + playerForward.y * randomOffset + 0.5f,
				playerPosition.z + playerForward.z * randomOffset
			};

			app::Transform_set_position(itemTransform, targetPosition, nullptr);
		}
	}
	catch (...) {
		// 静默处理所有异常
	}
}

// 专门传送钥匙的函数
void Base::Gameplay::TPKeys()
{
	try {
		auto* localPlayer = LocalPlayer::GetNolan();
		if (!localPlayer) return;

		auto* playerTransform = app::Component_get_transform(reinterpret_cast<app::Component*>(localPlayer), nullptr);
		if (!playerTransform) return;

		auto playerPosition = app::Transform_get_position(playerTransform, nullptr);
		auto playerForward = app::Transform_get_forward(playerTransform, nullptr);

		// 搜索所有 KeyBehaviour 对象 - 使用与C#相同的方法
		Il2cppData keyData;
		keyData.SetImage(ASSEMBLY_CSHARP);
		keyData.FindClass(NS_EMPTY, "KeyBehaviour");

		UObject<app::KeyBehaviour> keyFinder;
		auto& keys = keyFinder.FindAllObjects(keyData);



		for (auto* key : keys) {
			if (!key || !SafePtr::IsValid(reinterpret_cast<app::Object_1*>(key))) continue;

			auto* keyTransform = app::Component_get_transform(reinterpret_cast<app::Component*>(key), nullptr);
			if (!keyTransform) continue;

			// 计算目标位置：玩家位置 + 玩家前方方向 * 1.5f
			app::Vector3 targetPosition = {
				playerPosition.x + playerForward.x * 1.5f,
				playerPosition.y + playerForward.y * 1.5f + 0.5f, // 稍微抬高避免卡在地里
				playerPosition.z + playerForward.z * 1.5f
			};

			app::Transform_set_position(keyTransform, targetPosition, nullptr);
		}
	}
	catch (...) {
		// 静默处理所有异常
	}
}
