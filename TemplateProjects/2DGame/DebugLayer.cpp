#include "DebugLayer.h"
#include <iostream>
#include "Render/Renderer.h"
#include "Core/Model/Components/Camera.h"
#include "Application/Application.h"
#include "Core/Model/Components/Transform.h"
#include "Core/Model/Components/Collider.h"
#include "Core/Model/Components/ScriptingSystem.h"
void DebugLayer::onEvent()
{
}
void DebugLayer::onImGuiRender()
{
	
}
void DebugLayer::onAttach()
{
	std::cout << "DebugLayer attached!" << std::endl;
	debugShader = Shader::create("vertexShaderC.glsl", "fragmentShaderC.glsl");
	debugMaterial = std::make_shared<Material>(debugShader);
	
}
void DebugLayer::onDetach()
{
	delete currentScene;
	std::cout << "DebugLayer detached!" << std::endl;
}


    void DebugLayer::onUpdate(float deltaTime)
    {

        uint16_t width = Application::getInstance()->getWindow()->getWidth();
        uint16_t height = Application::getInstance()->getWindow()->getHeight();
        Orthographic camera(0.f, width, height, 0, 0, -20);

        auto view = currentScene->reg->view<Trasform, ScriptingSystem >();
        debugMeshes.clear();
        debugMeshData.clear();


        // Metodo sicuro: controlla ogni entity, poi dynamic_cast sulla reference del Collider
        for (auto [entity, transform, script] : view.each()) {
            if (script.hasScript<BoxCollider>() == false) {
                continue; // salta se non ha BoxCollider
            }
			auto& box = script.getScript<BoxCollider>();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);
            model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
            model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
            model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
            model = glm::scale(model, transform.scale);

            glm::vec3 minPoint = box.getMinPoint();
            glm::vec3 maxPoint = box.getMaxPoint();

            auto debugMesh = std::make_shared<Mesh>(Mesh{
               {
                     {minPoint.x, minPoint.y, 0},
                     {maxPoint.x, minPoint.y, 0},
                     {maxPoint.x, maxPoint.y, 0},
                     {minPoint.x, maxPoint.y, 0},
                },
                {
                    {1.0f, 0.0f, 0.0f,1.0f},
                    {1.0f, 0.0f, 0.0f,1.0f},
                    {1.0f, 0.0f, 0.0f,1.0f},
                    {1.0f, 0.0f, 0.0f,1.0f},
                 },
                {},
				{ },
                {    0, 1,
                    1, 2,
                    2, 3,
                    3, 0,
                }
                });

            if (debugMesh->position.empty()) {
                std::cerr << "DebugLayer: mesh empty for entity " << (uint32_t)entity << "\n";
                continue;
            }

            auto comp = std::make_unique<RenderMeshComponent>();
            // assegniamo la shared_ptr direttamente
            comp->mesh = debugMesh;
            comp->renderingType = RenderingTypeEnum::LINE;
            comp->material = debugMaterial;

            Render::getInstance()->addMeshRender(comp.get(), model);
            debugMeshes.push_back(std::move(comp));
            debugMeshData.push_back(debugMesh);
    }

    Render::getInstance()->draw();
    Render::getInstance()->clear();

}
