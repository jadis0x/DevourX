#include "pch-il2cpp.h"

#include "imgui/imgui.h"
#include "menu.h"

#include "pipeline/gui/tabs/EspTAB.h"
#include "pipeline/gui/tabs/ItemTab.h"
#include "pipeline/gui/tabs/MiscTab.h"
#include "pipeline/gui/tabs/NetworkTab.h"
#include "pipeline/gui/tabs/PlayerTAB.h"
#include "pipeline/gui/tabs/SettingsTAB.h"
#include "pipeline/gui/tabs/MapTab.h"
#include "pipeline/gui/tabs/ServerBrowserTAB.h"
#include "pipeline/settings.h"

namespace Menu {
	bool init = false;
	bool firstRender = true;

	void Init() {
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowSize(ImVec2(890, 720));
		init = true;
	}

	void Render() {
		if (!init)
			Menu::Init();

		ImGui::Begin("DevourX by Jadis0x", &settings.bShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		ImGui::BeginTabBar("LuridLab#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons);


		if (firstRender) {
			firstRender = false;
		}
		else {
			SettingsTAB::Render();
			NetworkTAB::Render();
			EspTAB::Render();
			MiscTAB::Render();
			ItemTAB::Render();
			PlayerTAB::Render();
			MapTAB::Render();
			ServerBrowserTAB::Render();
		}

		ImGui::EndTabBar();
		ImGui::End();
	}
}