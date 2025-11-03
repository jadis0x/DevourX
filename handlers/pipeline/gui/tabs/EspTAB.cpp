#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/EspTAB.h"
#include "pipeline/gui/widgets.h"
#include "settings.h"
#include <imgui/imgui.h>
#include <pipeline/localization/LocalizationManager.h>

void EspTAB::Render()
{
	if (!ImGui::BeginTabItem(Localization::Get("tabs.esp.title").c_str()))
	{
		return;
	}

	ImGui::Spacing();
	GuiWidgets::SectionTitle("tabs.esp.visual_tracking", ImVec4(0.45f, 0.75f, 1.0f, 1.0f));

	if (ImGui::BeginTable("##esp_layout", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody))
	{
		ImGui::TableNextColumn();
		ImGui::BeginGroup();
		GuiWidgets::SectionTitle("tabs.esp.azazel.title", ImVec4(0.78f, 0.58f, 1.0f, 1.0f));
		ImGui::Checkbox(Localization::Get("tabs.esp.azazel.enable").c_str(), &settings.bAzazelEsp);
		ImGui::Checkbox(Localization::Get("tabs.esp.azazel.offscreen").c_str(), &settings.bAzazelLine);
		ImGui::EndGroup();

		ImGui::TableNextColumn();
		ImGui::BeginGroup();
		GuiWidgets::SectionTitle("tabs.esp.players.title", ImVec4(0.55f, 0.85f, 0.68f, 1.0f));
		ImGui::Checkbox(Localization::Get("tabs.esp.players.enable").c_str(), &settings.bPlayerEsp);
		GuiWidgets::HelpMarker("tabs.esp.players.help");
		ImGui::Checkbox(Localization::Get("tabs.esp.players.offscreen").c_str(), &settings.bPlayerLine);
		ImGui::EndGroup();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::BeginGroup();
		GuiWidgets::SectionTitle("tabs.esp.items.title", ImVec4(0.95f, 0.74f, 0.41f, 1.0f));
		ImGui::Checkbox(Localization::Get("tabs.esp.items.enable").c_str(), &settings.bInteractableEsp);
		GuiWidgets::HelpMarker("tabs.esp.items.help");
		ImGui::EndGroup();
		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Separator();

	ImGui::EndTabItem();
}
