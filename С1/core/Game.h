#pragma once

#include <memory>
#include <vector>
#include <string>
#include <chrono>

#include "Component.h"
#include "render/Renderer.h"

class Window;
class Renderer;
class PaddleComponent;

class Game {
	friend Component::Component(Game*, Compositer*);
	friend Component::~Component();

public:
	bool Initialize(const std::string name, int windowWidth, int windowHeight);
	void ProcessInput();
	void Shutdown();
	void RunLoop();

	void Restart();
	void LoadData();
	void UnloadData();
	void UpdateGame();
	void GenerateOutput();

	Renderer* GetRenderer() const { return renderer.get(); }

	Window* GetWindow() const { return window; }

private:
	void AddComponent(Component* comp);
	void RemoveComponent(Component* comp);

private:
	std::unique_ptr<Renderer> renderer;

	std::vector<Component*> components;
	std::vector<Component*> pendingComponents;

	Window* window = nullptr;

	std::chrono::time_point<std::chrono::steady_clock> prevTime;
	float totalTime = 0.0f;
	float deltaTime = 0.0f;
	int frameNum = 0;

	bool isUpdatingComponents = false;
	bool isRunning = true;
};
