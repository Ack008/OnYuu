#include "VulkanAppLayer.h"
#include "Render/Renderer.h"
#include <ImGui/imgui.h>
glm::vec3 traslate = glm::vec3(0,0.f,0.f);
void VulkanAppLayer::onUpdate(float deltaTime)
{
	Render::getInstance()->BeginScene(camera.get());
	glm::mat4 model1(1.0f);
	model1 = glm::translate(model1, traslate);
	if (Input::isKeyPressed(KeyCode::Space))
	{
		glm::mat4 model2(1.0f);
		model2 = glm::translate(model2, glm::vec3(-2.0f, 0.0f, 0.0f));
		Render::getInstance()->addMeshRender(&renderMesh, glm::mat4(1.0f));
	}
	Render::getInstance()->addMeshRender(&renderMesh2, model1);
	glm::mat4 model3(1.0f);
	model3 = glm::translate(model3, glm::vec3(-2.0f, 0.0f, 0.0f));
	Render::getInstance()->addMeshRender(&renderMesh3, model3);
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
	// Testing meta shader
	std::shared_ptr<MetaShader> metaShader = MetaShader::create("Asset/Meta-shader/firstShader.meta");

	camera = std::make_shared<Perspective>(45.0f, 16/9, 0.1f, 100.0f);
	//camera = std::make_shared<OnYuu::Orthographic>(-10,10,-10,10,0.1,-100);
	camera->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
	//shader2 = Shader::create("Asset/VkShader/generated-ver.o", "Asset/VkShader/generated-frag.o");

	auto mat = AssetManager::instance().addMaterial("default_material", std::make_shared<Material>(metaShader));
	auto mat2 = AssetManager::instance().addMaterial("default_material2", std::make_shared<Material>(metaShader));


	auto texture = AssetManager::instance().addTexture("gatto", Texture::createTexture("Asset/Texture/gatto.png"));
	auto texture2 = AssetManager::instance().addTexture("ai", Texture::createTexture("Asset/Texture/ai.png"));

	mat->set("tex", texture2);
	mat->set("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	mat2->set("tex", texture);
	mat2->set("color", glm::vec4(.0f, 0.0f, 1.0f, 1.0f));


	renderMesh.material = mat;
	renderMesh.mesh = AssetManager::instance().getMeshPtr("sphere");
	renderMesh2.mesh = AssetManager::instance().getMeshPtr("cube");
	renderMesh2.material = mat2;
	renderMesh3.mesh = AssetManager::instance().getMeshPtr("cylinder");
	renderMesh3.material = mat2;

}

void VulkanAppLayer::onDetach()
{
}
