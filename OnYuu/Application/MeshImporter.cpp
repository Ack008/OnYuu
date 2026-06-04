#include "MeshImporter.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Application/AssetManager.h"
#include <assimp/version.h>
#include "../Core/Engine.h"
#include <filesystem> // C++17
using namespace std;
namespace OnYuu {

	namespace fs = std::filesystem;

	MeshImporter& MeshImporter::instance() {
		static MeshImporter mgr;
		return mgr;
	}
	GameObject MeshImporter::importMesh(const std::string& filePath, Scene* scene_, std::string shaderID)
	{
		std::cout << aiGetVersionMajor() << "."
			<< aiGetVersionMinor() << "."
			<< aiGetVersionRevision() << std::endl;
		std::vector<GameObject> mymesh;
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filePath,0);
		if (!scene) {
			fprintf(stderr, importer.GetErrorString());
			return GameObject();
		}
		GameObject obj = scene_->createEntity();
		obj.getComponent<TagComponent>().tag = "meshImported";
		const aiMesh* mesh;

		// base name (stem) del file per generare nomi univoci
		std::string fileStem = "import";
		try {
			fileStem = fs::path(filePath).stem().string();
		}
		catch (...) {
			fileStem = filePath;
		}

		// Fill vertices positions
		int num_meshes = scene->mNumMeshes;  //Numero di oggetti che compongono il modello
		int lights = scene->mNumLights; //Numero di luci presenti nel modello
		for (int i = 0; i < lights; i++)
		{
			std::cout << "Light " << i << ": " << scene->mLights[i]->mName.C_Str() << std::endl;
			GameObject lightObj = scene_->createEntity();
			lightObj.getComponent<TagComponent>().tag = fileStem + "_light_" + std::to_string(i);	
			glm::vec3 position(scene->mLights[i]->mPosition.x, scene->mLights[i]->mPosition.y, scene->mLights[i]->mPosition.z);
			lightObj.getComponent<Transform>().setPosition(position);
			auto &lightComponent = lightObj.addComponent<LightComponent>();
			lightComponent.intensity = scene->mLights[i]->mAttenuationConstant; // Usare l'attenuazione costante come intensità
			lightComponent.color = glm::vec4(scene->mLights[i]->mColorDiffuse.r, scene->mLights[i]->mColorDiffuse.g, scene->mLights[i]->mColorDiffuse.b, 1.0f);
		}
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
			aiString textureRelativePath;
			
			aiColor3D color;
			float value;
			// costruisco un nome più sicuro e unico: stem del file + nome mesh (se presente) + indice
			std::string meshBaseName = mesh->mName.C_Str() ? std::string(mesh->mName.C_Str()) : std::string();
			if (meshBaseName.empty()) meshBaseName = "submesh";
			std::string name = fileStem + "_" + meshBaseName + "_" + std::to_string(nm);

			mymesh[nm].getComponent<TagComponent>().tag = name;
			std::shared_ptr<Material> mat = AssetManager::instance().addMaterial(name, std::make_shared<Material>(shaderID));
			// Read mtl file vertex data
			if (material->GetTexture(aiTextureType_AMBIENT, 0, &textureRelativePath) == AI_SUCCESS)
			{
				std::string texturePath = fs::path(filePath).parent_path().string() + "/" + textureRelativePath.C_Str();
				std::cout << "ambient Texture path: " << texturePath << std::endl;
				std::string textureID = name + "_ambientTexture";
				auto texture = AssetManager::instance().addTexture(textureID, texturePath);
				mat->set("ambientTexture", texture);
			}
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &textureRelativePath) == AI_SUCCESS)
			{
				std::string texturePath = fs::path(filePath).parent_path().string() + "/" + textureRelativePath.C_Str();
				std::cout << "diffuse Texture path: " << texturePath << std::endl;
				std::string textureID = name + "_diffuseTexture";
				auto texture = AssetManager::instance().addTexture(textureID, texturePath);
				mat->set("diffuseTexture", texture);
			}
			if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &textureRelativePath) == AI_SUCCESS)
			{
				std::string texturePath = fs::path(filePath).parent_path().string() + "/" + textureRelativePath.C_Str();
				std::cout << "displacement Texture path: " << texturePath << std::endl;
				std::string textureID = name + "_displacementTexture";
				auto texture = AssetManager::instance().addTexture(textureID, texturePath);
				mat->set("displacementTexture", texture);	
			}
			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
			{
				mat->set("ambient", glm::vec3(color.r, color.g, color.b));
			}
			else
			{
				printf("Errore in ambientale \n");
			}
			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
			{
				mat->set("diffuse", glm::vec3(color.r, color.g, color.b));
			}
			else
			{
				mat->set("diffuse", glm::vec3(1.0, 0.2, 0.1));
			}
			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
			{
				mat->set("specular", glm::vec3(color.r, color.g, color.b));
			}
			else
			{
				printf("Errore in specular \n");
				mat->set("specular", glm::vec3(0.5, 0.5, 0.5));
			}
			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, value))
			{
				mat->set("shininess", value);
			}
			else
			{
				//mat->set("material.shininess", 50.0f);
			}
			// assegno il materiale alla componente
			currentMesh.getComponent<RenderMeshComponent>().setMaterialID(name);

			// Creo e registro la mesh nel AssetManager usando il nome unico
			std::shared_ptr<Mesh> tempMesh = AssetManager::instance().addMesh(name, std::make_shared<Mesh>());
			// Assegno la shared_ptr direttamente alla componente (migrazione completa)
			currentMesh.getComponent<RenderMeshComponent>().setMesh(tempMesh);

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

				aiVector3D pos = mesh->mVertices[i];
				aiVector3D uv;
				aiVector3D n;
				if (mesh->HasTextureCoords(0))
				{
					uv = mesh->mTextureCoords[0][i];
				}
				else
				{
					uv = { 0,0,0 };
				}
				if (mesh->HasNormals())
				{
					n = mesh->mNormals[i];
				}
				else
				{
					n = { 1,0,0 };

				}

				tempMesh->position.push_back(glm::vec3(pos.x, pos.y, pos.z));
				tempMesh->color.push_back(glm::vec4(1, 1, 1, 1));
				tempMesh->texCoord.push_back(glm::vec2(uv.x, uv.y));
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
}