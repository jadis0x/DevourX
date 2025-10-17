#include "pch-il2cpp.h"

#include "main.h"
#include "pipeline/gui/tabs/SettingsTAB.h"
#include "pipeline/gui/widgets.h"
#include <imgui/imgui.h>
#include <pipeline/keybinds.h>
#include <pipeline/settings.h>
#include <string>
#include <pipeline/localization/LocalizationManager.h>

void SettingsTAB::Render()
{
	if (!ImGui::BeginTabItem(Localization::Get("tabs.settings.title").c_str()))
	{
		return;
	}

	ImGui::Spacing();

	if (ImGui::BeginTable("##settings_layout", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody))
	{
		ImGui::TableNextColumn();
		GuiWidgets::SectionTitle("tabs.settings.session.title", ImVec4(0.55f, 0.85f, 0.8f, 1.0f));
		ImGui::TextUnformatted(Localization::Format("tabs.settings.session.framerate", ImGui::GetIO().Framerate).c_str());
		ImGui::TextUnformatted(Localization::Format("tabs.settings.session.version", "2.0").c_str());
		ImGui::TextUnformatted(Localization::Format("tabs.settings.session.menu_toggle", KeyBinds::ToString(settings.KeyBinds.Toggle_Menu)).c_str());
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), Localization::Get("tabs.settings.session.discord").c_str());

		ImGui::Spacing();
		GuiWidgets::SectionTitle("tabs.settings.language.title", ImVec4(0.65f, 0.76f, 1.0f, 1.0f));
		const auto cultures = Localization::GetAvailableCultures();
		if (cultures.empty())
		{
			ImGui::TextUnformatted(Localization::Get("common.language_unavailable").c_str());
		}
		else
		{
			const std::string comboLabel = Localization::Get("tabs.settings.language.dropdown");
			std::string comboId = comboLabel + "##language_selection";
			std::string currentDisplay = Localization::GetDisplayName(settings.localizationCulture);
			if (currentDisplay.empty())
			{
				currentDisplay = settings.localizationCulture;
			}

			if (ImGui::BeginCombo(comboId.c_str(), currentDisplay.c_str()))
			{
				for (const auto& culture : cultures)
				{
					const bool isSelected = (culture == settings.localizationCulture);
					std::string displayName = Localization::GetDisplayName(culture);
					if (displayName.empty())
					{
						displayName = culture;
					}

					if (ImGui::Selectable(displayName.c_str(), isSelected))
					{
						if (culture != settings.localizationCulture)
						{
							settings.localizationCulture = culture;
							SaveSettingsToConfig();
							settings.queueLanguageRestartNotification = true;
						}
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::TableNextColumn();
		GuiWidgets::SectionTitle("tabs.settings.visual.title", ImVec4(1.0f, 0.88f, 0.55f, 1.0f));
		ImGui::Checkbox(Localization::Get("tabs.settings.visual.post_fx").c_str(), &settings.bPostFX_Override);
		GuiWidgets::HelpMarker("tabs.settings.visual.post_fx_help");
		ImGui::Spacing();
		ImGui::Checkbox(Localization::Get("tabs.settings.visual.unity_logs").c_str(), &settings.bEnableUnityLogs);
		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextWrapped("%s", Localization::Format("tabs.settings.hint", KeyBinds::ToString(settings.KeyBinds.Toggle_Menu)).c_str());

	ImGui::Spacing();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.12f, 0.18f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.72f, 0.18f, 0.23f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.42f, 0.09f, 0.14f, 1.0f));
	if (ImGui::Button(Localization::Get("tabs.settings.unhook_button").c_str(), ImVec2(-1, 0)))
	{
		SetEvent(hUnloadEvent);
	}
	ImGui::PopStyleColor(3);

	ImGui::EndTabItem();
}
