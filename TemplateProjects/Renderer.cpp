#include "Renderer.h"
#include "OpenGLBatchRender.h"
std::shared_ptr<Render> Render::s_instance = std::make_shared<OpenGLBatchRender>();



std::shared_ptr<Render> Render::getInstance()
{
	return s_instance;
}





