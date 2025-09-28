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
        const ImVec4 bgDark = ImVec4(0.05f, 0.06f, 0.08f, 0.98f);        // Main canvas background
        const ImVec4 bgSurface = ImVec4(0.08f, 0.09f, 0.12f, 1.0f);      // Panels / child windows
        const ImVec4 bgHighlight = ImVec4(0.12f, 0.13f, 0.18f, 1.0f);    // Hover states
        const ImVec4 bgActive = ImVec4(0.16f, 0.18f, 0.24f, 1.0f);       // Active states
        const ImVec4 borderSoft = ImVec4(0.21f, 0.23f, 0.27f, 0.55f);
        const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        const ImVec4 textBright = ImVec4(0.90f, 0.92f, 0.94f, 1.0f);
        const ImVec4 textMuted = ImVec4(0.56f, 0.58f, 0.62f, 1.0f);

        // Vibrant accents for interactive elements
        const ImVec4 accentPrimary = ImVec4(0.35f, 0.28f, 0.66f, 1.0f);  // Buttons & tabs
        const ImVec4 accentPrimaryHover = ImVec4(0.42f, 0.35f, 0.78f, 1.0f);
        const ImVec4 accentPrimaryActive = ImVec4(0.24f, 0.18f, 0.52f, 1.0f);

        const ImVec4 accentSecondary = ImVec4(0.18f, 0.59f, 0.62f, 1.0f);      // Check marks & sliders
        const ImVec4 accentSecondaryHover = ImVec4(0.22f, 0.68f, 0.72f, 1.0f);
        const ImVec4 accentSecondaryActive = ImVec4(0.12f, 0.45f, 0.48f, 1.0f);

        const ImVec4 accentTertiary = ImVec4(0.88f, 0.35f, 0.47f, 1.0f);        // Drag handles / highlights
        const ImVec4 accentTertiaryHover = ImVec4(0.96f, 0.42f, 0.56f, 1.0f);
        const ImVec4 accentTertiaryActive = ImVec4(0.73f, 0.24f, 0.35f, 1.0f);

        // Color mapping
        colors[ImGuiCol_Text] = textBright;
        colors[ImGuiCol_TextDisabled] = textMuted;
        colors[ImGuiCol_WindowBg] = bgDark;
        colors[ImGuiCol_ChildBg] = bgSurface;
        colors[ImGuiCol_PopupBg] = bgSurface;

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

        colors[ImGuiCol_Tab] = accentPrimaryActive;
        colors[ImGuiCol_TabHovered] = accentPrimaryHover;
        colors[ImGuiCol_TabActive] = accentPrimary;
        colors[ImGuiCol_TabUnfocused] = bgSurface;
        colors[ImGuiCol_TabUnfocusedActive] = accentPrimaryActive;

        colors[ImGuiCol_TitleBg] = bgSurface;
        colors[ImGuiCol_TitleBgActive] = accentPrimary;
        colors[ImGuiCol_TitleBgCollapsed] = bgDark;

        colors[ImGuiCol_Separator] = borderSoft;
        colors[ImGuiCol_SeparatorHovered] = accentTertiaryHover;
        colors[ImGuiCol_SeparatorActive] = accentTertiaryActive;

        colors[ImGuiCol_ResizeGrip] = accentTertiary;
        colors[ImGuiCol_ResizeGripHovered] = accentTertiaryHover;
        colors[ImGuiCol_ResizeGripActive] = accentTertiaryActive;

        colors[ImGuiCol_NavHighlight] = accentSecondaryHover;
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(accentSecondaryHover.x, accentSecondaryHover.y, accentSecondaryHover.z, 0.70f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.65f);

        // Layout tuning
        style.WindowRounding = 12.0f;
        style.FrameRounding = 6.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 10.0f;

        style.WindowBorderSize = 0.0f;
        style.FrameBorderSize = 0.0f;
        style.ChildBorderSize = 0.0f;

        style.WindowPadding = ImVec2(22, 18);
        style.FramePadding = ImVec2(14, 9);
        style.ItemSpacing = ImVec2(12, 9);
        style.ItemInnerSpacing = ImVec2(10, 7);
        style.ScrollbarSize = 16.0f;
        style.GrabMinSize = 14.0f;
        style.TabBorderSize = 0.0f;
}
