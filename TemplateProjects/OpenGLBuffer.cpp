#include <glad/glad.h>
#include "OpenGLBuffer.h"
#include <algorithm>
OpenGLVertexBuffer::OpenGLVertexBuffer()
{
	glGenBuffers(1, &vbo);
}
OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &vbo);
}
void OpenGLVertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}
void OpenGLVertexBuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void OpenGLVertexBuffer::setLayout(const BufferLayout& layout)
{
	bufferLayout = layout;
}
const BufferLayout& OpenGLVertexBuffer::getLayout() const
{
	return bufferLayout;
}
void OpenGLVertexBuffer::setData(const void* data, size_t size, BufferUsage usage)
{
	GLenum glUsage = GL_STATIC_DRAW;
	switch (usage) {
	case BufferUsage::STATIC:
		glUsage = GL_STATIC_DRAW;
		break;
	case BufferUsage::DYNAMIC:
		glUsage = GL_DYNAMIC_DRAW;
		break;
	case BufferUsage::STREAM:
		glUsage = GL_STREAM_DRAW;
		break;
	}
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, glUsage);
	bufferSize = size;
	usedSize = size;
}

void OpenGLVertexBuffer::updateData(const void* data, size_t size, size_t offset)
{
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	usedSize = std::max(usedSize, offset + size);
}
void OpenGLVertexBuffer::resize(size_t newSize)
{
	bind();
	glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
	bufferSize = newSize;
	usedSize = 0;
}

OpenGLIndexBuffer::OpenGLIndexBuffer()
{
	glGenBuffers(1, &ibo);
}
OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &ibo);
}
void OpenGLIndexBuffer::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}
void OpenGLIndexBuffer::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void OpenGLIndexBuffer::setData(const void* data, size_t size, BufferUsage usage)
{
	GLenum glUsage = GL_STATIC_DRAW;
	switch (usage) {
	case BufferUsage::STATIC:
		glUsage = GL_STATIC_DRAW;
		break;
	case BufferUsage::DYNAMIC:
		glUsage = GL_DYNAMIC_DRAW;
		break;
	case BufferUsage::STREAM:
		glUsage = GL_STREAM_DRAW;
		break;
	}
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, glUsage);
	bufferSize = size;
	usedSize = size;
}
void OpenGLIndexBuffer::updateData(const void* data, size_t size, size_t offset)
{
	bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
	usedSize = std::max(usedSize, offset + size);
}
void OpenGLIndexBuffer::resize(size_t newSize)
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
	bufferSize = newSize;
	usedSize = 0;
}

OpenGLUniformBuffer::OpenGLUniformBuffer()
{
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 8, nullptr, GL_DYNAMIC_DRAW); // spazio iniziale
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo); // binding point 0
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	glDeleteBuffers(1, &ubo);
}

void OpenGLUniformBuffer::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
}

void OpenGLUniformBuffer::unbind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::setData(const void* data, size_t size, BufferUsage usage)
{
	GLenum glUsage = GL_STATIC_DRAW;
	switch (usage) {
	case BufferUsage::STATIC:
		glUsage = GL_STATIC_DRAW;
		break;
	case BufferUsage::DYNAMIC:
		glUsage = GL_DYNAMIC_DRAW;
		break;
	case BufferUsage::STREAM:
		glUsage = GL_STREAM_DRAW;
		break;
	}
	bind();
	glBufferData(GL_UNIFORM_BUFFER, size, data, glUsage);
	bufferSize = size;
	usedSize = size;
}

void OpenGLUniformBuffer::updateData(const void* data, size_t size, size_t offset)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	usedSize = std::max(usedSize, offset + size);
}

void OpenGLUniformBuffer::resize(size_t newSize)
{
	bind();
	glBufferData(GL_UNIFORM_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
	bufferSize = newSize;
	usedSize = 0;
}
