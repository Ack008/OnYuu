#pragma once
#include "Core/Texture.h"
#include <string>
#include <glad/glad.h>
namespace OnYuu {
class OpenGLTexture : public Texture {
	public:
	OpenGLTexture(const std::string& path, TextureFormat format, TextureWrap wrap, TextureType type);
	 ~OpenGLTexture() ;
	virtual void bind(unsigned int unit = 0) override;
};
} // namespace OnYuu