#include "AssetManager.h"

AssetManager& AssetManager::instance() {
    static AssetManager mgr;
    return mgr;
}

std::shared_ptr<Mesh> AssetManager::addMesh(const std::string& name, std::shared_ptr<Mesh> mesh) {
    meshes_[name] = std::move(mesh);
	return meshes_[name];
}

std::shared_ptr<Mesh> AssetManager::getMeshPtr(const std::string& name) const {
    auto it = meshes_.find(name);
    return it != meshes_.end() ? it->second : nullptr;
}

Mesh* AssetManager::getMesh(const std::string& name) const {
    auto ptr = getMeshPtr(name);
    return ptr ? ptr.get() : nullptr;
}

std::shared_ptr<Material> AssetManager::addMaterial(const std::string& name, std::shared_ptr<Material> mat) {
    materials_[name] = std::move(mat);
	return materials_[name];
}

std::shared_ptr<Material> AssetManager::getMaterialPtr(const std::string& name) const {
    auto it = materials_.find(name);
    return it != materials_.end() ? it->second : nullptr;
}

Material* AssetManager::getMaterial(const std::string& name) const {
    auto ptr = getMaterialPtr(name);
    return ptr ? ptr.get() : nullptr;
}

std::shared_ptr<Texture> AssetManager::addTexture(const std::string& name, std::shared_ptr<Texture> tex) {
    textures_[name] = std::move(tex);
	return textures_[name];
}

std::shared_ptr<Texture> AssetManager::getTexturePtr(const std::string& name) const {
    auto it = textures_.find(name);
    return it != textures_.end() ? it->second : nullptr;
}
Texture* AssetManager::getTexture(const std::string& name) const {
    auto ptr = getTexturePtr(name);
    return ptr ? ptr.get() : nullptr;
}