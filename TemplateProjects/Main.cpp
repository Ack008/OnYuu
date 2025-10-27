#include <iostream>
#include "Application.h"
int main()
{
    
	Application* app = Application::createApplication();
    app->Run();
	delete app;
}


