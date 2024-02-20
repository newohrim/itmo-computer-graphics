#include "Game.h"

#include <windows.h>
#include <WinUser.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <chrono>

#include "Component.h"
#include "os/Window.h"
#include "render/Renderer.h"
#include "render/RenderUtils.h"
#include "render/Shader.h"

#include "render/DrawComponent.h"
#include "components/TriangleComponent.h"

#ifdef _WIN32
#include "os/wnd.h"
#endif


bool Game::Initialize(const std::string name, int windowWidth, int windowHeight)
{
	window = osCreateWindow(name, windowWidth, windowHeight);
	if (!window) {
		// TODO: log error message
		return false;
	}

	renderer = std::make_unique<Renderer>();
	if (!renderer->Initialize(window)) {
		// TODO: log error message
		return false;
	}

	LoadData();

	prevTime = std::chrono::steady_clock::now();

	return true;
}

void Game::ProcessInput()
{
	// Handle the windows messages.
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// If windows signals to end the application then exit out.
	if (msg.message == WM_QUIT) {
		isRunning = false;
	}
}

void Game::Shutdown()
{
}

void Game::RunLoop()
{
	while (isRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::LoadData()
{
#if 0
	DrawComponent* tri = new DrawComponent(this);
	tri->SetGeometry(renderer->GetUtils()->GetQuadGeom(renderer.get()));
	tri->SetShader(renderer->GetUtils()->GetQuadShader(renderer.get()));
	tri->Initialize();
#else
	const float vertsA[] = {
		0.5f, 0.5f, 0.5f, 1.0f,	  1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f
	};
	TriangleComponent* triA = new TriangleComponent(this);
	triA->SetVerts(vertsA, sizeof(float) * std::size(vertsA));
	triA->SetShader(renderer->GetUtils()->GetQuadShader(renderer.get()));

	const float vertsB[] = {
		0.5f, 0.5f, 0.5f, 1.0f,	  1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};
	TriangleComponent* triB = new TriangleComponent(this);
	triB->SetVerts(vertsB, sizeof(float) * std::size(vertsB));
	triB->SetShader(renderer->GetUtils()->GetQuadShader(renderer.get()));
#endif

#if 1
	const float vertsC[] = {
		1.0f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,	  1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f
	};
	TriangleComponent* triC = new TriangleComponent(this);
	triC->SetVerts(vertsC, sizeof(float) * std::size(vertsC));
	triC->SetShader(renderer->GetUtils()->GetQuadShader(renderer.get()));

	const float vertsD[] = {
		1.0f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 1.0f, 0.5f, 1.0f,	  1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,	  1.0f, 0.0f, 0.0f, 1.0f
	};
	TriangleComponent* triD = new TriangleComponent(this);
	triD->SetVerts(vertsD, sizeof(float) * std::size(vertsD));
	triD->SetShader(renderer->GetUtils()->GetQuadShader(renderer.get()));

	const float vertsE[] = {
		-0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f
	};
	TriangleComponent* triE = new TriangleComponent(this);
	triE->SetVerts(vertsE, sizeof(float) * std::size(vertsE));
	triE->SetShader(renderer->GetUtils()->GetQuadShader(renderer.get()));

	const float vertsF[] = {
		-0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.5f, 1.0f,	  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 0.5f, 0.5f, 1.0f,	  1.0f, 0.0f, 0.0f, 1.0f
	};
	TriangleComponent* triF = new TriangleComponent(this);
	triF->SetVerts(vertsF, sizeof(float) * std::size(vertsF));
	triF->SetShader(renderer->GetUtils()->GetQuadShader(renderer.get()));
#endif
}

void Game::UpdateGame()
{
	const auto curTime = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
	prevTime = curTime;
	totalTime += deltaTime;
	frameNum++;

	if (totalTime > 1.0f) {
		float fps = frameNum / totalTime;

		totalTime -= 1.0f;

#ifdef _WIN32
		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(wndGetHWND(window), text);
#endif

		frameNum = 0;
	}

	float clearColor[] { totalTime, 0.1f, 0.1f, 0.1f};
	renderer->SetClearColor(clearColor);

	isUpdatingComponents = true;
	for (auto comp : components)
	{
		comp->Update(deltaTime);
	}
	isUpdatingComponents = false;

	for (auto comp : pendingComponents)
	{
		components.emplace_back(comp);
	}
	pendingComponents.clear();
}

void Game::GenerateOutput()
{
	renderer->Draw();
}

void Game::AddComponent(Component* comp)
{
#ifdef _DEBUG
	auto iter = std::find(pendingComponents.begin(), pendingComponents.end(), comp);
	assert(iter == pendingComponents.end());
	iter = std::find(components.begin(), components.end(), comp);
	assert(iter == components.end());
#endif

	if (isUpdatingComponents) {
		pendingComponents.push_back(comp);
		return;
	}
	components.push_back(comp);
}

void Game::RemoveComponent(Component* comp)
{
	auto iter = std::find(pendingComponents.begin(), pendingComponents.end(), comp);
	if (iter != pendingComponents.end())
	{
		std::iter_swap(iter, pendingComponents.end() - 1);
		pendingComponents.pop_back();
		return;
	}

	iter = std::find(components.begin(), components.end(), comp);
	if (iter != components.end())
	{
		std::iter_swap(iter, components.end() - 1);
		components.pop_back();
		return;
	}

	assert(false);
}
