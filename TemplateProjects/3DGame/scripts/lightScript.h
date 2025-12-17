#pragma once
#include "Core/Engine.h"
#include <iostream>
class lightScript : public Component {
	
public:
	lightScript() = default;
	void start() override {
		
	}
	void update(float dt) override {
		if (obj->hasComponent<LightComponent>()) {
			LightComponent& light = obj->getComponent<LightComponent>();
			glm::vec3 color = glm::vec3(light.color);
			if (obj->hasComponent<RenderMeshComponent>()) {
				RenderMeshComponent& renderMesh = obj->getComponent<RenderMeshComponent>();
				std::shared_ptr<Material> lightMaterial = renderMesh.material;
				if (lightMaterial) {
					lightMaterial->set("material.lightColor", color);
				}
			}
		}
		
	}
};