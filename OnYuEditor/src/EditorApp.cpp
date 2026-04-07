#include "Core/Engine.h"
#include "EditorLayer.h"
class EditorApp : public Application {
public:
	EditorApp()
		:Application(Vulkan)
	{
		pushLayer(new EditorLayer());
	}
private:
};


Application* Application::createApplication() {
	return new EditorApp();
}