#include "Application.h"
#include "Render/Renderer.h"
#include "Platform/API.h"
#include <iostream>
#include "AssetManager.h"
#include "Application/Input/Input.h"

OnYuu::Application* OnYuu::Application::instance = nullptr;
API OnYuu::Application::s_startupAPI = API::OpenGL;
bool OnYuu::Application::s_rendererChangeRequested = false;
API OnYuu::Application::s_requestedRendererAPI = API::Vulkan;
namespace OnYuu {
	Application::Application(API api)
	{
		Render::setAPI(api);
 		Application::instance = this;

		window = Window::create(1600, 900);
		Input::reset();

		Render::init();
		Render::getInstance()->registeringCallbacks();
		AssetManager::instance().initializeDefaultAssets();

		imGuiLayer = new ImGuiLayer();
		imGuiLayer->onAttach();
		//globalDataUBO = UniformBuffer::create();
		//globalDataUBO->setData(nullptr, sizeof(GlobalData), BufferUsage::DYNAMIC);
	}

	void Application::requestRendererChange(API api)
	{
		s_requestedRendererAPI = api;
		s_rendererChangeRequested = true;
	}

	bool Application::consumeRendererChangeRequest(API& api)
	{
		if (!s_rendererChangeRequested) {
			return false;
		}
		api = s_requestedRendererAPI;
		s_startupAPI = s_requestedRendererAPI;
		s_rendererChangeRequested = false;
		return true;
	}

	void Application::Run()
	{
		while (!window->shouldClose() && !s_rendererChangeRequested) {
			window->beginFrame();
			//sendGlobalShaderData();

			//Updating layers
			for (Layer* layer : layers) {
				layer->onUpdate(static_cast<float>(window->getFrameTime()));
			}
			Render::getInstance()->BeginFrame();
			//imgui drawing

			imGuiLayer->begin();
			for (Layer* layer : layers) {
				layer->onImGuiRender();
				
			}
			imGuiLayer->end();
				
			Render::getInstance()->submit();
			window->draw();
		}
		Shutdown();
	}

	void Application::sendGlobalShaderData()
	{
		GlobalData data;
		data.iTime = window->getTime();
		data.iResolution[0] = window->getWidth();
		data.iResolution[1] = window->getHeight();
		data.iFrame = data.iTime / window->getFrameTime();
		globalDataUBO->updateData(&data, sizeof(GlobalData), 0);
	}

	void Application::onResize(uint32_t width, uint32_t height)
	{
		window->resize(width, height);

	}
	void Application::Shutdown()
	{
		// Rimuovo e distruggo i layer allocati dinamicamente per evitare leak/use-after-free
		for (Layer* layer : layers) {
			if (layer) {
				layer->onDetach();
				delete layer;
			}
		}
		layers.clear();

		// Distruggo l'imGui layer se presente
		imGuiLayer->onDetach();
		delete imGuiLayer;

		AssetManager::instance().shutdown();
		Render::getInstance()->Shutdown();
	}
	Application::~Application()
	{

	}

	void Application::pushLayer(Layer* layer)
	{

		if (!layer) {
			std::cerr << "[pushLayer] NULL layer, skipping\n";
			return;
		}

		layers.push_back(layer);
		layer->onAttach();
	}
	void Application::removeLayer(Layer* layer)
	{
		auto it = std::find(layers.begin(), layers.end(), layer);
		if (it != layers.end()) {
			layer->onDetach();
			// se removeLayer viene chiamato su un layer allocato dinamicamente,
			// assumiamo ownership e lo distruggiamo
			delete* it;
			layers.erase(it);
		}
	}
}