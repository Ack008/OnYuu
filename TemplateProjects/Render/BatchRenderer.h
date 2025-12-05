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
typedef std::pair<std::shared_ptr<Material>, RenderingTypeEnum> BatchCouple;
struct BatchCoupleHash {
    std::size_t operator()(const BatchCouple& bc) const noexcept {
        // Hash sul puntatore interno dello shared_ptr (indirizzo oggetto)
        Material* raw = bc.first ? bc.first.get() : nullptr;
        std::size_t h1 = std::hash<Material*>{}(raw);
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
    virtual void setSkyBox(SkyBoxComponent* skybox) override;
	virtual void BeginScene(Camera *camera) override;
    virtual void EndScene() override;
    virtual void submit() override;
protected:
    struct RenderScene {
        Camera* activeCamera = nullptr;
        std::vector<RenderData> meshRenders;
        SkyBoxComponent* skybox = nullptr;
        Background2DRender* background2D = nullptr;
        std::unordered_map<BatchCouple, std::vector<RenderData>, BatchCoupleHash> batches;
    };
	std::vector<RenderScene> renderScenes;
private:
	bool sceneStarted = false;
};
