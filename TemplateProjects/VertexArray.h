#pragma once
#include "Buffer.h"
#include <vector>
enum class VertexAttributeType
{
	UINT,
	FLOAT,
	BOOL,
};

struct LayoutElement
{
	VertexAttributeType type;
	unsigned int count;
	bool normalized;
};
struct Layout
{
	std::vector<LayoutElement> elements_;
	uint32_t getStride() const
	{
		uint32_t stride = 0;
		for (const auto& element : elements_)
		{
			switch (element.type)
			{
			case VertexAttributeType::UINT:
				return stride += element.count * sizeof(unsigned int);
			case VertexAttributeType::FLOAT:
				return stride += element.count * sizeof(float);
			case VertexAttributeType::BOOL:
				return stride += element.count * sizeof(bool);
			}
		}
		return stride;
	}
};
class VertexArray
{
public:
	virtual ~VertexArray() = 0;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void setVertexBuffer( VertexBuffer& vbo) = 0;
	virtual void setIndexBuffer( IndexBuffer& ibo) = 0;
	virtual void setLayout(const Layout &layout) = 0;
};