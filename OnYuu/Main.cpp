#include <iostream>
#include "Application/Application.h"
int main()
{
	bool restartApplication = false;
	do {
		OnYuu::Application* app = OnYuu::Application::createApplication();
		app->Run();
		API nextAPI = OnYuu::Application::getStartupAPI();
		restartApplication = OnYuu::Application::consumeRendererChangeRequest(nextAPI);
		delete app;
		if (restartApplication) {
			OnYuu::Application::setStartupAPI(nextAPI);
		}
	} while (restartApplication);
}


