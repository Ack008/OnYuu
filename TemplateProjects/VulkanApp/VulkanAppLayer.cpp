#include "VulkanAppLayer.h"
#include "Render/Renderer.h"
#include <ImGui/imgui.h>
void VulkanAppLayer::onUpdate(float deltaTime)
{
	Render::getInstance()->BeginScene(camera.get());
	Render::getInstance()->addMeshRender(&renderMesh, glm::mat4(1.0f));
	Render::getInstance()->addMeshRender(&renderMesh2, glm::mat4(1.0f));
	Render::getInstance()->EndScene();
}

void VulkanAppLayer::onEvent()
{
}

void VulkanAppLayer::onImGuiRender()
{
	
}

void VulkanAppLayer::onAttach()
{
	triangle = std::make_shared<Mesh>();
	triangle->position = {
		{ -0.4f, -.3f, 0.0f },
		{ -.4f, .3f, 0.0f },
		{  .4f, .4f, 0.0f  },
		{ .4f, -1.0f, 0.0f },
	};
	triangle->indices = { 0, 1, 3 ,
		1,2,3};
	triangle->color = {
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f }

	};
	triangle->texCoord = {
		{ 0.0f, 0.0f },
		{ .0f, 1.0f },
		{ 1.0f, 1.0f },
		{  1.0f, 0.0f  }
	};
	triangle->normal = {
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{  0.0f, 0.0f, 1.0f  }
	};


	triangle2 = std::make_shared<Mesh>();
	triangle2->position = {
		{ -0.1f, -.3f, 0.0f },
		{ -.1f, .3f, 0.0f },
		{  .9f, .9f, 0.0f  },
		{ .9f, -1.0f, 0.0f },
	};
	triangle2->indices = { 0, 1, 3 ,
		1,2,3 };
	triangle2->color = {
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f }

	};
	triangle2->texCoord = {
		{ 0.0f, 0.0f },
		{ .0f, 1.0f },
		{ 1.0f, 1.0f },
		{  1.0f, 0.0f  }
	};
	triangle2->normal = {
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{  0.0f, 0.0f, 1.0f  }
	};
	renderMesh.mesh = triangle;
	camera = std::make_shared<Orthographic>(-1,1, -1,1, 0.1f, 100.0f);
	shader = Shader::create("Asset/vulkan-shader/vert.o", "Asset/vulkan-shader/frag.o");
	auto mat = AssetManager::instance().addMaterial("default_material", std::make_shared<Material>(shader));
	mat->set("specular", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mat->apply();
	renderMesh.material = mat;
	renderMesh2.mesh = triangle2;
	renderMesh2.material = mat;
}

void VulkanAppLayer::onDetach()
{
}
