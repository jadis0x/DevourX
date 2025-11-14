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

	const auto black = [](float alpha)
		{
			return ImVec4(0.0f, 0.0f, 0.0f, alpha);
		};
	const auto white = [](float alpha)
		{
			return ImVec4(1.0f, 1.0f, 1.0f, alpha);
		};


    colors[ImGuiCol_Text] = white(1.0f);
    colors[ImGuiCol_TextDisabled] = white(0.35f);
    colors[ImGuiCol_WindowBg] = black(0.98f);
    colors[ImGuiCol_ChildBg] = black(0.85f);
    colors[ImGuiCol_PopupBg] = black(0.95f);
    colors[ImGuiCol_MenuBarBg] = black(0.9f);
    colors[ImGuiCol_Border] = white(0.08f);
    colors[ImGuiCol_BorderShadow] = black(0.0f);
    colors[ImGuiCol_FrameBg] = black(0.8f);
    colors[ImGuiCol_FrameBgHovered] = white(0.08f);
    colors[ImGuiCol_FrameBgActive] = white(0.12f);
    colors[ImGuiCol_TitleBg] = black(0.92f);
    colors[ImGuiCol_TitleBgActive] = white(0.1f);
    colors[ImGuiCol_TitleBgCollapsed] = black(0.92f);
    colors[ImGuiCol_Header] = white(0.06f);
    colors[ImGuiCol_HeaderHovered] = white(0.12f);
    colors[ImGuiCol_HeaderActive] = white(0.16f);
    colors[ImGuiCol_Button] = white(0.05f);
    colors[ImGuiCol_ButtonHovered] = white(0.12f);
    colors[ImGuiCol_ButtonActive] = white(0.18f);
    colors[ImGuiCol_CheckMark] = white(1.0f);
    colors[ImGuiCol_SliderGrab] = white(0.9f);
    colors[ImGuiCol_SliderGrabActive] = white(1.0f);
    colors[ImGuiCol_Separator] = white(0.08f);
    colors[ImGuiCol_SeparatorHovered] = white(0.16f);
    colors[ImGuiCol_SeparatorActive] = white(0.24f);
    colors[ImGuiCol_ScrollbarBg] = black(0.6f);
    colors[ImGuiCol_ScrollbarGrab] = white(0.08f);
    colors[ImGuiCol_ScrollbarGrabHovered] = white(0.12f);
    colors[ImGuiCol_ScrollbarGrabActive] = white(0.18f);
    colors[ImGuiCol_ResizeGrip] = white(0.08f);
    colors[ImGuiCol_ResizeGripHovered] = white(0.12f);
    colors[ImGuiCol_ResizeGripActive] = white(0.18f);
    colors[ImGuiCol_Tab] = black(0.88f);
    colors[ImGuiCol_TabHovered] = white(0.12f);
    colors[ImGuiCol_TabActive] = white(0.16f);
    colors[ImGuiCol_TabUnfocused] = black(0.85f);
    colors[ImGuiCol_TabUnfocusedActive] = white(0.1f);
    colors[ImGuiCol_TableHeaderBg] = black(0.9f);
    colors[ImGuiCol_TableBorderStrong] = white(0.06f);
    colors[ImGuiCol_TableBorderLight] = white(0.02f);
    colors[ImGuiCol_TableRowBg] = black(0.6f);
    colors[ImGuiCol_TableRowBgAlt] = black(0.75f);
    colors[ImGuiCol_TextSelectedBg] = white(0.2f);
    colors[ImGuiCol_DragDropTarget] = white(0.9f);
    colors[ImGuiCol_NavHighlight] = white(0.14f);
    colors[ImGuiCol_NavWindowingHighlight] = white(0.18f);
    colors[ImGuiCol_NavWindowingDimBg] = black(0.7f);
    colors[ImGuiCol_ModalWindowDimBg] = black(0.65f);

    colors[ImGuiCol_PlotLines] = white(0.7f);
    colors[ImGuiCol_PlotLinesHovered] = white(0.9f);
    colors[ImGuiCol_PlotHistogram] = white(0.6f);
    colors[ImGuiCol_PlotHistogramHovered] = white(0.8f);

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.4f;

    style.WindowRounding = 6.0f * radiusScale;
    style.ChildRounding = 6.0f * radiusScale;
    style.FrameRounding = 4.0f * radiusScale;
    style.PopupRounding = 6.0f * radiusScale;
    style.ScrollbarRounding = 6.0f * radiusScale;
    style.GrabRounding = 4.0f * radiusScale;
    style.TabRounding = 4.0f * radiusScale;

    style.WindowBorderSize = borderScale * 0.6f;
    style.ChildBorderSize = borderScale * 0.6f;
    style.PopupBorderSize = borderScale * 0.6f;
    style.FrameBorderSize = borderScale * 0.4f;
    style.TabBorderSize = 0.0f;
    style.TabBarBorderSize = 0.0f;
    style.TabBarOverlineSize = scaled(1.0f);

    style.WindowPadding = scaledVec(18.0f, 16.0f);
    style.FramePadding = scaledVec(12.0f, 9.0f);
    style.CellPadding = scaledVec(12.0f, 9.0f);
    style.ItemSpacing = scaledVec(10.0f, 8.0f);
    style.ItemInnerSpacing = scaledVec(8.0f, 5.0f);
    style.IndentSpacing = scaled(18.0f);
    style.ScrollbarSize = scaled(16.0f);
    style.GrabMinSize = scaled(10.0f);

    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
    style.SeparatorTextBorderSize = 0.0f;
    style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
    style.SeparatorTextPadding = scaledVec(20.0f, 10.0f);
    style.WindowMinSize = scaledVec(320.0f, 180.0f);
    style.DisplaySafeAreaPadding = scaledVec(12.0f, 12.0f);
}
