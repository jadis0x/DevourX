#include "pch-il2cpp.h"

#include "Misc.h"
#include "Wrapper.h"
#include "ClientHelper.h"
#include "players/players.h"
#include "helpers.h"
#include "UnityEngine/Engine.hpp"
#include <iostream>
#include "Windows.h"
#include <map>


void Misc::ForceStart()
{
	std::string _scene = SceneName();

	if (IsHost() && _scene == std::string("Menu")) {
		app::Menu* _menu = Object::FindObjectOfType<app::Menu>("Menu", "Horror");

		if (_menu) {
			if (app::Menu_OnLobbyStartButtonClick) {
				app::Menu_OnLobbyStartButtonClick(_menu, nullptr);
			}
		}
	}
}

void Misc::CarryItem(const char* itemName)
{
	std::string _scene = SceneName();

	if (_scene == std::string("Menu")) {
		return;
	}

	std::map<std::string, std::string> itemMap = {
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
		{"Egg 1", "Egg-Clean-1"},
		{"Egg 2", "Egg-Clean-2"},
		{"Egg 3", "Egg-Clean-3"},
		{"Egg 4", "Egg-Clean-4"},
		{"Egg 5", "Egg-Clean-5"},
		{"Egg 6", "Egg-Clean-6"},
		{"Egg 7", "Egg-Clean-7"},
		{"Egg 8", "Egg-Clean-8"},
		{"Egg 9", "Egg-Clean-9"},
		{"Egg 10", "Egg-Clean-10"},
		{"Head (Dirty/Gardener)", "Head-Dirty-Gardener"},
		{"Head (Clean/Gardener)", "Head-Clean-Gardener"},
		{"Head (Dirty/Butler)", "Head-Dirty-Butler"},
		{"Head (Clean/Butler)", "Head-Clean-Butler)"},
		{"Head (Dirty/Bridesmaid_01)", "Head-Dirty-Bridesmaid_01"},
		{"Head (Clean/Bridesmaid_01)", "Head-Clean-Bridesmaid_01"},
		{"Head (Dirty/Bridesmaid_02)", "Head-Dirty-Bridesmaid_02"},
		{"Head (Clean/Bridesmaid_02)", "Head-Clean-Bridesmaid_02)"},
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
		{"Head (Clean/Priest)", "Head-Clean-Priest"}
	};

	std::string carryItem = itemMap[itemName];

	if (!carryItem.empty() && Players::GetLocalPlayer()) {
		app::NolanBehaviour_StartCarry(Players::GetNolan(), ConvertToSystemString(carryItem.c_str()), nullptr);
	}
}

void Misc::CarryAnimal(const char* animalName)
{
	std::string _scene = SceneName();

	if (_scene == std::string("Menu")) {
		return;
	}

	std::map<std::string, std::string> animalMap = {
		{"Rat", "SurvivalRat"},
		{"Goat", "SurvivalGoat"},
		{"Pig", "SurvivalPig"}
	};

	std::string carryAnimal = animalMap[animalName];

	if (!carryAnimal.empty() && Players::GetLocalPlayer()) {
		app::NolanBehaviour_StartCarry(Players::GetNolan(), ConvertToSystemString(carryAnimal.c_str()), nullptr);
	}
}

