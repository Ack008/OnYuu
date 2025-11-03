#include <iostream>
#include "Prova.h"
#include "FirstCustomComponent.h"
#include "Transform.h"
#include "MeshComponent.h"
#include "OpenGLShader.h"
#include "Camera.h"
#include "Collider.h"
#include "RigidBody.h"
#include "AssetManager.h"
Prova::Prova()
	:Scene()
{
	

	RenderMeshComponent& renderMesh2 = Player.addComponent<RenderMeshComponent>();
	renderMesh2.material = AssetManager::instance().getMaterial("defaultMaterial");
	renderMesh2.renderingType = RenderingTypeEnum::TRIANGLE;
	renderMesh2.mesh = AssetManager::instance().getMesh("squareMesh");
	Player.addComponent<BoxCollider>();

	RenderMeshComponent& asteroideRender = Asteroide.addComponent<RenderMeshComponent>();
	asteroideRender.material = AssetManager::instance().getMaterial("defaultMaterial");;
	asteroideRender.renderingType = RenderingTypeEnum::TRIANGLE;
	asteroideRender.mesh = AssetManager::instance().getMesh("triangoloMesh");
	Asteroide.addComponent<BoxCollider>();
	Asteroide.getComponent<Trasform>().position = glm::vec3(800, 400, 0);
	Asteroide.getComponent<Trasform>().scale = glm::vec3(100, 100, 1);
	//Asteroide.addComponent<RigidBody>(RigidBody::BodyType::DYNAMIC, 10.0f).setUseGravity(true);
	
	

	Player.addComponent<FirstCustomComponent>();
	Camera.addComponent<Orthographic>(0,1600,0,900,0,-20);
}

void Prova::OnResize(uint32_t width, uint32_t height)
{
	std::cout << "Resize Prova: " << width << "x" << height << std::endl;
	Camera.getComponent<Orthographic>().OnResize(width, height);
}
