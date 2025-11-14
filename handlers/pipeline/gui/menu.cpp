
#include "pch-il2cpp.h"
#include "imgui/imgui.h"
#include "menu.h"

#include "pipeline/devour_helper.h"
#include "pipeline/gui/tabs/EspTAB.h"
#include "pipeline/gui/tabs/ItemTab.h"
#include "pipeline/gui/tabs/MapTab.h"
#include "pipeline/gui/tabs/MiscTab.h"
#include "pipeline/gui/tabs/NetworkTab.h"
#include "pipeline/gui/tabs/PlayerTAB.h"
#include "pipeline/gui/tabs/SettingsTAB.h"
#include "pipeline/gui/widgets.h"
#include "pipeline/keybinds.h"
#include "settings.h"
#include "pipeline/localization/LocalizationManager.h"
#include "build_info.h"

#include <string>
#include <limits>

#ifdef max
#undef max
#endif

namespace
{
	constexpr ImVec4 kAccentColour{ 1.0f, 1.0f, 1.0f, 0.95f };
	constexpr ImVec4 kSuccessColour{ 1.0f, 1.0f, 1.0f, 0.85f };
	constexpr ImVec4 kWarningColour{ 1.0f, 1.0f, 1.0f, 0.65f };
	constexpr ImVec4 kDangerColour{ 1.0f, 1.0f, 1.0f, 0.45f };

	bool gOpenLanguagePrompt = false;
	bool gLanguagePromptDontShowAgain = false;
	std::string gLanguagePromptSelection;
	bool gOpenLanguageRestartPopup = false;

	void QueueLanguagePrompt()
	{
		gOpenLanguagePrompt = true;
		gLanguagePromptSelection = settings.localizationCulture.empty()
			? Localization::GetFallbackCulture()
			: settings.localizationCulture;
		gLanguagePromptDontShowAgain = !settings.showLanguagePromptOnStart;
	}

	void PrepareCenteredModal(float minimumWidth = 0.0f)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (minimumWidth > 0.0f)
		{
			const float maxWidth = std::numeric_limits<float>::max();
			ImGui::SetNextWindowSize(ImVec2(minimumWidth, 0.0f), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(minimumWidth, 0.0f), ImVec2(maxWidth, maxWidth));
		}
	}

	void RenderLanguagePrompt()
	{
		const std::string title = Localization::Get("popups.language.change_prompt_title");
		const std::string popupId = title + "##LanguagePrompt";
		if (gOpenLanguagePrompt)
		{
			ImGui::OpenPopup(popupId.c_str());
			gOpenLanguagePrompt = false;
		}

		PrepareCenteredModal(360.0f);

		if (ImGui::BeginPopupModal(popupId.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::TextWrapped("%s", Localization::Get("popups.language.change_prompt_message").c_str());
			ImGui::Spacing();

			const auto cultures = Localization::GetAvailableCultures();
			if (cultures.empty())
			{
				ImGui::TextUnformatted(Localization::Get("common.language_unavailable").c_str());
			}
			else
			{
				std::string previewLabel = Localization::GetDisplayName(gLanguagePromptSelection);
				if (previewLabel.empty())
				{
					previewLabel = gLanguagePromptSelection;
				}

				if (ImGui::BeginCombo("##language_prompt_selection", previewLabel.c_str()))
				{
					for (const auto& culture : cultures)
					{
						const bool isSelected = (culture == gLanguagePromptSelection);
						std::string displayName = Localization::GetDisplayName(culture);
						if (displayName.empty())
						{
							displayName = culture;
						}

						if (ImGui::Selectable(displayName.c_str(), isSelected))
						{
							gLanguagePromptSelection = culture;
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			ImGui::Spacing();
			ImGui::Checkbox(Localization::Get("popups.language.dont_show_again").c_str(), &gLanguagePromptDontShowAgain);
			ImGui::Spacing();

			const std::string saveLabel = Localization::Get("common.save");
			if (ImGui::Button(saveLabel.c_str(), ImVec2(120.0f, 0.0f)))
			{
				const bool languageChanged = (gLanguagePromptSelection != settings.localizationCulture);
				settings.localizationCulture = gLanguagePromptSelection;
				settings.showLanguagePromptOnStart = !gLanguagePromptDontShowAgain;
				SaveSettingsToConfig();

				if (languageChanged)
				{
					settings.queueLanguageRestartNotification = true;
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			const std::string cancelLabel = Localization::Get("common.cancel");
			if (ImGui::Button(cancelLabel.c_str(), ImVec2(120.0f, 0.0f)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void RenderLanguageRestartPopup()
	{
		if (settings.queueLanguageRestartNotification)
		{
			settings.queueLanguageRestartNotification = false;
			gOpenLanguageRestartPopup = true;
		}

		const std::string title = Localization::Get("popups.language.restart_title");
		const std::string popupId = title + "##LanguageRestart";
		if (gOpenLanguageRestartPopup)
		{
			ImGui::OpenPopup(popupId.c_str());
			gOpenLanguageRestartPopup = false;
		}

		PrepareCenteredModal(360.0f);

		if (ImGui::BeginPopupModal(popupId.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::TextWrapped("%s", Localization::Get("popups.language.restart_required").c_str());
			ImGui::Spacing();

			const std::string okLabel = Localization::Get("common.ok");
			if (ImGui::Button(okLabel.c_str(), ImVec2(120.0f, 0.0f)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void RenderHeader()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(18.0f, 14.0f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.92f));
		if (ImGui::BeginChild("##menu_header", ImVec2(0.0f, 96.0f), true,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::Columns(3, nullptr, false);

			ImGui::PushStyleColor(ImGuiCol_Text, kAccentColour);
			ImGui::TextUnformatted(Localization::Get("menu.header.title").c_str());
			ImGui::PopStyleColor();
			const std::string version = BuildInfo::kVersion;
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
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.88f));
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
		ImGui::SetNextWindowSize(ImVec2(1140, 820));
		init = true;
	}

	void Render() {
		if (!init)
			Menu::Init();

		const std::string windowLabel = Localization::Get("menu.window.title");
		const std::string windowId = windowLabel + "##menu_window";
		ImGui::Begin(windowId.c_str(), &settings.bShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		if (firstRender)
		{
			firstRender = false;

			if (settings.showLanguagePromptOnStart)
			{
				QueueLanguagePrompt();
			}
		}

		RenderHeader();
		ImGui::Spacing();

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 260.0f);
		RenderQuickActions();
		ImGui::NextColumn();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.9f));
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

		RenderLanguagePrompt();
		RenderLanguageRestartPopup();

		ImGui::End();
	}
}