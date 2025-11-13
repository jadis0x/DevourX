#pragma once

#include "pipeline/Dx11.h"

#include <imgui/imgui.h>
#include <mutex>

extern std::mutex g_d3d11_mutex;
extern D3D_PRESENT_FUNCTION oPresent;
HRESULT __stdcall dPresent(IDXGISwapChain* __this, UINT SyncInterval, UINT Flags);

namespace DirectX {
	extern HWND window;
	extern HANDLE hRenderSemaphore;
	void Shutdown();
	ImVec2 GetWindowSize();
}