#include "pch-il2cpp.h"

#define NOMINMAX

#include "RenderHook.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>

#include "pipeline/settings.h"

namespace
{
    using Microsoft::WRL::ComPtr;

    struct renderPipelineContext
    {
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11Texture2D> copyTexture;
        ComPtr<ID3D11ShaderResourceView> copySRV;
        ComPtr<ID3D11Texture2D> lowResTexture;
        ComPtr<ID3D11RenderTargetView> lowResRTV;
        ComPtr<ID3D11ShaderResourceView> lowResSRV;
        ComPtr<ID3D11SamplerState> linearSampler;
        ComPtr<ID3D11VertexShader> vertexShader;
        ComPtr<ID3D11PixelShader> pixelShader;
        ComPtr<ID3D11Buffer> constantBuffer;
        UINT backBufferWidth = 0;
        UINT backBufferHeight = 0;
        UINT lowResWidth = 0;
        UINT lowResHeight = 0;
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        bool pipelineReady = false;
    };

    renderPipelineContext g_context;
    bool g_enabled = false;

    float DetermineScaleFactor(int mode)
    {
        switch (mode)
        {
        case 0: return 0.77f; // Ultra Quality
        case 1: return 0.67f; // Quality
        case 2: return 0.59f; // Balanced
        case 3: return 0.50f; // Performance
        default: return 0.67f;
        }
    }

    bool EnsurePipeline(ID3D11Device* device)
    {
        if (g_context.pipelineReady)
        {
            return true;
        }

        static constexpr char kShaderSource[] = R"(Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer FSRParams : register(b0)
{
        float2 texelSize;
        float sharpness;
        float padding;
};

struct VSOutput
{
        float4 Position : SV_POSITION;
        float2 TexCoord : TEXCOORD0;
};

VSOutput VSMain(uint vertexId : SV_VertexID)
{
        static const float2 kPositions[4] = {
                float2(-1.0f, -1.0f),
                float2(-1.0f,  1.0f),
                float2( 1.0f, -1.0f),
                float2( 1.0f,  1.0f)
        };

        static const float2 kTexCoords[4] = {
                float2(0.0f, 1.0f),
                float2(0.0f, 0.0f),
                float2(1.0f, 1.0f),
                float2(1.0f, 0.0f)
        };

        VSOutput output;
        output.Position = float4(kPositions[vertexId], 0.0f, 1.0f);
        output.TexCoord = kTexCoords[vertexId];
        return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
        float4 color = gTexture.Sample(gSampler, input.TexCoord);
        if (sharpness <= 0.0001f)
        {
                return color;
        }

        const float2 offsetX = float2(texelSize.x, 0.0f);
        const float2 offsetY = float2(0.0f, texelSize.y);

        float4 north = gTexture.Sample(gSampler, input.TexCoord + offsetY);
        float4 south = gTexture.Sample(gSampler, input.TexCoord - offsetY);
        float4 east = gTexture.Sample(gSampler, input.TexCoord + offsetX);
        float4 west = gTexture.Sample(gSampler, input.TexCoord - offsetX);

        float4 laplacian = ((north + south + east + west) * 0.25f) - color;
        return saturate(color + laplacian * sharpness);
}
)";

        UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
        compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        ComPtr<ID3DBlob> vsBlob;
        ComPtr<ID3DBlob> psBlob;
        ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3DCompile(kShaderSource, sizeof(kShaderSource), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vsBlob, &errorBlob);
        if (FAILED(hr))
        {
            if (errorBlob)
            {
                std::cout << "[Dx] Vertex shader compilation failed: " << static_cast<const char*>(errorBlob->GetBufferPointer()) << '\n';
            }
            return false;
        }

        hr = D3DCompile(kShaderSource, sizeof(kShaderSource), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &psBlob, &errorBlob);
        if (FAILED(hr))
        {
            if (errorBlob)
            {
                std::cout << "[Dx] Pixel shader compilation failed: " << static_cast<const char*>(errorBlob->GetBufferPointer()) << '\n';
            }
            return false;
        }

