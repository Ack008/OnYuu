#include "AssetManager.h"
#include "Core/CubeMap.h"
#include <iostream>
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
    auto it = materials_.find(name);
    if (it != materials_.end()) {
        std::cout << "[AssetManager] addMaterial: replacing material '" << name << "' old_ptr=" << it->second.get()
            << " old_use_count=" << it->second.use_count() << std::endl;
    }
    materials_[name] = std::move(mat);
    return materials_[name];
}

std::shared_ptr<Material> AssetManager::getMaterialPtr(const std::string& name) const {
    auto it = materials_.find(name);
    if (it != materials_.end()) {
        return it->second;
    }
    return nullptr;
}

Material* AssetManager::getMaterial(const std::string& name) const {
    auto ptr = getMaterialPtr(name);
    Material* raw = ptr ? ptr.get() : nullptr;
    return raw;
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

std::shared_ptr<CubeMap> AssetManager::addCubeMap(const std::string& name, std::shared_ptr<CubeMap> cubeMap)
{
    cubeMaps_[name] = std::move(cubeMap);
    return cubeMaps_[name];
}

std::shared_ptr<CubeMap> AssetManager::getCubeMapPtr(const std::string& name) const
{
    auto it = cubeMaps_.find(name);
    return it != cubeMaps_.end() ? it->second : nullptr;
}

CubeMap* AssetManager::getCubeMap(const std::string& name) const
{
    return getCubeMapPtr(name).get();
}


void AssetManager::shutdown() {
	for (auto& [name, texture] : textures_) {
        if(texture) {
            texture->shutdown();
        }
    }
    meshes_.clear();
    materials_.clear();
    textures_.clear();
    cubeMaps_.clear();
}