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
    if (sceneStarted && !renderScenes.empty()) {
        BatchCouple couple = std::make_pair(mesh->material, mesh->renderingType);
        renderScenes.back().meshRenders.push_back(RenderData{ mesh,model });
		renderScenes.back().batches[couple].push_back(RenderData{ mesh,model });
    }
    else
    {
        throw std::runtime_error("No active scene. Call BeginScene() before adding mesh renders.");
	}
}

void BatchRender::addLight(LightComponent light, glm::vec3 Position)
{
	if (sceneStarted && !renderScenes.empty()) {
        renderScenes.back().sceneLight.push_back({ light ,Position});
    }
    else {
        throw std::runtime_error("No active scene. Call BeginScene() before adding lights.");
    }
}



void BatchRender::setSkyBox(SkyBoxComponent* skybox)
{
    if (sceneStarted && !renderScenes.empty()) {
        renderScenes.back().skybox = skybox;
    }
    else {
		throw std::runtime_error("No active scene. Call BeginScene() before setting skybox.");
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
    if (!sceneStarted)
	{
		throw std::runtime_error("No active scene to end. Call BeginScene() before ending a scene.");
	}
    sceneStarted = false;
}


void BatchRender::submit()
{
	if (sceneStarted)
        {
        throw std::runtime_error("Cannot submit while a scene is active. Call EndScene() before submitting.");
	}
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




