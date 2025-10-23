#pragma once
#include "View.h"
class Application {
public:
	Application();
	void Run();
	void onResize(uint32_t width, uint32_t height);
	static Application* getInstance() {
		if (instance == nullptr) {
			instance = new Application();
		}
		return instance;
	}
	Window* getWindow() const { return window; }
	 ~Application();
private:
	Window* window;
	Scene* currentScene;
	static Application* instance;
};