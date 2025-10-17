#include "pch-il2cpp.h"

#include "pipeline/gui/tabs/ItemTab.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>
#include <iostream>
#include <array>
#include <iterator>
#include <pipeline/localization/LocalizationManager.h>
#include <pipeline/gui/widgets.h>

namespace {
	struct ItemDefinition
	{
		const char* id;
		const char* labelKey;
	};

	constexpr ItemDefinition kItems[] = {
			{ "Hay", "tabs.items.items_section.hay" },
			{ "First aid", "tabs.items.items_section.first_aid" },
			{ "Spade", "tabs.items.items_section.spade" },
			{ "Cake", "tabs.items.items_section.cake" },
			{ "Battery", "tabs.items.items_section.battery" },
			{ "Gasoline", "tabs.items.items_section.gasoline" },
			{ "Fuse", "tabs.items.items_section.fuse" },
			{ "Food", "tabs.items.items_section.food" },
			{ "Bleach", "tabs.items.items_section.bleach" },
			{ "Bone", "tabs.items.items_section.bone" },
			{ "Salt", "tabs.items.items_section.salt" },
			{ "Ritual Book (in-active)", "tabs.items.items_section.ritual_book_inactive" },
			{ "Ritual Book (active)", "tabs.items.items_section.ritual_book_active" },
			{ "Matchbox", "tabs.items.items_section.matchbox" },
			{ "Egg-1", "tabs.items.items_section.egg_1" },
			{ "Egg-2", "tabs.items.items_section.egg_2" },
			{ "Egg-3", "tabs.items.items_section.egg_3" },
			{ "Egg-4", "tabs.items.items_section.egg_4" },
			{ "Egg-5", "tabs.items.items_section.egg_5" },
			{ "Egg-6", "tabs.items.items_section.egg_6" },
			{ "Egg-7", "tabs.items.items_section.egg_7" },
			{ "Egg-8", "tabs.items.items_section.egg_8" },
			{ "Egg-9", "tabs.items.items_section.egg_9" },
			{ "Egg-10", "tabs.items.items_section.egg_10" },
			{ "Head (Dirty/Gardener)", "tabs.items.items_section.head_dirty_gardener" },
			{ "Head (Clean/Gardener)", "tabs.items.items_section.head_clean_gardener" },
			{ "Head (Dirty/Butler)", "tabs.items.items_section.head_dirty_butler" },
			{ "Head (Clean/Butler)", "tabs.items.items_section.head_clean_butler" },
			{ "Head (Dirty/Bridesmaid_01)", "tabs.items.items_section.head_dirty_bridesmaid_01" },
			{ "Head (Clean/Bridesmaid_01)", "tabs.items.items_section.head_clean_bridesmaid_01" },
			{ "Head (Dirty/Bridesmaid_02)", "tabs.items.items_section.head_dirty_bridesmaid_02" },
			{ "Head (Clean/Bridesmaid_02)", "tabs.items.items_section.head_clean_bridesmaid_02" },
			{ "Head (Dirty/Cook)", "tabs.items.items_section.head_dirty_cook" },
			{ "Head (Clean/Cook)", "tabs.items.items_section.head_clean_cook" },
			{ "Head (Dirty/Groomsman_01)", "tabs.items.items_section.head_dirty_groomsman_01" },
			{ "Head (Clean/Groomsman_01)", "tabs.items.items_section.head_clean_groomsman_01" },
			{ "Head (Dirty/Groomsman_02)", "tabs.items.items_section.head_dirty_groomsman_02" },
			{ "Head (Clean/Groomsman_02)", "tabs.items.items_section.head_clean_groomsman_02" },
			{ "Head (Dirty/Maid)", "tabs.items.items_section.head_dirty_maid" },
			{ "Head (Clean/Maid)", "tabs.items.items_section.head_clean_maid" },
			{ "Head (Dirty/Photographer)", "tabs.items.items_section.head_dirty_photographer" },
			{ "Head (Clean/Photographer)", "tabs.items.items_section.head_clean_photographer" },
			{ "Head (Dirty/Priest)", "tabs.items.items_section.head_dirty_priest" },
			{ "Head (Clean/Priest)", "tabs.items.items_section.head_clean_priest" },
			{ "Coin", "tabs.items.items_section.coin" },
			{ "Music Box (Idle)", "tabs.items.items_section.music_box_idle" },
			{ "Music Box (Armed)", "tabs.items.items_section.music_box_armed" }
	};

	constexpr ItemDefinition kAnimals[] = {
			{ "Rat", "tabs.items.animals_section.rat" },
			{ "Goat", "tabs.items.animals_section.goat" },
			{ "Pig", "tabs.items.animals_section.pig" }
	};
}

void ItemTAB::Render()
{
	if (ImGui::BeginTabItem(Localization::Get("tabs.items.title").c_str())) {
		ImGui::Spacing();

		GuiWidgets::SectionTitle("tabs.items.items_section.title", ImVec4(0.7f, 0.6f, 0.95f, 1.0f));

		static int item_current = 0;
		const std::string currentItemLabel = Localization::Get(kItems[item_current].labelKey);
		if (ImGui::BeginCombo("##items_combo", currentItemLabel.c_str()))
		{
			for (int i = 0; i < static_cast<int>(std::size(kItems)); ++i)
			{
				const bool isSelected = (item_current == i);
				const std::string optionLabel = Localization::Get(kItems[i].labelKey);
				if (ImGui::Selectable(optionLabel.c_str(), isSelected))
				{
					item_current = i;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		std::string spawnItemLabel = Localization::Get("tabs.items.spawn");
		spawnItemLabel += "##i";
		if (ImGui::Button(spawnItemLabel.c_str())) {
			Base::Gameplay::StartCarryItem(kItems[item_current].id);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		GuiWidgets::SectionTitle("tabs.items.animals_section.title", ImVec4(0.6f, 0.8f, 0.7f, 1.0f));
		static int animal_current = 0;
		const std::string currentAnimalLabel = Localization::Get(kAnimals[animal_current].labelKey);
		if (ImGui::BeginCombo("##animals_combo", currentAnimalLabel.c_str()))
		{
			for (int i = 0; i < static_cast<int>(std::size(kAnimals)); ++i)
			{
				const bool isSelected = (animal_current == i);
				const std::string optionLabel = Localization::Get(kAnimals[i].labelKey);
				if (ImGui::Selectable(optionLabel.c_str(), isSelected))
				{
					animal_current = i;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		std::string spawnAnimalLabel = Localization::Get("tabs.items.spawn");
		spawnAnimalLabel += "##an";
		if (ImGui::Button(spawnAnimalLabel.c_str())) {
			Base::Gameplay::StartCarryAnimal(kAnimals[animal_current].id);
		}

		ImGui::EndTabItem();
	}
}