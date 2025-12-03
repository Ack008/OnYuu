#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Core/RenderingTypeEnum.h"
#include "Core/Shader.h"
#include "Core/Material.h"

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
};

// RenderMeshComponent: semplice wrapper che collega una `Mesh` a un
// `Material` e definisce il tipo di primitiva da disegnare.
//
// Nota: questo è un componente di alto livello, il renderer lo tradurrà in
// chiamate a `MeshGPUusage` per uploadare i dati in GPU e disegnarli.
struct RenderMeshComponent {
    Mesh* mesh = nullptr; // puntatore non-owning alla mesh
    std::shared_ptr<Material> material; // materiale condiviso per il draw
	RenderingTypeEnum renderingType = RenderingTypeEnum::TRIANGLE;

	size_t getSize() {
		// Calcola la dimensione approssimativa in byte dei dati del vertice.
		return mesh->position.size() * sizeof(glm::vec3) +mesh->color.size() * sizeof(glm::vec4);
	}
} ;