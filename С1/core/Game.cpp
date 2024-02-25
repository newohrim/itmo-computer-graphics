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

#include "components/QuadComponent.h"
#include "components/CompositeComponent.h"
#include "components/PaddleComponent.h"
#include "components/BallComponent.h"

#include "input/InputDevice.h"
#include "ScoreBoard.h"

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
	float clearColor[] = {0.1f, 0.1f, 0.1f, 0.1f};
	renderer->SetClearColor(clearColor);

	LoadData();

	globalInputDevice = new InputDevice(this);

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
		return;
	}

	for (Component* comp : components) {
		comp->ProceedInput(globalInputDevice);
	}
}

void Game::Shutdown()
{
	delete(globalInputDevice);
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

void Game::Restart()
{
	UnloadData();
	LoadData();
}

void Game::LoadData()
{
	PaddleComponent* paddleA = new PaddleComponent(this, {10.0f, 60.0f}, true);
	paddleA->Initialize();
	paddleA->SetPosition({60, 400});
	PaddleComponent* paddleB = new PaddleComponent(this, {10.0f, 60.0f}, false);
	paddleB->Initialize();
	paddleB->SetPosition({window->GetWidth() - 60.0f, 400});
	BallComponent* ball = new BallComponent(this, paddleA, paddleB, {20.0f, 20.0f});
	ball->Initialize();
	ball->SetPosition({400, 400});
	ScoreBoard::UpdateScoreBoard(window);
}

void Game::UnloadData()
{
	while (!components.empty())
	{
		delete components.back();
	}
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

//#ifdef _WIN32
//		WCHAR text[256];
//		swprintf_s(text, TEXT("FPS: %f"), fps);
//		SetWindowText(wndGetHWND(window), text);
//#endif

		frameNum = 0;
	}

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
