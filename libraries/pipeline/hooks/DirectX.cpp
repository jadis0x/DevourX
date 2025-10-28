#include "pch-il2cpp.h"

#include "DirectX.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "pipeline/gui/Menu.h"
#include <mutex>
#include "pipeline/Settings.h"

#include "pipeline/keybinds.h"
#include <iostream>
#include <pipeline/gui/GUITheme.h>
#include <esp/esp.h>
#include "devour/devourbase.h"

#include "pipeline/localization/LocalizationManager.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <unordered_set>
#include <filesystem>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND DirectX::window;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* pRenderTargetView = NULL;
D3D_PRESENT_FUNCTION oPresent = nullptr;
WNDPROC oWndProc;

HANDLE DirectX::hRenderSemaphore;
constexpr DWORD MAX_RENDER_THREAD_COUNT = 5;

namespace
{
	bool StartsWithInsensitive(const std::string& value, std::string_view prefix)
	{
		if (value.size() < prefix.size())
		{
			return false;
		}

		for (size_t i = 0; i < prefix.size(); ++i)
		{
			if (std::tolower(static_cast<unsigned char>(value[i])) != std::tolower(static_cast<unsigned char>(prefix[i])))
			{
				return false;
			}
		}

		return true;
	}

	void AddRangeIfNeeded(ImFontGlyphRangesBuilder& builder, std::unordered_set<std::string>& addedScripts, const char* key, const ImWchar* ranges)
	{
		if (!ranges)
		{
			return;
		}

		if (addedScripts.insert(key).second)
		{
			builder.AddRanges(ranges);
		}
	}

	void AddTurkishCharacters(ImFontGlyphRangesBuilder& builder, std::unordered_set<std::string>& addedScripts)
	{
		static const ImWchar kLatinExtendedARange[] = {
				0x0100, 0x017F,
				0,
		};

		if (addedScripts.insert("latin_extended_a").second)
		{
			builder.AddRanges(kLatinExtendedARange);
		}
	}

