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
    renderSquare.mesh = AssetManager::instance().getMeshPtr("cubeMesh");
    renderSquare.material = AssetManager::instance().getMaterialPtr("phongMat1");
	cube.addComponent<BoxCollider>();
	cube.getComponent<TagComponent>().tag = "cube";
	cube.getComponent<Trasform>().position = glm::vec3(0.0f, 0.0f, -5.0f);

	//defining sphere
    auto& renderSphere = sphere.addComponent<RenderMeshComponent>();
    renderSphere.mesh = AssetManager::instance().getMeshPtr("sphereMesh");
    renderSphere.material = AssetManager::instance().getMaterialPtr("phongMat1");
	sphere.getComponent<Trasform>().position = glm::vec3(2.0f, 0.0f, -5.0f);
	sphere.getComponent<TagComponent>().tag = "Sphere";
	sphere.addComponent<BoxCollider>();

	// defining toro
    auto& renderToro = toro.addComponent<RenderMeshComponent>();
    renderToro.mesh = AssetManager::instance().getMeshPtr("toroMesh");
    renderToro.material = AssetManager::instance().getMaterialPtr("phongMat1");
	toro.getComponent<Trasform>().position = glm::vec3(-2.0f, 0.0f, -5.0f);
	toro.getComponent<TagComponent>().tag = "Toro";
	toro.addComponent<BoxCollider>();

	cube.setFather(sphere);

    GameObject gatto = MeshImporter::instance().importMesh(
        "Asset/Mesh/auto.obj",
        this,
        AssetManager::instance().getMaterialPtr("bling-phong-Inter")->getShader()
    );
	gatto.getComponent<Trasform>().position = glm::vec3(-9.0f, 0.0f, -3.0f);

	//defining lights
	settingLight();

	//defining skybox
	auto& skyboxRender = skybox.addComponent<SkyBoxComponent>();
	skyboxRender.cubeMap = AssetManager::instance().getCubeMapPtr("skybox");
}

void Scene3D::settingLight()
{
	lightDirectional.getComponent<Trasform>().position = glm::vec3(0.0f, 1.0f, 0.0f);
    auto &rb = lightDirectional.addComponent<RenderMeshComponent>();
    rb.mesh = AssetManager::instance().getMeshPtr("sphereMesh");
    rb.material = AssetManager::instance().getMaterialPtr("defaultMaterial");
	lightDirectional.addComponent<LightComponent>();
	lightDirectional.getComponent<LightComponent>().color = glm::vec4(1,1,1,1);
	lightDirectional.getComponent<TagComponent>().tag = "DirectionalLight";

	lightDirectional2.getComponent<Trasform>().position = glm::vec3(3.0f, 1.0f, 0.0f);
    auto& rb2 = lightDirectional2.addComponent<RenderMeshComponent>();
    rb2.mesh = AssetManager::instance().getMeshPtr("sphereMesh");
    rb2.material = AssetManager::instance().getMaterialPtr("defaultMaterial");
	lightDirectional2.addComponent<LightComponent>();
	lightDirectional2.getComponent<LightComponent>().color = glm::vec4(1, 1, 1, 1);
	lightDirectional2.getComponent<TagComponent>().tag = "DirectionalLight2";
}

void Scene3D::OnResize(uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;
	if (camera.hasComponent<Perspective>()) {
		camera.getComponent<Perspective>().OnResize(width, height);
	}
}
void Scene3D::createPyramid()
{
	AssetManager::instance().addMesh("pyramidMesh", std::make_shared<Mesh>(Mesh()));
	Mesh* mesh = AssetManager::instance().getMesh("pyramidMesh");

	// base vertices (quad on Y=0)
	mesh->position.push_back(glm::vec3(-1.0f, 0.0f, 1.0f)); // 0
	mesh->color.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
	mesh->texCoord.push_back(glm::vec2(0.0f, 0.0f));

	mesh->position.push_back(glm::vec3(1.0f, 0.0f, 1.0f)); // 1
	mesh->color.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
	mesh->texCoord.push_back(glm::vec2(1.0f, 0.0f));

	mesh->position.push_back(glm::vec3(1.0f, 0.0f, -1.0f)); // 2
	mesh->color.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
	mesh->texCoord.push_back(glm::vec2(1.0f, 1.0f));

	mesh->position.push_back(glm::vec3(-1.0f, 0.0f, -1.0f)); // 3
	mesh->color.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
	mesh->texCoord.push_back(glm::vec2(0.0f, 1.0f));

	// apex
	mesh->position.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // 4
	mesh->color.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// apex UV placed at center of texture to minimise stretching
	mesh->texCoord.push_back(glm::vec2(0.5f, 0.5f));

	// base (two triangles)
	mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
	mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(3);

	// sides
	mesh->indices.push_back(0); mesh->indices.push_back(4); mesh->indices.push_back(3);
	mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(4);

	mesh->indices.push_back(3); mesh->indices.push_back(2); mesh->indices.push_back(4);
	mesh->indices.push_back(1); mesh->indices.push_back(2); mesh->indices.push_back(4);

	// optional decorative vertex (kept as original implementation)
	mesh->position.push_back(glm::vec3(0.0f, 0.3f, 0.0f)); // 5
	mesh->color.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	// give it a reasonable UV as well
	mesh->texCoord.push_back(glm::vec2(0.5f, 0.25f));

	// normals placeholder (will be computed later if needed)
	for (int i = 0; i < mesh->position.size(); i++)
	{
		mesh->normal.push_back(glm::vec3(0.0f));
	}


	int nv = mesh->position.size();
	mesh->indices.push_back(nv - 1);

}

