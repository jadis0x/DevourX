#include "pch-il2cpp.h"

#include <helpers.h>

#include "ClientHelper.h"
#include "players/players.h"
#include "UnityEngine/Engine.hpp"
#include <map>
#include <iostream>

bool IsSinglePlayer()
{
	return app::BoltNetwork_get_IsSinglePlayer(NULL);
}

bool IsOnline()
{
	return not app::BoltNetwork_get_IsSinglePlayer(NULL);
}

bool IsHost()
{
	return app::BoltNetwork_get_IsServer(NULL);
}

app::Survival* GetSurvivalObject()
{
	static app::Survival* cachedSurvival = nullptr;

	if (cachedSurvival == nullptr || Object::IsNull((app::Object_1*)cachedSurvival)) {
		cachedSurvival = Object::FindObjectOfType<app::Survival>("Survival");
	}

	return cachedSurvival;
}

app::OptionsHelpers* GetOptionsHelpersObject()
{
	static app::OptionsHelpers* cachedOptionsHelpers = nullptr;

	if (cachedOptionsHelpers == nullptr || Object::IsNull((app::Object_1*)cachedOptionsHelpers)) {
		cachedOptionsHelpers = Object::FindObjectOfType<app::OptionsHelpers>("OptionsHelpers");
	}

	return cachedOptionsHelpers;
}

bool IsLocalPlayer(app::NolanBehaviour* player)
{
	auto boltEntity = app::EntityBehaviour_get_entity((app::EntityBehaviour*)player, NULL);

	if (not boltEntity
		|| not app::BoltEntity_get_IsAttached(boltEntity, NULL)
		|| not app::BoltEntity_get_IsOwner(boltEntity, NULL))
		return false;

	return true;
}

bool IsPlayerCrawling()
{
	if (!Players::GetLocalPlayer())
		return false;

	return app::NolanBehaviour_IsCrawling(Players::GetNolan(), nullptr);
}

bool IsPlayerCrawling(app::GameObject* go)
{
	if (go == nullptr) return false;

	app::Component* nbComponent = GameObject::GetComponentByName(go, "NolanBehaviour");

	if (nbComponent) return false;

	app::NolanBehaviour* nb = reinterpret_cast<app::NolanBehaviour*>(nbComponent);

	if (nb)
		return app::NolanBehaviour_IsCrawling(nb, nullptr);
	else
		return false;
}

bool IsInGame()
{
	app::OptionsHelpers* optionsHelpers = GetOptionsHelpersObject();

	if (optionsHelpers)
		return optionsHelpers->fields._inGame_k__BackingField;

	return false;
}

bool IsSequencePlaying()
{
	app::Survival* survival = GetSurvivalObject();

	// Return false if the object was not found.
	if (survival == nullptr) return false;

	// Check if any of the sequences are playing and return the result directly.
	if (app::Survival_IsEndingPlaying && app::Survival_IsEndingPlaying(survival, nullptr)) return true;
	if (app::Survival_IsJumpScarePlaying && app::Survival_IsJumpScarePlaying(survival, nullptr)) return true;
	if (app::Survival_StartingToPlayFailEnding && app::Survival_StartingToPlayFailEnding(survival, nullptr)) return true;

	// If none of the sequences are playing, return false.
	return false;
}

app::GameObject* GetAzazel(app::Survival* survival)
{
	app::GameObject* ai = app::Survival_GetAzazel(survival, nullptr);

	if (ai) {
		return ai;
	}

	return NULL;
}



std::string SceneName()
{
	app::Scene _scene = app::SceneManager_GetActiveScene(nullptr);

	return il2cppi_to_string(app::Scene_get_name(&_scene, nullptr)).c_str();
}

std::string GetAzazelName()
{
	if (IsInGame()) {
		app::InGameHelpers* inGameHelpers = app::InGameHelpers_get_singleton(nullptr);

		if (inGameHelpers)
			return il2cppi_to_string(app::InGameHelpers_GetAzazelName(inGameHelpers, nullptr));
	}

	return std::string("Azazel");
}


float Time_DeltaTime()
{
	if (app::Time_1_get_deltaTime) {
		return app::Time_1_get_deltaTime(nullptr);
	}

	return 0.0f;
}

std::vector<app::Scene> GetAllScenes()
{
	std::vector<app::Scene> scenes;
	auto sceneCount = app::SceneManager_get_sceneCount(NULL);

	if (sceneCount == 0)
		return scenes;

	for (int i = 0; i < sceneCount; i++)
	{
		scenes.emplace_back(app::SceneManager_GetSceneAt(i, NULL));
	}

	return scenes;
}