	void AppendRangesForCulture(const std::string& culture, ImGuiIO& io, ImFontGlyphRangesBuilder& builder, std::unordered_set<std::string>& addedScripts)
	{
		if (culture.empty())
		{
			return;
		}

		std::string lowerCulture = culture;
		std::transform(lowerCulture.begin(), lowerCulture.end(), lowerCulture.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

		if (StartsWithInsensitive(lowerCulture, "tr"))
		{
			AddTurkishCharacters(builder, addedScripts);
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "zh-hant") || StartsWithInsensitive(lowerCulture, "zh-tw") || StartsWithInsensitive(lowerCulture, "zh-hk"))
		{
			AddRangeIfNeeded(builder, addedScripts, "chinese_full", io.Fonts->GetGlyphRangesChineseFull());
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "zh"))
		{
			AddRangeIfNeeded(builder, addedScripts, "chinese_simplified", io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "ja"))
		{
			AddRangeIfNeeded(builder, addedScripts, "japanese", io.Fonts->GetGlyphRangesJapanese());
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "ko"))
		{
			AddRangeIfNeeded(builder, addedScripts, "korean", io.Fonts->GetGlyphRangesKorean());
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "ru") || StartsWithInsensitive(lowerCulture, "uk") || StartsWithInsensitive(lowerCulture, "bg") ||
			StartsWithInsensitive(lowerCulture, "mk") || StartsWithInsensitive(lowerCulture, "sr") || StartsWithInsensitive(lowerCulture, "be"))
		{
			AddRangeIfNeeded(builder, addedScripts, "cyrillic", io.Fonts->GetGlyphRangesCyrillic());
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "el"))
		{
			AddRangeIfNeeded(builder, addedScripts, "greek", io.Fonts->GetGlyphRangesGreek());
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "th"))
		{
			AddRangeIfNeeded(builder, addedScripts, "thai", io.Fonts->GetGlyphRangesThai());
			return;
		}

		if (StartsWithInsensitive(lowerCulture, "vi"))
		{
			AddRangeIfNeeded(builder, addedScripts, "vietnamese", io.Fonts->GetGlyphRangesVietnamese());
			return;
		}

		// Default Latin range is already included.
	}

	bool TryAddFallbackFont(ImGuiIO& io, const char* fontPath, float size, const ImWchar* glyphRanges)
	{
		if (!glyphRanges)
		{
			return false;
		}

		namespace fs = std::filesystem;
		if (!fs::exists(fontPath))
		{
			std::cout << "[WARN] Fallback font file not found: " << fontPath << "\n";
			return false;
		}

		ImFontConfig mergeConfig{};
		mergeConfig.MergeMode = true;
		mergeConfig.PixelSnapH = true;
		mergeConfig.OversampleH = 3;
		mergeConfig.OversampleV = 1;
		mergeConfig.GlyphRanges = glyphRanges;

		if (ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath, size, &mergeConfig, glyphRanges))
		{
			std::cout << "[INFO] Loaded fallback font: " << fontPath << "\n";
			return true;
		}

		std::cout << "[WARN] Failed to load fallback font: " << fontPath << "\n";
		return false;
	}

	void LoadFallbackFonts(ImGuiIO& io, const std::unordered_set<std::string>& requiredScripts, float size)
	{
		if (requiredScripts.count("chinese_full"))
		{
			TryAddFallbackFont(io, "C:\\Windows\\Fonts\\msjh.ttc", size, io.Fonts->GetGlyphRangesChineseFull());
		}
		else if (requiredScripts.count("chinese_simplified"))
		{
			TryAddFallbackFont(io, "C:\\Windows\\Fonts\\msyh.ttc", size, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		}

		if (requiredScripts.count("japanese"))
		{
			TryAddFallbackFont(io, "C:\\Windows\\Fonts\\meiryo.ttc", size, io.Fonts->GetGlyphRangesJapanese());
		}

		if (requiredScripts.count("korean"))
		{
			TryAddFallbackFont(io, "C:\\Windows\\Fonts\\malgun.ttf", size, io.Fonts->GetGlyphRangesKorean());
		}
	}

	const ImWchar* BuildSupportedGlyphRanges(ImGuiIO& io, std::unordered_set<std::string>* outScripts = nullptr)
	{
		static ImVector<ImWchar> mergedRanges;
		ImFontGlyphRangesBuilder builder;
		std::unordered_set<std::string> addedScripts;

		AddRangeIfNeeded(builder, addedScripts, "default", io.Fonts->GetGlyphRangesDefault());

		const auto availableCultures = Localization::GetAvailableCultures();
		for (const auto& culture : availableCultures)
		{
			AppendRangesForCulture(culture, io, builder, addedScripts);
		}

		AppendRangesForCulture(Localization::GetFallbackCulture(), io, builder, addedScripts);
		AppendRangesForCulture(Localization::GetCurrentCulture(), io, builder, addedScripts);

		if (outScripts)
		{
			*outScripts = addedScripts;
		}

		mergedRanges.clear();
		builder.BuildRanges(&mergedRanges);
		return mergedRanges.Data;
	}

	ImFont* LoadFontsForCultures(ImGuiIO& io)
	{
		const float fontSize = 17.0f;
		std::unordered_set<std::string> requiredScripts;
		const ImWchar* glyphRanges = BuildSupportedGlyphRanges(io, &requiredScripts);

		io.Fonts->Clear();

		ImFontConfig config{};
		config.OversampleH = 3;
		config.OversampleV = 1;
		config.PixelSnapH = true;
		config.GlyphRanges = glyphRanges;

		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", fontSize, &config, glyphRanges);
		if (!font)
		{
			std::cout << "[WARN] Font not loaded, falling back to default font.\n";
			font = io.Fonts->AddFontDefault();
		}

		LoadFallbackFonts(io, requiredScripts, fontSize);

		if (font)
		{
			io.FontDefault = font;
		}

		return font;
	}
}

bool ReloadFontsPreservingScale()
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	const float previousFontScaleMain = style.FontScaleMain;
	const float previousFontScaleDpi = style.FontScaleDpi;
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	const float previousFontGlobalScale = io.FontGlobalScale;
#endif

	ImFont* font = LoadFontsForCultures(io);

	if (!ImGui_ImplDX11_CreateDeviceObjects())
	{
		std::cout << "[ERROR]: Failed to recreate ImGui device objects after font reload.\n";
		return false;
	}

	style.FontScaleMain = previousFontScaleMain;
	style.FontScaleDpi = previousFontScaleDpi;
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	io.FontGlobalScale = previousFontGlobalScale;
#endif

	return font != nullptr;
}

typedef struct Cache
{
	ImGuiWindow* Window = nullptr;
	ImVec2       Winsize;
} cache_t;

static cache_t s_Cache;

ImVec2 DirectX::GetWindowSize()
{
	if (app::Screen_get_fullScreen(nullptr))
	{
		RECT rect;
		GetWindowRect(window, &rect);

		return { (float)(rect.right - rect.left),  (float)(rect.bottom - rect.top) };
	}

	return { (float)app::Screen_get_width(nullptr), (float)app::Screen_get_height(nullptr) };

}

