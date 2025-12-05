#include "Core/Engine.h"
#include "VulkanAppLayer.h"
class VulkanApp : public Application {
public:
	VulkanApp() 
		:Application()
	{
		pushLayer(new VulkanAppLayer());
	}
private:
};


Application* Application::createApplication() {
	return new VulkanApp();
}