app::GameObject* GetPrefabIdByName(const std::string& name)
{
	if (!il2cppi_is_initialized(app::BoltPrefabs__TypeInfo))
		return nullptr;

	auto prefabs = (*app::BoltPrefabs__TypeInfo)->static_fields;

	std::map<std::string, app::PrefabId> spawnableObjects = {
		{"Animal_Gate", prefabs->Animal_Gate},
		{"AsylumDoor", prefabs->AsylumDoor},
		{"AsylumDoubleDoor", prefabs->AsylumDoubleDoor},
		{"AsylumWhiteDoor", prefabs->AsylumWhiteDoor},
		{"DevourDoorBack", prefabs->DevourDoorBack},
		{"DevourDoorMain", prefabs->DevourDoorMain},
		{"DevourDoorRoom", prefabs->DevourDoorRoom},
		{"DoorNumber", prefabs->DoorNumber},
		{"Elevator_Door", prefabs->Elevator_Door},
		{"Freezer_Room", prefabs->Freezer_Room},
		{"InnDoor", prefabs->InnDoor},
		{"InnDoubleDoor", prefabs->InnDoubleDoor},
		{"InnFountain", prefabs->InnFountain},
		{"InnShojiDoor", prefabs->InnShojiDoor},
		{"InnShojiDoubleDoor", prefabs->InnShojiDoubleDoor},
		{"InnShrine", prefabs->InnShrine},
		{"InnWardrobe", prefabs->InnWardrobe},
		{"InnWeb", prefabs->InnWeb},
		{"InnWoodenDoor", prefabs->InnWoodenDoor},
		{"InnWoodenDoubleDoor", prefabs->InnWoodenDoubleDoor},
		{"ManorBarnDoor", prefabs->ManorBarnDoor},
		{"ManorDoor", prefabs->ManorDoor},
		{"ManorFountain", prefabs->ManorFountain},
		{"ManorGate", prefabs->ManorGate},
		{"ManorLump", prefabs->ManorLump},
		{"ManorMausoleumDoor", prefabs->ManorMausoleumDoor},
		{"ManorMirror", prefabs->ManorMirror},
		{"ManorShrine", prefabs->ManorShrine},
		{"SlaughterhouseFireEscapeDoor", prefabs->SlaughterhouseFireEscapeDoor},
		{"SlaughterhousePushPullDoor", prefabs->SlaughterhousePushPullDoor},
		{"SurvivalAltar", prefabs->SurvivalAltar},
		{"SurvivalAltarMolly", prefabs->SurvivalAltarMolly},
		{"SurvivalAltarSlaughterhouse", prefabs->SurvivalAltarSlaughterhouse},
		{"SurvivalAltarTown", prefabs->SurvivalAltarTown},
		{"SurvivalCake", prefabs->SurvivalCake},
		{"SurvivalBurstPipe", prefabs->SurvivalBurstPipe},
		{"SurvivalBattery", prefabs->SurvivalBattery},
		{"SurvivalBleach", prefabs->SurvivalBleach},
		{"SurvivalCleanHead", prefabs->SurvivalCleanHead},
		{"SurvivalEgg", prefabs->SurvivalEgg},
		{"SurvivalFirstAid", prefabs->SurvivalFirstAid},
		{"SurvivalFuse", prefabs->SurvivalFuse},
		{"SurvivalGasoline", prefabs->SurvivalGasoline},
		{"SurvivalHay", prefabs->SurvivalHay},
		{"SurvivalInmate", prefabs->SurvivalInmate},
		{"SurvivalMatchbox", prefabs->SurvivalMatchbox},
		{"SurvivalMatchbox_02", prefabs->SurvivalMatchbox_02},
		{"SurvivalSpade", prefabs->SurvivalSpade},
		{"TownDoor", prefabs->TownDoor},
		{"TownLockbox", prefabs->TownLockbox},
		{"TownPentagram", prefabs->TownPentagram},
		{"TrashCan", prefabs->TrashCan},
		{"Truck_Shutter", prefabs->Truck_Shutter},
		{"TV", prefabs->TV},
		{"AzazelApril", prefabs->AzazelApril},
		{"AzazelNathan", prefabs->AzazelNathan},
		{"AzazelSam", prefabs->AzazelSam},
		{"AzazelZara", prefabs->AzazelZara},
		{"Spider", prefabs->Spider},
		{"Corpse", prefabs->Corpse},
		{"Boar", prefabs->Boar},
		{"Crow", prefabs->Crow},
		{"Ghost", prefabs->Ghost},
		{"SurvivalDemon", prefabs->SurvivalDemon},
		{"SurvivalGoat", prefabs->SurvivalGoat},
		{"SurvivalRat", prefabs->SurvivalRat},
		{"SurvivalPig", prefabs->SurvivalPig}
	};


	auto it = spawnableObjects.find(name);

	if (it != spawnableObjects.end()) {
		auto result = app::PrefabDatabase_Find(it->second, nullptr);

		return (result != nullptr) ? result : nullptr;
	}

	return nullptr;
}
