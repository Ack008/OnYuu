#pragma once
#include <glad/glad.h>
#include "VertexArray.h"
class OpenGLVertexArray :
	public VertexArray
{
	public:
	OpenGLVertexArray();
	virtual ~OpenGLVertexArray();
	// Inherited via VertexArray
	virtual void bind() override;
	virtual void unbind() override;
	virtual void setVertexBuffer( VertexBuffer& vbo) override;
	virtual void setIndexBuffer( IndexBuffer& ibo) override;
	virtual void setLayout(const Layout& layout) override;
private:
	GLuint vao;
};