#include "pch-il2cpp.h"

#include "devour/devourbase.h"
#include "helpers.h"
#include "pipeline/gui/tabs/PlayerTAB.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>
#include <player/player.h>
#include <pipeline/localization/LocalizationManager.h>
#include <cfloat>

#include <string>
#include <vector>
#include <resolver/il2cpp_resolver.h>

namespace
{
	void CenteredText(const std::string& label)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		const float columnWidth = ImGui::GetColumnWidth();
		const float available = columnWidth - style.CellPadding.x * 2.0f;
		const float labelWidth = ImGui::CalcTextSize(label.c_str()).x;
		const float offset = (available - labelWidth) * 0.5f;
		if (offset > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

		ImGui::TextUnformatted(label.c_str());
	}

	bool CenteredButton(const std::string& label, bool enabled)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		const ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
		const float buttonWidth = labelSize.x + style.FramePadding.x * 2.0f;
		const float columnWidth = ImGui::GetColumnWidth();
		const float available = columnWidth - style.CellPadding.x * 2.0f;
		const float offset = (available - buttonWidth) * 0.5f;
		if (offset > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

		if (!enabled)
			ImGui::BeginDisabled();

		const bool pressed = ImGui::Button(label.c_str(), ImVec2(buttonWidth, 0.0f));

		if (!enabled)
			ImGui::EndDisabled();

		return pressed && enabled;
	}

	std::string BuildPlayerName(app::GameObject* player)
	{
		if (!SafePtr::IsValid(player))
			return Localization::Get("tabs.player.table.null");

		std::string resolvedName;
		const bool success = SafePtr::safe_call([&]() {
			resolvedName = il2cppi_to_string(ToString(reinterpret_cast<app::Object_1*>(player)));
			});

		if (!success || resolvedName.empty())
			return Localization::Get("tabs.player.table.null");

		return resolvedName;
	}

	struct PlayerAction
	{
		const char* labelKey;
		void (*invoke)(app::GameObject*);
	};
}

void PlayerTAB::Render()
{
    if (!ImGui::BeginTabItem(Localization::Get("tabs.player.title").c_str()))
        return;

    ImGui::Spacing();
    ImGui::SeparatorText(Localization::Get("tabs.player.local_options").c_str());

    constexpr ImGuiTableFlags localFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_PadOuterX;
    if (ImGui::BeginTable("PlayerOptionsTable", 2, localFlags)) {
        constexpr float sliderWidth = -FLT_MIN;

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Checkbox(Localization::Get("tabs.player.modify_rank").c_str(), &settings.bModifyRank);
        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!settings.bModifyRank);
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderInt(Localization::Get("tabs.player.rank_slider").c_str(), &settings.newRank, 0, 666, "%d");
        ImGui::EndDisabled();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Checkbox(Localization::Get("tabs.player.modify_speed").c_str(), &settings.bModifyPlayerSpeed);
        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!settings.bModifyPlayerSpeed);
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat(Localization::Get("tabs.player.speed_slider").c_str(), &settings.playerSpeed, 0.5f, 200.0f, "%.1f");
        ImGui::EndDisabled();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Checkbox(Localization::Get("tabs.player.fly").c_str(), &settings.bFly);
        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!settings.bFly);
        ImGui::SetNextItemWidth(sliderWidth);
        ImGui::SliderFloat(Localization::Get("tabs.player.fly_speed").c_str(), &settings.flySpeed, 10.0f, 30.0f, "%.1f");
        ImGui::EndDisabled();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Checkbox(Localization::Get("tabs.player.fullbright").c_str(), &settings.bFullbright);
        ImGui::TableNextColumn();
        ImGui::Checkbox(Localization::Get("tabs.player.unlimited_uv").c_str(), &settings.bUnlimitedUVLight);

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::SeparatorText(Localization::Get("tabs.player.list_title").c_str());

    auto& players = Players::PlayersManager::Instance().GetAllPlayers();
    if (players.empty()) {
        ImGui::TextDisabled(Localization::Get("tabs.player.no_players").c_str());
        ImGui::EndTabItem();
        return;
    }

    const std::string sceneName = Base::Gameplay::GetSceneName();
    const bool canShoot = (sceneName == "Town");

    std::vector<PlayerAction> actions = {
            { "tabs.player.actions.kill", Base::Gameplay::Knockout },
            { "tabs.player.actions.revive", Base::Gameplay::Revive },
            { "tabs.player.actions.jumpscare", Base::Gameplay::Jumpscare },
            { "tabs.player.actions.teleport", Base::Gameplay::TP },
            { "tabs.player.actions.teleport_azazel", Base::Gameplay::TPAzazel }
    };

    if (canShoot) {
        actions.push_back({ "tabs.player.actions.shoot", Base::Gameplay::Shoot });
    }

    std::vector<std::string> actionLabels;
    actionLabels.reserve(actions.size());
    for (const PlayerAction& action : actions)
        actionLabels.emplace_back(Localization::Get(action.labelKey));

    const std::string idHeader = Localization::Get("tabs.player.table.id");
    const std::string nameHeader = Localization::Get("tabs.player.table.name");

    const int columnCount = static_cast<int>(actions.size()) + 2;
    const ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

    bool actionFailed = false;

    if (ImGui::BeginTable("PlayerManager.Table", columnCount, tableFlags)) {
        ImGui::TableSetupScrollFreeze(0, 1);

        ImGui::TableSetupColumn(idHeader.c_str(), ImGuiTableColumnFlags_WidthStretch, 0.6f);
        ImGui::TableSetupColumn(nameHeader.c_str(), ImGuiTableColumnFlags_WidthStretch, 2.0f);
        for (size_t i = 0; i < actionLabels.size(); ++i)
            ImGui::TableSetupColumn(actionLabels[i].c_str(), ImGuiTableColumnFlags_WidthStretch, 1.0f);

        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        ImGui::TableNextColumn();
        CenteredText(idHeader);
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(nameHeader.c_str());
        for (const std::string& header : actionLabels) {
            ImGui::TableNextColumn();
            CenteredText(header);
        }

        for (size_t index = 0; index < players.size(); ++index) {
            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(index));

            app::GameObject* player = players[index];
            const bool playerValid = SafePtr::IsValid(player);

            ImGui::TableNextColumn();
            CenteredText(std::to_string(index + 1));

            ImGui::TableNextColumn();
            const std::string playerName = BuildPlayerName(player);
            if (playerValid)
                ImGui::TextUnformatted(playerName.c_str());
            else
                ImGui::TextDisabled(playerName.c_str());

            for (size_t actionIndex = 0; actionIndex < actions.size(); ++actionIndex) {
                ImGui::TableNextColumn();
                ImGui::PushID(actions[actionIndex].labelKey);
                if (CenteredButton(actionLabels[actionIndex], playerValid)) {
                    const bool executed = SafePtr::safe_call([&]() {
                        actions[actionIndex].invoke(player);
                        });

                    actionFailed = actionFailed || !executed;
                }
                ImGui::PopID();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    if (actionFailed) {
        ImGui::Spacing();
        ImGui::TextDisabled(Localization::Get("tabs.player.action_failed").c_str());
    }

    ImGui::EndTabItem();
}