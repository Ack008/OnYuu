#pragma once
#include "Layer.h"
#include "View.h"
#include "ImGuiLayer.h"
class Application {
public:
	Application();
	void Run();
	void onResize(uint32_t width, uint32_t height);
	void pushLayer(Layer* layer);
	void removeLayer(Layer* layer);
	static Application* getInstance() {
		if (instance == nullptr) {
			instance = new Application();
		}
		return instance;
	}
	std::shared_ptr<Window> getWindow() const { return window; }
	 ~Application();
	static Application* createApplication();
private:
	std::vector<Layer*> layers;
	ImGuiLayer* imGuiLayer;
	std::shared_ptr<Window> window;
	static Application* instance;
};