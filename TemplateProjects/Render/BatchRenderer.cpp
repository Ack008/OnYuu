#include "BatchRenderer.h"
#include "Core/Model/Components/Camera.h"
#include <iostream>
BatchRender::BatchRender()
{

}

BatchRender::~BatchRender()
{
}

void BatchRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model)
{

    if (mesh == nullptr || !mesh->material)
        return;
    BatchCouple couple = std::make_pair(mesh->material, mesh->renderingType);
    auto& batch = (*(&batches))[couple];
    batch.push_back(RenderData{ mesh,model });
    if (sceneStarted && !renderScenes.empty()) {
        renderScenes.back().meshRenders.push_back(RenderData{ mesh,model });
	}
}

void BatchRender::clear()
{
	batches.clear();
}

void BatchRender::setSkyBox(SkyBoxComponent* skybox)
{
    if (sceneStarted && !renderScenes.empty()) {
        renderScenes.back().skybox = skybox;
	}
}

void BatchRender::BeginScene(Camera *camera)
{
    if (sceneStarted)
    {
		throw std::runtime_error("Scene already started. Call EndScene() before starting a new one.");
    }
    RenderScene scene;
    scene.activeCamera = camera;
	renderScenes.push_back(scene);
	sceneStarted = true;
}

void BatchRender::EndScene()
{
    sceneStarted = false;
}


void BatchRender::submit()
{
	std::cout << "BatchRender::submit() - Numero di scene da renderizzare: " << renderScenes.size() << std::endl;

	// stampa di debug
    for (const auto& scene : renderScenes) {
        Camera* cam = scene.activeCamera;
        size_t meshCount = scene.meshRenders.size();
        SkyBoxComponent* skybox = scene.skybox;
        Background2DRender* background2D = scene.background2D;
        std::cout << "Scene - Camera: " << (cam ? "Set" : "Not Set")
                  << ", Mesh Renders: " << meshCount
                  << ", SkyBox: " << (skybox ? "Set" : "Not Set")
                  << ", Background2D: " << (background2D ? "Set" : "Not Set")
			<< std::endl;

		
	}
}

std::unordered_map<BatchCouple, std::vector<BatchRender::RenderData>, BatchCoupleHash>* BatchRender::getBatches()
{
	return &batches;
}





