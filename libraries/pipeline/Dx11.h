#pragma once
#include <dxgi.h>
#include <d3d11.h>

typedef HRESULT(__stdcall* D3D_PRESENT_FUNCTION)(IDXGISwapChain* __this, UINT SyncInterval, UINT Flags);

class dx11api {
public:
    // Retrieve IDXGISwapChain::Present and cache the result.
    static D3D_PRESENT_FUNCTION GetPresentFunction();
    // Release cached resources.
    static void Shutdown();
private:
    static D3D_PRESENT_FUNCTION presentFunction;
    static bool initialized;
};