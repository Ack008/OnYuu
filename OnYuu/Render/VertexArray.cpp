#include "VertexArray.h"
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include <memory>
#include "Render/Renderer.h"
std::shared_ptr<VertexArray> VertexArray::create()
{
	switch (Render::getAPI())
	{
		case API::OpenGL:
		// Return OpenGLVertexArray instance
			return std::make_shared<OpenGLVertexArray>();
			break;
	default:
		break;
	}
}
