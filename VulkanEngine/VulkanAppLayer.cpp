#include "VulkanAppLayer.h"
#include "Render/Renderer.h"
#include "Application/ImGuiTextureWrapper.h"
#include <ImGui/imgui.h>
glm::vec3 traslate = glm::vec3(0,0.f,0.f);
glm::vec3 traslate2 = glm::vec3(2, 0.f, 0.f);
void VulkanAppLayer::onUpdate(float deltaTime)
{
	Render::getInstance()->BeginScene(camera.get());
	glm::mat4 model1(1.0f);
	model1 = glm::translate(model1, traslate);
	Render::getInstance()->addMeshRender(&renderMesh2, model1);
	
	Render::getInstance()->EndScene();



	Render::getInstance()->BeginScene(camera.get(), renderTarget);
	glm::mat4 model2(1.0f);
	model2 = glm::translate(model2, traslate2);
	Render::getInstance()->addMeshRender(&renderMesh, model2);
	if (Input::isKeyPressed(KeyCode::Space)) {
		glm::mat4 model3(1.0f);
		model3 = glm::translate(model3, glm::vec3(2.0f, 0.0f, 0.0f));
		Render::getInstance()->addMeshRender(&renderMesh3, model3);
	}
	Render::getInstance()->EndScene();
}

void VulkanAppLayer::onEvent()
{
}

void VulkanAppLayer::onImGuiRender()
{
	ImGui::Begin("Vulkan App Layer");
	ImVec2 avail = ImGui::GetContentRegionAvail(); // spazio interno disponibile
	void* textureId = renderTargetTextureWrapper ? renderTargetTextureWrapper->getTextureID() : nullptr;
	if (textureId)
		ImGui::Image(textureId, avail, ImVec2(0, 1), ImVec2(1, 0));
	else
		ImGui::Dummy(avail);
	ImGui::End();
	ImGui::Begin("Controlli");
	ImGui::DragFloat3("translate mesh", &traslate.x, 0.2, -10.0f, 10.0f);
	ImGui::DragFloat3("translate mesh2", &traslate2.x, 0.2, -10.0f, 10.0f);
	ImGui::End();
}

void VulkanAppLayer::onAttach()
{
	renderTarget = RenderTarget::create(1600, 900);
	renderTargetTextureWrapper = ImGuiTextureWrapper::create(renderTarget);
	// Testing meta shader
	std::shared_ptr<MetaShader> metaShader = AssetManager::instance().addShader("default_shader");

	camera = std::make_shared<Perspective>(45.0f, 16/9, 0.1f, 100.0f);
	//camera = std::make_shared<OnYuu::Orthographic>(-10,10,-10,10,0.1,-100);
	camera->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
	//shader2 = Shader::create("Asset/VkShader/generated-ver.o", "Asset/VkShader/generated-frag.o");

	auto mat = AssetManager::instance().addMaterial("default_material", std::make_shared<Material>("default"));
	auto mat2 = AssetManager::instance().addMaterial("default_material2", std::make_shared<Material>("default"));


	auto texture = AssetManager::instance().addTexture("gatto",Texture::createTexture("Asset/Texture/gatto.png"));
	auto texture2 = AssetManager::instance().addTexture("ai", Texture::createTexture("Asset/Texture/ai.png"));

	mat->set("tex", texture2);
	mat->set("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	mat2->set("tex", texture);
	mat2->set("color", glm::vec4(.0f, 0.0f, 1.0f, 1.0f));


	renderMesh.setMaterialID("default_material2");
	renderMesh.setMesh(AssetManager::instance().getMeshPtr("sphere"));
	renderMesh2.setMesh(AssetManager::instance().getMeshPtr("cube"));
	renderMesh2.setMaterialID("default_material2");
	renderMesh3.setMesh(AssetManager::instance().getMeshPtr("cylinder"));
	renderMesh3.setMaterialID("default_material2");	
}

void VulkanAppLayer::onDetach()
{
}