        if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_context.vertexShader)))
        {
            std::cout << "[Dx] Failed to create vertex shader.\n";
            return false;
        }

        if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_context.pixelShader)))
        {
            std::cout << "[Dx] Failed to create pixel shader.\n";
            return false;
        }

        D3D11_SAMPLER_DESC samplerDesc{};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        if (FAILED(device->CreateSamplerState(&samplerDesc, &g_context.linearSampler)))
        {
            std::cout << "[Dx] Failed to create linear sampler.\n";
            return false;
        }

        D3D11_BUFFER_DESC cbDesc{};
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.ByteWidth = sizeof(float) * 4;
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (FAILED(device->CreateBuffer(&cbDesc, nullptr, &g_context.constantBuffer)))
        {
            std::cout << "[Dx] Failed to create constant buffer.\n";
            return false;
        }

        g_context.pipelineReady = true;
        return true;
    }

    void ReleaseRenderResources()
    {
        g_context.copyTexture.Reset();
        g_context.copySRV.Reset();
        g_context.lowResTexture.Reset();
        g_context.lowResRTV.Reset();
        g_context.lowResSRV.Reset();
        g_context.backBufferWidth = 0;
        g_context.backBufferHeight = 0;
        g_context.lowResWidth = 0;
        g_context.lowResHeight = 0;
        g_context.format = DXGI_FORMAT_UNKNOWN;
    }

    bool EnsureRenderResources(ID3D11Device* device, IDXGISwapChain* swapChain)
    {
        DXGI_SWAP_CHAIN_DESC desc{};
        if (FAILED(swapChain->GetDesc(&desc)))
        {
            return false;
        }

        const UINT width = desc.BufferDesc.Width;
        const UINT height = desc.BufferDesc.Height;
        const DXGI_FORMAT format = desc.BufferDesc.Format;

        if (width == 0 || height == 0)
        {
            return false;
        }

        const float scale = DetermineScaleFactor(settings.renderQualityMode);
        const UINT lowWidth = std::max(1u, static_cast<UINT>(static_cast<float>(width) * scale));
        const UINT lowHeight = std::max(1u, static_cast<UINT>(static_cast<float>(height) * scale));

        if (g_context.backBufferWidth == width && g_context.backBufferHeight == height &&
            g_context.lowResWidth == lowWidth && g_context.lowResHeight == lowHeight && g_context.format == format &&
            g_context.copyTexture && g_context.lowResTexture)
        {
            return true;
        }

        ReleaseRenderResources();

        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = format;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

        if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &g_context.copyTexture)))
        {
            std::cout << "[Dx] Failed to create intermediate texture.\n";
            return false;
        }

        if (FAILED(device->CreateShaderResourceView(g_context.copyTexture.Get(), nullptr, &g_context.copySRV)))
        {
            std::cout << "[Dx] Failed to create intermediate SRV.\n";
            return false;
        }

        textureDesc.Width = lowWidth;
        textureDesc.Height = lowHeight;

        if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &g_context.lowResTexture)))
        {
            std::cout << "[Dx] Failed to create low resolution texture.\n";
            return false;
        }

        if (FAILED(device->CreateRenderTargetView(g_context.lowResTexture.Get(), nullptr, &g_context.lowResRTV)))
        {
            std::cout << "[Dx] Failed to create low resolution RTV.\n";
            return false;
        }

        if (FAILED(device->CreateShaderResourceView(g_context.lowResTexture.Get(), nullptr, &g_context.lowResSRV)))
        {
            std::cout << "[Dx] Failed to create low resolution SRV.\n";
            return false;
        }

        g_context.backBufferWidth = width;
        g_context.backBufferHeight = height;
        g_context.lowResWidth = lowWidth;
        g_context.lowResHeight = lowHeight;
        g_context.format = format;

        std::cout << "[Dx] Recreated render resources: " << lowWidth << "x" << lowHeight << " -> " << width << "x" << height << "\n";
        return true;
    }

    void ApplyPass(ID3D11DeviceContext* context, IDXGISwapChain* swapChain, ID3D11RenderTargetView* backBufferRTV)
    {
        if (!backBufferRTV)
        {
            return;
        }

        ComPtr<ID3D11Texture2D> backBuffer;
        if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()))))
        {
            return;
        }

        context->CopyResource(g_context.copyTexture.Get(), backBuffer.Get());

        ID3D11RenderTargetView* previousRTV = nullptr;
        ComPtr<ID3D11DepthStencilView> previousDSV;
        context->OMGetRenderTargets(1, &previousRTV, &previousDSV);

        UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> oldViewports{};
        context->RSGetViewports(&viewportCount, oldViewports.data());

        const D3D11_VIEWPORT lowViewport = { 0.0f, 0.0f, static_cast<float>(g_context.lowResWidth), static_cast<float>(g_context.lowResHeight), 0.0f, 1.0f };
        context->RSSetViewports(1, &lowViewport);

        ID3D11RenderTargetView* lowRTV = g_context.lowResRTV.Get();
        context->OMSetRenderTargets(1, &lowRTV, nullptr);

        const float downscaleParams[4] = {
                1.0f / static_cast<float>(g_context.backBufferWidth),
                1.0f / static_cast<float>(g_context.backBufferHeight),
                0.0f,
                0.0f
        };

        D3D11_MAPPED_SUBRESOURCE mapped{};
        if (SUCCEEDED(context->Map(g_context.constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        {
            std::memcpy(mapped.pData, downscaleParams, sizeof(downscaleParams));
            context->Unmap(g_context.constantBuffer.Get(), 0);
        }

        ComPtr<ID3D11InputLayout> previousLayout;
        context->IAGetInputLayout(&previousLayout);
        D3D11_PRIMITIVE_TOPOLOGY previousTopology;
        context->IAGetPrimitiveTopology(&previousTopology);

        ID3D11ShaderResourceView* copySrv = g_context.copySRV.Get();
        context->PSSetShaderResources(0, 1, &copySrv);
        ID3D11SamplerState* sampler = g_context.linearSampler.Get();
        context->PSSetSamplers(0, 1, &sampler);
        context->PSSetConstantBuffers(0, 1, g_context.constantBuffer.GetAddressOf());

        context->IASetInputLayout(nullptr);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        context->VSSetShader(g_context.vertexShader.Get(), nullptr, 0);
        context->PSSetShader(g_context.pixelShader.Get(), nullptr, 0);
        context->Draw(4, 0);

        ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
        context->PSSetShaderResources(0, 1, nullSrv);

        const float upscaleParams[4] = {
                1.0f / static_cast<float>(g_context.lowResWidth),
                1.0f / static_cast<float>(g_context.lowResHeight),
                settings.dSharpness,
                0.0f
        };

        if (SUCCEEDED(context->Map(g_context.constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        {
            std::memcpy(mapped.pData, upscaleParams, sizeof(upscaleParams));
            context->Unmap(g_context.constantBuffer.Get(), 0);
        }

        const D3D11_VIEWPORT fullViewport = { 0.0f, 0.0f, static_cast<float>(g_context.backBufferWidth), static_cast<float>(g_context.backBufferHeight), 0.0f, 1.0f };
        context->RSSetViewports(1, &fullViewport);
        context->OMSetRenderTargets(1, &backBufferRTV, nullptr);

        ID3D11ShaderResourceView* lowSrv = g_context.lowResSRV.Get();
        context->PSSetShaderResources(0, 1, &lowSrv);
        context->Draw(4, 0);

        context->PSSetShaderResources(0, 1, nullSrv);

        if (previousRTV)
        {
            context->OMSetRenderTargets(1, &previousRTV, previousDSV.Get());
            previousRTV->Release();
        }
        else
        {
            context->OMSetRenderTargets(0, nullptr, nullptr);
        }

        if (viewportCount > 0)
        {
            context->RSSetViewports(viewportCount, oldViewports.data());
        }

        context->IASetPrimitiveTopology(previousTopology);
        context->IASetInputLayout(previousLayout.Get());
        context->VSSetShader(nullptr, nullptr, 0);
        context->PSSetShader(nullptr, nullptr, 0);
        ID3D11SamplerState* nullSampler[1] = { nullptr };
        context->PSSetSamplers(0, 1, nullSampler);
        ID3D11Buffer* nullBuffer[1] = { nullptr };
        context->PSSetConstantBuffers(0, 1, nullBuffer);
    }
}

namespace renderPipeline
{
    void SetEnabled(bool enabled)
    {
        g_enabled = enabled;
        if (!g_enabled)
        {
            ReleaseRenderResources();
        }
    }

    bool IsEnabled()
    {
        return g_enabled;
    }

    void Process(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11RenderTargetView* backBufferRTV)
    {
        if (!g_enabled || !swapChain || !device || !context)
        {
            return;
        }

        if (!g_context.device)
        {
            g_context.device = device;
        }

        if (!EnsurePipeline(device))
        {
            return;
        }

        if (!EnsureRenderResources(device, swapChain))
        {
            return;
        }

        ApplyPass(context, swapChain, backBufferRTV);
    }

    void OnBeforeResize()
    {
        ReleaseRenderResources();
    }

    void Shutdown()
    {
        ReleaseRenderResources();
        g_context.linearSampler.Reset();
        g_context.vertexShader.Reset();
        g_context.pixelShader.Reset();
        g_context.constantBuffer.Reset();
        g_context.device.Reset();
        g_context.pipelineReady = false;
    }
}