#include "Prova.h"
#include "FirstCustomComponent.h"
Prova::Prova()
	:Scene()
{
	

	RenderMeshComponent& renderMesh2 = Player.addComponent<RenderMeshComponent>();
	renderMesh2.material = AssetManager::instance().getMaterial("defaultMaterial");
	renderMesh2.renderingType = RenderingTypeEnum::TRIANGLE;
	renderMesh2.mesh = AssetManager::instance().getMesh("squareMesh");
	Player.addComponent<BoxCollider>();

	RenderMeshComponent& asteroideRender = Asteroide.addComponent<RenderMeshComponent>();
	asteroideRender.material = AssetManager::instance().getMaterial("defaultMaterial");;
	asteroideRender.renderingType = RenderingTypeEnum::TRIANGLE;
	asteroideRender.mesh = AssetManager::instance().getMesh("hermiteMesh1");
	Asteroide.addComponent<BoxCollider>();
	Asteroide.getComponent<Trasform>().position = glm::vec3(800, 400, 0);
	Asteroide.getComponent<Trasform>().scale = glm::vec3(100, 100, 1);
	Asteroide.getComponent<TagComponent>().tag = "Asteroide";
	//Asteroide.addComponent<RigidBody>(RigidBody::BodyType::DYNAMIC, 10.0f).setUseGravity(true);
	
	//sfondo
	Background2DRender& backgroundRender = background.addComponent<Background2DRender>();
	backgroundRender.material = (AssetManager::instance().getMaterial("backgroundMaterial"));
	background.getComponent<Trasform>().scale = glm::vec3(300, 300, 1);

	Player.addComponent<FirstCustomComponent>();
	Camera.addComponent<Orthographic>(0,1600,0,900,0,-20);
}

void Prova::OnResize(uint32_t width, uint32_t height)
{
	std::cout << "Resize Prova: " << width << "x" << height << std::endl;
	Camera.getComponent<Orthographic>().OnResize(width, height);
}
