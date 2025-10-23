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
	unbind();
}
void OpenGLVertexBuffer::updateData(const void* data, size_t size, size_t offset)
{
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	usedSize = std::max(usedSize, offset + size);
	unbind();
}
void OpenGLVertexBuffer::resize(size_t newSize)
{
	bind();
	glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
	bufferSize = newSize;
	usedSize = 0;
	unbind();
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
	unbind();
}
void OpenGLIndexBuffer::updateData(const void* data, size_t size, size_t offset)
{
	bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
	usedSize = std::max(usedSize, offset + size);
	unbind();
}
void OpenGLIndexBuffer::resize(size_t newSize)
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
	bufferSize = newSize;
	usedSize = 0;
	unbind();
}


