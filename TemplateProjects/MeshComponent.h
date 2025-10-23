#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "RenderingTypeEnum.h"
#include "Shader.h"
#include "Material.h"
struct Mesh {
	std::vector<glm::vec3> position;
	std::vector<glm::vec4> color;
};
struct RenderMeshComponent {
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	RenderingTypeEnum renderingType = RenderingTypeEnum::TRIANGLE;
	size_t getSize() {
		return mesh->position.size() * sizeof(glm::vec3) +mesh->color.size() * sizeof(glm::vec4);
	}
} ;