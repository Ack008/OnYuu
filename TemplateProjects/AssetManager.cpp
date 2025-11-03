#include "AssetManager.h"

AssetManager& AssetManager::instance() {
    static AssetManager mgr;
    return mgr;
}

void AssetManager::addMesh(const std::string& name, std::shared_ptr<Mesh> mesh) {
    meshes_[name] = std::move(mesh);
}

std::shared_ptr<Mesh> AssetManager::getMeshPtr(const std::string& name) const {
    auto it = meshes_.find(name);
    return it != meshes_.end() ? it->second : nullptr;
}

Mesh* AssetManager::getMesh(const std::string& name) const {
    auto ptr = getMeshPtr(name);
    return ptr ? ptr.get() : nullptr;
}

void AssetManager::addMaterial(const std::string& name, std::shared_ptr<Material> mat) {
    materials_[name] = std::move(mat);
}

std::shared_ptr<Material> AssetManager::getMaterialPtr(const std::string& name) const {
    auto it = materials_.find(name);
    return it != materials_.end() ? it->second : nullptr;
}

Material* AssetManager::getMaterial(const std::string& name) const {
    auto ptr = getMaterialPtr(name);
    return ptr ? ptr.get() : nullptr;
}