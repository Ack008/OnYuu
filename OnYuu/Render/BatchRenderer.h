#pragma once
#include <utility>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Core/RenderingTypeEnum.h"
#include "Core/Model/Components/MeshComponent.h"
#include "Core/Shader.h"	
#include "Render/Renderer.h"
#include "Core/Material.h"
#include "Core/Model/Components/SkyBoxComponent.h"
#include "Core/Model/Components/BackGround2DRender.h"
#include "Core/Model/Components/Camera.h"
namespace OnYuu {

    typedef std::pair<std::string, RenderingTypeEnum> BatchCouple;
    struct BatchCoupleHash {
        std::size_t operator()(const BatchCouple& bc) const noexcept {
            // Hash sul puntatore interno dello shared_ptr (indirizzo oggetto)
            std::size_t h1 = std::hash<std::string>{}(bc.first);
            std::size_t h2 = std::hash<int>{}(static_cast<int>(bc.second));
            return h1 ^ (h2 << 1); // semplice combinazione XOR
        }
    };
    class BatchRender : public Render
    {
    protected:
        struct RenderData {
            RenderMeshComponent* renderMesh;
            glm::mat4 model;
        };
    public:
        BatchRender();
        ~BatchRender();
        virtual void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) override;
        virtual void addLight(LightComponent light, glm::vec3 position) override;
        virtual void setSkyBox(SkyBoxComponent* skybox) override;
        virtual void BeginScene(Camera* camera, std::shared_ptr<RenderTarget> target = nullptr) override;
        virtual void EndScene() override;
        virtual void submit() override;
    protected:
        struct LightInfo {
            LightComponent light;
            glm::vec3 position;
        };
        struct RenderScene {
            Camera* activeCamera = nullptr;
            std::vector<LightInfo> sceneLight;
            std::vector<RenderData> meshRenders;
            SkyBoxComponent* skybox = nullptr;
            Background2DRender* background2D = nullptr;
            std::unordered_map<BatchCouple, std::vector<RenderData>, BatchCoupleHash> batches;
            std::shared_ptr<RenderTarget> target;
        };
        std::vector<RenderScene> renderScenes;
    private:
        bool sceneStarted = false;
    };
} // namespace OnYuu
