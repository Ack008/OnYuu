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
	
	renderMesh.mesh = AssetManager::instance().getMeshPtr("cube");
	camera = std::make_shared<Perspective>(45.0f, 16/9, 0.1f, 100.0f);
	//camera = std::make_shared<OnYuu::Orthographic>(-10,10,-10,10,0.1,100);
	camera->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	shader2 = Shader::create("Asset/vulkan-shader/texture-ssbo-vert.o", "Asset/vulkan-shader/texture-ssbo-frag.o");

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
	renderMesh2.mesh = AssetManager::instance().getMeshPtr("cube");
	renderMesh2.material = mat2;

}

void VulkanAppLayer::onDetach()
{
}
