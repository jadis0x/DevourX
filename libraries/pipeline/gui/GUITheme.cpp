#include "pch-il2cpp.h"

#include "pipeline/gui/GUITheme.h"
#include <imgui/imgui.h>
#include <algorithm>

void ApplyTheme()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

        // Scale fonts according to the highest framebuffer scale (useful on ultra-wide and HiDPI setups)
        float scale = std::max(io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        if (scale <= 0.0f)
        {
                scale = 1.0f;
        }
        io.FontGlobalScale = std::clamp(scale, 1.0f, 2.5f);

        // --- Midnight Spectrum Palette ---
        const ImVec4 bgDarker = ImVec4(0.03f, 0.04f, 0.06f, 0.98f);          // Primary background
        const ImVec4 bgDark = ImVec4(0.05f, 0.06f, 0.09f, 1.0f);             // Secondary panels
        const ImVec4 bgSurface = ImVec4(0.07f, 0.08f, 0.12f, 1.0f);          // Frames / controls
        const ImVec4 bgHighlight = ImVec4(0.11f, 0.13f, 0.18f, 1.0f);        // Hover state
        const ImVec4 bgActive = ImVec4(0.16f, 0.19f, 0.26f, 1.0f);           // Active state

        const ImVec4 borderSoft = ImVec4(0.19f, 0.21f, 0.28f, 0.60f);
        const ImVec4 borderHard = ImVec4(0.31f, 0.33f, 0.41f, 0.90f);
        const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        const ImVec4 textBright = ImVec4(0.92f, 0.94f, 0.96f, 1.0f);
        const ImVec4 textMuted = ImVec4(0.56f, 0.58f, 0.64f, 1.0f);

        // Vibrant accents for interactive elements
        const ImVec4 accentPrimary = ImVec4(0.35f, 0.45f, 0.94f, 1.0f);       // Primary call-to-action
        const ImVec4 accentPrimaryHover = ImVec4(0.43f, 0.53f, 1.00f, 1.0f);
        const ImVec4 accentPrimaryActive = ImVec4(0.28f, 0.35f, 0.75f, 1.0f);

        const ImVec4 accentSecondary = ImVec4(0.18f, 0.80f, 0.62f, 1.0f);     // Secondary actions
        const ImVec4 accentSecondaryHover = ImVec4(0.26f, 0.88f, 0.68f, 1.0f);
        const ImVec4 accentSecondaryActive = ImVec4(0.14f, 0.58f, 0.46f, 1.0f);

        const ImVec4 accentWarning = ImVec4(0.90f, 0.68f, 0.29f, 1.0f);
        const ImVec4 accentDanger = ImVec4(0.90f, 0.36f, 0.42f, 1.0f);

        // Color mapping
        colors[ImGuiCol_Text] = textBright;
        colors[ImGuiCol_TextDisabled] = textMuted;
        colors[ImGuiCol_WindowBg] = bgDarker;
        colors[ImGuiCol_ChildBg] = bgDark;
        colors[ImGuiCol_PopupBg] = bgDark;

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

        colors[ImGuiCol_ScrollbarBg] = bgDark;
        colors[ImGuiCol_ScrollbarGrab] = accentSecondary;
        colors[ImGuiCol_ScrollbarGrabHovered] = accentSecondaryHover;
        colors[ImGuiCol_ScrollbarGrabActive] = accentSecondaryActive;

        colors[ImGuiCol_CheckMark] = accentSecondary;
        colors[ImGuiCol_SliderGrab] = accentSecondary;
        colors[ImGuiCol_SliderGrabActive] = accentSecondaryHover;

        colors[ImGuiCol_Tab] = ImVec4(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.80f);
        colors[ImGuiCol_TabHovered] = accentPrimaryHover;
        colors[ImGuiCol_TabActive] = accentPrimary;
        colors[ImGuiCol_TabUnfocused] = bgSurface;
        colors[ImGuiCol_TabUnfocusedActive] = accentPrimaryActive;

        colors[ImGuiCol_TitleBg] = bgSurface;
        colors[ImGuiCol_TitleBgActive] = accentPrimary;
        colors[ImGuiCol_TitleBgCollapsed] = bgSurface;

        colors[ImGuiCol_Separator] = borderHard;
        colors[ImGuiCol_SeparatorHovered] = accentSecondary;
        colors[ImGuiCol_SeparatorActive] = accentSecondaryActive;

        colors[ImGuiCol_ResizeGrip] = accentSecondary;
        colors[ImGuiCol_ResizeGripHovered] = accentSecondaryHover;
        colors[ImGuiCol_ResizeGripActive] = accentSecondaryActive;

        colors[ImGuiCol_NavHighlight] = accentSecondaryHover;
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(accentSecondaryHover.x, accentSecondaryHover.y, accentSecondaryHover.z, 0.70f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.72f);

        colors[ImGuiCol_PlotLines] = accentPrimary;
        colors[ImGuiCol_PlotLinesHovered] = accentPrimaryHover;
        colors[ImGuiCol_PlotHistogram] = accentWarning;
        colors[ImGuiCol_PlotHistogramHovered] = accentDanger;

        // Layout tuning
        style.WindowRounding = 10.0f;
        style.FrameRounding = 6.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 10.0f;

        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;

        style.WindowPadding = ImVec2(18, 16);
        style.FramePadding = ImVec2(12, 8);
        style.ItemSpacing = ImVec2(10, 8);
        style.ItemInnerSpacing = ImVec2(10, 6);
        style.ScrollbarSize = 18.0f;
        style.GrabMinSize = 16.0f;
        style.TabBorderSize = 0.0f;
}
