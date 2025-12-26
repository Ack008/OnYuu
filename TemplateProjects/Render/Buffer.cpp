#include "Buffer.h"
#include "Platform/API.h"
#include "Render/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include <memory>

// File: Buffer.cpp
// Implementazioni di helper per creare istanze concrete di buffer in base
// all'API grafica attiva (variabile globale `apiInUse`).

std::shared_ptr<VertexBuffer> VertexBuffer::create()
{
	switch (Render::getAPI())
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
	switch (Render::getAPI())
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

std::shared_ptr<UniformBuffer> UniformBuffer::create(uint32_t bindingPoint, size_t size)
{
	switch (Render::getAPI())
	{
	case API::OpenGL:
		// Restituisce l'implementazione OpenGL di UniformBuffer
		return std::make_shared<OpenGLUniformBuffer>(bindingPoint,size);
		break;
	case API::Vulkan:
		// Restituisce l'implementazione Vulkan di UniformBuffer
		return std::make_shared<VulkanUniformBuffer>(bindingPoint,size);
		break;
	default:
		break;
	}
	return nullptr;
}
