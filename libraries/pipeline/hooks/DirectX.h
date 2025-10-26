#pragma once

#include "pipeline/Dx11.h"

#include <imgui/imgui.h>

extern D3D_PRESENT_FUNCTION oPresent;
extern DXGI_RESIZEBUFFERS_FUNCTION oResizeBuffers;

HRESULT __stdcall dPresent(IDXGISwapChain* __this, UINT SyncInterval, UINT Flags);
HRESULT __stdcall dResizeBuffers(IDXGISwapChain* __this, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

namespace DirectX {
	extern HWND window;
	extern HANDLE hRenderSemaphore;
	void Shutdown();
	ImVec2 GetWindowSize();
}