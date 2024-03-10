#pragma once

#include <string>

class Game;
class CompositeComponent;

class MeshLoader {
public:
	static bool LoadMesh(const std::string& path, CompositeComponent* parent, class MeshComponent** outMesh = nullptr);
};
