#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/NetworkTab.h"
#include "pipeline/settings.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>

void NetworkTAB::Render()
{
	if (!ImGui::BeginTabItem("Network"))
		return;

	ImGui::Spacing();

	// Ping
	if (!Base::DevourNet::IsHost())
	{
		float ping = Base::DevourNet::GetPing();
		ImVec4 color;

		if (ping < 50.f)
			color = ImVec4(0.2f, 1.0f, 0.2f, 0.85f);
		else if (ping < 100.f)
			color = ImVec4(1.0f, 0.9f, 0.1f, 0.85f);
		else
			color = ImVec4(1.0f, 0.2f, 0.2f, 0.85f);

		ImGui::TextColored(color, "Ping: %dms", static_cast<int32_t>(ping));
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.1f, 0.85f, 1.0f, 1.0f), "Lobby Manager");
	ImGui::Separator();
	ImGui::Spacing();

	// Lobby Manager Box
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.07f, 0.07f, 1.0f));
	ImGui::BeginChild("LobbyBox", ImVec2(0, 215), true);

	ImGui::SetCursorPos(ImVec2(25, 25));

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(0, 120);

	ImGui::Text("Region");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(150);
	ImGui::Combo("##Region", &settings.selectedRegion, settings.regions, IM_ARRAYSIZE(settings.regions));

	ImGui::NextColumn();
	ImGui::Text("Lobby Size");
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("##LobbySize", &settings.lobbySize, 1, 64);

	ImGui::NextColumn();
	ImGui::Text("Private Lobby");
	ImGui::NextColumn();
	ImGui::Checkbox("##PrivateLobby", &settings.isPrivateLobby);

	ImGui::Columns(1);
	ImGui::Spacing();

	ImGui::SetCursorPosX(12);
	if (ImGui::Button("Create Lobby", ImVec2(240, 35))) {
		Base::Gameplay::CreateLobby(
			settings.regions[settings.selectedRegion],
			settings.lobbySize,
			settings.isPrivateLobby
		);
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Spacing();

	// Lobby Control
	ImGui::TextColored(ImVec4(0.1f, 0.85f, 1.0f, 1.0f), "Lobby Control");
	ImGui::Separator();

	ImGui::Checkbox("Change Room Name", &settings.bChangeRoomName);
	ImGui::BeginDisabled(!settings.bChangeRoomName);
	ImGui::InputText("New Room Name", settings.customRoomName, IM_ARRAYSIZE(settings.customRoomName));
	ImGui::EndDisabled();

	ImGui::Checkbox("Ghost Mode (Stealth Join)", &settings.bGhostMod);
	ImGui::TextWrapped("Join as invisible. No one sees you, but you see and hear them!");

	ImGui::Checkbox("Make Lobby Public", &settings.bForcePublic);
	ImGui::TextWrapped("If enabled, the current private lobby will be forcibly made public and visible to everyone.");

	ImGui::Checkbox("Fake Game State", &settings.bFakeGameInProgress);
	ImGui::TextWrapped("If enabled, the current lobby will appear as if the game is still in progress or not.");


	ImGui::Spacing();
	ImGui::Spacing();

	// Spoof Steam Name
	ImGui::Checkbox("Spoof Steam Name", &settings.bSpoofSteamName);
	if (settings.bSpoofSteamName) {
		ImGui::InputText("Custom Steam Name", settings.customSteamname, IM_ARRAYSIZE(settings.customSteamname));
	}

	ImGui::Separator();

	// Spoof Steam ID
	ImGui::Checkbox("Spoof Steam ID", &settings.bSpoofSteamId);
	if (settings.bSpoofSteamId) {
		ImGui::InputText("Custom Steam ID", settings.customSteamIdStr, IM_ARRAYSIZE(settings.customSteamIdStr));

		if (ImGui::Button("Generate Random SteamID")) {
			uint64_t randomID = Base::System::GenerateRandomSteamID64();
			snprintf(settings.customSteamIdStr, sizeof(settings.customSteamIdStr), "%llu", randomID);
		}
	}

	ImGui::EndTabItem();
}