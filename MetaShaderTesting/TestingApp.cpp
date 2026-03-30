#include "Core/Engine.h"
#include "TestLayer.h"
class MetaShaderTestApp : public Application {
public:
	MetaShaderTestApp() 
		:Application(OpenGL)
	{
		pushLayer(new MetaShaderTestLayer());
	}
private:
};


Application* Application::createApplication() {
	return new MetaShaderTestApp();
}