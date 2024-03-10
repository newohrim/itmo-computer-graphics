#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include "RenderUtils.h"
#include "DrawComponent.h"

class Window;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;

class Renderer {
	friend DrawComponent::DrawComponent(Game*, Compositer*);
	friend DrawComponent::~DrawComponent();

public:
	bool Initialize(Window* window);
	void Shutdown();
	//void UnloadData();

	void Draw();

	void SetClearColor(float* color);

	const Math::Matrix& GetViewMatrix() const { return viewMatr; }
	void SetViewMatrix(const Math::Matrix& view) { viewMatr = view; }

	ID3D11ShaderResourceView* GetTexture(const std::wstring& path);

	Window* GetWindow() const { return window; }
	RenderUtils* GetUtils() const { return utils.get(); }
	ID3D11Device* GetDevice() const { return device; }
	ID3D11DeviceContext* GetDeviceContext() const { return context; }
	IDXGISwapChain* GetSwapChain() const { return swapChain; }

private:
	void AddComponent(DrawComponent* comp);
	void RemoveComponent(DrawComponent* comp);

private:
	std::unique_ptr<RenderUtils> utils;

	std::vector<DrawComponent*> components;

	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> textures;

	Window* window = nullptr;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RasterizerState* rastState = nullptr;
	ID3D11RenderTargetView* rtv;
	ID3D11SamplerState* samplerState;
	ID3D11DepthStencilView* depthBuffer;
	ID3D11DepthStencilState* pDSState;

	Math::Matrix viewMatr;

	float clearColor[4] { 0.1f, 0.1f, 0.1f, 1.0f };
};
