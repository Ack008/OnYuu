#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "Scene.h"
#include "MeshComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Transform.h"
#include "Renderer.h"
entt::entity Scene::createEntity()
{
	return reg->create();
}

void Scene::update(float dt)
{
	for (auto comp : componentsList) {
		comp->update(dt);
	}
	auto cameraView = reg->view<Orthographic>();
	for (auto [entity, camera] : cameraView.each()) {
		Render::getInstance()->setCameraMatrix(camera.getVPMatrix());
		break; // Use the first found camera
	}
	auto view = reg->view<Trasform, RenderMeshComponent>();
	for (auto [entity, transform, mesh] : view.each()) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model,transform.position);
		model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
		model = glm::scale(model, transform.scale);
		Render::getInstance()->addMeshRender(&mesh, model);
	}

}

void Scene::start()
{
	for (auto comp : componentsList) {
		comp->start();
	}
}

Scene::~Scene()
{
	free(reg);
}


