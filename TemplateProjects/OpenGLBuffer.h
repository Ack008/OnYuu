#pragma once
#include <glad/glad.h>
#include "Buffer.h"
class OpenGLVertexBuffer :
	public VertexBuffer
{
	public:
	OpenGLVertexBuffer();
	~OpenGLVertexBuffer();
	void bind() override;
	void unbind() override;
	void setData(const void* data, size_t size, BufferUsage usage) override;
	void updateData(const void* data, size_t size, size_t offset) override;
	void resize(size_t newSize) override;
private:
	GLuint vbo;
};
class OpenGLIndexBuffer :
	public IndexBuffer
{
	public:
	OpenGLIndexBuffer();
	~OpenGLIndexBuffer();
	void bind() override;
	void unbind() override;
	void setData(const void* data, size_t size, BufferUsage usage) override;
	void updateData(const void* data, size_t size, size_t offset) override;
	void resize(size_t newSize) override;
private:
	GLuint ibo;
};