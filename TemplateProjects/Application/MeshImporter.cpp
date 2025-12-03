#include "MeshImporter.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Application/AssetManager.h"
using namespace std;
MeshImporter& MeshImporter::instance() {
	static MeshImporter mgr;
	return mgr;
}
GameObject MeshImporter::importMesh(const std::string& filePath, Scene* scene_, std::shared_ptr<Shader> shader)
{
	std::vector<GameObject> mymesh;
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filePath, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene) {
		fprintf(stderr, importer.GetErrorString());
		getchar();
		return GameObject();
	}
	GameObject obj = scene_->createEntity();
	obj.getComponent<TagComponent>().tag = "meshImported";
	const aiMesh* mesh;

	// Fill vertices positions
	int num_meshes = scene->mNumMeshes;  //Numero di oggetti che compongono il modello
	for (int i = 0; i < num_meshes; i++)
	{
		mymesh.push_back(scene_->createEntity());
		GameObject& currentMesh = mymesh.back();
		currentMesh.addComponent<RenderMeshComponent>();
	}
	for (unsigned int nm = 0; nm < num_meshes; nm++)
	{
		GameObject& currentMesh = mymesh[nm];
		//Per ogni mesh dell'oggetto 
		mesh = scene->mMeshes[nm];
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiColor3D color;
		float value;
		std::string name = std::string(mesh->mName.C_Str()) + std::to_string(nm);
		mymesh[nm].getComponent<TagComponent>().tag = name;
		std::shared_ptr<Material> mat = AssetManager::instance().addMaterial(name, std::make_shared<Material>(shader));
		// Read mtl file vertex data

		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
		{
			mat->set("material.ambient", glm::vec3(color.r, color.g, color.b));
		}
		else
		{
			printf("Errore in ambientale \n");
		}
		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
			mat->set("material.diffuse", glm::vec3(color.r, color.g, color.b));
		}
		else
		{
			mat->set("material.diffuse", glm::vec3(1.0, 0.2, 0.1));
		}
		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
		{
			mat->set("material.specular", glm::vec3(color.r, color.g, color.b));
		}
		else
		{
			printf("Errore in specular \n");
			mat->set("material.specular", glm::vec3(0.5, 0.5, 0.5));
		}
		if (aiReturn_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, value))
		{
			mat->set("material.shininess", value);
		}
		else
		{
			//printf("Errore in shininess \n");
			mat->set("material.shininess", 50.0f);
		}
		// http://assimp.sourceforge.net/lib_html/structai_material.html
		currentMesh.getComponent<RenderMeshComponent>().material = mat.get();
		std::shared_ptr<Mesh> tempMesh = AssetManager::instance().addMesh(name, std::make_shared<Mesh>());
		currentMesh.getComponent<RenderMeshComponent>().mesh = tempMesh.get();

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

			aiVector3D pos = mesh->mVertices[i];
			aiVector3D uv;
			//aiColor4D color = mesh->mColors[0][i];
			aiVector3D n;
			if (mesh->HasTextureCoords(0))
			{
				uv = mesh->mTextureCoords[0][i];
			}
			else
			{
				uv = { 0,0,0 };
			}
			if(mesh->HasNormals())
			{
				 n = mesh->mNormals[i];
			}
			else
			{
				 n = { 1,0,0 };

			}

			tempMesh->position.push_back(glm::vec3(pos.x, pos.y, pos.z));
			tempMesh->color.push_back(glm::vec4(1,1,1,1));
			tempMesh->texCoord.push_back(glm::vec2(uv.x,uv.y));
			tempMesh->normal.push_back(glm::vec3(n.x, n.y, n.z));
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			// Assume the model has only triangles.
			tempMesh->indices.push_back(mesh->mFaces[i].mIndices[0]);
			tempMesh->indices.push_back(mesh->mFaces[i].mIndices[1]);
			tempMesh->indices.push_back(mesh->mFaces[i].mIndices[2]);
		}
		currentMesh.setFather(obj);
		currentMesh.addComponent<BoxCollider>();
	}
	return obj;
}
