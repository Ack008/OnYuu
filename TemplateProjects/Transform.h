#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
struct Trasform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale{ 1,1,1 };
    Trasform& operator+=(const Trasform& tr) {
        position += tr.position;
        rotation += tr.rotation;
        scale *= tr.scale; // in genere la scala si moltiplica, non si somma
        return *this;
    }
    glm::mat4 getModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);
		return model;
    }
};