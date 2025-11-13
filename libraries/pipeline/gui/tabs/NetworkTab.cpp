#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/NetworkTab.h"
#include "pipeline/settings.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>

// 前向声明 ImFont（如果menu.h中已经包含imgui.h，可以省略这个）
struct ImFont;

// 声明外部字体变量
namespace Menu {
    extern ImFont* chineseFont;
}

void NetworkTAB::Render()
{
    // 在BeginTabItem之前应用中文字体
    if (Menu::chineseFont) {
        ImGui::PushFont(Menu::chineseFont);
    }

    if (!ImGui::BeginTabItem(u8"网络"))
    {
        // 如果标签项没有开始，也要弹出字体
        if (Menu::chineseFont) {
            ImGui::PopFont();
        }
        return;
    }

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

        ImGui::TextColored(color, u8"延迟: %dms", static_cast<int32_t>(ping));
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.1f, 0.85f, 1.0f, 1.0f), u8"房间管理");
    ImGui::Separator();
    ImGui::Spacing();

    // Lobby Manager Box
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.07f, 0.07f, 1.0f));
    ImGui::BeginChild("LobbyBox", ImVec2(0, 215), true);

    ImGui::SetCursorPos(ImVec2(25, 25));

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnWidth(0, 120);

    ImGui::Text(u8"地区");
    ImGui::NextColumn();
    ImGui::SetNextItemWidth(150);
    ImGui::Combo("##Region", &settings.selectedRegion, settings.regions, IM_ARRAYSIZE(settings.regions));

    ImGui::NextColumn();
    ImGui::Text(u8"房间大小");
    ImGui::NextColumn();
    ImGui::SetNextItemWidth(150);
    ImGui::SliderInt("##LobbySize", &settings.lobbySize, 1, 64);

    ImGui::NextColumn();
    ImGui::Text(u8"私人房间");
    ImGui::NextColumn();
    ImGui::Checkbox("##PrivateLobby", &settings.isPrivateLobby);

    ImGui::Columns(1);
    ImGui::Spacing();

    ImGui::SetCursorPosX(12);
    if (ImGui::Button(u8"创建房间", ImVec2(240, 35))) {
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
    ImGui::TextColored(ImVec4(0.1f, 0.85f, 1.0f, 1.0f), u8"房间控制");
    ImGui::Separator();

    ImGui::Checkbox(u8"更改房间名称", &settings.bChangeRoomName);
    ImGui::BeginDisabled(!settings.bChangeRoomName);
    ImGui::InputText(u8"新房间名称", settings.customRoomName, IM_ARRAYSIZE(settings.customRoomName));
    ImGui::EndDisabled();

    //ImGui::Checkbox(u8"隐身模式 (隐身加入)", &settings.bGhostMod);
    //ImGui::TextWrapped(u8"隐身加入。没人能看到你，但你能看到和听到他们！");

    ImGui::Checkbox(u8"强制公开房间", &settings.bForcePublic);
    ImGui::TextWrapped(u8"如果启用，当前的私人房间将被强制设为公开，对所有人可见。");

    ImGui::Checkbox(u8"伪造游戏状态", &settings.bFakeGameInProgress);
    ImGui::TextWrapped(u8"如果启用，当前房间将显示为游戏正在进行中或未进行的状态。");

    ImGui::Spacing();
    ImGui::Spacing();

    // Spoof Steam Name
    ImGui::Checkbox(u8"伪装Steam名称", &settings.bSpoofSteamName);
    if (settings.bSpoofSteamName) {
        ImGui::InputText(u8"自定义Steam名称", settings.customSteamname, IM_ARRAYSIZE(settings.customSteamname));
    }

    ImGui::Separator();

    // Spoof Steam ID
    ImGui::Checkbox(u8"伪装Steam ID", &settings.bSpoofSteamId);
    if (settings.bSpoofSteamId) {
        ImGui::InputText(u8"自定义Steam ID", settings.customSteamIdStr, IM_ARRAYSIZE(settings.customSteamIdStr));

        if (ImGui::Button(u8"生成随机SteamID")) {
            uint64_t randomID = Base::System::GenerateRandomSteamID64();
            snprintf(settings.customSteamIdStr, sizeof(settings.customSteamIdStr), "%llu", randomID);
        }
    }

    // 弹出字体
    if (Menu::chineseFont) {
        ImGui::PopFont();
    }

    ImGui::EndTabItem();
}