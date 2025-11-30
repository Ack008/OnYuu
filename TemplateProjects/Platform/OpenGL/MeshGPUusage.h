#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "Core/Model/Components/MeshComponent.h"
#include "Render/Buffer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Render/RenderCommand.h"
#include "Core/RenderingTypeEnum.h"

// MeshGPUusage
// -----------------
// Scopo:
//   - Gestire il caricamento di una Mesh sulla GPU e fornire le operazioni di
//     bind/draw per il rendering. Contiene la logica per creare VBO, IBO e VAO,
//     interlecciare i dati di posizione/colore e inviarli alla GPU.
//
// Nota importante - OpenGL-specific:
//   - Questa classe e fortemente legata a OpenGL: usa esplicitamente
//     `OpenGLVertexBuffer`, `OpenGLVertexArray`, chiamate a `RenderCommand` e
//     il layout degli attributi atteso da GLSL. Non e progettata per essere
//     riutilizzata con altre API grafiche (DirectX, Vulkan, Metal) nello stato
//     attuale. 
//

class MeshGPUusage {
public:
	void setMesh(Mesh* m) {
		uploaded = false;
		mesh = m;
		useIndexBuffer = mesh->indices.size() > 0;
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
		std::vector<BufferElement> elements;
		if (mesh->position.size() > 0)
			elements.push_back({ ShaderDataType::Float3, "aPos", false });
		if (mesh->color.size() > 0)
			elements.push_back({ ShaderDataType::Float4, "Color", false });
		if (mesh->texCoord.size() > 0)
			elements.push_back({ ShaderDataType::Float2, "aTexCoord", false });
		if (mesh->normal.size() > 0)
			elements.push_back({ ShaderDataType::Float3, "aNormal", false });
		vbo->setLayout(BufferLayout(elements));

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
			// TexCoord
			if (mesh->texCoord.size() > 0) {
				bufferData.push_back(mesh->texCoord[i].x);
				bufferData.push_back(mesh->texCoord[i].y);
			}
			// Normal
			if (mesh->normal.size() > 0) {
				bufferData.push_back(mesh->normal[i].x);
				bufferData.push_back(mesh->normal[i].y);
				bufferData.push_back(mesh->normal[i].z);
			}
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
		
		// If an index buffer is intended to be used, create and bind it safely.
		if (useIndexBuffer) {
			if (!ibo) {
				ibo = IndexBuffer::create();
				ibo->setData(mesh->indices.data(), mesh->indices.size() * sizeof(uint32_t), BufferUsage::STATIC);
			}
			if (vao && ibo) {
				vao->setIndexBuffer(ibo.get());
			} else {
				std::cerr << "Failed to bind IBO to VAO: vao or ibo is null." << std::endl;
				// non return: può essere opzionale, ma loggato
			}
		}

		uploaded = true;
	}
	void bind() {
		if (vao) {
			vao->bind();
		}
	}
	VertexBuffer* getVBO() const { return vbo.get(); }
	void draw(RenderingTypeEnum renderingMode) {
		if (!uploaded) {
			std::cerr << "Mesh not uploaded to GPU, cannot draw!" << std::endl;
			return;
		}
		bind();
		if (useIndexBuffer && ibo) {
			RenderCommand::DRAW_ELEMENTS(renderingMode, mesh->indices.size());
		} else {
			RenderCommand::DRAW_ARRAYS(renderingMode, 0, mesh->position.size());
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