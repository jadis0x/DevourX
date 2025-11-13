#include "pch-il2cpp.h"

#include "devour/devourbase.h"
#include "helpers.h"
#include "pipeline/gui/tabs/PlayerTAB.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>
#include <player/player.h>

// 前向声明 ImFont
struct ImFont;

// 声明外部字体变量
namespace Menu {
    extern ImFont* chineseFont;
}

void PlayerTAB::Render()
{
    // 在BeginTabItem之前应用中文字体
    if (Menu::chineseFont) {
        ImGui::PushFont(Menu::chineseFont);
    }

    if (ImGui::BeginTabItem(u8"玩家管理"))
    {
        ImGui::Spacing();

        ImGui::Text(u8"本地玩家选项:");
        // ========== 添加怪物无视功能 ==========
        ImGui::Checkbox(u8"怪物无视我", &settings.bMonsterIgnoreMe);

        // V键快速开关设置
        ImGui::Text(u8"V键快速开关:");
        ImGui::SameLine();

        const char* vKeyModes[] = {
            u8"关闭",
            u8"切换开关",
            u8"按住开启"
        };
        ImGui::Combo("##VKeyMode", &settings.monsterIgnoreVKeyMode, vKeyModes, IM_ARRAYSIZE(vKeyModes));

        if (settings.bMonsterIgnoreMe) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), u8"隐身模式激活 - 怪物无法看到或听到你");
        }
        else {
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), u8"隐身模式关闭");
        }

        // 使用说明
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), u8"使用说明:");
        if (settings.monsterIgnoreVKeyMode == 1) {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), u8"按V键切换勾选状态");
        }
        else if (settings.monsterIgnoreVKeyMode == 2) {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), u8"按住V键勾选，松开关闭");
        }
        // ========== 添加结束 ==========
        // ========== 添加运物宏功能 ==========
        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Checkbox(u8"启用运物宏", &settings.bCarryMacro);
        if (settings.bCarryMacro) {
            // 触发按键选择
            ImGui::Text(u8"触发按键:");
            ImGui::SameLine();

            const char* triggerKeys[] = {
                u8"鼠标左键",
                u8"E键"
            };
            ImGui::Combo("##CarryMacroKey", &settings.carryMacroKey, triggerKeys, IM_ARRAYSIZE(triggerKeys));

            // 显示当前设置
            const char* currentKey = triggerKeys[settings.carryMacroKey];
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), u8"按下%s自动放下物品(G键)", currentKey);
        }
        // ========== 运物宏功能结束 ==========

        ImGui::Checkbox(u8"修改玩家等级", &settings.bModifyRank);
        if (settings.bModifyRank)
            ImGui::SliderInt(u8"新等级", &settings.newRank, 0, 666, "%d");

        ImGui::Checkbox(u8"修改玩家速度", &settings.bModifyPlayerSpeed);
        if (settings.bModifyPlayerSpeed)
            ImGui::SliderFloat(u8"新速度", &settings.playerSpeed, 0.5f, 20.0f, "%.1f");
        // 跳跃高度控制
        ImGui::Checkbox(u8"修改跳跃高度", &settings.bModifyJumpHeight);
        if (settings.bModifyJumpHeight)
            ImGui::SliderFloat(u8"跳跃力", &settings.jumpHeight, 0.01f, 20.00f, "%.2f");

        // === 添加N键跳跃设置 ===
// === 添加跳跃设置 ===
        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Checkbox(u8"超级跳跃", &settings.bSuperJump);
        if (settings.bSuperJump)
        {
            ImGui::SliderFloat(u8"跳跃倍数", &settings.superJumpMultiplier, 0.01f, 10.00f, "%.2f");

            // 添加自定义按键设置
            ImGui::Text(u8"跳跃按键:");
            ImGui::SameLine();

            // 按键选择下拉菜单（默认Q，其他为侧键）
            const char* keyNames[] = {
                u8"Q键",
                u8"鼠标侧键1 (前进)",
                u8"鼠标侧键2 (后退)",
                u8"鼠标侧键3",
                u8"鼠标侧键4",
                u8"鼠标侧键5"
            };
            int currentKey = settings.superJumpKey;
            if (ImGui::Combo("##JumpKey", &currentKey, keyNames, IM_ARRAYSIZE(keyNames)))
            {
                settings.superJumpKey = currentKey;
            }

            // 显示当前按键提示
            const char* currentKeyName = keyNames[settings.superJumpKey];
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), u8"提示: 按%s触发超级跳跃", currentKeyName);
        }

        ImGui::Checkbox(u8"飞行模式", &settings.bFly);
        if (settings.bFly)
            ImGui::SliderFloat(u8"飞行速度", &settings.flySpeed, 10.f, 30.0f, "%.1f");

        ImGui::Checkbox(u8"全亮模式", &settings.bFullbright);
        ImGui::Checkbox(u8"无限UV光", &settings.bUnlimitedUVLight);


        ImGui::Spacing();
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), u8"玩家列表:");

        auto& players = Players::PlayersManager::Instance().GetAllPlayers();
        if (!players.empty()) {
            std::string scene = Base::Gameplay::GetSceneName();
            bool isTown = (scene == "Town");

            int columnCount = isTown ? 8 : 7;

            if (ImGui::BeginTable("PlayersTable", columnCount, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn(u8"ID", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableSetupColumn(u8"名称", ImGuiTableColumnFlags_WidthStretch);
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
                    if (ImGui::Button((u8"击倒##" + idStr).c_str()))
                        Base::Gameplay::Knockout(players[i]);

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::Button((u8"复活##" + idStr).c_str()))
                        Base::Gameplay::Revive(players[i]);

                    ImGui::TableSetColumnIndex(4);
                    if (ImGui::Button((u8"惊吓##" + idStr).c_str()))
                        Base::Gameplay::Jumpscare(players[i]);

                    ImGui::TableSetColumnIndex(5);
                    if (ImGui::Button((u8"传送##" + idStr).c_str()))
                        Base::Gameplay::TP(players[i]);

                    ImGui::TableSetColumnIndex(6);
                    if (ImGui::Button((u8"传送恶魔##" + idStr).c_str()))
                        Base::Gameplay::TPAzazel(players[i]);

                    if (isTown) {
                        ImGui::TableSetColumnIndex(7);
                        if (ImGui::Button((u8"射击##" + idStr).c_str()))
                            Base::Gameplay::Shoot(players[i]);
                    }
                }

                ImGui::EndTable();
            }
        }
        else {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), u8"未找到玩家。");
        }

        // 弹出字体
        if (Menu::chineseFont) {
            ImGui::PopFont();
        }

        ImGui::EndTabItem();
    }
    else
    {
        // 如果标签项没有开始，也要弹出字体
        if (Menu::chineseFont) {
            ImGui::PopFont();
        }
    }
}