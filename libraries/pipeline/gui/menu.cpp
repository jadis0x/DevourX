#include "pch-il2cpp.h"

#include "imgui/imgui.h"
#include "menu.h"

#include "devour/devourbase.h"
#include "pipeline/gui/tabs/EspTAB.h"
#include "pipeline/gui/tabs/ItemTab.h"
#include "pipeline/gui/tabs/MapTab.h"
#include "pipeline/gui/tabs/MiscTab.h"
#include "pipeline/gui/tabs/NetworkTab.h"
#include "pipeline/gui/tabs/PlayerTAB.h"
#include "pipeline/gui/tabs/ServerBrowserTAB.h"
#include "pipeline/gui/tabs/SettingsTAB.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/keybinds.h"
#include "pipeline/settings.h"

#include <string>

namespace
{
	constexpr ImVec4 kAccentColour{ 0.55f, 0.82f, 1.0f, 1.0f };
	constexpr ImVec4 kSuccessColour{ 0.32f, 0.85f, 0.55f, 1.0f };
	constexpr ImVec4 kWarningColour{ 0.95f, 0.78f, 0.28f, 1.0f };
	constexpr ImVec4 kDangerColour{ 0.95f, 0.39f, 0.39f, 1.0f };

	void RenderHeader()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(18.0f, 14.0f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.08f, 0.95f));
		if (ImGui::BeginChild("##menu_header", ImVec2(0.0f, 96.0f), true,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::Columns(3, nullptr, false);

			// 使用加载的字体（如果有）
			if (Menu::chineseFont) {
				ImGui::PushFont(Menu::chineseFont);
			}

			ImGui::PushStyleColor(ImGuiCol_Text, kAccentColour);
			ImGui::TextUnformatted(u8"jadis0x");
			ImGui::PopStyleColor();
			const std::string version = "2.7";
			ImGui::Text(u8"版本 %s", version.empty() ? "-" : version.c_str());
			ImGui::Text(u8"切换菜单: %s", KeyBinds::ToString(settings.KeyBinds.Toggle_Menu));

			ImGui::NextColumn();
			ImGui::TextColored(kAccentColour, u8"Steam账户");
			ImGui::Text("ID: %llu",
				static_cast<unsigned long long>(Base::Steam::GetUserID()));
			const bool isHost = Base::DevourNet::IsHost();
			ImGui::TextColored(isHost ? kSuccessColour : kWarningColour,
				isHost ? u8"房主" : u8"客户端");

			ImGui::NextColumn();
			ImGui::TextColored(kAccentColour, u8"游戏状态");
			const std::string scene = Base::Gameplay::GetSceneName();
			ImGui::Text(u8"场景: %s", scene.empty() ? "-" : scene.c_str());
			const float ping = Base::DevourNet::IsHost() ? 0.0f : Base::DevourNet::GetPing();
			if (!Base::DevourNet::IsHost())
			{
				const ImVec4 pingColour = ping < 60.0f ? kSuccessColour : (ping < 110.0f ? kWarningColour : kDangerColour);
				ImGui::TextColored(pingColour, u8"延迟: %d ms", static_cast<int>(ping));
			}
			else
			{
				ImGui::TextColored(kSuccessColour, u8"延迟: 0 ms");
			}

			if (Menu::chineseFont) {
				ImGui::PopFont();
			}

			ImGui::Columns(1);
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

	void RenderQuickActions()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.06f, 0.1f, 0.9f));
		if (ImGui::BeginChild("##quick_actions", ImVec2(0.0f, 0.0f), true,
			ImGuiWindowFlags_NoScrollbar))
		{
			// 应用字体到包含中文的部分
			if (Menu::chineseFont) {
				ImGui::PushFont(Menu::chineseFont);
			}
			GuiWidgets::SectionTitle(u8"快速操作", kAccentColour);

			ImGui::Checkbox(u8"恶魔透视", &settings.bAzazelEsp);
			GuiWidgets::HelpMarker(u8"高亮显示恶魔位置，即使隔墙也能看到");
			ImGui::Checkbox(u8"小鬼透视", &settings.bSimpleDemonEsp);
			GuiWidgets::HelpMarker(u8"高亮显示小鬼和仪式物品位置，即使隔墙也能看到");
			ImGui::Checkbox(u8"玩家透视", &settings.bPlayerEsp);
			GuiWidgets::HelpMarker(u8"显示所有玩家轮廓和距离，便于快速协调");
			ImGui::Checkbox(u8"可交互物品透视", &settings.bInteractableEsp);
			GuiWidgets::HelpMarker(u8"为钥匙、保险丝等重要物品显示标记");
			ImGui::Spacing();
			GuiWidgets::SectionTitle(u8"环境设置", kAccentColour);
			// 新增滤镜功能
			ImGui::Checkbox(u8"亮度增强滤镜", &settings.bBrightnessFilter);
			GuiWidgets::HelpMarker(u8"大幅提升游戏整体亮度，让黑暗区域更清晰可见");

			if (settings.bBrightnessFilter) {
				ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), u8"基础调整");
				ImGui::SliderFloat(u8"对比度", &settings.contrast, -50.0f, 30.0f, "%.1f");
				ImGui::SliderFloat(u8"伽马值", &settings.gamma, 0.5f, 10.0f, "%.1f");

				ImGui::Spacing();

				// 核心色彩调整
				ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.8f, 1.0f), u8"色彩调整");
				ImGui::SliderFloat(u8"饱和度", &settings.saturation, 0.0f, 50.0f, "%.1f");
				ImGui::SliderFloat(u8"色温", &settings.temperature, -10.0f, 10.0f, "%.2f");
				// 保存功能和预设
				ImGui::Text(u8"配置管理:");
				ImGui::SameLine();

				static bool saved = false;
				static float saveTimer = 0.0f;

				if (ImGui::Button(u8"保存")) {
					settings.SaveFilterSettings();
					saved = true;
					saveTimer = 2.0f; // 显示3秒成功提示
				}
				// 显示保存成功提示
				if (saved) {
					ImGui::SameLine();
					if (saveTimer > 0) {
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"保存成功，重启后自动加载");
						saveTimer -= ImGui::GetIO().DeltaTime;
					}
					else {
						saved = false;
					}
				}
			}

			ImGui::Checkbox(u8"全亮模式", &settings.bFullbright);
			GuiWidgets::HelpMarker(u8"移除几乎所有黑暗");
			ImGui::Checkbox(u8"无限紫外线光", &settings.bUnlimitedUVLight);

			ImGui::Spacing();
			GuiWidgets::SectionTitle(u8"状态", kAccentColour);
			ImGui::Text(u8"认证: %s", settings.bSteamInitialized ? "OK" : u8"等待 Steam");
			// 添加这行：弹出字体
			if (Menu::chineseFont) {
				ImGui::PopFont();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}
}

