#include "Texture.h"
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLTexture.h"
std::shared_ptr<Texture> Texture::createTexture(const std::string& path, TextureFormat format, TextureWrap wrap, TextureType type)
{
	switch (apiInUse)
	{
		case API::OpenGL:
			return std::make_shared<OpenGLTexture>(path, format, wrap, type);
	}
}
