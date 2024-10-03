#include "DeferredRendering.h"

#include "Renderer.h"
#include "DrawComponent.h"
#include "Shader.h"
#include "os/Window.h"

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


GBuffer::GBuffer(Renderer* renderer)
{
    const Window* window = renderer->GetWindow();
    const uint32_t viewportWidth = (uint32_t)window->GetWidth();
    const uint32_t viewportHeight = (uint32_t)window->GetHeigth();
    auto device = renderer->GetDevice();
    rtViews.resize(_VIEW_IDX_NUM, nullptr);
    srViews.resize(_VIEW_IDX_NUM, nullptr);

    {   // WORLD_POSITION
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = viewportWidth;
        texDesc.Height = viewportHeight;
        texDesc.MipLevels = 1;
        texDesc.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.ArraySize = 1;

        ID3D11Texture2D* tex;
        if (FAILED(device->CreateTexture2D(&texDesc, NULL, &tex))) {
            assert(false);
            return;
        }

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        if (FAILED(device->CreateRenderTargetView(tex, &rtvDesc, &rtViews[WORLD_POS]))) {
            assert(false);
            return;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srtDesc = {};
        srtDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        srtDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srtDesc.Texture2D.MipLevels = 1;
        if (FAILED(device->CreateShaderResourceView(tex, &srtDesc, &srViews[WORLD_POS]))) {
            assert(false);
            return;
        }

        tex->Release();
    }

    {   // DEPTH_STENCIL
        D3D11_TEXTURE2D_DESC texDesc = {};
        ZeroMemory(&texDesc, sizeof(texDesc));
        texDesc.Width = viewportWidth;
        texDesc.Height = viewportHeight;
        texDesc.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.ArraySize = 1;
        texDesc.MipLevels = 1;

        ID3D11Texture2D* tex;
        if (FAILED(renderer->GetDevice()->CreateTexture2D(&texDesc, NULL, &tex))) {
            assert(false);
            return;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        ZeroMemory(&dsvDesc, sizeof(dsvDesc));
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        if (FAILED(device->CreateDepthStencilView(tex, &dsvDesc, &dsv))) {
            assert(false);
            return;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        if (FAILED(device->CreateShaderResourceView(tex, &srvDesc, &srViews[DEPTH_STENCIL]))) {
            assert(false);
            return;
        }

        tex->Release();
    }
}

GBuffer::~GBuffer()
{
}

void GBuffer::Clear(Renderer* renderer)
{
    auto context = renderer->GetDeviceContext();
    float const* clearColor = renderer->GetClearColor();
    for (auto rtv : rtViews) {
        if (!rtv) {
            continue;
        }
        context->ClearRenderTargetView(rtv, clearColor);
    }
    context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

DeferredRenderer::DeferredRenderer(Renderer* renderer)
    : gBuffer(renderer)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};

    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    // Stencil test parameters
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    renderer->GetDevice()->CreateDepthStencilState(&dsDesc, &dsState);

    const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        D3D11_INPUT_ELEMENT_DESC {
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        D3D11_INPUT_ELEMENT_DESC {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
    };
    //assert(sizeof(MeshCBVS) % 16 == 0);
    const D3D11_BUFFER_DESC cbVSDescs[] = {
        {
            GeometryPass::CBVS::GetBufferSize(),	// UINT ByteWidth;
            D3D11_USAGE_DYNAMIC,		// D3D11_USAGE Usage;
            D3D11_BIND_CONSTANT_BUFFER, // UINT BindFlags;
            D3D11_CPU_ACCESS_WRITE,		// UINT CPUAccessFlags;
            0,							// UINT MiscFlags;
            0,							// UINT StructureByteStride;
        }
    };
    //assert(sizeof(MeshCBPS) % 16 == 0);
    const D3D11_BUFFER_DESC cbPSDescs[] = {
        {
            GeometryPass::CBPS::GetBufferSize(),	// UINT ByteWidth;
            D3D11_USAGE_DYNAMIC,		// D3D11_USAGE Usage;
            D3D11_BIND_CONSTANT_BUFFER, // UINT BindFlags;
            D3D11_CPU_ACCESS_WRITE,		// UINT CPUAccessFlags;
            0,							// UINT MiscFlags;
            0,							// UINT StructureByteStride;
        }
    };
    geomShader = std::make_shared<Shader>(L"shaders/geometry_pass.hlsl", renderer->GetDevice(), inputElements, std::size(inputElements), cbVSDescs, 1, cbPSDescs, 1);
}

void DeferredRenderer::Draw(Renderer* renderer)
{
    gBuffer.Clear(renderer);
    DrawGeometry(renderer);
}

void DeferredRenderer::DrawGeometry(Renderer* renderer)
{
    auto context = renderer->GetDeviceContext();
    context->OMSetRenderTargets(GBuffer::VIEW_IDX::_VIEW_IDX_NUM, gBuffer.rtViews.data(), gBuffer.dsv);
    context->OMSetDepthStencilState(dsState, 1);
    geomShader->Activate(context);
    for (DrawComponent* comp : renderer->GetComponents()) {
        comp->Draw(renderer, true);
    }
}
