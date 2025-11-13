#include "pch-il2cpp.h"
#include "pipeline/gui/tabs/MapTab.h"
#include "pipeline/settings.h"
#include <devour/devourbase.h>
#include <imgui/imgui.h>
#include <helpers.h>
#include "player/player.h"
#include <iostream>
#include <vector>
#include <algorithm>  // 添加这个头文件，包含 std::sort
// 前向声明 ImFont
struct ImFont;

// 声明外部字体变量
namespace Menu {
    extern ImFont* chineseFont;
}

namespace MapTAB {

    // 重命名相关静态变量
    static int renamingIndex = -1;
    static char renameBuffer[64] = "";
    static std::vector<int> itemsToDelete; // 用于延迟删除

    void LoadMap(const std::string& mapName) {
        if (Base::DevourNet::IsHost()) {
            try {
                app::BoltNetwork_LoadScene(convert_to_system_string(mapName.c_str()), nullptr);
            }
            catch (...) {
                try {
                    app::SceneManager_LoadScene(convert_to_system_string(mapName.c_str()), nullptr);
                }
                catch (...) {
                    std::cout << "[Map] 无法加载地图，请检查函数名是否正确" << std::endl;
                    return;
                }
            }

            std::cout << "[Map] 正在加载地图: " << mapName << std::endl;
            std::cout << "[Map] 请只按一次按钮，地图加载可能需要一些时间。" << std::endl;
        }
        else {
            std::cout << "[Map] 你必须是大厅主机才能切换地图！" << std::endl;
        }
    }

    // 保存当前位置的函数
    void SaveCurrentPosition() {
        // 使用 LocalPlayerManager 获取本地玩家
        app::NolanBehaviour* nolan = LocalPlayer::LocalPlayerManager::Instance().GetNolan();
        if (!nolan) {
            std::cout << "[Teleport] 错误: 找不到本地玩家 NolanBehaviour" << std::endl;
            return;
        }

        app::Transform* playerTransform = app::Component_get_transform((app::Component*)nolan, nullptr);
        if (!playerTransform) {
            std::cout << "[Teleport] 错误: 找不到玩家变换组件" << std::endl;
            return;
        }

        app::Vector3 currentPosition = app::Transform_get_position(playerTransform, nullptr);

        // 创建新的传送点
        CustomTeleportLocation newLocation;
        newLocation.position = currentPosition;
        newLocation.isValid = true;

        // 生成名称 (位置1, 位置2, ...)
        char nameBuffer[64];
        snprintf(nameBuffer, sizeof(nameBuffer), u8"位置%d", (int)settings.customTeleportLocations.size() + 1);
        newLocation.name = nameBuffer;

        settings.customTeleportLocations.push_back(newLocation);

        std::cout << "[Teleport] 已保存位置: " << newLocation.name << " ("
            << currentPosition.x << ", " << currentPosition.y << ", " << currentPosition.z << ")" << std::endl;
        // 自动保存到文件
        settings.SaveTeleportLocations();  // 添加这行
    }


    // 处理延迟删除
    void ProcessDeferredDeletions() {
        if (!itemsToDelete.empty()) {
            // 从大到小排序，这样从后往前删除不会影响索引
            std::sort(itemsToDelete.begin(), itemsToDelete.end(), std::greater<int>());

            for (int index : itemsToDelete) {
                if (index >= 0 && index < settings.customTeleportLocations.size()) {
                    std::cout << "[Teleport] 删除位置: " << settings.customTeleportLocations[index].name << std::endl;
                    settings.customTeleportLocations.erase(settings.customTeleportLocations.begin() + index);

                    // 如果删除的是选中的位置，重置选中索引
                    if (settings.selectedTeleportIndex == index) {
                        settings.selectedTeleportIndex = -1;
                    }
                    // 如果删除的位置在选中位置之前，调整选中索引
                    else if (settings.selectedTeleportIndex > index) {
                        settings.selectedTeleportIndex--;
                    }
                }
            }
            itemsToDelete.clear();
            // 自动保存到文件
            settings.SaveTeleportLocations();  // 添加这行
        }
    }

    void Render()
    {
        // 处理延迟删除
        ProcessDeferredDeletions();

        // 在BeginTabItem之前应用中文字体
        if (Menu::chineseFont) {
            ImGui::PushFont(Menu::chineseFont);
        }

        if (ImGui::BeginTabItem(u8"地图管理"))
        {
            ImGui::Checkbox(u8"[大厅] 动态鼠标跟随", &settings.bMouseFollower);

            if (settings.bMouseFollower) {
                ImGui::SliderFloat(u8"X轴偏移", &settings.MF_offsetX, -5.0f, -50.0f, "%.1f");
                ImGui::SliderFloat(u8"Y轴偏移", &settings.MF_offsetY, 5.0f, 50.0f, "%.1f");
                ImGui::SliderFloat(u8"平滑速度", &settings.MF_Speed, 1.f, 10.0f, "%.1f");
            }

            ImGui::Spacing();
            ImGui::Separator();

            // === 切换地图功能 ===
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), u8"切换地图 (仅主机):");

