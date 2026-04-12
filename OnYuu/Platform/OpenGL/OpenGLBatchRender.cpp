#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include "OpenGLBatchRender.h"
#include "Render/BatchRenderer.h"
#include "Application/Application.h"
#include "Application/AssetManager.h"
#include "Platform/OpenGL/OpenGLRenderTarget.h"
#define INITIAL_BUFFER_SIZE_MULTIPLIER 5
namespace OnYuu {

	OpenGLBatchRender::~OpenGLBatchRender()
	{
	}

	OpenGLBatchRender::OpenGLBatchRender()
	{
		// Non chiamare funzioni OpenGL qui: il contesto potrebbe non essere pronto.
		// Inizializziamo il VAO in modo lazy durante draw().
		vao = 0;
		cameraUBO = UniformBuffer::create(2, sizeof(CameraInfo));
		lightsUBO = UniformBuffer::create(1, sizeof(LightInfo));

	}
	void OpenGLBatchRender::submit()
	{
		BatchRender::submit();

		renderScenes.clear();
	}

	void OpenGLBatchRender::BeginFrame()
	{
		for (RenderScene& scene : renderScenes) {
			if (scene.target) {
				auto glTarget = std::dynamic_pointer_cast<OpenGLRenderTarget>(scene.target);
				if (glTarget) {
					glTarget->bind();
					glViewport(0, 0, static_cast<GLsizei>(glTarget->getWidth()), static_cast<GLsizei>(glTarget->getHeight()));
				}
				else {
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}
			}
			else {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				uint32_t width = Application::getInstance()->getWindow()->getWidth();
				uint32_t height = Application::getInstance()->getWindow()->getHeight();
				glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
			}

			glClearColor(0.1f, 0.2f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			BindGlobalVariables(scene);
			drawScene(scene);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		uint32_t width = Application::getInstance()->getWindow()->getWidth();
		uint32_t height = Application::getInstance()->getWindow()->getHeight();
		glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}

	void OpenGLBatchRender::BindGlobalVariables(OnYuu::BatchRender::RenderScene& scene)
	{
		CameraInfo camInfo;
		camInfo.position = glm::vec4(scene.activeCamera->getPosition(), 1.0f);
		camInfo.view = scene.activeCamera->getViewMatrix();
		camInfo.projection = scene.activeCamera->getProjectionMatrix();
		cameraUBO->bind();
		cameraUBO->updateData(&camInfo, sizeof(CameraInfo), 0);
		LightInfo lightInfo;
		lightInfo.count = static_cast<int>(scene.sceneLight.size());
		for (size_t i = 0; i < scene.sceneLight.size() && i < 128; ++i) {
			const auto& light = scene.sceneLight[i];
			lightInfo.lights[i].position = glm::vec4(light.position, 1.0f);
			lightInfo.lights[i].intensity = light.light.intensity;
			lightInfo.lights[i].color = light.light.color;
		}
		lightsUBO->bind();
		lightsUBO->updateData(&lightInfo, sizeof(LightInfo), 0);
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
					key.first->set("u_model", rd.model);
					key.first->apply();
				}

				// rd.renderMesh->mesh è ora std::shared_ptr<Mesh>
				if (!rd.renderMesh || !rd.renderMesh->getMesh()) continue;
				Mesh* mesh = rd.renderMesh->getMesh().get();
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

	void OpenGLBatchRender::Shutdown()
	{
		for (auto& pair : meshGPUmap) {
			pair.second.cleanup();
		}
		if (vao != 0) {
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}
	}
}