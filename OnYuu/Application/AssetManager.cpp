#include "AssetManager.h"
#include "Core/CubeMap.h"
#include "json/json.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {
    using json = nlohmann::json;

    bool readFloatArray(const json& value, std::vector<float>& out)
    {
        if (!value.is_array()) {
            return false;
        }
        out.clear();
        out.reserve(value.size());
        for (const auto& item : value) {
            if (!item.is_number()) {
                return false;
            }
            out.push_back(item.get<float>());
        }
        return true;
    }

    bool parseMaterialParam(const json& paramJson, OnYuu::AssetManager::MaterialParam& out)
    {
        if (!paramJson.is_object() || !paramJson.contains("type") || !paramJson.contains("value")) {
            return false;
        }

        const std::string typeName = paramJson.value("type", std::string{});
        const json& value = paramJson["value"];

        if (typeName == "Int") {
            if (!value.is_number_integer()) return false;
            out.type = OnYuu::AssetManager::MaterialParam::Type::Int;
            out.value = value.get<int>();
            return true;
        }
        if (typeName == "Float") {
            if (!value.is_number()) return false;
            out.type = OnYuu::AssetManager::MaterialParam::Type::Float;
            out.value = value.get<float>();
            return true;
        }
        if (typeName == "Bool") {
            if (!value.is_boolean()) return false;
            out.type = OnYuu::AssetManager::MaterialParam::Type::Bool;
            out.value = value.get<bool>();
            return true;
        }

        std::vector<float> f;
        if (!readFloatArray(value, f)) {
            return false;
        }

        if (typeName == "Vec2" && f.size() == 2) {
            out.type = OnYuu::AssetManager::MaterialParam::Type::Vec2;
            out.value = glm::vec2(f[0], f[1]);
            return true;
        }
        if (typeName == "Vec3" && f.size() == 3) {
            out.type = OnYuu::AssetManager::MaterialParam::Type::Vec3;
            out.value = glm::vec3(f[0], f[1], f[2]);
            return true;
        }
        if (typeName == "Vec4" && f.size() == 4) {
            out.type = OnYuu::AssetManager::MaterialParam::Type::Vec4;
            out.value = glm::vec4(f[0], f[1], f[2], f[3]);
            return true;
        }
        if (typeName == "Mat3" && f.size() == 9) {
            glm::mat3 m(1.0f);
            std::memcpy(glm::value_ptr(m), f.data(), 9 * sizeof(float));
            out.type = OnYuu::AssetManager::MaterialParam::Type::Mat3;
            out.value = m;
            return true;
        }
        if (typeName == "Mat4" && f.size() == 16) {
            glm::mat4 m(1.0f);
            std::memcpy(glm::value_ptr(m), f.data(), 16 * sizeof(float));
            out.type = OnYuu::AssetManager::MaterialParam::Type::Mat4;
            out.value = m;
            return true;
        }

        return false;
    }
}

namespace OnYuu {
    AssetManager::AssetManager() {
        loadDefaultAssets();
    }    
    AssetManager& AssetManager::instance() {
        static AssetManager mgr;
        if (mgr.meshes_.empty() || mgr.materials_.empty() || mgr.shaders_.empty()) {
            mgr.loadDefaultAssets();
            mgr.rebuildShaderMaterialDependencies();
        }
        return mgr;
    }

    std::string AssetManager::findShaderNameForMaterial(const std::shared_ptr<Material>& material) const
    {
        if (!material || !material->getShader()) {
            return "";
        }

        auto shaderPtr = material->getShader();
        for (const auto& [shaderName, metaShader] : shaders_) {
            if (metaShader && metaShader->getShader() == shaderPtr) {
                return shaderName;
            }
        }

        return "";
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

        auto oldMetaIt = materialMetadatas_.find(name);
        if (oldMetaIt != materialMetadatas_.end() && !oldMetaIt->second.shaderName.empty()) {
            auto depIt = shaderToMaterials_.find(oldMetaIt->second.shaderName);
            if (depIt != shaderToMaterials_.end()) {
                depIt->second.erase(name);
            }
        }

        materials_[name] = std::move(mat);

        auto& metadata = materialMetadatas_[name];
        if (metadata.shaderName.empty()) {
            metadata.shaderName = findShaderNameForMaterial(materials_[name]);
        }
        if (!metadata.shaderName.empty()) {
            shaderToMaterials_[metadata.shaderName].insert(name);
        }

        return materials_[name];
    }

