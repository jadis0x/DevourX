#include "pch-il2cpp.h"

#include "main.h"
#include "pipeline/gui/tabs/SettingsTAB.h"
#include "pipeline/gui/widgets.h"
#include <imgui/imgui.h>
#include <pipeline/keybinds.h>
#include <pipeline/settings.h>

void SettingsTAB::Render()
{
        if (!ImGui::BeginTabItem("Settings"))
        {
                return;
        }

        ImGui::Spacing();

        if (ImGui::BeginTable("##settings_layout", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody))
        {
                ImGui::TableNextColumn();
                GuiWidgets::SectionTitle("Session", ImVec4(0.55f, 0.85f, 0.8f, 1.0f));
                ImGui::Text("Framerate: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Version: 2.0");
                ImGui::Text("Menu Toggle: %s", KeyBinds::ToString(settings.KeyBinds.Toggle_Menu));
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Discord: Jadis0x");

                ImGui::TableNextColumn();
                GuiWidgets::SectionTitle("Visual Overrides", ImVec4(1.0f, 0.88f, 0.55f, 1.0f));
                ImGui::Checkbox("Post FX Override (Experimental)", &settings.bPostFX_Override);
                GuiWidgets::HelpMarker(
                        "Forces custom post-processing settings on the next level load.\n"
                        "Great for brightening horror maps but cannot be disabled mid-match."
                );
                ImGui::Spacing();
                ImGui::Checkbox("Show Unity Logs", &settings.bEnableUnityLogs);
                ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("Need to hide DevourX quickly? Press %s at any time to toggle the menu.",
                KeyBinds::ToString(settings.KeyBinds.Toggle_Menu));

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.12f, 0.18f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.72f, 0.18f, 0.23f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.42f, 0.09f, 0.14f, 1.0f));
        if (ImGui::Button("Safely Unhook & Close", ImVec2(-1, 0)))
        {
                SetEvent(hUnloadEvent);
        }
        ImGui::PopStyleColor(3);

        ImGui::EndTabItem();
}
