#pragma once
#include "Component.h"
#include "Collider.h"
#include "MeshComponent.h"
class Trasform;
class FirstCustomComponent : public Component
{
	// Ereditato tramite Component
	virtual void update(float dt) override;

	void istantiate(Trasform& transform);

	virtual void start() override;
	virtual void onCollisionStay(Collider* other) override;
private:
	Mesh triangoloMesh = {
		std::vector<glm::vec3>{
			{-1.0, -1.0, 0},
			{-1.0,1,0 },
			{1.0,0,0 }
		},
		std::vector<glm::vec4>{
			{ 1,0,0,1  },
			{ 0,1,0,1 },
			{ 0,0,1,1 },
		}
	};
	std::shared_ptr <Shader> shader = Shader::create("vertexShaderC.glsl", "fragmentShaderC.glsl");
	std::shared_ptr<Material> material = std::make_shared<Material>(shader);
	std::shared_ptr<GameObject> projectileObject;
};

