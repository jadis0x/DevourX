#include "pch-il2cpp.h"

#include "pipeline/gui/GUITheme.h"
#include <imgui/imgui.h>
#include <algorithm>

void ApplyTheme()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    float scale = std::max(io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    if (scale <= 0.0f)
        scale = 1.0f;
    io.FontGlobalScale = std::clamp(scale, 1.0f, 2.5f);

    const ImVec4 bgWindow = ImVec4(0.04f, 0.04f, 0.06f, 0.98f);
    const ImVec4 bgPanel = ImVec4(0.07f, 0.06f, 0.10f, 1.0f);
    const ImVec4 bgSurface = ImVec4(0.10f, 0.08f, 0.15f, 1.0f);
    const ImVec4 bgHighlight = ImVec4(0.18f, 0.12f, 0.25f, 1.0f);
    const ImVec4 bgActive = ImVec4(0.25f, 0.15f, 0.35f, 1.0f);

    const ImVec4 borderSoft = ImVec4(0.28f, 0.22f, 0.36f, 0.55f);
    const ImVec4 borderHard = ImVec4(0.45f, 0.30f, 0.55f, 0.85f);
    const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    const ImVec4 textBright = ImVec4(0.92f, 0.90f, 0.97f, 1.0f);
    const ImVec4 textMuted = ImVec4(0.60f, 0.55f, 0.68f, 1.0f);

    const ImVec4 accentPrimary = ImVec4(0.55f, 0.25f, 0.85f, 1.0f);
    const ImVec4 accentPrimaryHover = ImVec4(0.65f, 0.35f, 0.95f, 1.0f);
    const ImVec4 accentPrimaryActive = ImVec4(0.45f, 0.20f, 0.75f, 1.0f);

    const ImVec4 accentSecondary = ImVec4(0.35f, 0.20f, 0.55f, 1.0f);
    const ImVec4 accentSecondaryHover = ImVec4(0.45f, 0.25f, 0.70f, 1.0f);
    const ImVec4 accentSecondaryActive = ImVec4(0.30f, 0.15f, 0.50f, 1.0f);

    const ImVec4 accentWarning = ImVec4(0.85f, 0.70f, 0.25f, 1.0f);
    const ImVec4 accentDanger = ImVec4(0.85f, 0.25f, 0.45f, 1.0f);

    colors[ImGuiCol_Text] = textBright;
    colors[ImGuiCol_TextDisabled] = textMuted;
    colors[ImGuiCol_WindowBg] = bgWindow;
    colors[ImGuiCol_ChildBg] = bgPanel;
    colors[ImGuiCol_PopupBg] = bgPanel;

    colors[ImGuiCol_Border] = borderSoft;
    colors[ImGuiCol_BorderShadow] = transparent;

    colors[ImGuiCol_FrameBg] = bgSurface;
    colors[ImGuiCol_FrameBgHovered] = bgHighlight;
    colors[ImGuiCol_FrameBgActive] = bgActive;

    colors[ImGuiCol_Button] = accentPrimary;
    colors[ImGuiCol_ButtonHovered] = accentPrimaryHover;
    colors[ImGuiCol_ButtonActive] = accentPrimaryActive;

    colors[ImGuiCol_Header] = accentPrimary;
    colors[ImGuiCol_HeaderHovered] = accentPrimaryHover;
    colors[ImGuiCol_HeaderActive] = accentPrimaryActive;

    colors[ImGuiCol_ScrollbarBg] = bgPanel;
    colors[ImGuiCol_ScrollbarGrab] = accentSecondary;
    colors[ImGuiCol_ScrollbarGrabHovered] = accentSecondaryHover;
    colors[ImGuiCol_ScrollbarGrabActive] = accentSecondaryActive;

    colors[ImGuiCol_CheckMark] = accentPrimary;
    colors[ImGuiCol_SliderGrab] = accentPrimary;
    colors[ImGuiCol_SliderGrabActive] = accentPrimaryHover;

    colors[ImGuiCol_Tab] = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.80f);
    colors[ImGuiCol_TabHovered] = accentPrimaryHover;
    colors[ImGuiCol_TabActive] = accentPrimary;
    colors[ImGuiCol_TabUnfocused] = bgSurface;
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.65f);

    colors[ImGuiCol_TitleBg] = bgSurface;
    colors[ImGuiCol_TitleBgActive] = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.90f);
    colors[ImGuiCol_TitleBgCollapsed] = bgSurface;

    colors[ImGuiCol_Separator] = borderHard;
    colors[ImGuiCol_SeparatorHovered] = accentPrimaryHover;
    colors[ImGuiCol_SeparatorActive] = accentPrimaryActive;

    colors[ImGuiCol_ResizeGrip] = accentSecondary;
    colors[ImGuiCol_ResizeGripHovered] = accentSecondaryHover;
    colors[ImGuiCol_ResizeGripActive] = accentSecondaryActive;

    colors[ImGuiCol_NavHighlight] = accentPrimaryHover;
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(accentPrimaryHover.x, accentPrimaryHover.y, accentPrimaryHover.z, 0.65f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.75f);

    colors[ImGuiCol_PlotLines] = accentPrimary;
    colors[ImGuiCol_PlotLinesHovered] = accentPrimaryHover;
    colors[ImGuiCol_PlotHistogram] = accentWarning;
    colors[ImGuiCol_PlotHistogramHovered] = accentDanger;

    style.WindowRounding = 14.0f;
    style.FrameRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 10.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;

    style.WindowPadding = ImVec2(22, 20);
    style.FramePadding = ImVec2(14, 10);
    style.ItemSpacing = ImVec2(12, 10);
    style.ItemInnerSpacing = ImVec2(10, 6);
    style.ScrollbarSize = 18.0f;
    style.GrabMinSize = 18.0f;
    style.TabBorderSize = 0.0f;
}
