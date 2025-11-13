#include "pch-il2cpp.h"

#include "devour/devourbase.h"
#include "pipeline/gui/tabs/ServerBrowserTAB.h"
#include <helpers.h>
#include <imgui/imgui.h>
#include <iostream>
#include <resolver/il2cpp_resolver.h>

// 前向声明 ImFont
struct ImFont;

// 声明外部字体变量
namespace Menu {
    extern ImFont* chineseFont;
}

std::vector<ServerEntry> g_ServerList;

void ServerBrowserTAB::Render() {
    // 在BeginTabItem之前应用中文字体
    if (Menu::chineseFont) {
        ImGui::PushFont(Menu::chineseFont);
    }

    if (ImGui::BeginTabItem(u8"服务器浏览器"))
    {
        float btnWidth = 200.f;
        float btnHeight = 45.f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - btnWidth);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.05f, 0.05f, 1.0f));     // Base
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.1f, 0.1f, 1.0f)); // Hover
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.05f, 0.05f, 1.0f));
        if (ImGui::Button(u8"卡在加载中？修复它", ImVec2(btnWidth, btnHeight))) {
            if (Base::GlobalVar::__browser) {
                auto* _menu = Base::GlobalVar::__browser->fields.menuController;
                if (_menu) {
                    app::Menu_SetLoading(_menu, false, nullptr);
                    app::BoltNetwork_LoadScene(convert_to_system_string("Devour"), nullptr);
                }
            }
        }

        ImGui::PopStyleColor(3);

        static ImGuiTextFilter filter;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        ImGui::PushItemWidth(-1);
        filter.Draw(u8" 搜索服务器...", 320);
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Spacing();

        bool foundAny = false;
        for (size_t i = 0; i < g_ServerList.size(); ++i) {
            const auto& entry = g_ServerList[i];
            if (!filter.PassFilter(entry.name.c_str()))
                continue;

            foundAny = true;
            ImGui::PushID(static_cast<int>(i));


            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(18, 18, 20, 255));
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(30, 30, 40, 150)); // Gölge gibi

            ImGui::BeginChild("card", ImVec2(-1, 145), true,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::AlignTextToFramePadding();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
            ImGui::TextColored(ImVec4(1, 1, 1, 0.95f), entry.name.empty() ? u8"*<未知>" : entry.name.c_str());
            ImGui::TextDisabled(u8"会话ID:");
            ImGui::SameLine();
            ImGui::TextWrapped(entry.udpSessionId.c_str());

            ImGui::PopFont();

            float btnWidth = 120;
            float btnHeight = 35;
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.47f, 0.84f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.55f, 0.95f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.40f, 0.75f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f,
                1.0f));

            if (ImGui::Button(u8"加入", ImVec2(btnWidth, btnHeight))) {

                if (!entry.udpSessionId.empty() && Base::GlobalVar::__browser &&
                    Base::GlobalVar::__browser->fields.menuController) {
                    SafePtr::safe_call([&]() {
                        auto* invite_key = convert_to_system_string("password");

                        app::CSteamID lobbyId{};
                        lobbyId.m_SteamID = entry.steamLobbyId;

                        Il2CppDomain* domain = il2cpp_domain_get();

                        const Il2CppAssembly* asmCS = il2cpp_domain_assembly_open(domain, "Assembly-CSharp.dll");
                        const Il2CppImage* img = il2cpp_assembly_get_image(asmCS);

                        Il2CppClass* klass = il2cpp_class_from_name(img, "Horror", "ServerConnectToken");

                        Il2CppObject* obj = il2cpp_object_new(klass);
                        app::ServerConnectToken* connect_token = reinterpret_cast<app::ServerConnectToken*>(obj);

                        connect_token->fields.isFromReconnect = true;
                        connect_token->fields.username = nullptr;
                        connect_token->fields.playerId = nullptr;
                        connect_token->fields.dissonanceId = nullptr;
                        connect_token->fields.survivalPrefabIdPreference = nullptr;

                        app::ServerConnectToken__ctor(connect_token, nullptr);

                        if (auto* convertToToken = reinterpret_cast<app::IProtocolToken*>(connect_token)) {
                            Base::GlobalVar::__browser->fields.menuController->fields.steamLobbyID = lobbyId;

                            app::SteamMatchmaking_JoinLobby(lobbyId, nullptr);

                            std::cout << "(server "
                                "browser) lobby "
                                "id: "
                                << lobbyId.m_SteamID << "\n";
                        }
                        });
                }
            }

            ImGui::PopStyleColor(4);
            ImGui::EndChild();
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(2);
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::PopID();
        }

        if (!foundAny) {
            ImGui::Spacing();

            ImGui::TextDisabled(u8"没有可显示的服务器。在主菜单中点击'加入游戏'来填充此列表。");
            ImGui::Spacing();
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

void ServerBrowserTAB::CollectServers(app::Map_2_System_Guid_UdpSession_* node) {
    if (!node)
        return;

    CollectServers(node->fields._left);

    auto session = node->fields._value;
    if (session) {
        ServerEntry serverEntry{};
        auto photonSession = reinterpret_cast<app::PhotonSession*>(session);
        auto token = reinterpret_cast<app::PhotonRoomProperties*>(app::BoltUtils_GetProtocolToken(session, nullptr));

        if (!photonSession || !token || !token->klass) {
            CollectServers(node->fields._right);
            return;
        }

        auto props = app::PhotonSession_get_Properties(photonSession, nullptr);
        if (!props || !props->klass || !props->fields._._entries) {
            CollectServers(node->fields._right);
            return;
        }

        serverEntry.session = session;
        serverEntry.photonSession = photonSession;

        if (auto roomToken = reinterpret_cast<app::RoomProtocolToken*>(token)) {
            serverEntry.roomToken = roomToken;
            serverEntry.steamLobbyId = roomToken->fields.steamLobbyId;
        }

        if (!serverEntry.name.empty() && !serverEntry.udpSessionId.empty())
            g_ServerList.push_back(serverEntry);
    }

    CollectServers(node->fields._right);
}