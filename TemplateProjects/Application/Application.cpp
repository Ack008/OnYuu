#include "Application.h"
#include "Render/Renderer.h"
#include <iostream>

Application* Application::instance = nullptr;

Application::Application()
{
	Application::instance = this;

	window = Window::create(1280, 720);
	Render::init();

	imGuiLayer = new ImGuiLayer();
	imGuiLayer->onAttach();
	//globalDataUBO = UniformBuffer::create();
	//globalDataUBO->setData(nullptr, sizeof(GlobalData), BufferUsage::DYNAMIC);
}

void Application::Run()
{
	while (!window->shouldClose()) {
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
		delete *it;
		layers.erase(it);
	}
}
