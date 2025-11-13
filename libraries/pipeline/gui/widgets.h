#pragma once

#include <imgui/imgui.h>

namespace GuiWidgets
{
        inline void HelpMarker(const char* description)
        {
                if (description == nullptr || description[0] == '\0')
                {
                        return;
                }

                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
                {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 34.0f);
                        ImGui::TextUnformatted(description);
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                }
                ImGui::Spacing();
        }

        inline void SectionTitle(const char* title, const ImVec4& colour)
        {
                ImGui::PushStyleColor(ImGuiCol_Text, colour);
                ImGui::TextUnformatted(title);
                ImGui::PopStyleColor();
                ImGui::Separator();
                ImGui::Spacing();
        }
}
