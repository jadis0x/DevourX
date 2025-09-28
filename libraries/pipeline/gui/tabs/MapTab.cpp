#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/MapTab.h"
#include "pipeline/settings.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>

void MapTAB::Render()
{
	if (ImGui::BeginTabItem("Map Manager")) {

		ImGui::Checkbox("[Lobby] Dynamic Mouse Follower", &settings.bMouseFollower);

		if (settings.bMouseFollower) {
			ImGui::SliderFloat("Offset X", &settings.MF_offsetX, -5.0f, -50.0f, "%.1f");
			ImGui::SliderFloat("Offset Y", &settings.MF_offsetY, 5.0f, 50.0f, "%.1f");
			ImGui::SliderFloat("Lerp Speed", &settings.MF_Speed, 1.f, 10.0f, "%.1f");
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("[FORCE] Lobby Start")) {
			Base::Gameplay::ForceLobbyStart();
		}

		if (ImGui::Button("[FORCE] Set Progress To:")) {
			Base::Gameplay::SetProgressTo(settings.progress);
		}
		ImGui::SliderInt("Map Progress", &settings.progress, 0, 10);

		ImGui::EndTabItem();
	}
}