    std::shared_ptr<Material> AssetManager::getMaterialPtr(const std::string& name) const {
        auto it = materials_.find(name);
        if (it != materials_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void AssetManager::removeMaterial(const std::string& name)
    {
		materials_.erase(name);
    }

    Material* AssetManager::getMaterial(const std::string& name) const {
        auto ptr = getMaterialPtr(name);
        Material* raw = ptr ? ptr.get() : nullptr;
        return raw;
    }

    void AssetManager::setMaterialMetadata(const std::string& materialName, const MaterialMetadata& metadata)
    {
        auto oldIt = materialMetadatas_.find(materialName);
        if (oldIt != materialMetadatas_.end() && !oldIt->second.shaderName.empty()) {
            auto depIt = shaderToMaterials_.find(oldIt->second.shaderName);
            if (depIt != shaderToMaterials_.end()) {
                depIt->second.erase(materialName);
            }
        }

        materialMetadatas_[materialName] = metadata;
        if (!metadata.shaderName.empty()) {
            shaderToMaterials_[metadata.shaderName].insert(materialName);
        }
    }

    const AssetManager::MaterialMetadata* AssetManager::getMaterialMetadata(const std::string& materialName) const
    {
        auto it = materialMetadatas_.find(materialName);
        return it != materialMetadatas_.end() ? &it->second : nullptr;
    }

    std::vector<std::string> AssetManager::getMaterialsUsingShader(const std::string& shaderName) const
    {
        std::vector<std::string> result;
        auto it = shaderToMaterials_.find(shaderName);
        if (it == shaderToMaterials_.end()) {
            return result;
        }

        result.reserve(it->second.size());
        for (const auto& materialName : it->second) {
            result.push_back(materialName);
        }
        return result;
    }

    void AssetManager::rebuildShaderMaterialDependencies()
    {
        shaderToMaterials_.clear();

        for (auto& [materialName, material] : materials_) {
            auto& metadata = materialMetadatas_[materialName];
            if (metadata.shaderName.empty()) {
                metadata.shaderName = findShaderNameForMaterial(material);
            }

            if (!metadata.shaderName.empty()) {
                shaderToMaterials_[metadata.shaderName].insert(materialName);
                
            }
        }
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

    std::shared_ptr<MetaShader> AssetManager::addShader(const std::string& name)
    {
        try {
            auto metaShader = MetaShader::create(name);
            shaders_[name] = std::move(metaShader);
            rebuildShaderMaterialDependencies();
            return shaders_[name];
        } catch (const std::exception& e) {
            std::cerr << "[AssetManager] Failed to load shader '" << name << "': " << e.what() << std::endl;
            return nullptr;
		}
       
	}
    std::shared_ptr<MetaShader> AssetManager::getShaderPtr(const std::string& name) const
    {
        auto it = shaders_.find(name);
        return it != shaders_.end() ? it->second : nullptr;
	}
    void AssetManager::shutdown() {
        for (auto& [name, texture] : textures_) {
            if (texture) {
                texture->shutdown();
            }
        }
        for (auto& [name, shader] : shaders_) {
            if (shader) {
                shader->shutdown();
			}
        }
        meshes_.clear();
        materials_.clear();
        textures_.clear();
        cubeMaps_.clear();
        shaders_.clear();
        materialMetadatas_.clear();
        shaderToMaterials_.clear();

    }
    void AssetManager::loadDefaultAssets()
    {
		loadCube();
		loadSphere();
		loadCylinder();
		loadQuad();
		loadPlane();
		loadDefaultShaders();
		loadDefaultMaterials();
    }
    void AssetManager::loadCube()
    {
        addMesh("cube", std::make_shared<Mesh>(Mesh()));
        Mesh* mesh = getMesh("cube");
        mesh->position.push_back(glm::vec3(-1.0, -1.0, 1.0));
        mesh->color.push_back(glm::vec4(1.0, 0.0, 0.0, 0.5));
        mesh->texCoord.push_back(glm::vec2(0.0f, 0.0f)); // front bottom-left

        mesh->position.push_back(glm::vec3(1.0, -1.0, 1.0));
        mesh->color.push_back(glm::vec4(0.0, 1.0, 0.0, 0.5));
        mesh->texCoord.push_back(glm::vec2(1.0f, 0.0f)); // front bottom-right

        mesh->position.push_back(glm::vec3(1.0, 1.0, 1.0));
        mesh->color.push_back(glm::vec4(0.0, 0.0, 1.0, 0.5));
        mesh->texCoord.push_back(glm::vec2(1.0f, 1.0f)); // front top-right

        mesh->position.push_back(glm::vec3(-1.0, 1.0, 1.0));
        mesh->color.push_back(glm::vec4(1.0, 0.0, 1.0, 0.5));
        mesh->texCoord.push_back(glm::vec2(0.0f, 1.0f)); // front top-left
        // back
        mesh->position.push_back(glm::vec3(-1.0, -1.0, -1.0));
        mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
        // For the back face we mirror U so the texture isn't mirrored when viewed from behind
        mesh->texCoord.push_back(glm::vec2(1.0f, 0.0f)); // back bottom-left (mapped to texture bottom-right)

        mesh->position.push_back(glm::vec3(1.0, -1.0, -1.0));
        mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
        mesh->texCoord.push_back(glm::vec2(0.0f, 0.0f)); // back bottom-right

        mesh->position.push_back(glm::vec3(1.0, 1.0, -1.0));
        mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
        mesh->texCoord.push_back(glm::vec2(0.0f, 1.0f)); // back top-right

        mesh->position.push_back(glm::vec3(-1.0, 1.0, -1.0));
        mesh->color.push_back(glm::vec4(1.0, 1.0, 1.0, 0.5));
        mesh->texCoord.push_back(glm::vec2(1.0f, 1.0f)); // back top-left



        mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
        mesh->indices.push_back(2); mesh->indices.push_back(3); mesh->indices.push_back(0);
        mesh->indices.push_back(1); mesh->indices.push_back(5); mesh->indices.push_back(6);
        mesh->indices.push_back(6); mesh->indices.push_back(2); mesh->indices.push_back(1);
        mesh->indices.push_back(7); mesh->indices.push_back(6); mesh->indices.push_back(5);
        mesh->indices.push_back(5); mesh->indices.push_back(4); mesh->indices.push_back(7);
        mesh->indices.push_back(4); mesh->indices.push_back(0); mesh->indices.push_back(3);
        mesh->indices.push_back(3); mesh->indices.push_back(7); mesh->indices.push_back(4);
        mesh->indices.push_back(4); mesh->indices.push_back(5); mesh->indices.push_back(1);
        mesh->indices.push_back(1); mesh->indices.push_back(0); mesh->indices.push_back(4);
        mesh->indices.push_back(3); mesh->indices.push_back(2); mesh->indices.push_back(6);
        mesh->indices.push_back(6); mesh->indices.push_back(7); mesh->indices.push_back(3);

        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, 1)));
        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));
        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));
        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));
        mesh->normal.push_back(glm::normalize(glm::vec3(0, 0, -1)));
    }
    void AssetManager::loadSphere()
    {
		const int stacks = 50;
		const int slices = 50;
		glm::vec4 color(.5f, .5f, 0.5f, 1.0f);
        addMesh("sphere", std::make_shared<Mesh>(Mesh()));
        Mesh* mesh = getMesh("sphere");

        glm::vec3 center(0.0f);
        float radius = 1.0f;

        // Vertici e normali
        for (int i = 0; i <= stacks; ++i)
        {
            float V = (float)i / stacks;
            float phi = V * glm::pi<float>();

            for (int j = 0; j <= slices; ++j)
            {
                float U = (float)j / slices;
                float theta = U * (glm::pi<float>() * 2.0f);

                float x = center.x + radius * sinf(phi) * cosf(theta);
                float y = center.y + radius * cosf(phi);
                float z = center.z + radius * sinf(phi) * sinf(theta);

                glm::vec3 pos(x, y, z);
                glm::vec3 normal = glm::normalize(pos - center);

                mesh->position.push_back(pos);
                mesh->normal.push_back(normal);
                mesh->color.push_back(color);
                mesh->texCoord.push_back(glm::vec2(U, V));
            }
        }

        // Indici
        for (int i = 0; i < stacks; ++i)
        {
            for (int j = 0; j < slices; ++j)
            {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;

                // Primo triangolo
                mesh->indices.push_back(first);
                mesh->indices.push_back(second);
                mesh->indices.push_back(first + 1);

                // Secondo triangolo
                mesh->indices.push_back(second);
                mesh->indices.push_back(second + 1);
                mesh->indices.push_back(first + 1);
            }
        }
    }
    void AssetManager::loadCylinder()
    {
		glm::vec4 color(.5f, .5f, 0.5f, 1.0f);
        addMesh("cylinder", std::make_shared<Mesh>(Mesh()));
        Mesh* mesh = getMesh("cylinder");
        int Stacks = 30;  //numero di suddivisioni sull'asse x
        int Slices = 30;  // numero di suddivisioni sull'asse y

        float s, t;
        //Calc The Vertices
        for (int i = 0; i <= Stacks; ++i) {

            float V = i / (float)Stacks;
            float h = V;

            // Loop Through Slices
            for (int j = 0; j <= Slices; ++j) {

                float U = j / (float)Slices;
                float theta = U * (glm::pi <float>() * 2);

                // Calc The Vertex Positions
                float x = cosf(theta);
                float y = h;
                float z = sinf(theta);


                // Push Back Vertex Data
                mesh->position.push_back(glm::vec3(x, y, z));
                mesh->color.push_back(color);
                mesh->normal.push_back(glm::vec3(glm::normalize(glm::vec3(cos(theta), 0, sin(theta)))));
                //Coordinata di texture
                s = U;
                t = V;
                mesh->texCoord.push_back(glm::vec2(s, t));
            }
        }

        // Calc The Index Positions
        for (int i = 0; i < Slices * Stacks + Slices; ++i) {

            mesh->indices.push_back(i);
            mesh->indices.push_back(i + Slices + 1);
            mesh->indices.push_back(i + Slices);


            mesh->indices.push_back(i + Slices + 1);
            mesh->indices.push_back(i);
            mesh->indices.push_back(i + 1);
        }



    }
    void AssetManager::loadQuad()
    {
        glm::vec4 color(.5f, .5f, 0.5f, 1.0f);
        addMesh("quad", std::make_shared<Mesh>(Mesh()));
        Mesh* mesh = getMesh("quad");
        mesh->position.push_back(glm::vec3(-1, -1.0, 0));
        mesh->color.push_back(color);
		mesh->texCoord.push_back(glm::vec2(0,0));
		mesh->normal.push_back(glm::vec3(0, 0, 0));
        //
        mesh->position.push_back(glm::vec3(-1, 1.0, 0));
        mesh->color.push_back(color);
        mesh->texCoord.push_back(glm::vec2(0, 1));
        mesh->normal.push_back(glm::vec3(0, 1, 0));
        //
        mesh->position.push_back(glm::vec3(1, 1.0, 0));
        mesh->color.push_back(color);
        mesh->texCoord.push_back(glm::vec2(1, 1));
        mesh->normal.push_back(glm::vec3(0, 1, 0));
        //
		mesh->position.push_back(glm::vec3(1, -1.0, 0));
        mesh->color.push_back(color);
        mesh->texCoord.push_back(glm::vec2(1, 0));
        mesh->normal.push_back(glm::vec3(0, 1, 0));
        //
        mesh->indices.push_back(0);
        mesh->indices.push_back(1);
        mesh->indices.push_back(2);
        //
        mesh->indices.push_back(0);
        mesh->indices.push_back(2);
		mesh->indices.push_back(3);
    }
    void AssetManager::loadPlane()
    {
        glm::vec4 color = { 0.5,0.5,0.5,1 };
        auto mesh = addMesh("plane", std::make_shared<Mesh>(Mesh()));

        mesh->position.push_back(glm::vec3(-0.5, 0.0, 0.5));
        mesh->color.push_back(color);
        mesh->position.push_back(glm::vec3(0.5, 0.0, 0.5));
        mesh->color.push_back(color);
        mesh->position.push_back(glm::vec3(0.5, 0.0, -0.5));
        mesh->color.push_back(color);
        mesh->position.push_back(glm::vec3(-0.5, 0.0, -0.5));
        mesh->color.push_back(color);

        mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
        mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(3);

        mesh->texCoord.push_back(glm::vec2(0.0, 1.0));
        mesh->texCoord.push_back(glm::vec2(1.0, 1.0));
        mesh->texCoord.push_back(glm::vec2(1.0, 0.0));
        mesh->texCoord.push_back(glm::vec2(0.0, 0.0));

        mesh->normal.push_back(glm::vec3(0.0, 1.0, 0.0));
        mesh->normal.push_back(glm::vec3(0.0, 1.0, 0.0));
        mesh->normal.push_back(glm::vec3(0.0, 1.0, 0.0));
        mesh->normal.push_back(glm::vec3(0.0, 1.0, 0.0));



        int nv = mesh->position.size();
        mesh->indices.push_back(nv - 1);
    }
    void AssetManager::loadDefaultMaterials()
    {
		addMaterial("default", std::make_shared<Material>(getShaderPtr("default")))->set("color",glm::vec4(1,0,1,1));
    }
    void AssetManager::loadDefaultShaders()
    {
        std::string defaultMetaShaderCode = R"(
uniform vec4 color;
void fragmentMain()
{
	COLOR = color ;
}

void vertexMain()
{
	POSITION = CAMERA_PROJ * CAMERA_VIEW * vec4(V_WORLD_POS,1);
}
        )";
		shaders_["default"] = MetaShader::create(defaultMetaShaderCode, true);
    }

    bool AssetManager::importMaterialMetadataFromJson(const std::string& jsonPath, const std::string& materialName)
    {
        std::ifstream in(jsonPath);
        if (!in.is_open()) {
            std::cerr << "[AssetManager] importMaterialMetadataFromJson: cannot open file '" << jsonPath << "'\n";
            return false;
        }

        json j;
        try {
            in >> j;
        }
        catch (const std::exception& e) {
            std::cerr << "[AssetManager] importMaterialMetadataFromJson: invalid json in '"
                << jsonPath << "' -> " << e.what() << "\n";
            return false;
        }

        MaterialMetadata metadata{};
        metadata.shaderName = j.value("shaderName", std::string{});
        metadata.sourcePath = j.value("sourcePath", jsonPath);
        metadata.version = j.value("version", 1u);

        if (metadata.shaderName.empty()) {
            std::cerr << "[AssetManager] importMaterialMetadataFromJson: missing 'shaderName' in '" << jsonPath << "'\n";
            return false;
        }

        if (j.contains("params") && j["params"].is_object()) {
            for (auto it = j["params"].begin(); it != j["params"].end(); ++it) {
                MaterialParam param{};
                if (parseMaterialParam(it.value(), param)) {
                    metadata.params[it.key()] = param;
                }
                else {
                    std::cerr << "[AssetManager] importMaterialMetadataFromJson: skipped invalid param '" << it.key() << "' in '" << jsonPath << "'\n";
                }
            }
        }

        if (j.contains("textures") && j["textures"].is_object()) {
            for (auto it = j["textures"].begin(); it != j["textures"].end(); ++it) {
                if (it.value().is_string()) {
                    metadata.textures[it.key()] = it.value().get<std::string>();
                }
            }
        }

        std::string resolvedMaterialName = materialName;
        if (resolvedMaterialName.empty()) {
            resolvedMaterialName = std::filesystem::path(jsonPath).stem().string();
        }

        setMaterialMetadata(resolvedMaterialName, metadata);
        return true;
    }

    bool AssetManager::createMaterialFromMetadata(const std::string& materialName)
    {
        auto metaIt = materialMetadatas_.find(materialName);
        if (metaIt == materialMetadatas_.end()) {
            std::cerr << "[AssetManager] createMaterialFromMetadata: metadata not found for '" << materialName << "'\n";
            return false;
        }

        const MaterialMetadata& metadata = metaIt->second;

        std::shared_ptr<MetaShader> shader = getShaderPtr(metadata.shaderName);
        if (!shader && !metadata.sourcePath.empty()) {
            shader = getShaderPtr(metadata.sourcePath);
            if (!shader) {
                shader = addShader(metadata.sourcePath);
            }
        }
        if (!shader) {
            shader = addShader(metadata.shaderName);
        }

        if (!shader) {
            std::cerr << "[AssetManager] createMaterialFromMetadata: shader not found for material '" << materialName << "'\n";
            return false;
        }

        auto material = std::make_shared<Material>(shader);

        for (const auto& [paramName, param] : metadata.params) {
            switch (param.type) {
            case MaterialParam::Type::Int:
                material->set(paramName, std::get<int>(param.value));
                break;
            case MaterialParam::Type::Float:
                material->set(paramName, std::get<float>(param.value));
                break;
            case MaterialParam::Type::Vec2:
                material->set(paramName, std::get<glm::vec2>(param.value));
                break;
            case MaterialParam::Type::Vec3:
                material->set(paramName, std::get<glm::vec3>(param.value));
                break;
            case MaterialParam::Type::Vec4:
                material->set(paramName, std::get<glm::vec4>(param.value));
                break;
            case MaterialParam::Type::Mat3:
                material->set(paramName, std::get<glm::mat3>(param.value));
                break;
            case MaterialParam::Type::Mat4:
                material->set(paramName, std::get<glm::mat4>(param.value));
                break;
            case MaterialParam::Type::Bool:
                material->set(paramName, std::get<bool>(param.value) ? 1 : 0);
                break;
            }
        }

        for (const auto& [uniformName, textureRef] : metadata.textures) {
            std::shared_ptr<Texture> texture = getTexturePtr(textureRef);
            if (!texture && std::filesystem::exists(textureRef)) {
                texture = Texture::createTexture(textureRef);
                if (texture) {
                    addTexture(textureRef, texture);
                }
            }

            if (texture) {
                material->set(uniformName, texture);
            }
            else {
                std::cerr << "[AssetManager] createMaterialFromMetadata: texture not found '" << textureRef
                    << "' for material '" << materialName << "'\n";
            }
        }

        addMaterial(materialName, material);
        return true;
    }

}