#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/EspTAB.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>

// 前向声明 ImFont
struct ImFont;

// 声明外部字体变量
namespace Menu {
    extern ImFont* chineseFont;
}

void EspTAB::Render()
{
    // 在BeginTabItem之前应用中文字体
    if (Menu::chineseFont) {
        ImGui::PushFont(Menu::chineseFont);
    }

    if (!ImGui::BeginTabItem(u8"透视"))
    {
        // 如果标签项没有开始，也要弹出字体
        if (Menu::chineseFont) {
            ImGui::PopFont();
        }
        return;
    }

    ImGui::Spacing();
    GuiWidgets::SectionTitle(u8"视觉追踪", ImVec4(0.45f, 0.75f, 1.0f, 1.0f));

    if (ImGui::BeginTable("##esp_layout", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody))
    {
        ImGui::TableNextColumn();
        ImGui::BeginGroup();
        GuiWidgets::SectionTitle(u8"恶魔", ImVec4(0.78f, 0.58f, 1.0f, 1.0f));
        ImGui::Checkbox(u8"启用透视", &settings.bAzazelEsp);
        ImGui::Checkbox(u8"屏幕外指示器", &settings.bAzazelLine);
        ImGui::EndGroup();

        ImGui::TableNextColumn();
        ImGui::BeginGroup();
        GuiWidgets::SectionTitle(u8"玩家", ImVec4(0.55f, 0.85f, 0.68f, 1.0f));
        ImGui::Checkbox(u8"玩家透视", &settings.bPlayerEsp);
        GuiWidgets::HelpMarker(u8"显示穿墙的玩家及其距离。");
        ImGui::Checkbox(u8"玩家屏幕外连线", &settings.bPlayerLine);
        ImGui::EndGroup();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::BeginGroup();
        GuiWidgets::SectionTitle(u8"世界物品", ImVec4(0.95f, 0.74f, 0.41f, 1.0f));
        ImGui::Checkbox(u8"可交互物品透视", &settings.bInteractableEsp);
        GuiWidgets::HelpMarker(u8"高亮显示钥匙、仪式物品和其他目标，便于快速规划路线。");
        ImGui::EndGroup();

        // 添加恶魔生物ESP选项
        ImGui::TableSetColumnIndex(1);
        ImGui::BeginGroup();
        GuiWidgets::SectionTitle(u8"恶魔生物", ImVec4(0.95f, 0.41f, 0.41f, 1.0f)); // 红色系
        ImGui::Checkbox(u8"恶魔生物透视", &settings.bSimpleDemonEsp);
        GuiWidgets::HelpMarker(u8"显示恶魔、蜘蛛、幽灵、野猪等生物的位置。");
        ImGui::EndGroup();

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();

    // 弹出字体
    if (Menu::chineseFont) {
        ImGui::PopFont();
    }

    ImGui::EndTabItem();
}