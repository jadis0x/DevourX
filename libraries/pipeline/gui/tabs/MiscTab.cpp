#include "pch-il2cpp.h"

#include "pipeline/gui/tabs/MiscTab.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>

void MiscTAB::Render()
{
        if (!ImGui::BeginTabItem("Misc"))
        {
                return;
        }

        ImGui::Spacing();

        if (ImGui::BeginTable("##misc_layout", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody))
        {
                ImGui::TableNextColumn();
                GuiWidgets::SectionTitle("Progression", ImVec4(0.65f, 0.84f, 0.55f, 1.0f));
                ImGui::Checkbox("Modify EXP Gain", &settings.exp_modifier);
                if (settings.exp_modifier)
                {
                        ImGui::SliderFloat("XP", &settings.newExp, 1000.f, 3000.0f, "%.0f");
                }

                ImGui::Spacing();
                ImGui::Checkbox("Modify UV Color", &settings.bModifyFlashlightColor);
                if (settings.bModifyFlashlightColor)
                {
                        ImGui::ColorEdit4("##FlashlightColor", settings.flashlightColor);
                }

                ImGui::TableNextColumn();
                GuiWidgets::SectionTitle("World Quality of Life", ImVec4(0.95f, 0.76f, 0.45f, 1.0f));
                ImGui::Checkbox("Unlock Doors", &settings.bUnlockDoors);
                ImGui::Checkbox("Disable Long Interact", &settings.bDisableLongInteract);
                ImGui::Spacing();
                ImGui::Checkbox("Auto-Calm When Nearby", &settings.bAutoCalmWhenNearby);
                ImGui::Checkbox("Never Calm", &settings.bNeverCalm);

                ImGui::Spacing();
                GuiWidgets::SectionTitle("Azazel Behaviour", ImVec4(0.86f, 0.56f, 0.9f, 1.0f));
                ImGui::Checkbox("Modify Azazel Speed", &settings.bFreezeAzazel);
                GuiWidgets::HelpMarker("Change how quickly Azazel moves across the map.");
                if (settings.bFreezeAzazel)
                {
                        ImGui::SliderFloat("New Speed", &settings.azazelSpeed, 0.f, 15.0f, "%.1f");
                }

                ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::EndTabItem();
}
