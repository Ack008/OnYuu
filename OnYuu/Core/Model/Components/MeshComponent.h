#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Core/RenderingTypeEnum.h"
#include "Core/Shader.h"
#include "Core/Material.h"
namespace OnYuu {
// Mesh: struttura che contiene i dati geometrici in CPU.
// - `position`: posizione dei vertici
// - `color`: colore per vertice (opzionale)
// - `indices`: indici per l'index buffer
struct Mesh {
	std::vector<glm::vec3> position;
	std::vector<glm::vec4> color;
	std::vector<glm::vec2> texCoord;
	std::vector<glm::vec3> normal;
	std::vector<uint32_t> indices;
	bool empty() const {
		return position.empty() || indices.empty();
	}
};

// RenderMeshComponent: semplice wrapper che collega una `Mesh` a un
// `Material` e definisce il tipo di primitiva da disegnare.
//
// Migrazione: usiamo `std::shared_ptr<Mesh>` per mantenere ownership e prevenire
// dangling pointer quando AssetManager rimpiazza una mesh.
struct RenderMeshComponent {
    std::shared_ptr<Mesh> mesh; // shared ownership della mesh
	std::string materialID;
	RenderingTypeEnum renderingType = RenderingTypeEnum::TRIANGLE;

	size_t getSize() {
		if (!mesh) return 0;
		// Calcola la dimensione approssimativa in byte dei dati del vertice.
		size_t pos = mesh->position.size() * sizeof(glm::vec3);
		size_t col = mesh->color.size() * sizeof(glm::vec4);
		size_t tex = mesh->texCoord.size() * sizeof(glm::vec2);
		size_t nor = mesh->normal.size() * sizeof(glm::vec3);
		return pos + col + tex + nor;
	}
} ;
} // namespace OnYuu