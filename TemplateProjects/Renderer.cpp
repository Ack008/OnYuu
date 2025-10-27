#include "Renderer.h"
#include "OpenGLBatchRender.h"
#include "API.h"
std::shared_ptr<Render> Render::s_instance = Render::create();

std::shared_ptr<Render> Render::create()
{
	// Here you can add logic to choose which Render subclass to instantiate
	// based on the current graphics API or other criteria.
	// For simplicity, we'll just return an OpenGLBatchRender instance.
	switch (apiInUse)
	{
		case API::OpenGL:
			// Return OpenGLBatchRender instance
			return std::make_shared<OpenGLBatchRender>();
			break;
	}
}


std::shared_ptr<Render> Render::getInstance()
{
	return s_instance;
}





