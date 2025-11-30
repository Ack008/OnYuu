#include "Scene3D.h"
#include "scripts/CameraScript.h"
#include "scripts/Controller.h"
Scene3D::Scene3D()
	:Scene()
{

	
}

void Scene3D::initializeScene()
{
	Controller & controllerComp = controller.addComponent<Controller>();

	camera.addComponent<Perspective>(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	camera.addComponent<CameraScript>(controllerComp);
	camera.getComponent<TagComponent>().tag = "MainCamera";
	//defining cube
	auto& renderSquare = cube.addComponent<RenderMeshComponent>();
	renderSquare.mesh = AssetManager::instance().getMesh("cubeMesh");
	renderSquare.material = AssetManager::instance().getMaterial("defaultMaterial");
	cube.addComponent<BoxCollider>();
	cube.getComponent<TagComponent>().tag = "cube";
	cube.getComponent<Trasform>().position = glm::vec3(0.0f, 0.0f, -5.0f);

	//defining sphere
	auto& renderSphere = sphere.addComponent<RenderMeshComponent>();
	renderSphere.mesh = AssetManager::instance().getMesh("sphereMesh");
	renderSphere.material = AssetManager::instance().getMaterial("defaultMaterial");
	sphere.getComponent<Trasform>().position = glm::vec3(2.0f, 0.0f, -5.0f);
	sphere.getComponent<TagComponent>().tag = "Sphere";
	sphere.addComponent<BoxCollider>();
}

void Scene3D::OnResize(uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;
	if (camera.hasComponent<Perspective>()) {
		camera.getComponent<Perspective>().OnResize(width, height);
	}
}
void Scene3D::initializeMaterials()
{
	createCube();
	createSphere(glm::vec4(0.0, 1.0, 0.0, 1.0));
	std::shared_ptr<Shader> shader = Shader::create("MeshVertexShader.glsl", "fragmentShaderC.glsl");
	AssetManager::instance().addMaterial("defaultMaterial", std::make_shared<Material>(shader));
}
void Scene3D::createCube()
{
	AssetManager::instance().addMesh("cubeMesh", std::make_shared<Mesh>(Mesh()));
	Mesh *mesh = AssetManager::instance().getMesh("cubeMesh");
	mesh->position.push_back(glm::vec3(-1.0, -1.0, 1.0));
	mesh->color.push_back(glm::vec4(1.0, 0.0, 0.0, 1));
	mesh->position.push_back(glm::vec3(1.0, -1.0, 1.0));
	mesh->color.push_back(glm::vec4(0.0, 1.0, 0.0, 1));
	mesh->position.push_back(glm::vec3(1.0, 1.0, 1.0));
	mesh->color.push_back(glm::vec4(0.0, 0.0, 1.0, 1));
	mesh->position.push_back(glm::vec3(-1.0, 1.0, 1.0));
	mesh->color.push_back(glm::vec4(1.0, 0.0, 1.0, 1.0));
	// back
	mesh->position.push_back(glm::vec3(-1.0, -1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));
	mesh->position.push_back(glm::vec3(1.0, -1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));
	mesh->position.push_back(glm::vec3(1.0, 1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));
	mesh->position.push_back(glm::vec3(-1.0, 1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));





	mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
	mesh->indices.push_back(2); mesh->indices.push_back(3); mesh->indices.push_back(0);
	mesh->indices.push_back(1); mesh->indices.push_back(5); mesh->indices.push_back(6);
	mesh->indices.push_back(6); mesh->indices.push_back(2); mesh->indices.push_back(1);
	mesh->indices.push_back(7); mesh->indices.push_back(6); mesh->indices.push_back(5);
	mesh->indices.push_back(5); mesh->indices.push_back(4); mesh->indices.push_back(7);
	mesh->indices.push_back(4); mesh->indices.push_back(0); mesh->indices.push_back(3);
	mesh->indices.push_back(3); mesh->indices.push_back(7); mesh->indices.push_back(4);
	mesh->indices.push_back(4); mesh->indices.push_back(5); mesh->indices.push_back(1);
	mesh->indices.push_back(1); mesh->indices.push_back(0); mesh->indices.push_back(4);
	mesh->indices.push_back(3); mesh->indices.push_back(2); mesh->indices.push_back(6);
	mesh->indices.push_back(6); mesh->indices.push_back(7); mesh->indices.push_back(3);
	int nv = mesh->position.size();

	mesh->indices.push_back(nv - 1);
}

void Scene3D::createSphere(glm::vec4 color)
{
	AssetManager::instance().addMesh("sphereMesh", std::make_shared<Mesh>(Mesh()));
	Mesh* mesh = AssetManager::instance().getMesh("sphereMesh");
	glm::vec3 centro = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 raggio = glm::vec3(1.0, 1.0, 1.0);

	int Stacks = 10;  //numero di suddivisioni sull'asse y
	int Slices = 10;  // numero di suddivisioni sull'asse x

	//Calc The Vertices

	//Stacks= suddivisioni lungo l'asse y
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float phi = V * glm::pi <float>();

		// Loop Through Slices suddivisioni lungo l'asse x
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = centro.x + raggio.x * (cosf(theta) * sinf(phi));
			float y = centro.y + raggio.y * cosf(phi);
			float z = centro.z + raggio.z * sinf(theta) * sinf(phi);

			mesh->position.push_back(glm::vec3(x, y, z)),
			mesh->color.push_back(color);
			mesh->texCoord.push_back(glm::vec2(U, V));
			mesh->normal.push_back(glm::normalize(glm::vec3(x, y, z)));

		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		mesh->indices.push_back(i);
		mesh->indices.push_back(i + Slices + 1);
		mesh->indices.push_back(i + Slices);


		mesh->indices.push_back(i + Slices + 1);
		mesh->indices.push_back(i);
		mesh->indices.push_back(i + 1);
	}




	int nv = mesh->position.size();
	mesh->indices.push_back(nv - 1);


}
