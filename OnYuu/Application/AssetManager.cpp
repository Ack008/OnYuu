#include "AssetManager.h"
#include "Core/CubeMap.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace OnYuu {
    AssetManager::AssetManager() {
        loadDefaultAssets();
    }    
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
            if (texture) {
                texture->shutdown();
            }
        }
        meshes_.clear();
        materials_.clear();
        textures_.clear();
        cubeMaps_.clear();
    }
    void AssetManager::loadDefaultAssets()
    {
		loadCube();
		loadSphere();
		loadCylinder();
		loadQuad();
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

        mesh->position.push_back(glm::vec3(0.0, 0.0, 0.0));
        mesh->color.push_back(glm::vec4(0.0, 1.0, 0.0, 1.0));
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
        auto mesh = addMesh("pianoMesh", std::make_shared<Mesh>(Mesh()));

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
    }
}