#include "pch-il2cpp.h"

#include "devour/devourbase.h"
#include "helpers.h"
#include "pipeline/gui/tabs/PlayerTAB.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>
#include <player/player.h>
#include <string>
#include <pipeline/localization/LocalizationManager.h>
#include <cfloat>

void PlayerTAB::Render()
{
	if (ImGui::BeginTabItem(Localization::Get("tabs.player.title").c_str())) {
		ImGui::Spacing();

		ImGui::TextUnformatted(Localization::Get("tabs.player.local_options").c_str());
		ImGui::Checkbox(Localization::Get("tabs.player.modify_rank").c_str(), &settings.bModifyRank);
		if (settings.bModifyRank)
			ImGui::SliderInt(Localization::Get("tabs.player.rank_slider").c_str(), &settings.newRank, 0, 666, "%d");

		ImGui::Checkbox(Localization::Get("tabs.player.modify_speed").c_str(), &settings.bModifyPlayerSpeed);
		if (settings.bModifyPlayerSpeed)
			ImGui::SliderFloat(Localization::Get("tabs.player.speed_slider").c_str(), &settings.playerSpeed, 0.5f, 20.0f, "%.1f");

		ImGui::Checkbox(Localization::Get("tabs.player.fly").c_str(), &settings.bFly);
		if (settings.bFly)
			ImGui::SliderFloat(Localization::Get("tabs.player.fly_speed").c_str(), &settings.flySpeed, 10.f, 30.0f, "%.1f");

		ImGui::Checkbox(Localization::Get("tabs.player.fullbright").c_str(), &settings.bFullbright);
		ImGui::Checkbox(Localization::Get("tabs.player.unlimited_uv").c_str(), &settings.bUnlimitedUVLight);

		ImGui::Spacing();
		ImGui::Separator();

		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), Localization::Get("tabs.player.list_title").c_str());

		auto& players = Players::PlayersManager::Instance().GetAllPlayers();
		if (!players.empty()) {
			std::string scene = Base::Gameplay::GetSceneName();
			bool isTown = (scene == "Town");

			int columnCount = isTown ? 8 : 7;

			if (ImGui::BeginTable("PlayersTable", columnCount, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
				ImGui::TableSetupColumn(Localization::Get("tabs.player.table.id").c_str(), ImGuiTableColumnFlags_WidthFixed, 40.0f);
				ImGui::TableSetupColumn(Localization::Get("tabs.player.table.name").c_str(), ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 40.0f);
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 60.0f);
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 80.0f);
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 35.0f);
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 80.0f);
				if (isTown)
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 80.0f);

				ImGui::TableHeadersRow();

				for (size_t i = 0; i < players.size(); ++i) {
					ImGui::TableNextRow();
					std::string idStr = std::to_string(i);

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%zu", i + 1);

					ImGui::TableSetColumnIndex(1);
					if (players[i])
						ImGui::Text("%s", il2cppi_to_string(ToString((app::Object_1*)players[i])).c_str());
					else
						ImGui::TextUnformatted(Localization::Get("tabs.player.table.null").c_str());

					ImGui::TableSetColumnIndex(2);
					std::string killLabel = Localization::Get("tabs.player.actions.kill");
					killLabel += "##";
					killLabel += idStr;
					if (ImGui::Button(killLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
						Base::Gameplay::Knockout(players[i]);

					ImGui::TableSetColumnIndex(3);
					std::string reviveLabel = Localization::Get("tabs.player.actions.revive");
					reviveLabel += "##";
					reviveLabel += idStr;
					if (ImGui::Button(reviveLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
						Base::Gameplay::Revive(players[i]);

					ImGui::TableSetColumnIndex(4);
					std::string jumpscareLabel = Localization::Get("tabs.player.actions.jumpscare");
					jumpscareLabel += "##";
					jumpscareLabel += idStr;
					if (ImGui::Button(jumpscareLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
						Base::Gameplay::Jumpscare(players[i]);

					ImGui::TableSetColumnIndex(5);
					std::string tpLabel = Localization::Get("tabs.player.actions.teleport");
					tpLabel += "##";
					tpLabel += idStr;
					if (ImGui::Button(tpLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
						Base::Gameplay::TP(players[i]);

					ImGui::TableSetColumnIndex(6);
					std::string tpAzazelLabel = Localization::Get("tabs.player.actions.teleport_azazel");
					tpAzazelLabel += "##";
					tpAzazelLabel += idStr;
					if (ImGui::Button(tpAzazelLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
						Base::Gameplay::TPAzazel(players[i]);

					if (isTown) {
						ImGui::TableSetColumnIndex(7);
						std::string shootLabel = Localization::Get("tabs.player.actions.shoot");
						shootLabel += "##";
						shootLabel += idStr;
						if (ImGui::Button(shootLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
							Base::Gameplay::Shoot(players[i]);
					}

				}

				ImGui::EndTable();
			}
		}
		else {
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), Localization::Get("tabs.player.no_players").c_str());
		}

		ImGui::EndTabItem();
	}
}