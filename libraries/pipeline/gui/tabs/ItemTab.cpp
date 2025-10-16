#include "pch-il2cpp.h"

#include "pipeline/gui/tabs/ItemTab.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>
#include <iostream>

void ItemTAB::Render()
{
	if (ImGui::BeginTabItem("Items")) {

		ImGui::Spacing();

		ImGui::Text("Items");

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
				"Coin",
				"Music Box (Idle)",
				"Music Box (Armed)"
		};

		static int item_current = 0;
		ImGui::Combo("##i", &item_current, items_items, IM_ARRAYSIZE(items_items));
		if (ImGui::Button("Spawn##i")) {
			Base::Gameplay::StartCarryItem(items_items[item_current]);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Animals");
		const char* animals_items[] = { "Rat", "Goat", "Pig" };
		static int animal_current = 0;
		ImGui::Combo("##an", &animal_current, animals_items, IM_ARRAYSIZE(animals_items));
		if (ImGui::Button("Spawn##an")) {
			Base::Gameplay::StartCarryAnimal(animals_items[animal_current]);
		}

		ImGui::EndTabItem();
	}
}