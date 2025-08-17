#include "pch-il2cpp.h"

#include "main.h"
#include "pipeline/gui/tabs/SettingsTAB.h"
#include <imgui/imgui.h>
#include <pipeline/settings.h>

void SettingsTAB::Render()
{
	if (ImGui::BeginTabItem("Settings")) {

		ImGui::Spacing();

		ImGui::TextColored(ImVec4(0.55f, 0.85f, 0.8f, 0.7f), "FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // biraz boşluk

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Version: 1.0.8");
		ImGui::Spacing();

		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 550.0f);
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Discord: Jadis0x");
		ImGui::PopTextWrapPos();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Checkbox("[DEBUG] Show Unity Logs", &settings.bEnableUnityLogs); // Test checkbox
		ImGui::Spacing();

		ImGui::Checkbox("Post FX Override (Experimental)", &settings.bPostFX_Override);

		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(
				"Overrides default post-processing settings like DOF, color grading, and lens effects.\n"
				"Applies on the next level load and stays active until another level is loaded.\n"
				"Can't be turned off during gameplay."
			);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Unhook"))
		{
			SetEvent(hUnloadEvent);
		}

		ImGui::EndTabItem();
	}
}
