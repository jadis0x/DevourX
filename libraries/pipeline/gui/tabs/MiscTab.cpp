#include "pch-il2cpp.h"

#include "pipeline/gui/tabs/MiscTab.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>

void MiscTAB::Render()
{
	if (ImGui::BeginTabItem("Misc")) {

		ImGui::Spacing();


		ImGui::Checkbox("Modify EXP Gain", &settings.exp_modifier);
		if (settings.newExp) {
			ImGui::SliderFloat("XP", &settings.newExp, 1000.f, 3000.0f, "%.1f");
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Checkbox("Modify UV Color", &settings.bModifyFlashlightColor); // Test checkbox
		if (settings.bModifyFlashlightColor)
		{
			ImGui::ColorEdit4("##FlashlightColor", settings.flashlightColor);
			ImGui::Spacing();
			ImGui::Spacing();
		}

		ImGui::Checkbox("Unlock Doors", &settings.bUnlockDoors);
		ImGui::Checkbox("Disable Long Interact", &settings.bDisableLongInteract);

		ImGui::Checkbox("Auto-Calm When Nearby", &settings.bAutoCalmWhenNearby);
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Auto calms Azazel when a player is nearby,\nwithout needing UV light.");
		}

		ImGui::Checkbox("Never Calm", &settings.bNeverCalm);
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Prevents Azazel from calming down under any condition.\nPermanent rage mode.");
		}


		ImGui::Checkbox("Modify Azazel Speed", &settings.bFreezeAzazel);
		if (settings.bFreezeAzazel) {
			ImGui::SliderFloat("New Speed", &settings.azazelSpeed, 0.f, 15.0f, "%.1f");
		}


		ImGui::EndTabItem();
	}
}
