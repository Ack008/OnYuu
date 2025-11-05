#include "Engine.h"
#include "GameLayer.h"
#include "GameScene.h"
#include "DebugLayer.h"
class Game2dApplication :
	public Application {
public:
	Game2dApplication() {
		pushLayer(new GameLayer(&scene));
		pushLayer(new DebugLayer(&scene));
	}
private:
	GameScene scene;
};


Application* Application::createApplication() {
	return new Game2dApplication();
}