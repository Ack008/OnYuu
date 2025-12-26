#pragma once
#include "Buffer.h"
using namespace OnYuu;
class VertexArray
{
public:
	virtual ~VertexArray() = default;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void setVertexBuffer( VertexBuffer *vbo) = 0;
	virtual void setIndexBuffer( IndexBuffer *ibo) = 0;
	static std::shared_ptr<VertexArray> create();


};