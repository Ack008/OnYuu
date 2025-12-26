#include "Core/Engine.h"
#include "Scene3D.h"
#include "Game3DLayer.h"
class Game3DApplication : public Application {
	public:
	Game3DApplication() {
		pushLayer(new Game3DLayer(&scene));
		//pushLayer(new DebugLayer(&scene));
		scene.start();
	}
private:
	Scene3D scene;
};
/*
Application* Application::createApplication() {
	return new Game3DApplication();
}*/

