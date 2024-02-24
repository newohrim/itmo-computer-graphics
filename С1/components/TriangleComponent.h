#pragma once

#include "render/DrawComponent.h"

class TriangleComponent : public DrawComponent
{
public:
	TriangleComponent(Game* game);

	void SetVerts(const void* verts, int vertsSize);
};
