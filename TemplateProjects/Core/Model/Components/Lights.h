#pragma once
#include <glm/glm.hpp>
struct LightComponent {
	float intensity = 1.0f;
	glm::vec4 color = glm::vec4(1.0f);
};