#include "Dx11.h"

D3D_PRESENT_FUNCTION dx11api::presentFunction = nullptr;
DXGI_RESIZEBUFFERS_FUNCTION dx11api::resizeBuffersFunction = nullptr;
bool dx11api::initialized = false;

D3D_PRESENT_FUNCTION dx11api::GetPresentFunction()
{
    if (initialized)
        return presentFunction;

    WNDCLASSEX windowClass = {
            sizeof(WNDCLASSEX),
            CS_HREDRAW | CS_VREDRAW,
            DefWindowProc,
            0,
            0,
            GetModuleHandle(NULL),
            NULL,
            NULL,
            NULL,
            NULL,
            L"DirectX 11",
            NULL,
    };

    RegisterClassEx(&windowClass);
    HWND window = CreateWindow(windowClass.lpszClassName, L"DirectX 11 Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

    DXGI_MODE_DESC bufferDesc = {
            100,
            100,
            {60, 1},
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
            DXGI_MODE_SCALING_UNSPECIFIED,
    };

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {
            bufferDesc,
            {1, 0},
            DXGI_USAGE_RENDER_TARGET_OUTPUT,
            1,
            window,
            1,
            DXGI_SWAP_EFFECT_DISCARD,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
    };

    IDXGISwapChain* pSwapChain;
    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;

    HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, &featureLevel, &pContext);

    if (FAILED(result)) {
        DestroyWindow(window);
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        return nullptr;
    }

    void** pVMT = *(void***)pSwapChain;
    presentFunction = (D3D_PRESENT_FUNCTION)pVMT[8];
    resizeBuffersFunction = (DXGI_RESIZEBUFFERS_FUNCTION)pVMT[13];

    if (pSwapChain) {
        pSwapChain->Release();
        pSwapChain = NULL;
    }

    if (pDevice) {
        pDevice->Release();
        pDevice = NULL;
    }

    if (pContext) {
        pContext->Release();
        pContext = NULL;
    }

    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

    initialized = true;
    return presentFunction;
}

DXGI_RESIZEBUFFERS_FUNCTION dx11api::GetResizeBuffersFunction()
{
    if (!initialized)
    {
        GetPresentFunction();
    }

    return resizeBuffersFunction;
}

void dx11api::Shutdown()
{
    presentFunction = nullptr;
    initialized = false;
}