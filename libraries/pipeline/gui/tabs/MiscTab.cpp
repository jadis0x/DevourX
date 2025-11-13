#include "pch-il2cpp.h"
#include <devour/devourbase.h>
#include "pipeline/gui/tabs/MiscTab.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/settings.h"
#include <imgui/imgui.h>

// 前向声明 ImFont
struct ImFont;

// 声明外部字体变量
namespace Menu {
    extern ImFont* chineseFont;
}

void MiscTAB::Render()
{
    // 在BeginTabItem之前应用中文字体
    if (Menu::chineseFont) {
        ImGui::PushFont(Menu::chineseFont);
    }

    if (!ImGui::BeginTabItem(u8"杂项"))
    {
        // 如果标签项没有开始，也要弹出字体
        if (Menu::chineseFont) {
            ImGui::PopFont();
        }
        return;
    }

    ImGui::Spacing();

    if (ImGui::BeginTable("##misc_layout", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody))
    {
        ImGui::TableNextColumn();
        GuiWidgets::SectionTitle(u8"个人修改", ImVec4(0.65f, 0.84f, 0.55f, 1.0f));
        ImGui::Checkbox(u8"修改经验获取", &settings.exp_modifier);
        if (settings.exp_modifier)
        {
            ImGui::SliderFloat(u8"经验值", &settings.newExp, 1000.f, 7000.0f, "%.0f");
        }

        ImGui::Spacing();
        ImGui::Checkbox(u8"修改UV灯颜色", &settings.bModifyFlashlightColor);
        if (settings.bModifyFlashlightColor)
        {
            ImGui::ColorEdit4("##FlashlightColor", settings.flashlightColor);
        }
        if (ImGui::Button(u8"获取物品", ImVec2(140, 30)))
        {
            Base::Gameplay::TPItems();
        }
        GuiWidgets::HelpMarker(u8"将地图上所有可交互物品传送到玩家位置");
        // 在 MiscTab.cpp 的适当位置添加钥匙传送按钮
        ImGui::Spacing();
        if (ImGui::Button(u8"获取钥匙", ImVec2(120, 30)))
        {
            Base::Gameplay::TPKeys();
        }
        GuiWidgets::HelpMarker(u8"将地图上所有钥匙传送到玩家位置");

        ImGui::TableNextColumn();
        GuiWidgets::SectionTitle(u8"世界便利功能", ImVec4(0.95f, 0.76f, 0.45f, 1.0f));
        ImGui::Checkbox(u8"解锁所有门", &settings.bUnlockDoors);
        ImGui::Checkbox(u8"禁用长按交互", &settings.bDisableLongInteract);
        ImGui::Spacing();
        ImGui::Checkbox(u8"靠近时自动冷静", &settings.bAutoCalmWhenNearby);
        ImGui::Checkbox(u8"解锁所有", &settings.bUnlockAll);
        ImGui::Checkbox(u8"永不冷静", &settings.bNeverCalm);
        ImGui::Spacing();
        GuiWidgets::SectionTitle(u8"恶魔行为", ImVec4(0.86f, 0.56f, 0.9f, 1.0f));
        ImGui::Checkbox(u8"修改恶魔速度", &settings.bFreezeAzazel);
        GuiWidgets::HelpMarker(u8"改变恶魔在地图上的移动速度。");
        if (settings.bFreezeAzazel)
        {
            ImGui::SliderFloat(u8"新速度", &settings.azazelSpeed, 0.f, 15.0f, "%.1f");
        }

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