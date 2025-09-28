#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/EspTAB.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>

void EspTAB::Render()
{
        if (!ImGui::BeginTabItem("ESP"))
        {
                return;
        }

        ImGui::Spacing();
        GuiWidgets::SectionTitle("Visual Tracking", ImVec4(0.45f, 0.75f, 1.0f, 1.0f));

        if (ImGui::BeginTable("##esp_layout", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody))
        {
                ImGui::TableNextColumn();
                ImGui::BeginGroup();
                GuiWidgets::SectionTitle("Azazel", ImVec4(0.78f, 0.58f, 1.0f, 1.0f));
                ImGui::Checkbox("Enable ESP", &settings.bAzazelEsp);
                ImGui::Checkbox("Off-Screen Indicator", &settings.bAzazelLine);
                ImGui::EndGroup();

                ImGui::TableNextColumn();
                ImGui::BeginGroup();
                GuiWidgets::SectionTitle("Players", ImVec4(0.55f, 0.85f, 0.68f, 1.0f));
                ImGui::Checkbox("Player ESP", &settings.bPlayerEsp);
                GuiWidgets::HelpMarker("Shows players through walls alongside their distance.");
                ImGui::Checkbox("Player Off-Screen Line", &settings.bPlayerLine);
                ImGui::EndGroup();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::BeginGroup();
                GuiWidgets::SectionTitle("World Items", ImVec4(0.95f, 0.74f, 0.41f, 1.0f));
                ImGui::Checkbox("Interactable Item ESP", &settings.bInteractableEsp);
                GuiWidgets::HelpMarker("Highlights keys, ritual pieces and other objectives for quick routing.");
                ImGui::EndGroup();
                ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::EndTabItem();
}
