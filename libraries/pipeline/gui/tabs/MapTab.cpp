#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/MapTab.h"
#include "pipeline/settings.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>
#include <pipeline/localization/LocalizationManager.h>

void MapTAB::Render()
{
	if (ImGui::BeginTabItem(Localization::Get("tabs.map.title").c_str())) {

		ImGui::Checkbox(Localization::Get("tabs.map.mouse_follower").c_str(), &settings.bMouseFollower);

		if (settings.bMouseFollower) {
			ImGui::SliderFloat(Localization::Get("tabs.map.offset_x").c_str(), &settings.MF_offsetX, -5.0f, -50.0f, "%.1f");
			ImGui::SliderFloat(Localization::Get("tabs.map.offset_y").c_str(), &settings.MF_offsetY, 5.0f, 50.0f, "%.1f");
			ImGui::SliderFloat(Localization::Get("tabs.map.lerp_speed").c_str(), &settings.MF_Speed, 1.f, 10.0f, "%.1f");
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button(Localization::Get("tabs.map.force_start").c_str())) {
			Base::Gameplay::ForceLobbyStart();
		}

		if (ImGui::Button(Localization::Get("tabs.map.force_progress").c_str())) {
			Base::Gameplay::SetProgressTo(settings.progress);
		}
		ImGui::SliderInt(Localization::Get("tabs.map.progress_slider").c_str(), &settings.progress, 0, 10);

		ImGui::EndTabItem();
	}
}