#include "VulkanAppLayer.h"
#include "Render/Renderer.h"
#include <ImGui/imgui.h>
glm::vec3 traslate = glm::vec3(0,0.f,0.f);
void VulkanAppLayer::onUpdate(float deltaTime)
{
	Render::getInstance()->BeginScene(camera.get());
	glm::mat4 model1(1.0f);
	model1 = glm::translate(model1, traslate);
	Render::getInstance()->addMeshRender(&renderMesh, glm::mat4(1.0f));
	Render::getInstance()->addMeshRender(&renderMesh2, model1);
	Render::getInstance()->EndScene();
}

void VulkanAppLayer::onEvent()
{
}

void VulkanAppLayer::onImGuiRender()
{
	ImGui::Begin("Vulkan App Layer");
	ImGui::DragFloat3("translate mesh", &traslate.x, 0.2, -10.0f, 10.0f);
	ImGui::End();
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
		{ -1.f, -1.f, 2.0f },
		{ -1.f, 1.f, 2.0f },
		{  1.f, 1.f, 2.0f  },
		{ 1.f, -1.0f, 2.0f },
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
	camera = std::make_shared<Orthographic>(-5,5, -5,5, -100.f, .3f);
	camera->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	shader2 = Shader::create("Asset/vulkan-shader/texture-vert.o", "Asset/vulkan-shader/texture-frag.o");

	auto mat = AssetManager::instance().addMaterial("default_material", std::make_shared<Material>(shader2));
	auto mat2 = AssetManager::instance().addMaterial("default_material2", std::make_shared<Material>(shader2));


	auto texture = AssetManager::instance().addTexture("gatto", Texture::createTexture("Asset/Texture/gatto.png"));
	auto texture2 = AssetManager::instance().addTexture("ai", Texture::createTexture("Asset/Texture/ai.png"));

	mat->set("specular", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mat->set("diffuse",glm::vec3(1,0,1));
	mat->set("diffuseMap", texture2);

	mat2->set("specular", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mat2->set("diffuse", glm::vec3(1, 0, 0));
	mat2->set("diffuseMap", texture);

	renderMesh.material = mat;
	renderMesh2.mesh = triangle2;
	renderMesh2.material = mat2;
	camera->setPosition(glm::vec3(0.0f, 0.0f, 2.0f));

}

void VulkanAppLayer::onDetach()
{
}
