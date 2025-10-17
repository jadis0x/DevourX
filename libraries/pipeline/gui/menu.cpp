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
#include "pipeline/localization/LocalizationManager.h"

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

			ImGui::PushStyleColor(ImGuiCol_Text, kAccentColour);
			ImGui::TextUnformatted(Localization::Get("menu.header.title").c_str());
			ImGui::PopStyleColor();
			const std::string version = "2.1";
			const std::string buildLabel = Localization::Format("menu.header.build", version.empty() ? Localization::Get("common.placeholder_dash").c_str() : version.c_str());
			ImGui::TextUnformatted(buildLabel.c_str());
			const std::string toggleLabel = Localization::Format("menu.header.toggle", KeyBinds::ToString(settings.KeyBinds.Toggle_Menu));
			ImGui::TextUnformatted(toggleLabel.c_str());

			ImGui::NextColumn();
			ImGui::TextColored(kAccentColour, Localization::Get("menu.header.steam_account").c_str());
			const std::string steamIdLabel = Localization::Format("menu.header.steam_id",
				static_cast<unsigned long long>(Base::Steam::GetUserID()));
			ImGui::TextUnformatted(steamIdLabel.c_str());
			const bool isHost = Base::DevourNet::IsHost();
			ImGui::TextColored(isHost ? kSuccessColour : kWarningColour,
				Localization::Get(isHost ? "menu.header.host" : "menu.header.client").c_str());

			ImGui::NextColumn();
			ImGui::TextColored(kAccentColour, Localization::Get("menu.header.game_state").c_str());
			const std::string scene = Base::Gameplay::GetSceneName();
			const std::string sceneLabel = Localization::Format("menu.header.scene",
				scene.empty() ? Localization::Get("common.placeholder_dash").c_str() : scene.c_str());
			ImGui::TextUnformatted(sceneLabel.c_str());
			const float ping = Base::DevourNet::IsHost() ? 0.0f : Base::DevourNet::GetPing();
			if (!Base::DevourNet::IsHost())
			{
				const ImVec4 pingColour = ping < 60.0f ? kSuccessColour : (ping < 110.0f ? kWarningColour : kDangerColour);
				ImGui::TextColored(pingColour, Localization::Format("menu.header.ping", static_cast<int>(ping)).c_str());
			}
			else
			{
				ImGui::TextColored(kSuccessColour, Localization::Format("menu.header.ping", 0).c_str());
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
			GuiWidgets::SectionTitle("menu.quick_actions.title", kAccentColour);

			ImGui::Checkbox(Localization::Get("menu.quick_actions.azazel_esp").c_str(), &settings.bAzazelEsp);
			GuiWidgets::HelpMarker("menu.quick_actions.azazel_esp_help");
			ImGui::Checkbox(Localization::Get("menu.quick_actions.player_esp").c_str(), &settings.bPlayerEsp);
			GuiWidgets::HelpMarker("menu.quick_actions.player_esp_help");
			ImGui::Checkbox(Localization::Get("menu.quick_actions.interactable_esp").c_str(), &settings.bInteractableEsp);
			GuiWidgets::HelpMarker("menu.quick_actions.interactable_esp_help");

			ImGui::Spacing();
			GuiWidgets::SectionTitle("menu.environment.title", kAccentColour);
			ImGui::Checkbox(Localization::Get("menu.environment.fullbright").c_str(), &settings.bFullbright);
			GuiWidgets::HelpMarker("menu.environment.fullbright_help");
			ImGui::Checkbox(Localization::Get("menu.environment.unlimited_uv").c_str(), &settings.bUnlimitedUVLight);

			ImGui::Spacing();
			GuiWidgets::SectionTitle("menu.status.title", kAccentColour);
			const std::string& authState = settings.bSteamInitialized ? Localization::Get("menu.status.ok") : Localization::Get("menu.status.waiting");
			ImGui::TextUnformatted(Localization::Format("menu.status.auth", authState.c_str()).c_str());
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}
}

namespace Menu {
	bool init = false;
	bool firstRender = true;

	void Init() {
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowSize(ImVec2(1080, 720));
		init = true;
	}

	void Render() {
		if (!init)
			Menu::Init();

		ImGui::Begin(Localization::Get("menu.window.title").c_str(), &settings.bShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

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
				// ServerBrowserTAB::Render(); marked for removal
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::Columns(1);

		ImGui::End();
	}
}