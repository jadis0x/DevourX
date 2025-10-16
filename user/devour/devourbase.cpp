
#include "pch-il2cpp.h"

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
		{"Ritual Book (inactive)", "RitualBook-InActive-1"},
		{"Ritual Book (active)", "RitualBook-Active-1"},
		{"Matchbox", "Matchbox-3"},
		{"Egg 1", "Egg-Clean-1"}, {"Egg 2", "Egg-Clean-2"}, {"Egg 3", "Egg-Clean-3"},
		{"Egg 4", "Egg-Clean-4"}, {"Egg 5", "Egg-Clean-5"}, {"Egg 6", "Egg-Clean-6"},
		{"Egg 7", "Egg-Clean-7"}, {"Egg 8", "Egg-Clean-8"}, {"Egg 9", "Egg-Clean-9"},
		{"Egg 10", "Egg-Clean-10"},
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
		{"Coin", "SurvivalCoin"},
		{"Music Box (Idle)", "MusicBox-Idle"},
		{"Music Box (Armed)", "MusicBox-Armed"},
	};

	const auto it = itemMap.find(itemName);
	if (it == itemMap.end())
	{
		return;
	}

	if (app::GameObject* local_player = LocalPlayer::GetLocalPlayer(); !SafePtr::IsValid(local_player))
	{
		return;
	}

	app::NolanBehaviour* nolan = LocalPlayer::GetNolan();
	if (!nolan)
	{
		return;
	}

	const std::string internal_name = it->second;
	std::cout << internal_name << "\n";

	app::String* item_str = convert_to_system_string(internal_name.c_str());

	if (!item_str)
	{
		return;
	}

	// FIX!!
	SafePtr::safe_call([&]()
	{
		if (app::NolanBehaviour_StartCarry != nullptr)
		{
			app::NolanBehaviour_StartCarry(nolan, item_str, nullptr);
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
		{"Pig", "SurvivalPig"}
	};

	const std::string carry_animal = animal_map[animal_name];

	if (!carry_animal.empty() && LocalPlayer::GetLocalPlayer())
	{
		app::NolanBehaviour_StartCarry(LocalPlayer::GetNolan(), convert_to_system_string(carry_animal.c_str()), nullptr);
	}
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
