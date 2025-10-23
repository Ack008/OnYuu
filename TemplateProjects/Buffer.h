#pragma once
enum class BufferUsage
{
	STATIC,
	DYNAMIC,
	STREAM
};
class VertexBuffer
{
public:
	size_t bufferSize = 0;
	size_t usedSize = 0;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual ~VertexBuffer() = default;
	virtual void setData(const void* data, size_t size, BufferUsage usage) = 0;
	virtual void updateData(const void* data, size_t size, size_t offset) = 0;
	virtual void resize(size_t newSize) = 0;
};

class IndexBuffer
{
public:
	size_t bufferSize = 0;
	size_t usedSize = 0;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual ~IndexBuffer() = default;
	virtual void setData(const void* data, size_t size, BufferUsage usage) = 0;
	virtual void updateData(const void* data, size_t size, size_t offset) = 0;
	virtual void resize(size_t newSize) = 0;
};