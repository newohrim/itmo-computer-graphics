#include "Renderer.h"

#include "os/Window.h"

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include <algorithm>

#ifdef _WIN32
#include "os/wnd.h"
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


bool Renderer::Initialize(Window* _window)
{
	window = _window;
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = window->GetWidth();
	swapDesc.BufferDesc.Height = window->GetHeigth();
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
#ifdef _WIN32
	swapDesc.OutputWindow = wndGetHWND(window);
#elif
	static_assert(false, "only windows support");
#endif
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context);

	if (FAILED(res))
	{
		// Well, that was unexpected
		__debugbreak();
	}

	ID3D11Texture2D* backTex;
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = device->CreateRasterizerState(&rastDesc, &rastState);
	context->RSSetState(rastState);

	utils = std::make_unique<RenderUtils>();

    return true;
}

void Renderer::Shutdown()
{
	UnloadData();
	utils.reset();
	// TODO: code d3d11 termination logic here
}

void Renderer::UnloadData()
{
	components.clear();
}

void Renderer::Draw()
{
	context->ClearState();
	context->RSSetState(rastState);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(window->GetWidth());
	viewport.Height = static_cast<float>(window->GetHeigth());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	context->OMSetRenderTargets(1, &rtv, nullptr);

	context->ClearRenderTargetView(rtv, clearColor);

	for (DrawComponent* comp : components) {
		comp->Draw(this);
	}

	context->OMSetRenderTargets(0, nullptr, nullptr);

	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Renderer::SetClearColor(float* color)
{
	memcpy(clearColor, color, sizeof(float) * 4);
}

void Renderer::AddComponent(DrawComponent* comp)
{
	components.push_back(comp);
}

void Renderer::RemoveComponent(DrawComponent* comp)
{
	auto iter = std::find(components.begin(), components.end(), comp);
	if (iter != components.end())
	{
		std::iter_swap(iter, components.end() - 1);
		components.pop_back();
		return;
	}
}
