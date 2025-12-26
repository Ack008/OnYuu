#include <iostream>
#include "Application/Application.h"
int main()
{
    
	OnYuu::Application* app = OnYuu::Application::createApplication();
    app->Run();
	delete app;
}


