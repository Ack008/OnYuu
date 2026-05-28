#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include "Render/BatchRenderer.h"
#include "Core/Shader.h"
#include "Platform/OpenGL/MeshGPUusage.h"
namespace OnYuu {

	class OpenGLBatchRender
		: public BatchRender
	{
	public:
		virtual void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) override;
		virtual void setSkyBox(SkyBoxComponent* skybox) override;
		virtual void submit() override;
		virtual void Shutdown() override;
		virtual void BeginFrame() override;
		virtual void registeringCallbacks() override {}
		void BindGlobalVariables(OnYuu::BatchRender::RenderScene& scene);
		OpenGLBatchRender();
		~OpenGLBatchRender();
	private:
		void drawSkybox(RenderScene& scene);
		void drawScene(RenderScene& scene);
	private:
		struct CameraInfo {
			glm::mat4 view;
			glm::mat4 projection;
			glm::vec4 position;
		};
		struct Light {
			glm::vec4 position;
			float intensity;
			glm::vec4 color;
		};
		struct LightInfo {
			int count;
			Light lights[128];
		};
		std::shared_ptr<UniformBuffer> cameraUBO;
		std::shared_ptr<UniformBuffer> lightsUBO;

		struct VertexBuffer {
			GLuint vbo;
			size_t usedSize = 0;
			size_t size = 0;
		};
		GLuint vao;
		std::unordered_map<Mesh*, MeshGPUusage> meshGPUmap;
		SkyBoxComponent* skybox = nullptr;
		inline static float skyboxVertices[] =
		{
			//   Coordinates
			-1.0f, -1.0f,  1.0f,//        7--------6
			 1.0f, -1.0f,  1.0f,//       /|       /|
			 1.0f, -1.0f, -1.0f,//      4--------5 |
			-1.0f, -1.0f, -1.0f,//      | |      | |
			-1.0f,  1.0f,  1.0f,//      | 3------|-2
			 1.0f,  1.0f,  1.0f,//      |/       |/
			 1.0f,  1.0f, -1.0f,//      0--------1
			-1.0f,  1.0f, -1.0f
		};

		inline static unsigned int skyboxIndices[] =
		{
			// Right
			1, 2, 6,
			6, 5, 1,
			// Left
			0, 4, 7,
			7, 3, 0,
			// Top
			4, 5, 6,
			6, 7, 4,
			// Bottom
			0, 3, 2,
			2, 1, 0,
			// Back
			0, 1, 5,
			5, 4, 0,
			// Front
			3, 7, 6,
			6, 2, 3
		};

	};

}