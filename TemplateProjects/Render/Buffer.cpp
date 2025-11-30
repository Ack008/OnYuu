#include "Buffer.h"
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include <memory>

// File: Buffer.cpp
// Implementazioni di helper per creare istanze concrete di buffer in base
// all'API grafica attiva (variabile globale `apiInUse`).

std::shared_ptr<VertexBuffer> VertexBuffer::create()
{
	switch (apiInUse)
	{
	case API::OpenGL:
		// Restituisce l'implementazione OpenGL di VertexBuffer
		return std::make_shared<OpenGLVertexBuffer>();
		break;
	default:
		break;
	}
	return nullptr;
}

std::shared_ptr<IndexBuffer> IndexBuffer::create()
{
	switch (apiInUse)
	{
	case API::OpenGL:
		// Restituisce l'implementazione OpenGL di IndexBuffer
		return std::make_shared<OpenGLIndexBuffer>();
		break;
	default:
		break;
	}
	return nullptr;
}

std::shared_ptr<UniformBuffer> UniformBuffer::create(uint32_t bindingPoint)
{
	switch (apiInUse)
	{
	case API::OpenGL:
		// Restituisce l'implementazione OpenGL di UniformBuffer
		return std::make_shared<OpenGLUniformBuffer>(bindingPoint);
		break;
	default:
		break;
	}
	return nullptr;
}
