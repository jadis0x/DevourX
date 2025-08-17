#include "pch-il2cpp.h"

#include "devour/devourbase.h"
#include "helpers.h"
#include "pipeline/gui/tabs/PlayerTAB.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>
#include <player/player.h>

void PlayerTAB::Render()
{
    if (ImGui::BeginTabItem("Player Manager")) {
       
        ImGui::Spacing();

        ImGui::Text("Local Player Options:");
        ImGui::Checkbox("Modify Player Rank", &settings.bModifyRank);
        if (settings.bModifyRank)
            ImGui::SliderInt("New Rank", &settings.newRank, 0, 666, "%d");

        ImGui::Checkbox("Modify Player Speed", &settings.bModifyPlayerSpeed);
        if (settings.bModifyPlayerSpeed)
            ImGui::SliderFloat("New Speed", &settings.playerSpeed, 0.5f, 20.0f, "%.1f");

        ImGui::Checkbox("Fly", &settings.bFly);
        if (settings.bFly)
            ImGui::SliderFloat("Fly Speed", &settings.flySpeed, 10.f, 30.0f, "%.1f");

        ImGui::Checkbox("Fullbright", &settings.bFullbright);
        ImGui::Checkbox("Unlimited UV Light", &settings.bUnlimitedUVLight);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Player List:");

        auto& players = Players::PlayersManager::Instance().GetAllPlayers();
        if (!players.empty()) {
            std::string scene = Base::Gameplay::GetSceneName();
            bool isTown = (scene == "Town");

            int columnCount = isTown ? 8 : 7;

            if (ImGui::BeginTable("PlayersTable", columnCount, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
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
                        ImGui::Text("null");

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::Button(("Kill##" + idStr).c_str()))
                        Base::Gameplay::Knockout(players[i]);

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::Button(("Revive##" + idStr).c_str()))
                        Base::Gameplay::Revive(players[i]);

                    ImGui::TableSetColumnIndex(4);
                    if (ImGui::Button(("Jumpscare##" + idStr).c_str()))
                        Base::Gameplay::Jumpscare(players[i]);

                    ImGui::TableSetColumnIndex(5);
                    if (ImGui::Button(("TP##" + idStr).c_str()))
                        Base::Gameplay::TP(players[i]);

                    ImGui::TableSetColumnIndex(6);
                    if (ImGui::Button(("TP Azazel##" + idStr).c_str()))
                        Base::Gameplay::TPAzazel(players[i]);

                    if (isTown) {
                        ImGui::TableSetColumnIndex(7);
                        if (ImGui::Button(("Shoot##" + idStr).c_str()))
                            Base::Gameplay::Shoot(players[i]);
                    }
                }

                ImGui::EndTable();
            }
        }
        else {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "No players found.");
        }

        ImGui::EndTabItem();
    }
}