void Misc::InstantWin()
{
	// app::InnMapController -> Inn
	// app::SlaughterhouseAltarController -> Slaughterhouse
	// SurvivalObjectBurnController -> other

	std::string _scene = SceneName();

	if (_scene == std::string("Menu") || !IsHost() || !Players::GetLocalPlayer()) return;

	int32_t progress = 10;

	if (_scene == std::string("Inn")) {
		app::MapController* _MapController = Object::FindObjectOfType<app::MapController>("MapController");

		if (_MapController) {

			// DO_APP_FUNC(0x00930CD0, void, MapController_SetProgressTo, (MapController * __this, int32_t progress, MethodInfo * method));
			if (app::MapController_SetProgressTo != nullptr) {
				app::MapController_SetProgressTo(_MapController, progress, nullptr);
			}
		}
	}
	else if (_scene == std::string("Slaughterhouse")) {
		app::SlaughterhouseAltarController* _SlaughterhouseAltarController = Object::FindObjectOfType<app::SlaughterhouseAltarController>("SlaughterhouseAltarController");

		if (_SlaughterhouseAltarController) {

			// DO_APP_FUNC(0x0050DEB0, void, SlaughterhouseAltarController_SkipToGoat, (SlaughterhouseAltarController * __this, int32_t number, MethodInfo * method));
			if (app::SlaughterhouseAltarController_SkipToGoat != nullptr) {
				app::SlaughterhouseAltarController_SkipToGoat(_SlaughterhouseAltarController, progress, nullptr);
			}
		}
	}
	else if (_scene == std::string("Manor")) {
		app::MapController* _MapController = Object::FindObjectOfType<app::MapController>("MapController");

		if (_MapController) {

			// DO_APP_FUNC(0x00930CD0, void, MapController_SetProgressTo, (MapController * __this, int32_t progress, MethodInfo * method));
			if (app::MapController_SetProgressTo != nullptr) {
				app::MapController_SetProgressTo(_MapController, progress, nullptr);
			}
		}
	}
	else {
		//app::SurvivalObjectBurnController* _SurvivalObjectBurnController = UnityEngine::Object::FindObjectOfType("SurvivalObjectBurnController");
		app::SurvivalObjectBurnController* _SurvivalObjectBurnController = Object::FindObjectOfType<app::SurvivalObjectBurnController>("SurvivalObjectBurnController");

		if (_SurvivalObjectBurnController) {
			// DO_APP_FUNC(0x00562590, void, SurvivalObjectBurnController_SkipToGoat, (SurvivalObjectBurnController * __this, int32_t number, MethodInfo * method));
			if (app::SurvivalObjectBurnController_SkipToGoat != nullptr) {
				app::SurvivalObjectBurnController_SkipToGoat(_SurvivalObjectBurnController, progress, nullptr);
			}
		}
	}
}


void Misc::RankSpoofer(int value) {

	app::GameObject* player = Players::GetLocalPlayer();

	if (player == nullptr) {
		return;
	}
	else {

		app::NolanRankController* rankController = reinterpret_cast<app::NolanRankController*>(GameObject::GetComponentByName(player, "NolanRankController"));

		if (rankController != nullptr) {
			if (app::NolanRankController_SetRank != nullptr) {
				app::NolanRankController_SetRank(rankController, value, nullptr);
			}
		}
	}
}

void Misc::FullBright(app::NolanBehaviour* nb)
{
	if (nb != nullptr) {
		app::Light* _flashlight = nb->fields.flashlightSpot;

		app::Light_set_intensity(_flashlight, 0.7f, nullptr);
		app::Light_set_range(_flashlight, 700.0f, nullptr);
		app::Light_set_spotAngle(_flashlight, 179.0f, nullptr);

		app::Light_set_shadows(_flashlight, app::LightShadows__Enum::None, nullptr);
	}
}


void Misc::Revive(app::GameObject* selectedPlayer)
{
	std::string _scene = SceneName();

	if (_scene == std::string("Menu") || IsPlayerCrawling() == false)
		return;

	if (selectedPlayer == nullptr) return;

	app::Transform* playerTransform = Transform::GetTransform(selectedPlayer);

	if (playerTransform == nullptr) return;

	app::Vector3 lastPosition = Transform::GetPosition(playerTransform);


	// Opsive.UltimateCharacterController.Traits.CharacterRespawner
	app::Component* component = GameObject::GetComponentByName(selectedPlayer, "SurvivalReviveInteractable");

	if (component == nullptr) return;

	app::SurvivalReviveInteractable* reviveInteract = reinterpret_cast<app::SurvivalReviveInteractable*>(component);

	if (reviveInteract == nullptr) return;

	app::SurvivalReviveInteractable_Interact(reviveInteract, selectedPlayer, nullptr);

	// HOOK CAN INTERACT!!
}

void Misc::Jumpscare(app::GameObject* selectedPlayer) {

	std::string _scene = SceneName();

	if (_scene == std::string("Menu")) return;

	if (IsHost() == false && selectedPlayer == nullptr) return;

	app::Survival* survival = Object::FindObjectOfType<app::Survival>("Survival");

	if (survival == nullptr) return;

	app::SurvivalAzazelBehaviour* azazelBehaviour = Object::FindObjectOfType<app::SurvivalAzazelBehaviour>("SurvivalAzazelBehaviour");

	app::GameObject* azazel = GetAzazel(survival);

	if (azazel == nullptr) return;

	app::Component* component = GameObject::GetComponentByName(selectedPlayer, "NolanBehaviour");

	if (component == nullptr) return;

	app::NolanBehaviour* nb = reinterpret_cast<app::NolanBehaviour*>(component);

	if (nb == nullptr) return;

	if (app::SurvivalAzazelBehaviour_OnPickedUpPlayer != nullptr) {
		app::SurvivalAzazelBehaviour_OnPickedUpPlayer(azazelBehaviour, azazel, selectedPlayer, true, nullptr);
	}
}

