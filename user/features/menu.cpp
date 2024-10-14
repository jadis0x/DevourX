#include "pch-il2cpp.h"
#include "Menu.hpp"
#include "main.h"
#include "settings/settings.hpp"
#include "misc/misc.h"
#include "players/players.h"
#include <iostream>
#include <string>
#include <vector>
#include "UnityEngine/Object.h"
#include <map>

void InitStyle()
{
	ImGui::StyleColorsDark();

	auto& Style = ImGui::GetStyle();
	auto& Colors = Style.Colors;

	// Yuvarlatma ve aralık ayarları
	Style.WindowRounding = 0.0f;
	Style.FrameRounding = 0.0f;
	Style.GrabRounding = 0.0f;
	Style.ScrollbarRounding = 0.0f;
	Style.TabRounding = 0.0f;

	Style.FramePadding = ImVec2(8.0f, 6.0f);
	Style.ItemSpacing = ImVec2(8.0f, 6.0f);

	// Siyah ve transparan renk paleti
	Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);             // Beyaz metin
	Colors[ImGuiCol_TextDisabled] = ImColor(128, 128, 128, 255);     // Gri metin
	Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 200);               // Siyah arka plan, %78 opaklık
	Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 0);                  // Tamamen transparan
	Colors[ImGuiCol_PopupBg] = ImColor(0, 0, 0, 220);
	Colors[ImGuiCol_Border] = ImColor(255, 255, 255, 30);
	Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
	Colors[ImGuiCol_FrameBg] = ImColor(64, 64, 64, 255);            // Koyu gri
	Colors[ImGuiCol_FrameBgHovered] = ImColor(80, 80, 80, 255);     // Biraz daha açık gri
	Colors[ImGuiCol_FrameBgActive] = ImColor(96, 96, 96, 255);
	Colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0, 180);
	Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 220);
	Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 160);
	Colors[ImGuiCol_MenuBarBg] = ImColor(0, 0, 0, 170);
	Colors[ImGuiCol_ScrollbarBg] = ImColor(0, 0, 0, 150);
	Colors[ImGuiCol_ScrollbarGrab] = ImColor(255, 255, 255, 50);
	Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(255, 255, 255, 75);
	Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(255, 255, 255, 100);
	Colors[ImGuiCol_CheckMark] = ImColor(128, 128, 128, 255);  // Orta ton gri
	Colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 150);
	Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 255, 255, 200);
	Colors[ImGuiCol_Button] = ImColor(128, 0, 128, 255);          // Mor
	Colors[ImGuiCol_ButtonHovered] = ImColor(147, 112, 219, 255); // Orta mor ton
	Colors[ImGuiCol_ButtonActive] = ImColor(186, 85, 211, 255);   // Açık mor
	Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 180);
	Colors[ImGuiCol_HeaderHovered] = ImColor(255, 255, 255, 20);
	Colors[ImGuiCol_HeaderActive] = ImColor(255, 255, 255, 40);
	Colors[ImGuiCol_Separator] = ImColor(255, 255, 255, 30);
	Colors[ImGuiCol_SeparatorHovered] = ImColor(255, 255, 255, 50);
	Colors[ImGuiCol_SeparatorActive] = ImColor(255, 255, 255, 70);
	Colors[ImGuiCol_ResizeGrip] = ImColor(255, 255, 255, 20);
	Colors[ImGuiCol_ResizeGripHovered] = ImColor(255, 255, 255, 40);
	Colors[ImGuiCol_ResizeGripActive] = ImColor(255, 255, 255, 60);
	Colors[ImGuiCol_Tab] = ImColor(0, 0, 0, 150);
	Colors[ImGuiCol_TabHovered] = ImColor(255, 255, 255, 20);
	Colors[ImGuiCol_TabActive] = ImColor(255, 255, 255, 40);
	Colors[ImGuiCol_TabUnfocused] = ImColor(0, 0, 0, 100);
	Colors[ImGuiCol_TabUnfocusedActive] = ImColor(0, 0, 0, 120);
	Colors[ImGuiCol_PlotLines] = ImColor(255, 255, 255, 150);
	Colors[ImGuiCol_PlotLinesHovered] = ImColor(255, 255, 255, 200);
	Colors[ImGuiCol_PlotHistogram] = ImColor(255, 255, 255, 150);
	Colors[ImGuiCol_PlotHistogramHovered] = ImColor(255, 255, 255, 200);
	Colors[ImGuiCol_TextSelectedBg] = ImColor(255, 255, 255, 50);
	Colors[ImGuiCol_DragDropTarget] = ImColor(255, 255, 0, 200);
	Colors[ImGuiCol_NavHighlight] = ImColor(255, 255, 255, 100);
	Colors[ImGuiCol_NavWindowingHighlight] = ImColor(255, 255, 255, 100);
	Colors[ImGuiCol_NavWindowingDimBg] = ImColor(0, 0, 0, 50);
	Colors[ImGuiCol_ModalWindowDimBg] = ImColor(0, 0, 0, 75);


	// Font ayarı (İsteğinize göre fontu ayarlayabilirsiniz)
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16.0f);
}

