#pragma once
#include <string>
#include "Core/Model/Components/MeshComponent.h"
#include "Core/Material.h"
#include "Core/Model/GameObject.h"
namespace OnYuu {

class MeshImporter{
public:
	static MeshImporter& instance();
public:
	MeshImporter() = default;
	~MeshImporter() = default;
	// Importa un modello 3D da file e crea un GameObject con MeshComponent.
	// 'filePath' è il percorso del file del modello.
	// Ritorna il GameObject creato.
	GameObject importMesh(const std::string& filePath, Scene* scene_, std::shared_ptr<Shader> shader);
};
} // namespace OnYuu