            // 第一行地图按钮
            if (ImGui::Button(u8"农舍")) {
                LoadMap("Devour");
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"精神病院")) {
                LoadMap("Molly");
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"居酒屋")) {
                LoadMap("Inn");
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"小镇")) {
                LoadMap("Town");
            }

            // 第二行地图按钮
            if (ImGui::Button(u8"屠宰场")) {
                LoadMap("Slaughterhouse");
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"庄园")) {
                LoadMap("Manor");
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"嘉年华")) {
                LoadMap("Carnival");
            }

            ImGui::Spacing();
            ImGui::Separator();
            // === 切换地图功能结束 ===

            ImGui::Spacing();

            if (ImGui::Button(u8"[强制] 大厅开始")) {
                Base::Gameplay::ForceLobbyStart();
            }

            if (ImGui::Button(u8"[强制] 设置进度至:")) {
                Base::Gameplay::SetProgressTo(settings.progress);
            }
            ImGui::SliderInt(u8"地图进度", &settings.progress, 0, 10);

            ImGui::Spacing();
            ImGui::Separator();

            // === 自定义传送功能 ===
            ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.2f, 1.0f), u8"自定义传送点管理");

            // 保存当前位置
            if (ImGui::Button(u8"保存当前位置")) {
                SaveCurrentPosition();
            }
            ImGui::SameLine();
            ImGui::Text(u8"将当前玩家位置保存为传送点");

            ImGui::Spacing();

            // 传送点列表
            if (!settings.customTeleportLocations.empty()) {
                ImGui::Text(u8"已保存的传送点:");

                // 显示传送点列表 - 简化版，只显示选择按钮
                for (int i = 0; i < settings.customTeleportLocations.size(); i++) {
                    auto& location = settings.customTeleportLocations[i];
                    if (location.isValid) {
                        ImGui::PushID(i);

                        // 只显示选择按钮，不显示操作按钮
                        bool isSelected = (settings.selectedTeleportIndex == i);
                        if (ImGui::Selectable(location.name.c_str(), isSelected)) {
                            settings.selectedTeleportIndex = i;
                        }

                        ImGui::PopID();
                    }
                }

                ImGui::Spacing();

                // 选中的传送点操作区域
                if (settings.selectedTeleportIndex >= 0 &&
                    settings.selectedTeleportIndex < settings.customTeleportLocations.size()) {

                    auto& selectedLocation = settings.customTeleportLocations[settings.selectedTeleportIndex];

                    // 显示选中位置信息
                    ImGui::Text(u8"选中位置: %s", selectedLocation.name.c_str());

                    // 操作按钮行
                    if (ImGui::Button(u8"传送到选中位置")) {
                        settings.bTriggerCustomTeleport = true;
                    }
                    ImGui::SameLine();

                    // 重命名按钮
                    if (renamingIndex == settings.selectedTeleportIndex) {
                        // 重命名模式
                        ImGui::InputText("##rename", renameBuffer, sizeof(renameBuffer));
                        ImGui::SameLine();
                        if (ImGui::Button(u8"确认")) {
                            selectedLocation.name = renameBuffer;
                            renamingIndex = -1;
                            renameBuffer[0] = '\0';
                            // 自动保存到文件
                            settings.SaveTeleportLocations();  // 添加这行
                        }
                        ImGui::SameLine();
                        if (ImGui::Button(u8"取消")) {
                            renamingIndex = -1;
                            renameBuffer[0] = '\0';
                        }
                    }
                    else {
                        // 正常模式 - 重命名按钮
                        if (ImGui::Button(u8"重命名")) {
                            renamingIndex = settings.selectedTeleportIndex;
                            snprintf(renameBuffer, sizeof(renameBuffer), "%s", selectedLocation.name.c_str());
                        }
                        ImGui::SameLine();
                    }

                    // 删除按钮
                    if (ImGui::Button(u8"删除")) {
                        itemsToDelete.push_back(settings.selectedTeleportIndex);
                    }
                }

                // 清空所有传送点按钮
                ImGui::Spacing();
                if (ImGui::Button(u8"清空所有传送点")) {
                    settings.customTeleportLocations.clear();
                    settings.selectedTeleportIndex = -1;
                    std::cout << "[Teleport] 已清空所有传送点" << std::endl;
                    // 自动保存到文件
                    settings.SaveTeleportLocations();  // 添加这行
                }
                ImGui::SameLine();
                ImGui::Text(u8"当前有 %d 个传送点", (int)settings.customTeleportLocations.size());
            }
            else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), u8"没有保存的传送点");
                ImGui::Text(u8"点击'保存当前位置'来创建第一个传送点");
            }
            // === 自定义传送功能结束 ===
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
}