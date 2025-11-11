#include "OpenGLVertexArray.h"
#include "Render/Buffer.h"
#include <iostream>
VertexBuffer *vbo_ = NULL;
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
GLenum toOpenGLType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:   return GL_FLOAT;
	case ShaderDataType::Float2:  return GL_FLOAT;
	case ShaderDataType::Float3:  return GL_FLOAT;
	case ShaderDataType::Float4:  return GL_FLOAT;
	case ShaderDataType::Mat3:    return GL_FLOAT;
	case ShaderDataType::Mat4:    return GL_FLOAT;
	case ShaderDataType::Int:     return GL_INT;
	case ShaderDataType::Int2:    return GL_INT;
	case ShaderDataType::Int3:    return GL_INT;
	case ShaderDataType::Int4:    return GL_INT;
	case ShaderDataType::Bool:    return GL_BOOL;
	}
	return 0;
}
void OpenGLVertexArray::setVertexBuffer(VertexBuffer *vbo)
{	

	
	glBindVertexArray(vao);
	vbo->bind();
	
	unsigned int m_VertexBufferIndex = 0;
	const auto& layout = vbo->getLayout();
	for (const auto& element : layout)
	{
		switch (element.type)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				toOpenGLType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.offset);
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
		case ShaderDataType::Bool:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribIPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				toOpenGLType(element.type),
				layout.GetStride(),
				(const void*)element.offset);
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
		{
			uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					count,
					toOpenGLType(element.type),
					element.normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.offset + sizeof(float) * count * i));
				glVertexAttribDivisor(m_VertexBufferIndex, 1);
				m_VertexBufferIndex++;
			}
			break;
		}
		default:
			break;
		}	
		
	}
	vertexBuffer = vbo;
}
void OpenGLVertexArray::setIndexBuffer(IndexBuffer *ibo)
{
	bind();
	ibo->bind();
	indexBuffer = ibo;
}

