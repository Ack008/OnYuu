#pragma once
#include <glm/glm.hpp>
namespace OnYuu {
struct Ray {
	glm::vec3 origin;    // Punto di origine del raggio
	glm::vec3 direction; // Direzione del raggio (deve essere normalizzata)
	Ray(const glm::vec3& orig, const glm::vec3& dir)
		: origin(orig), direction(glm::normalize(dir)) {}
	Ray() = default;
};
} // namespace OnYuu