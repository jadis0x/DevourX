#include "pch-il2cpp.h"
#include "pipeline/gui/GUITheme.h" 
#include "pipeline/gui/tabs/EspTAB.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>
#include <iostream>

void EspTAB::Render()
{
	if (ImGui::BeginTabItem("ESP")) {

		ImGui::Spacing();

		ImGui::Checkbox("Azazel ESP", &settings.bAzazelEsp);
		ImGui::Checkbox("Azazel Off-Screen Line", &settings.bAzazelLine);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Checkbox("Player ESP", &settings.bPlayerEsp);
		ImGui::Checkbox("Player Off-Screen Line", &settings.bPlayerLine);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Checkbox("Interactable Item ESP", &settings.bInteractableEsp);
		ImGui::EndTabItem();
	}
}
