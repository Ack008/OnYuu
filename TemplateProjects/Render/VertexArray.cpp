#include "VertexArray.h"
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include <memory>
std::shared_ptr<VertexArray> VertexArray::create()
{
	switch (apiInUse)
	{
		case API::OpenGL:
		// Return OpenGLVertexArray instance
			return std::make_shared<OpenGLVertexArray>();
			break;
	default:
		break;
	}
}
