#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "Material.h"
#include "Shader.h"
class Prova : public Scene {
public:
	Prova();
	~Prova() = default;
	void OnResize(uint32_t width, uint32_t height) override;
private:
	GameObject Square;
	GameObject Triangle;
	GameObject Camera;
	Mesh triangoloMesh = {
		std::vector<glm::vec3>{
			{ 0, 0, 0 },
			{ 0,100,0 },
			{ 100,50,0 },
		},
		std::vector<glm::vec4>{
			{1,0,0,1},
			{ 0,1,0,1 },
			{ 0,0,1,1 },
		}
	};
	Mesh squareMesh = {
		std::vector<glm::vec3>{
			{150, 0, 0},
			{150,150,0 },
			{100,0,0 }
		},
		std::vector<glm::vec4>{
			{ 1,0,0,1  },
			{ 0,1,0,1 },
			{ 0,0,1,1 },
		}
	};
	std::shared_ptr <Shader> shader;
	Material material;
};