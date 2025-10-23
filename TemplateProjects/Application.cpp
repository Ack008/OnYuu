#include "Application.h"
#include "OpenGLView.h"
#include "Prova.h"
Application* Application::instance = nullptr;
Application::Application()
{
	Application::instance = this;
	window = new OpenGLWindow(1600, 900);
	currentScene = new Prova();
	currentScene->start();
}

void Application::Run()
{
	while (!window->shouldClose()) {
		currentScene->update(static_cast<float>(window->getFrameTime()));
		window->draw();
	}
}

void Application::onResize(uint32_t width, uint32_t height)
{
	currentScene->OnResize(width, height);
	window->resize(width, height);
}

Application::~Application()
{
	delete window;
	delete currentScene;
}
