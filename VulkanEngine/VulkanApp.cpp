#include "Application/Application.h"
#include "VulkanAppLayer.h"
class VulkanApp : public Application {
public:
	VulkanApp() 
		:Application(Vulkan)
	{
		pushLayer(new VulkanAppLayer());
	}
private:
};


Application* Application::createApplication() {
	return new VulkanApp();
}