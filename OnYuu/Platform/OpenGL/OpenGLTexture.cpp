#include "OpenGLTexture.h"
#include <stb/stbi_image.h>
#include <iostream>
#include <glad/glad.h>
namespace OnYuu {
	OpenGLTexture::OpenGLTexture(const std::string& path, TextureFormat format, TextureWrap wrap, TextureType type)
	{
		// Protezione: inizializza a 0
		textureID = 0;
		stbi_set_flip_vertically_on_load(1); // Flip the image vertically on load
		int width_, height_, nrChannels_;
		unsigned char* data = stbi_load(path.c_str(), &width_, &height_, &nrChannels_, 0);
		width = width_;
		height = height_;
		nrChannels = nrChannels_;
		if (!data)
		{
			std::cerr << "Failed to load texture at path: " << path << "\n";
			const char* reason = stbi_failure_reason();
			if (reason && reason[0])
				std::cerr << "stb image error: " << reason << "\n";
			else
				std::cerr << "stb image returned null pointer without reason.\n";
			return;
		}

		// Genera e bind della texture
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Imposta wrapping
		switch (wrap)
		{
		case Texture::REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		case Texture::MIRRORED_REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			break;
		case Texture::CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case Texture::CLAMP_TO_BORDER:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			{
				// Imposta un colore di bordo esplicito (evita comportamenti non deterministici)
				float borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			}
			break;
		default:
			// valore sconosciuto: lascia i valori di default di OpenGL
			break;
		}

		// Filtri
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Determina internalFormat e dataFormat separatamente
		GLenum internalFormat = GL_RGB8;
		GLenum dataFormat = GL_RGB;

		if (nrChannels == 1)
		{
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}
		else if (nrChannels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (nrChannels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else
		{
			// Default fallback
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			std::cerr << "Warning: unexpected number of channels (" << nrChannels << "), fallback to RGB.\n";
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Allineamento: se i bordi non sono multipli di 4, impostare l'unpack alignment a 1
		// (utile specialmente per GL_RED o immagini con larghezza non multipla di 4)
		// Upload della texture
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Pulizia
		stbi_image_free(data);

		// Info utili per il debug
		std::cerr << "Loaded texture '" << path << "' (w=" << width << ", h=" << height << ", ch=" << nrChannels << "), textureID=" << textureID << "\n";
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &textureID);
	}

	void OpenGLTexture::bind(unsigned int unit)
	{
		Texture::bind(unit);
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
}