void Scene3D::createToro(glm::vec4 color)
{
	AssetManager::instance().addMesh("toroMesh", std::make_shared<Mesh>(Mesh()));
	Mesh* mesh = AssetManager::instance().getMesh("toroMesh");
	int Stacks = 30;  //numero di suddivisioni sull'asse x
	int Slices = 30;  // numero di suddivisioni sull'asse y
	float R = 1, r = 0.5;
	float s, t;


	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float phi = V * glm::pi <float>() * 2;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = (R + r * cosf(phi)) * cosf(theta);
			float y = r * sinf(phi);
			float z = (R + r * cosf(phi)) * sinf(theta);


			// Push Back Vertex Data
			mesh->position.push_back(glm::vec3(x, y, z));
			mesh->color.push_back(color);
			// UV coordinates: U around main ring, V around tube cross-section
			mesh->texCoord.push_back(glm::vec2(U, V));
			//Normale nel vertice
			mesh->normal.push_back(glm::vec3(glm::normalize(glm::vec3(sin(phi) * cos(theta), cos(phi), sin(theta) * sin(phi)))));


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
	mesh->position.push_back(glm::vec3(0.0, 0.0, 0.0));
	mesh->color.push_back(glm::vec4(0.0, 1.0, 0.0, 1.0));
	// aggiunta UV per il vertice finale (coerente come centro texture)
	mesh->texCoord.push_back(glm::vec2(0.5f, 0.5f));
	// Assicuriamo che anche le normali corrispondano al numero di posizioni per evitare accessi fuori range
	mesh->normal.push_back(glm::vec3(0.0f, 1.0f, 0.0f));


	int nv = mesh->position.size();
	mesh->indices.push_back(nv - 1);

}
void Scene3D::initializeMaterials()
{
	createCube();
	createSphere(glm::vec4(0.0, 1.0, 0.0, 1.0),100,100);
	createPyramid();
	createToro({0,1,1,1});
	std::shared_ptr<Shader> shader = Shader::create("MeshVertexShader.glsl", "fragmentShaderC.glsl");
	std::shared_ptr<Shader> shaderPhongInter = Shader::create("Asset/Shader/phongVerInter.glsl", "Asset/Shader/phongFragInter.glsl");
	std::shared_ptr<Shader> shaderPhong = Shader::create("Asset/Shader/phongVer.glsl", "Asset/Shader/phongFrag.glsl");	
	std::shared_ptr<Shader> shaderBlingPhong = Shader::create("Asset/Shader/bling-phong-ver.glsl", "Asset/Shader/bling-phong-frag.glsl");
	std::shared_ptr<Shader> shaderBlingPhongInter = Shader::create("Asset/Shader/bling-phongVerInter.glsl", "Asset/Shader/bling-phongFragInter.glsl");
	std::shared_ptr<Shader> shaderBlingPhongInterTexture = Shader::create("Asset/Shader/bling-phongVerInter.glsl", "Asset/Shader/bling-phongFragInter-Texture.glsl");
	std::shared_ptr<Shader> shaderReflective = Shader::create("Asset/Shader/ReflectiveShader/reflective-ver.glsl", "Asset/Shader/ReflectiveShader/reflective-frag.glsl");
	// skybox shader
	std::shared_ptr<Shader> skyboxShader = Shader::create(
		"Asset/Shader/SkyBoxShader/SkyBoxVer.glsl",
		"Asset/Shader/SkyBoxShader/SkyBoxFrag.glsl"
	);
	AssetManager::instance().addMaterial("skyboxMaterial", std::make_shared<Material>(skyboxShader));
	std::shared_ptr<CubeMap> textureSkybox = AssetManager::instance().addCubeMap("skybox",CubeMap::createCubeMap({
		"Asset/Texture/SkyBox2/px.png",
		"Asset/Texture/SkyBox2/nx.png",
		"Asset/Texture/SkyBox2/py.png",
		"Asset/Texture/SkyBox2/ny.png",
		"Asset/Texture/SkyBox2/pz.png",
		"Asset/Texture/SkyBox2/nz.png"
		}));
	AssetManager::instance().addMaterial("defaultMaterial", std::make_shared<Material>(shader));

	AssetManager::instance().addMaterial("phongInterMat1", std::make_shared<Material>(shaderPhongInter));
	auto mat = AssetManager::instance().getMaterial("phongInterMat1");
	mat->set("material.ambient",glm::vec3(0.19125f, 0.0735f, 0.0225f));
	mat->set("material.shininess",13.f);
	mat->set("material.diffuse", glm::vec3(0.7038f, 0.27048f, 0.0828f));
	mat->set("material.specular", glm::vec3(0.256777f, 0.137622f, 0.086014f));

	AssetManager::instance().addMaterial("phongMat1", std::make_shared<Material>(shaderPhong));
	auto mat2 = AssetManager::instance().getMaterial("phongMat1");
	mat2->set("material.ambient", glm::vec3(0.19125f, 0.0735f, 0.0225f));
	mat2->set("material.shininess", 13.f);
	mat2->set("material.diffuse", glm::vec3(0.7038f, 0.27048f, 0.0828f));
	mat2->set("material.specular", glm::vec3(0.256777f, 0.137622f, 0.086014f));



	AssetManager::instance().addMaterial("bling-phong", std::make_shared<Material>(shaderBlingPhong));
	auto mat3 = AssetManager::instance().getMaterial("bling-phong");
	mat3->set("material.ambient", glm::vec3(0.19125f, 0.0735f, 0.0225f));
	mat3->set("material.shininess", 13.f);
	mat3->set("material.diffuse", glm::vec3(0.7038f, 0.27048f, 0.0828f));
	mat3->set("material.specular", glm::vec3(0.256777f, 0.137622f, 0.086014f));

	AssetManager::instance().addMaterial("bling-phong-Inter", std::make_shared<Material>(shaderBlingPhongInter));
	auto mat4 = AssetManager::instance().getMaterial("bling-phong-Inter");
	mat4->set("material.ambient", glm::vec3(0.19125f, 0.0735f, 0.0225f));
	mat4->set("material.shininess", 13.f);
	mat4->set("material.diffuse", glm::vec3(0.7038f, 0.27048f, 0.0828f));
	mat4->set("material.specular", glm::vec3(0.256777f, 0.137622f, 0.086014f));

	AssetManager::instance().addTexture("interLoveTexture", Texture::createTexture("Asset/Texture/ai.png"));
	AssetManager::instance().addTexture("catTexture", Texture::createTexture("Asset/Texture/gatto.png"));

	AssetManager::instance().addMaterial("bling-phong-InterLove", std::make_shared<Material>(shaderBlingPhongInterTexture));
	auto mat5 = AssetManager::instance().getMaterial("bling-phong-InterLove");
	mat5->set("material.ambient", glm::vec3(0.19125f, 0.0735f, 0.0225f));
	mat5->set("material.shininess", 13.f);
	mat5->set("material.diffuse", glm::vec3(0.7038f, 0.27048f, 0.0828f));
	mat5->set("material.specular", glm::vec3(0.256777f, 0.137622f, 0.086014f));
	mat5->set("texture1", AssetManager::instance().getTexture("interLoveTexture"));

	auto mat6 = AssetManager::instance().addMaterial("bling-phong-cat", std::make_shared<Material>(shaderBlingPhongInterTexture));
	mat6->set("material.ambient", glm::vec3(0.19125f, 0.0735f, 0.0225f));
	mat6->set("material.shininess", 13.f);
	mat6->set("material.diffuse", glm::vec3(0.7038f, 0.27048f, 0.0828f));
	mat6->set("material.specular", glm::vec3(0.256777f, 0.137622f, 0.086014f));
	mat6->set("texture1", AssetManager::instance().getTexture("catTexture"));

	mat6 = AssetManager::instance().addMaterial("reflectiveMaterial", std::make_shared<Material>(shaderReflective));
	mat6->set("material.ambient", glm::vec3(0.19125f, 0.0735f, 0.0225f));
	mat6->set("material.shininess", 13.f);
	mat6->set("material.diffuse", glm::vec3(0.7038f, 0.27048f, 0.0828f));
	mat6->set("material.specular", glm::vec3(0.256777f, 0.137622f, 0.086014f));
}
void Scene3D::createCube()
{
	AssetManager::instance().addMesh("cubeMesh", std::make_shared<Mesh>(Mesh()));
	Mesh* mesh = AssetManager::instance().getMesh("cubeMesh");
	mesh->position.push_back(glm::vec3(-1.0, -1.0, 1.0));
	mesh->color.push_back(glm::vec4(1.0, 0.0, 0.0, 0.5));
	mesh->texCoord.push_back(glm::vec2(0.0f, 0.0f)); // front bottom-left

	mesh->position.push_back(glm::vec3(1.0, -1.0, 1.0));
	mesh->color.push_back(glm::vec4(0.0, 1.0, 0.0, 0.5));
	mesh->texCoord.push_back(glm::vec2(1.0f, 0.0f)); // front bottom-right

	mesh->position.push_back(glm::vec3(1.0, 1.0, 1.0));
	mesh->color.push_back(glm::vec4(0.0, 0.0, 1.0, 0.5));
	mesh->texCoord.push_back(glm::vec2(1.0f, 1.0f)); // front top-right

	mesh->position.push_back(glm::vec3(-1.0, 1.0, 1.0));
	mesh->color.push_back(glm::vec4(1.0, 0.0, 1.0, 0.5));
	mesh->texCoord.push_back(glm::vec2(0.0f, 1.0f)); // front top-left
	// back
	mesh->position.push_back(glm::vec3(-1.0, -1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
	// For the back face we mirror U so the texture isn't mirrored when viewed from behind
	mesh->texCoord.push_back(glm::vec2(1.0f, 0.0f)); // back bottom-left (mapped to texture bottom-right)

	mesh->position.push_back(glm::vec3(1.0, -1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
	mesh->texCoord.push_back(glm::vec2(0.0f, 0.0f)); // back bottom-right

	mesh->position.push_back(glm::vec3(1.0, 1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
	mesh->texCoord.push_back(glm::vec2(0.0f, 1.0f)); // back top-right

	mesh->position.push_back(glm::vec3(-1.0, 1.0, -1.0));
	mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
	mesh->texCoord.push_back(glm::vec2(1.0f, 1.0f)); // back top-left



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

	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));
	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));
	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));
	mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));

}


