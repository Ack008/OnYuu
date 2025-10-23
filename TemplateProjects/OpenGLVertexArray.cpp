#include "OpenGLVertexArray.h"
OpenGLVertexArray::OpenGLVertexArray()
{
	glGenVertexArrays(1, &vao);
}
OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &vao);
}
void OpenGLVertexArray::bind()
{
	glBindVertexArray(vao);
}
void OpenGLVertexArray::unbind()
{
	glBindVertexArray(0);
}
void OpenGLVertexArray::setVertexBuffer(VertexBuffer& vbo)
{
	bind();
	vbo.bind();
	unbind();
}
void OpenGLVertexArray::setIndexBuffer(IndexBuffer& ibo)
{
	bind();
	ibo.bind();
	unbind();
}
GLenum toOpenGLType(VertexAttributeType type)
{
	switch (type)
	{
	case VertexAttributeType::UINT:
		return GL_UNSIGNED_INT;
	case VertexAttributeType::FLOAT:
		return GL_FLOAT;
	case VertexAttributeType::BOOL:
		return GL_BOOL;
	default:
		return GL_FLOAT; // Default a FLOAT
	}
}
size_t getTypeSize(VertexAttributeType type)
{
	switch (type)
	{
	case VertexAttributeType::UINT:
		return sizeof(unsigned int);
	case VertexAttributeType::FLOAT:
		return sizeof(float);
	case VertexAttributeType::BOOL:
		return sizeof(bool);
	default:
		return sizeof(float); // Default a FLOAT
	}
}
void OpenGLVertexArray::setLayout(const Layout& layout)
{
	bind();
	size_t offset = 0;
	for (size_t i = 0; i < layout.elements_.size(); i++) {
		const LayoutElement& element = layout.elements_[i];
		glEnableVertexAttribArray(static_cast<GLuint>(i));
		glVertexAttribPointer(
			static_cast<GLuint>(i),
			element.count,
			toOpenGLType(element.type),
			element.normalized ? GL_TRUE : GL_FALSE,
			static_cast<GLsizei>(layout.getStride()),
			reinterpret_cast<const void*>(offset)
		);
		offset += element.count * getTypeSize(element.type); // Assumendo GLfloat per semplicità
	}
	unbind();
}