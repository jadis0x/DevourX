#pragma once
#include <dxgi.h>
#include <d3d11.h>

typedef HRESULT(__stdcall* D3D_PRESENT_FUNCTION)(IDXGISwapChain* __this, UINT SyncInterval, UINT Flags);

class dx11api {
public:
    D3D_PRESENT_FUNCTION presentFunction;
    dx11api();
};