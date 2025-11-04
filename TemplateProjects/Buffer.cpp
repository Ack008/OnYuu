#include "Buffer.h"
#include "API.h"
#include "OpenGLBuffer.h"
#include <memory>
std::shared_ptr<VertexBuffer> VertexBuffer::create()
{
	switch (apiInUse)
	{
	case API::OpenGL:
		// Return OpenGLVertexArray instance
		return std::make_shared<OpenGLVertexBuffer>();
		break;
	default:
		break;
	}
}

std::shared_ptr<IndexBuffer> IndexBuffer::create()
{
	switch (apiInUse)
	{
	case API::OpenGL:
		// Return OpenGLVertexArray instance
		return std::make_shared<OpenGLIndexBuffer>();
		break;
	default:
		break;
	}
}

std::shared_ptr<UniformBuffer> UniformBuffer::create()
{
	switch (apiInUse)
	{
	case API::OpenGL:
		// Return OpenGLVertexArray instance
		return std::make_shared<OpenGLUniformBuffer>();
		break;
	default:
		break;
	}
}
