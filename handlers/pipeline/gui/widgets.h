#pragma once

#include <imgui/imgui.h>
#include <string>
#include "pipeline/localization/LocalizationManager.h"

namespace GuiWidgets
{
	inline void HelpMarker(const char* descriptionKey)
	{
		if (descriptionKey == nullptr || descriptionKey[0] == '\0')
		{
			return;
		}

		ImGui::SameLine();
		const std::string& icon = Localization::Get("common.help_icon");
		ImGui::TextDisabled(icon.c_str());
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			const std::string& localizedDescription = Localization::Get(descriptionKey);
			if (localizedDescription.empty())
			{
				return;
			}

			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 34.0f);
			ImGui::TextUnformatted(localizedDescription.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::Spacing();
	}

	inline void SectionTitle(const char* titleKey, const ImVec4& colour)
	{
		const std::string& title = Localization::Get(titleKey);

		ImGui::PushStyleColor(ImGuiCol_Text, colour);
		ImGui::TextUnformatted(title.c_str());
		ImGui::PopStyleColor();
		ImGui::Separator();
		ImGui::Spacing();
	}
}
