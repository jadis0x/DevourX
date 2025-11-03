#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/NetworkTab.h"
#include "settings.h"
#include "pipeline/devour_helper.h"
#include <imgui/imgui.h>
#include <pipeline/localization/LocalizationManager.h>
void NetworkTAB::Render()
{
	if (!ImGui::BeginTabItem(Localization::Get("tabs.network.title").c_str()))
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

		ImGui::TextColored(color, Localization::Format("tabs.network.ping", static_cast<int32_t>(ping)).c_str());
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.1f, 0.85f, 1.0f, 1.0f), Localization::Get("tabs.network.manager.title").c_str());
	ImGui::Separator();
	ImGui::Spacing();

	// Lobby Manager Box
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.07f, 0.07f, 1.0f));
	ImGui::BeginChild("LobbyBox", ImVec2(0, 215), true);

	ImGui::SetCursorPos(ImVec2(25, 25));

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(0, 120);

	ImGui::TextUnformatted(Localization::Get("tabs.network.manager.region").c_str());
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(150);
	ImGui::Combo("##Region", &settings.selectedRegion, settings.regions, IM_ARRAYSIZE(settings.regions));

	ImGui::NextColumn();
	ImGui::TextUnformatted(Localization::Get("tabs.network.manager.lobby_size").c_str());
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(150);
	ImGui::SliderInt("##LobbySize", &settings.lobbySize, 1, 64);

	ImGui::NextColumn();
	ImGui::TextUnformatted(Localization::Get("tabs.network.manager.private_lobby").c_str());
	ImGui::NextColumn();
	ImGui::Checkbox("##PrivateLobby", &settings.isPrivateLobby);

	ImGui::Columns(1);
	ImGui::Spacing();

	ImGui::SetCursorPosX(12);
	if (ImGui::Button(Localization::Get("tabs.network.manager.create").c_str(), ImVec2(240, 35))) {
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
	ImGui::TextColored(ImVec4(0.1f, 0.85f, 1.0f, 1.0f), Localization::Get("tabs.network.control.title").c_str());
	ImGui::Separator();

	ImGui::Checkbox(Localization::Get("tabs.network.control.change_room").c_str(), &settings.bChangeRoomName);
	ImGui::BeginDisabled(!settings.bChangeRoomName);
	ImGui::InputText(Localization::Get("tabs.network.control.new_room").c_str(), settings.customRoomName, IM_ARRAYSIZE(settings.customRoomName));
	ImGui::EndDisabled();

	/*
	ImGui::Checkbox(Localization::Get("tabs.network.control.ghost").c_str(), &settings.bGhostMod);
	ImGui::TextWrapped("%s", Localization::Get("tabs.network.control.ghost_help").c_str());
	*/

	ImGui::Checkbox(Localization::Get("tabs.network.control.force_public").c_str(), &settings.bForcePublic);
	ImGui::TextWrapped("%s", Localization::Get("tabs.network.control.force_public_help").c_str());

	ImGui::Checkbox(Localization::Get("tabs.network.control.fake_state").c_str(), &settings.bFakeGameInProgress);
	ImGui::TextWrapped("%s", Localization::Get("tabs.network.control.fake_state_help").c_str());

	ImGui::Spacing();
	ImGui::Spacing();

	// Spoof Steam Name
	ImGui::Checkbox(Localization::Get("tabs.network.spoof.name").c_str(), &settings.bSpoofSteamName);
	if (settings.bSpoofSteamName) {
		ImGui::InputText(Localization::Get("tabs.network.spoof.custom_name").c_str(), settings.customSteamname, IM_ARRAYSIZE(settings.customSteamname));
	}

	ImGui::Separator();

	// Spoof Steam ID
	ImGui::Checkbox(Localization::Get("tabs.network.spoof.id").c_str(), &settings.bSpoofSteamId);
	if (settings.bSpoofSteamId) {
		ImGui::InputText(Localization::Get("tabs.network.spoof.custom_id").c_str(), settings.customSteamIdStr, IM_ARRAYSIZE(settings.customSteamIdStr));

		if (ImGui::Button(Localization::Get("tabs.network.spoof.generate").c_str())) {
			uint64_t randomID = Base::System::GenerateRandomSteamID64();
			snprintf(settings.customSteamIdStr, sizeof(settings.customSteamIdStr), "%llu", randomID);
		}
	}

	ImGui::EndTabItem();
}