void DrawVisualsTab() {
	ImGui::Checkbox("Unlimited UV", &settings::unlimited_uv);
	ImGui::Checkbox("Fullbright", &settings::fullBright);
}

void DrawEntitiesTab() {
	ImGui::Text("Items");

	const char* spawnableObjects[] = {
	"Animal_Gate",
	"AsylumDoor",
	"AsylumDoubleDoor",
	"AsylumWhiteDoor",
	"DevourDoorBack",
	"DevourDoorMain",
	"DevourDoorRoom",
	"DoorNumber",
	"Elevator_Door",
	"Freezer_Room",
	"InnDoor",
	"InnDoubleDoor",
	"InnFountain",
	"InnShojiDoor",
	"InnShojiDoubleDoor",
	"InnShrine",
	"InnWardrobe",
	"InnWeb",
	"InnWoodenDoor",
	"InnWoodenDoubleDoor",
	"ManorBarnDoor",
	"ManorDoor",
	"ManorFountain",
	"ManorGate",
	"ManorLump",
	"ManorMausoleumDoor",
	"ManorMirror",
	"ManorShrine",
	"SlaughterhouseFireEscapeDoor",
	"SlaughterhousePushPullDoor",
	"SurvivalAltar",
	"SurvivalAltarMolly",
	"SurvivalAltarSlaughterhouse",
	"SurvivalAltarTown",
	"SurvivalCake",
	"SurvivalBurstPipe",
	"SurvivalBattery",
	"SurvivalBleach",
	"SurvivalCleanHead",
	"SurvivalEgg",
	"SurvivalFirstAid",
	"SurvivalFuse",
	"SurvivalGasoline",
	"SurvivalHay",
	"SurvivalInmate",
	"SurvivalMatchbox",
	"SurvivalMatchbox_02",
	"SurvivalSpade",
	"TownDoor",
	"TownLockbox",
	"TownPentagram",
	"TrashCan",
	"Truck_Shutter",
	"TV",
	"AzazelApril",
	"AzazelNathan",
	"AzazelSam",
	"AzazelZara",
	"Spider",
	"Corpse",
	"Boar",
	"Crow",
	"Ghost",
	"SurvivalDemon",
	"SurvivalGoat",
	"SurvivalRat",
	"SurvivalPig"
	};

	const char* items_items[] = {
			"Hay",
			"First aid",
			"Spade",
			"Cake",
			"Battery",
			"Gasoline",
			"Fuse",
			"Food",
			"Bleach",
			"Bone",
			"Salt",
			"Ritual Book (in-active)",
			"Ritual Book (active)",
			"Matchbox",
			"Egg-1",
			"Egg-2",
			"Egg-3",
			"Egg-4",
			"Egg-5",
			"Egg-6",
			"Egg-7",
			"Egg-8",
			"Egg-9",
			"Egg-10",
			"Head (Dirty/Gardener)",
			"Head (Clean/Gardener)",
			"Head (Dirty/Butler)",
			"Head (Clean/Butler)",
			"Head (Dirty/Bridesmaid_01)",
			"Head (Clean/Bridesmaid_01)",
			"Head (Dirty/Bridesmaid_02)",
			"Head (Clean/Bridesmaid_02)",
			"Head (Dirty/Cook)",
			"Head (Clean/Cook)",
			"Head (Dirty/Groomsman_01)",
			"Head (Clean/Groomsman_01)",
			"Head (Dirty/Groomsman_02)",
			"Head (Clean/Groomsman_02)",
			"Head (Dirty/Maid)",
			"Head (Clean/Maid)",
			"Head (Dirty/Photographer)",
			"Head (Clean/Photographer)",
			"Head (Dirty/Priest)",
			"Head (Clean/Priest)",
	};

	static int item_current = 0;
	ImGui::Combo("##i", &item_current, items_items, IM_ARRAYSIZE(items_items));
	if (ImGui::Button("Spawn##i")) {
		Misc::CarryItem(items_items[item_current]);
	}

	ImGui::Spacing();

	ImGui::Text("Spawnable Objects");

	static int spawnable_current = 0;
	ImGui::Combo("##sp", &spawnable_current, spawnableObjects, IM_ARRAYSIZE(spawnableObjects));
	if (ImGui::Button("Spawn##sp")) {
		Misc::SafeInstantiate(spawnableObjects[spawnable_current]);
	}

	ImGui::Spacing();

	ImGui::Text("Animals");
	const char* animals_items[] = { "Rat", "Goat", "Pig" };
	static int animal_current = 0;
	ImGui::Combo("##an", &animal_current, animals_items, IM_ARRAYSIZE(animals_items));
	if (ImGui::Button("Spawn##an")) {
		Misc::CarryAnimal(animals_items[animal_current]);
	}

	ImGui::Spacing();
}

