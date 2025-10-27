#pragma once
#include <vector>
#include <string>
#include <memory>
#include "API.h"
enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

inline unsigned int shaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return 4;
	case ShaderDataType::Float2:   return 4 * 2;
	case ShaderDataType::Float3:   return 4 * 3;
	case ShaderDataType::Float4:   return 4 * 4;
	case ShaderDataType::Mat3:     return 4 * 3 * 3;
	case ShaderDataType::Mat4:     return 4 * 4 * 4;
	case ShaderDataType::Int:      return 4;
	case ShaderDataType::Int2:     return 4 * 2;
	case ShaderDataType::Int3:     return 4 * 3;
	case ShaderDataType::Int4:     return 4 * 4;
	case ShaderDataType::Bool:     return 1;
	}

	return 0;
}

struct BufferElement
{
	std::string name;
	ShaderDataType type;
	uint32_t size;
	size_t offset;
	bool normalized;

	BufferElement() = default;

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: name(name), type(type), size(shaderDataTypeSize(type)), offset(0), normalized(normalized)
	{
	}

	uint32_t GetComponentCount() const
	{
		switch (type)
		{
		case ShaderDataType::Float:   return 1;
		case ShaderDataType::Float2:  return 2;
		case ShaderDataType::Float3:  return 3;
		case ShaderDataType::Float4:  return 4;
		case ShaderDataType::Mat3:    return 3; // 3* float3
		case ShaderDataType::Mat4:    return 4; // 4* float4
		case ShaderDataType::Int:     return 1;
		case ShaderDataType::Int2:    return 2;
		case ShaderDataType::Int3:    return 3;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::Bool:    return 1;
		}
		return 0;
	}
};

class BufferLayout
{
public:
	BufferLayout() {}

	BufferLayout(std::vector<BufferElement> elements)
		: m_Elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	uint32_t GetStride() const { return m_Stride; }
	const std::vector<BufferElement>& GetElements() const { return m_Elements; }

	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
private:
	void CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.offset = offset;
			offset += element.size;
			m_Stride += element.size;
		}
	}
private:
	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride = 0;
};
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
	virtual void setLayout(const BufferLayout& layout) = 0;
	virtual const BufferLayout& getLayout() const = 0;
	std::shared_ptr<VertexBuffer> create();
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
	static std::shared_ptr<IndexBuffer> create();
};