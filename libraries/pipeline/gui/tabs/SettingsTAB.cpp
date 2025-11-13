#include "pch-il2cpp.h"

#include "main.h"
#include "pipeline/gui/tabs/SettingsTAB.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/gui/menu.h"  // 包含menu.h以使用chineseFont
#include <imgui/imgui.h>
#include <pipeline/keybinds.h>
#include <pipeline/settings.h>

void SettingsTAB::Render()
{
    // 在BeginTabItem之前就应用中文字体
    if (Menu::chineseFont) {
        ImGui::PushFont(Menu::chineseFont);
    }

    if (!ImGui::BeginTabItem(u8"设置"))
    {
        // 如果标签项没有开始，也要弹出字体
        if (Menu::chineseFont) {
            ImGui::PopFont();
        }
        return;
    }

    ImGui::Spacing();

    if (ImGui::BeginTable("##settings_layout", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody))
    {
        ImGui::TableNextColumn();
        GuiWidgets::SectionTitle(u8"会话信息", ImVec4(0.55f, 0.85f, 0.8f, 1.0f));
        ImGui::Text(u8"帧率: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text(u8"版本: 2.7");
        ImGui::Text(u8"菜单切换: %s", KeyBinds::ToString(settings.KeyBinds.Toggle_Menu));
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), u8"jadis0x");

        ImGui::TableNextColumn();
        GuiWidgets::SectionTitle(u8"视觉覆盖", ImVec4(1.0f, 0.88f, 0.55f, 1.0f));
        ImGui::Checkbox(u8"后期特效覆盖 (实验性)", &settings.bPostFX_Override);
        GuiWidgets::HelpMarker(
            u8"在下一关卡加载时强制使用自定义后期处理设置。\n"
            u8"非常适合增亮恐怖地图，但无法在游戏中禁用。"
        );
        ImGui::Spacing();
        ImGui::Checkbox(u8"显示Unity日志", &settings.bEnableUnityLogs);
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextWrapped(u8"需要快速隐藏 DevourX？随时按 %s 键切换菜单显示。",
        KeyBinds::ToString(settings.KeyBinds.Toggle_Menu));

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.12f, 0.18f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.72f, 0.18f, 0.23f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.42f, 0.09f, 0.14f, 1.0f));
    if (ImGui::Button(u8"安全卸载并关闭", ImVec2(-1, 0)))
    {
        SetEvent(hUnloadEvent);
    }
    ImGui::PopStyleColor(3);

    // 弹出字体
    if (Menu::chineseFont) {
        ImGui::PopFont();
    }

    ImGui::EndTabItem();
}