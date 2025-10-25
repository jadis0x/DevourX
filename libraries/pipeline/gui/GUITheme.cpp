#include "pch-il2cpp.h"

#include "pipeline/gui/GUITheme.h"
#include <imgui/imgui.h>
#include <algorithm>

void ApplyTheme()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	float framebufferScale = std::max(io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	if (framebufferScale <= 0.0f)
	{
		framebufferScale = 1.0f;
	}

	const float targetScale = std::clamp(framebufferScale, 1.0f, 2.5f);
	io.FontGlobalScale = targetScale;

	const float layoutScale = targetScale;
	const float radiusScale = std::clamp(layoutScale, 1.0f, 1.6f);
	const float borderScale = std::clamp(layoutScale, 1.0f, 2.0f);

	const auto scaled = [layoutScale](float value)
		{
			return value * layoutScale;
		};

	const auto scaledVec = [layoutScale](float x, float y)
		{
			return ImVec2(x * layoutScale, y * layoutScale);
		};


	const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	const ImVec4 canvas = ImVec4(0.04f, 0.04f, 0.05f, 0.98f);
	const ImVec4 surface = ImVec4(0.06f, 0.06f, 0.08f, 1.0f);
	const ImVec4 surfaceHover = ImVec4(0.10f, 0.09f, 0.12f, 1.0f);
	const ImVec4 surfaceActive = ImVec4(0.13f, 0.12f, 0.16f, 1.0f);
	const ImVec4 panel = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);

	const ImVec4 accentPrimary = ImVec4(0.82f, 0.21f, 0.48f, 1.0f);
	const ImVec4 accentPrimaryHover = ImVec4(0.88f, 0.29f, 0.56f, 1.0f);
	const ImVec4 accentPrimaryActive = ImVec4(0.70f, 0.16f, 0.40f, 1.0f);
	const ImVec4 accentSecondary = ImVec4(0.35f, 0.16f, 0.28f, 1.0f);
	const ImVec4 accentSecondaryHover = ImVec4(0.43f, 0.20f, 0.34f, 1.0f);
	const ImVec4 accentSecondaryActive = ImVec4(0.28f, 0.12f, 0.22f, 1.0f);

	const ImVec4 textPrimary = ImVec4(0.93f, 0.90f, 0.94f, 1.0f);
	const ImVec4 textMuted = ImVec4(0.60f, 0.56f, 0.62f, 1.0f);

	const ImVec4 warning = ImVec4(0.96f, 0.73f, 0.33f, 1.0f);
	const ImVec4 danger = ImVec4(0.92f, 0.28f, 0.46f, 1.0f);
	const ImVec4 success = ImVec4(0.34f, 0.62f, 0.42f, 1.0f);

	const ImVec4 border = ImVec4(0.17f, 0.12f, 0.18f, 1.0f);
	const ImVec4 borderStrong = ImVec4(0.10f, 0.08f, 0.12f, 1.0f);

	colors[ImGuiCol_Text] = textPrimary;
	colors[ImGuiCol_TextDisabled] = textMuted;
	colors[ImGuiCol_WindowBg] = canvas;
	colors[ImGuiCol_ChildBg] = panel;
	colors[ImGuiCol_PopupBg] = ImVec4(surface.x, surface.y, surface.z, 0.98f);
	colors[ImGuiCol_MenuBarBg] = surface;
	colors[ImGuiCol_Border] = border;
	colors[ImGuiCol_BorderShadow] = transparent;
	colors[ImGuiCol_FrameBg] = surface;
	colors[ImGuiCol_FrameBgHovered] = surfaceHover;
	colors[ImGuiCol_FrameBgActive] = surfaceActive;
	colors[ImGuiCol_TitleBg] = surface;
	colors[ImGuiCol_TitleBgActive] = accentSecondaryHover;
	colors[ImGuiCol_TitleBgCollapsed] = surface;
	colors[ImGuiCol_Header] = surfaceHover;
	colors[ImGuiCol_HeaderHovered] = accentPrimaryHover;
	colors[ImGuiCol_HeaderActive] = accentPrimaryActive;
	colors[ImGuiCol_Button] = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.85f);
	colors[ImGuiCol_ButtonHovered] = accentPrimaryHover;
	colors[ImGuiCol_ButtonActive] = accentPrimaryActive;
	colors[ImGuiCol_CheckMark] = accentPrimary;
	colors[ImGuiCol_SliderGrab] = accentPrimary;
	colors[ImGuiCol_SliderGrabActive] = accentPrimaryHover;
	colors[ImGuiCol_Separator] = border;
	colors[ImGuiCol_SeparatorHovered] = accentPrimaryHover;
	colors[ImGuiCol_SeparatorActive] = accentPrimaryActive;
	colors[ImGuiCol_ScrollbarBg] = ImVec4(panel.x, panel.y, panel.z, 0.6f);
	colors[ImGuiCol_ScrollbarGrab] = accentSecondary;
	colors[ImGuiCol_ScrollbarGrabHovered] = accentSecondaryHover;
	colors[ImGuiCol_ScrollbarGrabActive] = accentSecondaryActive;
	colors[ImGuiCol_ResizeGrip] = accentSecondary;
	colors[ImGuiCol_ResizeGripHovered] = accentPrimaryHover;
	colors[ImGuiCol_ResizeGripActive] = accentPrimaryActive;
	colors[ImGuiCol_Tab] = ImVec4(surfaceHover.x, surfaceHover.y, surfaceHover.z, 0.90f);
	colors[ImGuiCol_TabHovered] = accentPrimaryHover;
	colors[ImGuiCol_TabActive] = accentPrimary;
	colors[ImGuiCol_TabUnfocused] = ImVec4(surface.x, surface.y, surface.z, 0.90f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.70f);
	colors[ImGuiCol_TableHeaderBg] = surface;
	colors[ImGuiCol_TableBorderStrong] = borderStrong;
	colors[ImGuiCol_TableBorderLight] = ImVec4(border.x, border.y, border.z, 0.55f);
	colors[ImGuiCol_TableRowBg] = ImVec4(panel.x, panel.y, panel.z, 0.52f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(panel.x, panel.y, panel.z, 0.75f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(success.x, success.y, success.z, 0.90f);
	colors[ImGuiCol_NavHighlight] = accentPrimaryHover;
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(accentPrimaryHover.x, accentPrimaryHover.y, accentPrimaryHover.z, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(canvas.x, canvas.y, canvas.z, 0.60f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.03f, 0.03f, 0.04f, 0.70f);

	colors[ImGuiCol_PlotLines] = accentSecondary;
	colors[ImGuiCol_PlotLinesHovered] = accentPrimary;
	colors[ImGuiCol_PlotHistogram] = warning;
	colors[ImGuiCol_PlotHistogramHovered] = danger;

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.45f;

	style.WindowRounding = 12.0f * radiusScale;
	style.ChildRounding = 10.0f * radiusScale;
	style.FrameRounding = 8.0f * radiusScale;
	style.PopupRounding = 10.0f * radiusScale;
	style.ScrollbarRounding = 12.0f * radiusScale;
	style.GrabRounding = 6.0f * radiusScale;
	style.TabRounding = 8.0f * radiusScale;

	style.WindowBorderSize = borderScale;
	style.ChildBorderSize = borderScale;
	style.PopupBorderSize = borderScale;
	style.FrameBorderSize = borderScale;
	style.TabBorderSize = 0.0f;
	style.TabBarBorderSize = 0.0f;
	style.TabBarOverlineSize = scaled(1.0f);

	style.WindowPadding = scaledVec(20.0f, 18.0f);
	style.FramePadding = scaledVec(14.0f, 10.0f);
	style.CellPadding = scaledVec(14.0f, 10.0f);
	style.ItemSpacing = scaledVec(12.0f, 10.0f);
	style.ItemInnerSpacing = scaledVec(10.0f, 6.0f);
	style.IndentSpacing = scaled(20.0f);
	style.ScrollbarSize = scaled(18.0f);
	style.GrabMinSize = scaled(12.0f);

	style.WindowTitleAlign = ImVec2(0.02f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
	style.SeparatorTextBorderSize = 0.0f;
	style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
	style.SeparatorTextPadding = scaledVec(24.0f, 12.0f);
	style.WindowMinSize = scaledVec(320.0f, 180.0f);
	style.DisplaySafeAreaPadding = scaledVec(12.0f, 12.0f);
}
