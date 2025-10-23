#pragma once
#include <iostream>
#include "MeshComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLBuffer.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Buffer.h"
class MeshGPUusage {
public:
	void setMesh(Mesh* m) {
		mesh = m;
	}
	Mesh* getMesh() const {
		return mesh;
	}
	bool isUploaded() const {
		return uploaded;
	}
	void uploadToGPU() {
		if (uploaded) {
			return;
		}
		if (mesh == nullptr) {
			std::cerr << "No mesh to upload!" << std::endl;
			return;
		}
		// Create and bind VBO
		vbo = std::make_shared<OpenGLVertexBuffer>();
		size_t positionSize = mesh->position.size() * sizeof(glm::vec3);
		size_t colorSize = mesh->color.size() * sizeof(glm::vec4);
		size_t totalSize = positionSize + colorSize;
		std::vector<float> bufferData;
		bufferData.reserve((positionSize + colorSize) / sizeof(float));
		for (int i = 0; i < mesh->position.size(); i++) {
			const float* posPtr = glm::value_ptr(mesh->position[i]);
			bufferData.insert(bufferData.end(), posPtr, posPtr + 3);
			const float* colPtr = glm::value_ptr(mesh->color[i]);
			bufferData.insert(bufferData.end(), colPtr, colPtr + 4);
		}
		vbo->setData(bufferData.data(), totalSize, BufferUsage::STATIC);
		vao->setVertexBuffer(*vbo);
		vao->setIndexBuffer(*ibo);
		
		// Define layout
		Layout layout;
		layout.elements_.push_back({ VertexAttributeType::FLOAT, 3, false }); // position
		layout.elements_.push_back({ VertexAttributeType::FLOAT, 4, false }); // color
		vao->setLayout(layout);
		uploaded = true;
	}
	void bind() {
		if (vao) {
			vao->bind();
		}
	}
	
private:
	std::shared_ptr<VertexBuffer> vbo;
	std::shared_ptr<IndexBuffer> ibo;
	std::shared_ptr<VertexArray> vao;
	bool uploaded = false;
	bool useIndexBuffer = false;
	Mesh* mesh = nullptr;

};