namespace Menu {
	bool init = false;
	bool firstRender = true;
	ImFont* chineseFont = nullptr;  // 初始化字体指针

	void Init() {
		// 确保在初始化时加载字体（只执行一次）
		ImGuiIO& io = ImGui::GetIO();

		// 加载中文字体
		chineseFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simhei.ttf",
			18.0f,
			nullptr,
			io.Fonts->GetGlyphRangesChineseFull());

		// 如果加载失败，使用默认字体
		if (!chineseFont) {
			chineseFont = io.Fonts->AddFontDefault();
		}

		// 重建字体纹理（重要！）
		io.Fonts->Build();
		// 自动加载滤镜设置
		settings.LoadFilterSettings();
		// 自动加载传送点设置
		settings.LoadTeleportLocations();
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowSize(ImVec2(1080, 720));
		init = true;
	}

	void Render() {
		if (!init)
			Menu::Init();

		ImGui::Begin("jadis0x", &settings.bShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		if (firstRender)
		{
			firstRender = false;
		}

		RenderHeader();
		ImGui::Spacing();

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 260.0f);
		RenderQuickActions();
		ImGui::NextColumn();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.04f, 0.04f, 0.06f, 0.95f));
		if (ImGui::BeginChild("##menu_content", ImVec2(0.0f, 0.0f), true))
		{


			if (ImGui::BeginTabBar("LuridLab#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons))
			{
				SettingsTAB::Render();
				NetworkTAB::Render();
				EspTAB::Render();
				MiscTAB::Render();
				ItemTAB::Render();
				PlayerTAB::Render();
				MapTAB::Render();
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::Columns(1);

		ImGui::End();
	}
}