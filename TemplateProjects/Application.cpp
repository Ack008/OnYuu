#include "Application.h"
Application* Application::instance = nullptr;
Application::Application()
{
	Application::instance = this;
	window = Window::create(1280, 720);
	imGuiLayer = new ImGuiLayer();
	imGuiLayer->onAttach();
	globalDataUBO = UniformBuffer::create();
	globalDataUBO->setData(nullptr, sizeof(GlobalData), BufferUsage::DYNAMIC);
}

void Application::Run()
{
	while (!window->shouldClose()) {
		window->beginFrame();
		GlobalData data;
		data.iTime = window->getTime();
		data.iResolution[0] = window->getWidth();
		data.iResolution[1] = window->getHeight();
		data.iFrame = data.iTime / window->getFrameTime();
		globalDataUBO->updateData(&data, sizeof(GlobalData), 0);
		
		//Updating layers
		for (Layer* layer : layers) {
			layer->onUpdate(static_cast<float>(window->getFrameTime()));
		}
		
		//imgui drawing
		imGuiLayer->begin();
		for (Layer* layer : layers) {
			layer->onImGuiRender();
		}
		imGuiLayer->end();
		window->draw();
	}
}

void Application::onResize(uint32_t width, uint32_t height)
{
	window->resize(width, height);
	
}

Application::~Application()
{
	delete imGuiLayer;
}

void Application::pushLayer(Layer* layer)
{
	layers.push_back(layer);
	layer->onAttach();
}
void Application::removeLayer(Layer* layer)
{
	auto it = std::find(layers.begin(), layers.end(), layer);
	if (it != layers.end()) {
		layer->onDetach();
		layers.erase(it);
	}
}
