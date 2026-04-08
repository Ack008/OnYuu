#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace OnYuu::Math {

    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat orientation;

        if (!glm::decompose(transform, scale, orientation, translation, skew, perspective))
            return false;

        orientation = glm::normalize(orientation);
        rotation = glm::degrees(glm::eulerAngles(orientation));

        // Normalizza ogni angolo in [-180, 180]
        for (int i = 0; i < 3; i++) {
            rotation[i] = fmod(rotation[i] + 180.0f, 360.0f);
            if (rotation[i] < 0.0f) rotation[i] += 360.0f;
            rotation[i] -= 180.0f;
        }

        return true;
    }
}