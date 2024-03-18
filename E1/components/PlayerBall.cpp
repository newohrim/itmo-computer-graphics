#include "PlayerBall.h"

#include "core/Game.h"
#include "components/MeshComponent.h"
#include "components/ThirdPersonCamera.h"
#include "render/Renderer.h"
#include "render/MeshLoader.h"
#include "os/Window.h"

#include <iostream>

PlayerBall::PlayerBall(Game* game)
	: CompositeComponent(game)
{
	boundingSphereRadius = 3.0f;
}

void PlayerBall::Initialize(Compositer* parent)
{
	CameraParamsPerspective perspective;
	perspective.aspectRatio = (float)GetGame()->GetWindow()->GetWidth() / GetGame()->GetWindow()->GetHeigth();
	ThirdPersonCamera* camera = new ThirdPersonCamera(GetGame(), perspective, this);

	Renderer* renderer = GetGame()->GetRenderer();
	meshSocket = new CompositeComponent(GetGame(), this);
	MeshComponent* mesh = new MeshComponent(GetGame(), meshSocket);
	mesh->SetShader(GetGame()->GetRenderer()->GetUtils()->GetMeshShader(renderer));
	mesh->SetGeometry(GetGame()->GetRenderer()->GetUtils()->GetSphereGeom(renderer));

	CompositeComponent* tempC = new CompositeComponent(GetGame(), meshSocket);
	MeshComponent* cube = new MeshComponent(GetGame(), tempC);
	cube->SetShader(GetGame()->GetRenderer()->GetUtils()->GetMeshShader(renderer));
	cube->SetGeometry(GetGame()->GetRenderer()->GetUtils()->GetCubeGeom(renderer));
	cube->SetColor(Math::Color{1.0f, 0.0f, 0.0f});
	tempC->SetPosition(Math::Vector3{1.0f, 0.0f, 0.0f});
	tempC->SetScale(Math::Vector3{2.0f, 0.2f, 0.2f});

	/*MeshComponent* rootMesh = nullptr;
	MeshLoader::LoadMesh("assets/flop.fbx", meshSocket, &rootMesh);
	Texture tex(0, L"assets/flopTex.png", renderer);
	if (rootMesh) {
		rootMesh->SetTexture(tex);
	}
	meshSocket->SetScale(Math::Vector3{ 0.01f });
	meshSocket->SetRotation(Math::Quaternion::CreateFromYawPitchRoll(0.0f, Math::Pi / 2, 0.0f));*/

	CompositeComponent::Initialize();
}

void PlayerBall::ProceedInput(InputDevice* inpDevice)
{
	const Math::Vector3 prevPos = GetPosition();
	CompositeComponent::ProceedInput(inpDevice);
	Math::Vector3 moveVec = GetPosition() - prevPos;
	if (moveVec.LengthSquared() < 0.001f) {
		return;
	}
	//std::cout << '{' << GetPosition().x << ", " << GetPosition().y << ", " << GetPosition().z << "}\n";
	const float moveDist = moveVec.Length();
	moveVec.Normalize();
	const Math::Vector3 up = {0.0f, 0.0f, 1.0f};
	const Math::Vector3 right = Math::Vector3::Transform(moveVec, Math::Quaternion::CreateFromAxisAngle(up, -Math::Pi / 2));
	const Math::Vector3 rightLocal = Math::Vector3::Transform(right, meshSocket->GetRotation());
	const Math::Quaternion rot = Math::Quaternion::CreateFromAxisAngle(right, -moveDist);
	meshSocket->SetRotation(Math::Quaternion::Concatenate(rot, meshSocket->GetRotation()));

	std::vector<int> newChilds;
	for (int i = 0; i < sceneObjects.size(); ++i) {
		const float dist = (GetPosition() - sceneObjects[i]->GetPosition()).Length();
		if (dist > boundingSphereRadius * GetScale().x + sceneObjects[i]->boundingSphereRadius * sceneObjects[i]->GetScale().x) {
			continue;
		}
		newChilds.push_back(i);
	}
	//std::cout << '{' << GetPosition().x << ", " << GetPosition().y << ", " << GetPosition().z << "}\n";
	for (int idx : newChilds) {
		GetGame()->RemoveComponent(sceneObjects[idx]);
		/*Math::Vector3 pos = sceneObjects[idx]->GetPosition() - GetPosition();
		pos = Math::Vector3::Transform(pos, -meshSocket->GetRotation());*/
		
		//{
		//	CompositeComponent* temp = new CompositeComponent(GetGame());
		//	MeshComponent* mesh = new MeshComponent(GetGame(), temp);
		//	Renderer* renderer = GetGame()->GetRenderer();
		//	mesh->SetShader(GetGame()->GetRenderer()->GetUtils()->GetMeshShader(renderer));
		//	mesh->SetGeometry(GetGame()->GetRenderer()->GetUtils()->GetSphereGeom(renderer));
		//	temp->SetPosition(pos);
		//	//meshSocket->AddChild({ temp });
		//}
		//{
		//	CompositeComponent* temp = new CompositeComponent(GetGame(), this);
		//	MeshComponent* mesh = new MeshComponent(GetGame(), temp);
		//	Renderer* renderer = GetGame()->GetRenderer();
		//	mesh->SetShader(GetGame()->GetRenderer()->GetUtils()->GetMeshShader(renderer));
		//	mesh->SetGeometry(GetGame()->GetRenderer()->GetUtils()->GetSphereGeom(renderer));
		//	mesh->SetColor(Math::Color{ 0.0f, 1.0f, 0.0f });
		//	temp->SetPosition(pos);
		//	meshSocket->AddChild({ temp });
		//}

		//sceneObjects[idx]->SetPosition(pos);
		////sceneObjects[idx]->SetPosition(Math::Vector3::Zero);
		//sceneObjects[idx]->SetRotation(Math::Quaternion::Concatenate(sceneObjects[idx]->GetRotation(), -meshSocket->GetRotation()));
		//meshSocket->AddChild({sceneObjects[idx]});

		const Math::Matrix& local = meshSocket->GetWorldTransform(this);
		const Math::Matrix& other = sceneObjects[idx]->GetWorldTransform();
		const Math::Matrix res = other * local.Invert();
		sceneObjects[idx]->SetWorldTransform(res);
		meshSocket->AddChild({sceneObjects[idx]});
	}
	for (int i = newChilds.size() - 1; i >= 0; --i) {
		sceneObjects.erase(sceneObjects.begin() + newChilds[i]);
	}
}
