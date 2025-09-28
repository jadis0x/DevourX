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

			ImGui::PushStyleColor(ImGuiCol_Text, kAccentColour);
			ImGui::TextUnformatted("DevourX Control Center");
			ImGui::PopStyleColor();
			const std::string version = "2.0";
			ImGui::Text("Build %s", version.empty() ? "-" : version.c_str());
			ImGui::Text("Toggle Menu: %s", KeyBinds::ToString(settings.KeyBinds.Toggle_Menu));

			ImGui::NextColumn();
			ImGui::TextColored(kAccentColour, "Steam Account");
			ImGui::Text("ID: %llu",
				static_cast<unsigned long long>(Base::Steam::GetUserID()));
			const bool isHost = Base::DevourNet::IsHost();
			ImGui::TextColored(isHost ? kSuccessColour : kWarningColour,
				isHost ? "Hosting Lobby" : "Client");

			ImGui::NextColumn();
			ImGui::TextColored(kAccentColour, "Game State");
			const std::string scene = Base::Gameplay::GetSceneName();
			ImGui::Text("Scene: %s", scene.empty() ? "-" : scene.c_str());
			const float ping = Base::DevourNet::IsHost() ? 0.0f : Base::DevourNet::GetPing();
			if (!Base::DevourNet::IsHost())
			{
				const ImVec4 pingColour = ping < 60.0f ? kSuccessColour : (ping < 110.0f ? kWarningColour : kDangerColour);
				ImGui::TextColored(pingColour, "Ping: %d ms", static_cast<int>(ping));
			}
			else
			{
				ImGui::TextColored(kSuccessColour, "Ping: 0 ms");
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
			GuiWidgets::SectionTitle("Quick Actions", kAccentColour);

			ImGui::Checkbox("Azazel ESP", &settings.bAzazelEsp);
			GuiWidgets::HelpMarker("Highlights the main azazel's position, even through walls.");
			ImGui::Checkbox("Player ESP", &settings.bPlayerEsp);
			GuiWidgets::HelpMarker("Shows every player's outline and distance for rapid coordination.");
			ImGui::Checkbox("Interactable ESP", &settings.bInteractableEsp);
			GuiWidgets::HelpMarker("Draws markers for keys, fuses and other important pick-ups.");

			ImGui::Spacing();
			GuiWidgets::SectionTitle("Environment", kAccentColour);
			ImGui::Checkbox("Fullbright", &settings.bFullbright);
			GuiWidgets::HelpMarker("Removes almost all darkness.");
			ImGui::Checkbox("Unlimited UV Light", &settings.bUnlimitedUVLight);

			ImGui::Spacing();
			GuiWidgets::SectionTitle("Status", kAccentColour);
			ImGui::Text("Auth: %s", settings.bSteamInitialized ? "OK" : "Waiting for Steam");
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

		ImGui::Begin("DevourX by Jadis0x", &settings.bShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

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
				ServerBrowserTAB::Render();
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::Columns(1);

		ImGui::End();
	}
}