void Misc::Kill(app::GameObject* selectedPlayer) {
	std::string _scene = SceneName();

	if (_scene == std::string("Menu")) return;

	if (IsHost() == false && selectedPlayer == nullptr) return;

	app::Survival* survival = Object::FindObjectOfType<app::Survival>("Survival");

	if (survival == nullptr) return;

	app::SurvivalAzazelBehaviour* azazelBehaviour = Object::FindObjectOfType<app::SurvivalAzazelBehaviour>("SurvivalAzazelBehaviour");

	app::GameObject* azazel = GetAzazel(survival);

	if (azazel == nullptr) return;

	app::Component* component = GameObject::GetComponentByName(selectedPlayer, "NolanBehaviour");

	if (component == nullptr) return;

	app::NolanBehaviour* nb = reinterpret_cast<app::NolanBehaviour*>(component);

	if (nb == nullptr) return;

	if (app::SurvivalAzazelBehaviour_OnKnockout != nullptr) {
		app::SurvivalAzazelBehaviour_OnKnockout(azazelBehaviour, azazel, selectedPlayer, nullptr);
	}
}

void Misc::TpToAzazel()
{
	std::string _scene = SceneName();

	if (_scene == std::string("Menu")) return;

	app::GameObject* go = Players::GetLocalPlayer();

	if (go) {
		app::NolanBehaviour* nb = Players::GetNolan();

		if (nb == nullptr) return;


		app::Survival* _survival = Object::FindObjectOfType<app::Survival>("Survival");
		if (_survival == nullptr) return;

		// get azazel
		app::GameObject* azazelGo = GetAzazel(_survival);
		if (azazelGo == nullptr) return;

		// get transform of azazel
		app::Transform* transform = app::GameObject_get_transform(azazelGo, nullptr);
		if (transform == nullptr) return;

		app::Vector3 v3 = app::Transform_get_position(transform, nullptr);
		app::Quaternion rotation = app::Quaternion_get_identity(nullptr);

		if (app::NolanBehaviour_TeleportTo != nullptr) {
			app::NolanBehaviour_TeleportTo(nb, v3, rotation, false, nullptr);
		}
	}
}

void Misc::Teleport(app::GameObject* selectedPlayer) {
	std::string _scene = SceneName();

	if (_scene == std::string("Menu") || selectedPlayer == nullptr) return;

	app::GameObject* localPlayer = Players::GetLocalPlayer();

	if (localPlayer == nullptr) return;

	app::Transform* selectedPlayerTransform = Transform::GetTransform(selectedPlayer);

	if (selectedPlayerTransform == nullptr) return;

	app::NolanBehaviour* nb = Players::GetNolan();

	app::Vector3 pos = Transform::GetPosition(selectedPlayerTransform);

	if (app::NolanBehaviour_TeleportTo != nullptr) {
		app::NolanBehaviour_TeleportTo(nb, pos, Transform::QuaternionIdentity(), false, nullptr);
	}
}

app::BoltEntity* Misc::SafeInstantiate(const char* prefabName)
{
	app::GameObject* player = Players::GetLocalPlayer();
	if (player == nullptr) return nullptr;

	app::BoltEntity* spawnedEntity = nullptr;

	// Normal C++ kodu dışarıda çalıştır
	app::Transform* _transform = nullptr;
	app::Vector3 pos;
	app::GameObject* id = nullptr;

	if (il2cppi_is_initialized(app::BoltPrefabs__TypeInfo)) {
		_transform = Transform::GetTransform(player);

		if (_transform) {
			pos = Transform::GetPosition(_transform);
			id = GetPrefabIdByName(prefabName);
		}
	}

	if (_transform) {
		spawnedEntity = InstantiateWithSEH(id, pos);
	}

	return spawnedEntity;
}

app::BoltEntity* Misc::InstantiateWithSEH(app::GameObject* id, app::Vector3 pos)
{
	app::BoltEntity* spawnedEntity = nullptr;

	__try {
		spawnedEntity = app::BoltNetwork_Instantiate_2(id, pos, Transform::QuaternionIdentity(), nullptr);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		std::cerr << "Access violation occurred during instantiate!\n";
		return nullptr;
	}

	return spawnedEntity;
}