void Scene3D::createSphere(glm::vec4 color, int stacks , int slices )
{
	AssetManager::instance().addMesh("sphereMesh", std::make_shared<Mesh>(Mesh()));
	Mesh* mesh = AssetManager::instance().getMesh("sphereMesh");

	glm::vec3 center(0.0f);
	float radius = 1.0f;

	// Vertici e normali
	for (int i = 0; i <= stacks; ++i)
	{
		float V = (float)i / stacks;
		float phi = V * glm::pi<float>();

		for (int j = 0; j <= slices; ++j)
		{
			float U = (float)j / slices;
			float theta = U * (glm::pi<float>() * 2.0f);

			float x = center.x + radius * sinf(phi) * cosf(theta);
			float y = center.y + radius * cosf(phi);
			float z = center.z + radius * sinf(phi) * sinf(theta);

			glm::vec3 pos(x, y, z);
			glm::vec3 normal = glm::normalize(pos - center);

			mesh->position.push_back(pos);
			mesh->normal.push_back(normal);
			mesh->color.push_back(color);
			mesh->texCoord.push_back(glm::vec2(U, V));
		}
	}

	// Indici
	for (int i = 0; i < stacks; ++i)
	{
		for (int j = 0; j < slices; ++j)
		{
			int first = i * (slices + 1) + j;
			int second = first + slices + 1;

			// Primo triangolo
			mesh->indices.push_back(first);
			mesh->indices.push_back(second);
			mesh->indices.push_back(first + 1);

			// Secondo triangolo
			mesh->indices.push_back(second);
			mesh->indices.push_back(second + 1);
			mesh->indices.push_back(first + 1);
		}
	}
}
