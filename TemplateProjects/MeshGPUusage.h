#pragma once
#include <iostream>
#include "MeshComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <memory>
#include <vector>

class MeshGPUusage {
public:
	void setMesh(Mesh* m) {
		uploaded = false;
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
		if (mesh->position.empty()) {
			std::cerr << "Mesh has no positions to upload!" << std::endl;
			return;
		}
		// Ensure colors match positions (simple validation)
		if (mesh->color.size() != mesh->position.size()) {
			std::cerr << "Mesh color/position size mismatch: "
				<< mesh->color.size() << " colors vs " << mesh->position.size() << " positions." << std::endl;
			return;
		}

		// Create VAO if missing
		if (!vao) {
			vao = std::make_shared<OpenGLVertexArray>();
		}

		// Create VBO
		if (!vbo) {
			vbo = std::make_shared<OpenGLVertexBuffer>();
		}
		vao->bind();
		vbo->setLayout(BufferLayout({
				{ ShaderDataType::Float3, "aPos", false },
				{ ShaderDataType::Float4, "Color", false }
			}));

		// Interleave position (vec3) and color (vec4) into a float buffer
		std::vector<float> bufferData;
		for (size_t i = 0; i < mesh->position.size(); ++i) {
			// Position
			bufferData.push_back(mesh->position[i].x);
			bufferData.push_back(mesh->position[i].y);
			bufferData.push_back(mesh->position[i].z);
			// Color
			bufferData.push_back(mesh->color[i].r);
			bufferData.push_back(mesh->color[i].g);
			bufferData.push_back(mesh->color[i].b);
			bufferData.push_back(mesh->color[i].a);
		}
		
		// total size in bytes
		size_t totalSizeBytes = bufferData.size() * sizeof(float);
		if (totalSizeBytes == 0) {
			std::cerr << "Computed buffer size is 0 bytes, aborting upload." << std::endl;
			return;
		}

		// Upload data to GPU VBO
		vbo->setData(bufferData.data(), totalSizeBytes, BufferUsage::STATIC);
		


		vao->setVertexBuffer(vbo.get());
		

		// Attach VBO to VAO (only if vao valid)
		
		// If an index buffer is intended to be used, create and bind it safely.
		if (useIndexBuffer) {
			if (!ibo) {
				ibo = std::make_shared<OpenGLIndexBuffer>();
			}
			if (vao && ibo) {
				vao->setIndexBuffer(ibo.get());
			} else {
				std::cerr << "Failed to bind IBO to VAO: vao or ibo is null." << std::endl;
				// non return: può essere opzionale, ma loggato
			}
		}

		// Define layout: position (3 floats) then color (4 floats)
		
		uploaded = true;
	}
	void bind() {
		if (vao) {
			vao->bind();
		}
	}
	VertexBuffer* getVBO() const { return vbo.get(); }
	
private:
	std::shared_ptr<VertexBuffer> vbo;
	std::shared_ptr<IndexBuffer> ibo;
	std::shared_ptr<VertexArray> vao;
	bool uploaded = false;
	bool useIndexBuffer = false;
	Mesh* mesh = nullptr;

};