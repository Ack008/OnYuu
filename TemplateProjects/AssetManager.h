#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "MeshComponent.h"
#include "Material.h"

class AssetManager {
public:
    static AssetManager& instance();

    // Mesh
    void addMesh(const std::string& name, std::shared_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> getMeshPtr(const std::string& name) const;
    Mesh* getMesh(const std::string& name) const; // raw pointer convenience

    // Material
    void addMaterial(const std::string& name, std::shared_ptr<Material> mat);
    std::shared_ptr<Material> getMaterialPtr(const std::string& name) const;
    Material* getMaterial(const std::string& name) const;

private:
    AssetManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
};