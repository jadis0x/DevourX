#include "pch-il2cpp.h"

#include "DirectX.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "imgui/imstb_truetype.h"
#include "pipeline/gui/Menu.h"
#include <mutex>
#include "pipeline/Settings.h"

#include "pipeline/keybinds.h"
#include <iostream>
#include <pipeline/gui/GUITheme.h>
#include <esp/esp.h>
#include "devour/devourbase.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND DirectX::window;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* pRenderTargetView = NULL;
D3D_PRESENT_FUNCTION oPresent = nullptr;
WNDPROC oWndProc;

HANDLE DirectX::hRenderSemaphore;
constexpr DWORD MAX_RENDER_THREAD_COUNT = 5;

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
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		DirectX::window = sd.OutputWindow;

		ID3D11Texture2D* pBackBuffer = nullptr;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (!pBackBuffer)
			return false;

		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
		pBackBuffer->Release();

		oWndProc = (WNDPROC)SetWindowLongPtr(DirectX::window, GWLP_WNDPROC, (LONG_PTR)dWndProc);

		if (!ImGui::GetCurrentContext())
			ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		ImFontConfig config;
		config.OversampleH = 3; 
		config.OversampleV = 1; 
		config.PixelSnapH = true; 

		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 17.0f, &config);
		io.Fonts->Build();

		if (font)
			io.FontDefault = font;
		else
			std::cout << "[WARN] Font not loaded, using consolas.\n";

		ImGui_ImplWin32_Init(DirectX::window);
		ImGui_ImplDX11_Init(pDevice, pContext);

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
			if (DirectX::hRenderSemaphore) // güvenli kontrol
				ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
			return oPresent(__this, SyncInterval, Flags);
		}
	}

	WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);

	// resolution changed
	if (!pRenderTargetView) {
		ID3D11Texture2D* pBackBuffer = nullptr;
		if (SUCCEEDED(__this->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)) && pBackBuffer)
		{
			if (SUCCEEDED(pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView)))
			{
				ImVec2 size = DirectX::GetWindowSize();
				//std::cout << "[INFO]: DirectX Window Size: " << +size.x << "x" << +size.y << std::endl;
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
	CloseHandle(hRenderSemaphore);
}