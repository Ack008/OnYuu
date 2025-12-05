#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include "OpenGLBatchRender.h"
#include "Render/BatchRenderer.h"
#include "Application/Application.h"
#include "Application/AssetManager.h"
#define INITIAL_BUFFER_SIZE_MULTIPLIER 5

OpenGLBatchRender::~OpenGLBatchRender()
{
}

OpenGLBatchRender::OpenGLBatchRender()
{
	// Non chiamare funzioni OpenGL qui: il contesto potrebbe non essere pronto.
	// Inizializziamo il VAO in modo lazy durante draw().
	vao = 0;
}
void OpenGLBatchRender::submit()
{
	BatchRender::submit();
	for (RenderScene& scene : renderScenes) {
		drawScene(scene);
	}
	renderScenes.clear();
}

void OpenGLBatchRender::BeginFrame()
{
	glClearColor(0.1, 0.2, 0.7, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Pulisce il buffer colore
}

void OpenGLBatchRender::drawSkybox(RenderScene& scene)
{
	glDepthFunc(GL_LEQUAL);

	Camera& camera = *scene.activeCamera;
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	view = glm::mat4(glm::mat3(glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getTarget() - camera.getPosition(), camera.getUpVector())));
	uint32_t width = Application::getInstance()->getWindow()->getWidth();
	uint32_t height = Application::getInstance()->getWindow()->getHeight();
	projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
	AssetManager& am = AssetManager::instance();
	auto cubeMeshPtr = am.getMeshPtr("cubeMesh");
	Mesh* cubeMesh = cubeMeshPtr ? cubeMeshPtr.get() : nullptr;
	if (!cubeMesh) return;

	if (meshGPUmap.find(cubeMesh) == meshGPUmap.end()) {
		meshGPUmap[cubeMesh] = MeshGPUusage();
		meshGPUmap[cubeMesh].setMesh(cubeMesh);
	}
	meshGPUmap[cubeMesh].uploadToGPU();
	Material* currentSkyboxMaterial = AssetManager::instance().getMaterial("skyboxMaterial");
	meshGPUmap[cubeMesh].bind();
	currentSkyboxMaterial->bind();
	currentSkyboxMaterial->set("view", view);
	currentSkyboxMaterial->set("projection", projection);
	currentSkyboxMaterial->apply();
	if (scene.skybox && scene.skybox->cubeMap) scene.skybox->cubeMap->bind();

	GLsizei vertexCount = static_cast<GLsizei>(cubeMesh->position.size());
	if (vertexCount > 0) {
		meshGPUmap[cubeMesh].draw(TRIANGLE);
	}

	glDepthFunc(GL_LESS);
}
void OpenGLBatchRender::drawScene(RenderScene& scene)
{
	auto batches = scene.batches;
	for (const auto& pair : batches) {
		const BatchCouple& key = pair.first;

		// Usa lo shader
		if (key.first) {
			key.first->bind();
			key.first->apply();
			// Imposta la matrice della camera
		}
		for (RenderData rd : pair.second) {
			if (key.first) {
				key.first->set("u_modelMatrix", rd.model);
				key.first->apply();
			}

			// rd.renderMesh->mesh è ora std::shared_ptr<Mesh>
			if (!rd.renderMesh || !rd.renderMesh->mesh) continue;
			Mesh* mesh = rd.renderMesh->mesh.get();
			if (!mesh) continue;

			if (meshGPUmap.find(mesh) == meshGPUmap.end()) {
				meshGPUmap[mesh] = MeshGPUusage();
				meshGPUmap[mesh].setMesh(mesh);
			}
			meshGPUmap[mesh].uploadToGPU();

			meshGPUmap[mesh].bind();
			// Determina il tipo di disegno

			GLsizei vertexCount = static_cast<GLsizei>(mesh->position.size());
			if (vertexCount > 0) {
				meshGPUmap[mesh].draw(key.second);
			}
		}

	}

	if (scene.skybox) {
		drawSkybox(scene);
	}
}
void OpenGLBatchRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model)
{
	BatchRender::addMeshRender(mesh, model);
}
void OpenGLBatchRender::setSkyBox(SkyBoxComponent* skybox)
{
	BatchRender::setSkyBox(skybox);
	this->skybox = skybox;
	if (skybox && skybox->cubeMap) skybox->cubeMap->bind();
}
