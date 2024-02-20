#include "TriangleComponent.h"

#include "core/Game.h"
#include "render/Renderer.h"
#include "render/GeometryData.h"

TriangleComponent::TriangleComponent(Game* game) 
	: DrawComponent(game)
{
}

void TriangleComponent::SetVerts(const void* verts, int vertsSize)
{
	Renderer* renderer = GetGame()->GetRenderer();
	GeometryData::PTR geom = renderer->GetUtils()->GetTriGeom(renderer, verts, vertsSize);
	SetGeometry(geom);
}
