#include "OpenGLCubeMap.h"
#include <stb/stbi_image.h>
#include <glad/glad.h>
#include <iostream>
namespace OnYuu {

	OpenGLCubeMap::OpenGLCubeMap(const std::vector<std::string>& faces) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
		int width, height, nrChannels;
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (unsigned int i = 0; i < faces.size(); i++) {
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				GLint internalFormat = GL_RGB;
				GLenum format = GL_RGB;
				if (nrChannels == 1) {
					internalFormat = GL_RED;
					format = GL_RED;
				}
				else if (nrChannels == 3) {
					internalFormat = GL_RGB;
					format = GL_RGB;
				}
				else if (nrChannels == 4) {
					internalFormat = GL_RGBA;
					format = GL_RGBA;
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else {
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}

	}

	void OpenGLCubeMap::bind(unsigned int unit) {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}
} // namespace OnYuu