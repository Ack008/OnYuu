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
	//struct per i dati globali da passare agli shader
	struct GlobalData {
		float iTime;
		float padding1[3];  // allineamento std140 richiede multipli di 16 byte
		float iResolution[2];
		float padding2[2];     // padding per allineamento
		int   iFrame;
		float padding3[3];  // std140: int occupa 4 byte ma deve allinearsi a 16
	};
	GlobalData data;
	std::shared_ptr<UniformBuffer> globalDataUBO;
};