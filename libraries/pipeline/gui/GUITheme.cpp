#include "pch-il2cpp.h"

#include "pipeline/gui/GUITheme.h"
#include <imgui/imgui.h>
#include <algorithm>

void ApplyTheme()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	float scale = io.DisplayFramebufferScale.y;
	io.FontGlobalScale = std::clamp(scale, 1.1f, 2.0f);

	ImVec4 bgDark = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);   
	ImVec4 bgMid = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);    
	ImVec4 bgLight = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);    
	ImVec4 grayBorder = ImVec4(0.25f, 0.25f, 0.28f, 0.6f); 
	ImVec4 accent = ImVec4(0.25f, 0.25f, 0.35f, 1.0f); 
	ImVec4 accentHover = ImVec4(0.35f, 0.35f, 0.45f, 1.0f);
	ImVec4 accentActive = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);

	ImVec4 whiteText = ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
	ImVec4 grayText = ImVec4(0.55f, 0.55f, 0.60f, 1.0f);
	ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	ImVec4 mintGreen = ImVec4(0.1f, 0.1f, 0.3f, 1.0f);
	ImVec4 mintGreenHover = ImVec4(0.1f, 0.1f, 0.3f, 0.5f);
	ImVec4 mintGreenClick = ImVec4(0.1f, 0.1f, 0.3f, 0.3f);

	// Renk atamaları
	colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	colors[ImGuiCol_TextDisabled] = grayText;
	colors[ImGuiCol_WindowBg] = bgDark;
	colors[ImGuiCol_ChildBg] = bgMid;
	colors[ImGuiCol_PopupBg] = bgMid;

	colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
	colors[ImGuiCol_BorderShadow] = transparent;

	colors[ImGuiCol_FrameBg] = bgMid;
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f); 
	colors[ImGuiCol_FrameBgActive] = accentActive;

	colors[ImGuiCol_Button] = mintGreen;
	colors[ImGuiCol_ButtonHovered] = mintGreenHover;
	colors[ImGuiCol_ButtonActive] = mintGreenClick;

	colors[ImGuiCol_Header] = accent;
	colors[ImGuiCol_HeaderHovered] = accentHover;
	colors[ImGuiCol_HeaderActive] = accentActive;

	colors[ImGuiCol_ScrollbarBg] = bgDark;
	colors[ImGuiCol_ScrollbarGrab] = accent;
	colors[ImGuiCol_ScrollbarGrabHovered] = accentHover;
	colors[ImGuiCol_ScrollbarGrabActive] = accentActive;

	colors[ImGuiCol_CheckMark] = whiteText;
	colors[ImGuiCol_SliderGrab] = ImVec4(0.6f, 0.6f, 1.0f, 1.0f);  
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.9f, 1.0f, 1.0f);

	colors[ImGuiCol_Tab] = bgMid;
	colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.3f, 0.3f, 0.7f);
	colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_TabUnfocused] = bgMid;
	colors[ImGuiCol_TabUnfocusedActive] = accent;

	colors[ImGuiCol_TitleBg] = bgMid;
	colors[ImGuiCol_TitleBgActive] = bgLight;
	colors[ImGuiCol_TitleBgCollapsed] = bgDark;

	colors[ImGuiCol_Separator] = grayBorder;
	colors[ImGuiCol_SeparatorHovered] = accentHover;
	colors[ImGuiCol_SeparatorActive] = accentActive;

	colors[ImGuiCol_ResizeGrip] = accent;
	colors[ImGuiCol_ResizeGripHovered] = accentHover;
	colors[ImGuiCol_ResizeGripActive] = accentActive;

	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

	// style settings
	style.WindowRounding = 9.0f;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 10.0f;

	style.WindowBorderSize = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ChildBorderSize = 0.0f;

	style.WindowPadding = ImVec2(22,22);
	style.FramePadding = ImVec2(12, 9);
	style.ItemSpacing = ImVec2(10, 8);
	style.ItemInnerSpacing = ImVec2(9, 6);
	style.ScrollbarSize = 14.0f;
	style.GrabMinSize = 12.0f;
	style.TabBorderSize = 0.0f;
}
