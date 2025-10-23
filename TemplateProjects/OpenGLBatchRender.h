#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include "BatchRenderer.h"
#include "Shader.h"


class OpenGLBatchRender 
	: public BatchRender
{
public:
	virtual void draw() override;
	virtual void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) override;

	OpenGLBatchRender();
	~OpenGLBatchRender();

private:
	std::vector<float> getData(const BatchCouple& couple);
	struct VertexBuffer {
		GLuint vbo;
		size_t usedSize = 0;
		size_t size = 0;
	};
	GLuint vao;
	std::unordered_map<BatchCouple, VertexBuffer,BatchCoupleHash> vbosMap;

};

