#pragma once

#include <vector>
#include <memory>
#include "DrawComponent.h"
#include "RenderUtils.h"

class Window;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;

class Renderer {
	friend DrawComponent::DrawComponent(class Game*);
	friend DrawComponent::~DrawComponent();

public:
	bool Initialize(Window* window);
	void Shutdown();
	void UnloadData();

	void Draw();

	void SetClearColor(float* color);

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

	Window* window = nullptr;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RasterizerState* rastState = nullptr;
	ID3D11RenderTargetView* rtv;

	float clearColor[4] { 0.1f, 0.1f, 0.1f, 1.0f };
};