LRESULT __stdcall dWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!settings.ImGuiInitialized)
		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

	if (uMsg == WM_SIZE && pRenderTargetView)
	{
		if (wParam != SIZE_MINIMIZED)
		{
			WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);
			pRenderTargetView->Release();
			pRenderTargetView = nullptr;
			ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
		}
	}

	if (ImGui::GetCurrentContext() && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	KeyBinds::WndProc(uMsg, wParam, lParam);

	if (KeyBinds::IsKeyPressed(settings.KeyBinds.Toggle_Menu))
	{
		settings.bShowMenu = !settings.bShowMenu;

		if (Base::Game::IsInGame())
		{
			if (settings.bShowMenu)
				app::UIHelpers_ShowMouseCursor(nullptr);
			else
				app::UIHelpers_HideMouseCursor(nullptr);
		}
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool ImGuiInitialization(IDXGISwapChain* pSwapChain) {
	if ((pDevice != nullptr) || (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))) {
		pDevice->GetImmediateContext(&pContext);

		DXGI_SWAP_CHAIN_DESC sd{};
		pSwapChain->GetDesc(&sd);
		DirectX::window = sd.OutputWindow;

		ID3D11Texture2D* pBackBuffer = nullptr;
		if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)) || !pBackBuffer)
			return false;

		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
		pBackBuffer->Release();

		oWndProc = (WNDPROC)SetWindowLongPtr(DirectX::window, GWLP_WNDPROC, (LONG_PTR)dWndProc);

		if (!ImGui::GetCurrentContext())
			ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		ImGui_ImplWin32_Init(DirectX::window);
		ImGui_ImplDX11_Init(pDevice, pContext);

		if (!LoadFontsForCultures(io))
		{
			std::cout << "[WARN] Font not loaded, using consolas.\n";
		}

		ImGui_ImplDX11_InvalidateDeviceObjects();
		ImGui_ImplDX11_CreateDeviceObjects();

		DirectX::hRenderSemaphore = CreateSemaphore(NULL, MAX_RENDER_THREAD_COUNT, MAX_RENDER_THREAD_COUNT, NULL);
		return true;
	}
	return false;
}

std::once_flag init_d3d;

HRESULT __stdcall dPresent(IDXGISwapChain* __this, UINT SyncInterval, UINT Flags) {
	std::call_once(init_d3d, [&] {
		if (SUCCEEDED(__this->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
		}
		});
	if (!settings.ImGuiInitialized) {
		if (ImGuiInitialization(__this)) {
			ImVec2 size = DirectX::GetWindowSize();
			settings.ImGuiInitialized = true;
			std::cout << "[INFO]: ImGui Initialized successfully!\n";
			std::cout << "[INFO]: Fullscreen: " << app::Screen_get_fullScreen(nullptr) << std::endl;
			std::cout << "[INFO]: DirectX Window Size: " << +size.x << "x" << +size.y << std::endl;
		}
		else {
			if (DirectX::hRenderSemaphore)
				ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
			return oPresent(__this, SyncInterval, Flags);
		}
	}

	WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);

	if (settings.bPendingFontReload)
	{
		if (ReloadFontsPreservingScale())
		{
			std::cout << "[INFO]: ImGui fonts reloaded after culture change.\n";
		}
		settings.bPendingFontReload = false;
	}

	// resolution changed
	if (!pRenderTargetView) {
		ID3D11Texture2D* pBackBuffer = nullptr;
		if (SUCCEEDED(__this->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)) && pBackBuffer)
		{
			if (SUCCEEDED(pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView)))
			{
				ImVec2 size = DirectX::GetWindowSize();
				std::cout << "[INFO]: DirectX Window Size: " << +size.x << "x" << +size.y << std::endl;
			}
			else
			{
				std::cout << "[ERROR]: Failed to create render target view.\n";
			}
			pBackBuffer->Release();
		}
		else
		{
			std::cout << "[ERROR]: Failed to get back buffer from swapchain.\n";
			ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
			return oPresent(__this, SyncInterval, Flags);
		}
	}

	il2cpp_gc_disable();

	ApplyTheme();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (settings.bShowMenu)
	{
		Menu::Render();
	}

	if (Base::Game::IsInGame() && !Base::Gameplay::IsSequencePlaying())
	{
		ESPManager::DrawAll();
	}
	else
	{
		settings.bInteractableEspUpdated = false;
	}

	ImGui::EndFrame();
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
	if (ImGui::GetDrawData())
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	il2cpp_gc_enable();

	HRESULT result = oPresent(__this, SyncInterval, Flags);

	ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);

	return result;
}

void DirectX::Shutdown() {
	assert(hRenderSemaphore != NULL);
	for (uint8_t i = 0; i < MAX_RENDER_THREAD_COUNT; i++)
	{
		DWORD res = WaitForSingleObject(hRenderSemaphore, 5000);
		if (res != WAIT_OBJECT_0) {
			std::cout << "[ERROR] Render thread cleanup timeout!\n";
		}
	}
	SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	if (ImGui::GetCurrentContext())
		ImGui::DestroyContext();
	if (pRenderTargetView)
	{
		pRenderTargetView->Release();
		pRenderTargetView = nullptr;
	}
	CloseHandle(hRenderSemaphore);
}