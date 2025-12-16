#pragma once
#include <string>
#include <memory>
class Texture {
public:
	// Enums for texture types, formats, and wrapping modes
	enum TextureType {
		DIFFUSE,
		SPECULAR,
		NORMAL,
		HEIGHT
	};
	enum TextureFormat {
		RGB,
		RGBA
	};
	enum TextureWrap {
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};
	public:


	unsigned int getID() const { return textureID; }
	unsigned int getSlot() const { return slot; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	virtual void bind(unsigned int unit = 0) { slot = unit; };
	operator bool() const { return textureID != 0; }
	operator unsigned int() const { return textureID; }
	virtual void shutdown() {};
	static std::shared_ptr<Texture> createTexture(const std::string& path, TextureFormat format = RGBA, TextureWrap wrap = REPEAT, TextureType type = NORMAL);
	
protected:
	unsigned int textureID;
	int width, height, nrChannels;
	TextureFormat format;
	TextureWrap wrapS;
	TextureType type;
	unsigned int slot;
};