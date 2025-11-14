#include "pch-il2cpp.h"

#include "pipeline/gui/tabs/MiscTab.h"
#include "pipeline/gui/widgets.h"
#include "settings.h"
#include <imgui/imgui.h>
#include <pipeline/localization/LocalizationManager.h>

void MiscTAB::Render()
{
	if (!ImGui::BeginTabItem(Localization::Get("tabs.misc.title").c_str()))
	{
		return;
	}

	ImGui::Spacing();

	if (ImGui::BeginTable("##misc_layout", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody))
	{
		ImGui::TableNextColumn();
		GuiWidgets::SectionTitle("tabs.misc.progression.title", ImVec4(1.0f, 1.0f, 1.0f, 0.9f));
		ImGui::Checkbox(Localization::Get("tabs.misc.progression.modify_exp").c_str(), &settings.exp_modifier);
		if (settings.exp_modifier)
		{
			ImGui::SliderFloat(Localization::Get("tabs.misc.progression.xp_slider").c_str(), &settings.newExp, 1000.f, 3000.0f, "%.0f");
		}

		ImGui::Spacing();
		ImGui::Checkbox(Localization::Get("tabs.misc.progression.modify_uv").c_str(), &settings.bModifyFlashlightColor);
		if (settings.bModifyFlashlightColor)
		{
			ImGui::ColorEdit4("##FlashlightColor", settings.flashlightColor);
		}

		ImGui::TableNextColumn();
		GuiWidgets::SectionTitle("tabs.misc.world.title", ImVec4(1.0f, 1.0f, 1.0f, 0.9f));
		ImGui::Checkbox(Localization::Get("tabs.misc.world.unlock_doors").c_str(), &settings.bUnlockDoors);
		ImGui::Checkbox(Localization::Get("tabs.misc.world.unlock_all").c_str(), &settings.bUnlockAll);
		ImGui::Checkbox(Localization::Get("tabs.misc.world.disable_long_interact").c_str(), &settings.bDisableLongInteract);
		ImGui::Spacing();
		ImGui::Checkbox(Localization::Get("tabs.misc.world.auto_calm").c_str(), &settings.bAutoCalmWhenNearby);
		ImGui::Checkbox(Localization::Get("tabs.misc.world.never_calm").c_str(), &settings.bNeverCalm);

		ImGui::Spacing();
		GuiWidgets::SectionTitle("tabs.misc.azazel.title", ImVec4(1.0f, 1.0f, 1.0f, 0.9f));
		ImGui::Checkbox(Localization::Get("tabs.misc.azazel.modify_speed").c_str(), &settings.bFreezeAzazel);
		GuiWidgets::HelpMarker("tabs.misc.azazel.help");
		if (settings.bFreezeAzazel)
		{
			ImGui::SliderFloat(Localization::Get("tabs.misc.azazel.speed_slider").c_str(), &settings.azazelSpeed, 0.f, 15.0f, "%.1f");
		}

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Separator();

	ImGui::EndTabItem();
}
