#include "PlayerBall.h"

#include "core/Game.h"
#include "components/MeshComponent.h"

PlayerBall::PlayerBall(Game* game)
	: CompositeComponent(game)
{
}

void PlayerBall::Initialize(Compositer* parent)
{
	MeshComponent* mesh = new MeshComponent(GetGame(), this);
	Renderer* renderer = GetGame()->GetRenderer();
	mesh->SetShader(GetGame()->GetRenderer()->GetUtils()->GetMeshShader(renderer));
	mesh->SetGeometry(GetGame()->GetRenderer()->GetUtils()->GetCubeGeom(renderer));
}

void PlayerBall::ProceedInput(InputDevice* inpDevice)
{

}
