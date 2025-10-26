#pragma once

#include <d3d11.h>

namespace renderPipeline
{
    void SetEnabled(bool enabled);
    bool IsEnabled();
    void Process(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* backBufferRTV);
    void OnBeforeResize();
    void Shutdown();
}