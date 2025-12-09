#include "VulkanAppLayer.h"
#include "Render/Renderer.h"
#include <ImGui/imgui.h>
void VulkanAppLayer::onUpdate(float deltaTime)
{
	Render::getInstance()->BeginScene(camera.get());
	Render::getInstance()->addMeshRender(&renderMesh, glm::mat4(1.0f));
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
		{ 0.0f, -0.5f, 0.0f },
		{ 0.5f, 0.5f, 0.0f },
		{ -0.5f, 0.5f, 0.0f }
	};
	triangle->indices = { 0, 1, 2 };
	triangle->color = {
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f }
	};
	triangle->normal = {
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	renderMesh.mesh = triangle;
	camera = std::make_shared<Orthographic>(-1,1, -1,1, 0.1f, 100.0f);
	shader = Shader::create("Asset/vulkan-shader/vert.o", "Asset/vulkan-shader/frag.o");
	
}

void VulkanAppLayer::onDetach()
{
}
