#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include "RenderUtils.h"
#include "DrawComponent.h"
#include "materials/DefaultMeshMaterial.h" // TODO: TEMP E1
#include "DeferredRendering.h" // TODO: TEMP G1
#include "Mesh.h" // TODO: forward declare

class Window;
class Light;

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
	friend class Light;
	friend class ShadowMapper;

public:
	bool Initialize(Window* window);
	void Shutdown();
	//void UnloadData();

	void Draw();

	const std::vector<DrawComponent*> GetComponents() const { return components; }

	const float* GetClearColor() const { return clearColor; }
	void SetClearColor(float* color);

	void PopulateLightsBuffer(DeferredRenderer::LightingPass::CBPS& buffer) const;

	const Math::Matrix& GetViewMatrix() const { return viewMatr; }
	void SetViewMatrix(const Math::Matrix& view) { viewMatr = view; }

	const Math::Matrix& GetViewProjMatrix() const { return viewProjMatr; }
	void SetViewProjMatrix(const Math::Matrix& viewProj) { viewProjMatr = viewProj; }

	const Mesh::PTR& GetMesh(const std::string& path);
	ID3D11ShaderResourceView* GetTexture(const std::wstring& path);

	Window* GetWindow() const { return window; }
	RenderUtils* GetUtils() const { return utils.get(); }
	ID3D11Device* GetDevice() const { return device; }
	ID3D11DeviceContext* GetDeviceContext() const { return context; }
	IDXGISwapChain* GetSwapChain() const { return swapChain; }

public:
	bool isCastingShadows = false;

private:
	void AddComponent(DrawComponent* comp);
	void RemoveComponent(DrawComponent* comp);

	void AddLight(Light* light);
	void RemoveLight(Light* light);

private:
	std::unique_ptr<RenderUtils> utils;

	std::vector<DrawComponent*> components;

	std::unordered_map<std::string, Mesh::PTR> meshes;

	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> textures;

	std::vector<Light*> lightSources;

	Window* window = nullptr;

	class DeferredRenderer* rendererImpl = nullptr;
	class ShadowMapper* shadowMapper = nullptr;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RasterizerState* rastState = nullptr;
	ID3D11RenderTargetView* rtv;
	ID3D11SamplerState* samplerState;
	ID3D11DepthStencilView* depthBuffer;
	ID3D11DepthStencilState* pDSState;

	Math::Matrix viewMatr;
	Math::Matrix viewProjMatr;

	float clearColor[4] { 0.1f, 0.1f, 0.1f, 1.0f };
};