void DrawMiscTab() {
	ImGui::Checkbox("Change Steam name", &settings::steam_name_spoof);
	ImGui::InputText("New name##steam", &settings::new_name);

	if (ImGui::Button("TP To Azazel")) {
		Misc::TpToAzazel();
	}
}

void DrawGameplayTab() {

	if (ImGui::Button("Force Lobby Start")) {
		Misc::ForceStart();
	}

	if (ImGui::Button("Instant Win")) {
		Misc::InstantWin();
	}

	ImGui::Checkbox("Azazel Speed", &settings::freeze_azazel);
	ImGui::SliderFloat("Multiplier", &settings::new_azazel_speed, 0, 15);
}

void DrawPlayersTab() {
	static int selectedPlayerIndex = 0;
	std::vector<std::string> playerNames;
	std::vector<app::GameObject*> players = Players::GetAllPlayers();

	for (const auto& player : players) {
		if (player != nullptr) {
			std::string playerNameStr = Object::GetObjectName(reinterpret_cast<app::Object_1*>(player));

			playerNames.push_back(playerNameStr);
		}
		else {
			playerNames.push_back("Unknown Player");
		}
	}

	if (selectedPlayerIndex >= playerNames.size()) {
		selectedPlayerIndex = 0;
	}

	// Combo box
	if (ImGui::BeginCombo("Select Player", selectedPlayerIndex < playerNames.size() ? playerNames[selectedPlayerIndex].c_str() : "No Player")) {
		for (int n = 0; n < playerNames.size(); ++n) {
			const bool isSelected = (selectedPlayerIndex == n);
			if (ImGui::Selectable(playerNames[n].c_str(), isSelected)) {
				selectedPlayerIndex = n;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// Seçili oyuncuya ait işlemler
	if (selectedPlayerIndex >= 0 && selectedPlayerIndex < players.size() && players[selectedPlayerIndex] != nullptr) {
		if (ImGui::Button("Kill")) {
			Misc::Kill(players[selectedPlayerIndex]);
		}

		if (ImGui::Button("Revive")) {
			Misc::Revive(players[selectedPlayerIndex]);
		}

		if (ImGui::Button("Jumpscare")) {
			Misc::Jumpscare(players[selectedPlayerIndex]);
		}

		if (ImGui::Button("Teleport")) {
			Misc::Teleport(players[selectedPlayerIndex]);
		}
	}

	ImGui::Separator();
	ImGui::Text("Local player settings:");

	ImGui::Checkbox("Spoof level", &settings::spoof_level);
	ImGui::InputInt("New level", &settings::new_level);

	ImGui::Checkbox("Fly", &settings::fly);
	ImGui::SliderFloat("Speed: ", &settings::fly_speed, 5.f, 20.f);

	ImGui::Checkbox("Change player speed", &settings::change_player_speed);
	ImGui::SliderInt("Multiplier", &settings::new_speed, 0, 10);
}

void DrawHookTab() {

	ImGui::Checkbox("[HOOK] EXP Modifier", &settings::exp_modifier);
	ImGui::SliderInt("Amount", &settings::new_exp, 0, 5000);

	ImGui::Checkbox("[HOOK] Always can interact", &settings::canInteractHook);
	ImGui::Checkbox("[HOOK] Has retrieved user inventory items", &settings::retrievedItems);
	ImGui::Checkbox("[HOOK] Unlock robes", &settings::robe_unlock);
	ImGui::Checkbox("[HOOK] Unlock emotes", &settings::unlockEmote);
	ImGui::Checkbox("[HOOK] Unlock perks", &settings::unlockPerk);
	ImGui::Checkbox("[HOOK] Unlock flashlights", &settings::unlockFlashlight);
	ImGui::Checkbox("[HOOK] Unlocked character selected", &settings::unlockCharacter);
	ImGui::Checkbox("[HOOK] All players ready", &settings::allPlayersReady);
	ImGui::Checkbox("[HOOK] Disable long interact", &settings::disable_longInteract);
	ImGui::Checkbox("[HOOK] Disable lock interacts", &settings::lockedInteractable);
	ImGui::Checkbox("[HOOK] Lobby Mouse", &settings::mouseHook);

	ImGui::SliderInt("[HOOK] Server player limit", &settings::player_count, 4, 20);
}

tabs current_tab = tabs::VISUALS;
void DrawMenu(bool open_menu) {
	ImGui::SetNextWindowSize(ImVec2(800.0f, 600.0f), ImGuiCond_Once);
	ImGui::Begin("DevourX", NULL, ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginTabBar("MainTabBar")) {

		if (ImGui::BeginTabItem("Visuals")) {
			DrawVisualsTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Entities")) {
			DrawEntitiesTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Map Specific")) {
			DrawMiscTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Gameplay Settings")) {
			DrawGameplayTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Players")) {
			DrawPlayersTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Hooks")) {
			DrawHookTab();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}
