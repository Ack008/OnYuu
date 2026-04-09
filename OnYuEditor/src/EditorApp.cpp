#include "Core/Engine.h"
#include "EditorLayer.h"
class EditorApp : public Application {
public:
	EditorApp()
		:Application(Application::getStartupAPI())
	{
		pushLayer(new OnYuu::EditorLayer());
	}
private:
};


Application* Application::createApplication() {
	return new EditorApp();
}