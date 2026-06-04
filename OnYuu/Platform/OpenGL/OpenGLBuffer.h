#pragma once
#include <glad/glad.h>
#include "Render/Buffer.h"
#include <vector>

namespace OnYuu {

	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer();
		~OpenGLVertexBuffer();
		void bind() override;
		void unbind() override;
		void setData(const void* data, size_t size, BufferUsage usage) override;
		void updateData(const void* data, size_t size, size_t offset) override;
		void resize(size_t newSize) override;
		void setLayout(const BufferLayout& layout) override;
		const BufferLayout& getLayout() const override;
	private:
		GLuint vbo;
		BufferLayout bufferLayout;
	};

	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer();
		~OpenGLIndexBuffer();
		void bind() override;
		void unbind() override;
		void setData(const void* data, size_t size, BufferUsage usage) override;
		void updateData(const void* data, size_t size, size_t offset) override;
		void resize(size_t newSize) override;
	private:
		GLuint ibo;
	};

	class OpenGLUniformBuffer : public UniformBuffer {
	public:
		OpenGLUniformBuffer(uint32_t bindingPoint, size_t size);
		~OpenGLUniformBuffer();
		void bind() override;
		void unbind() override;
		void bindToBindingPoint(uint32_t bindingPoint = 0) override;
		void setData(const void* data, size_t size, BufferUsage usage) override;
		void updateData(const void* data, size_t size, size_t offset) override;
		void resize(size_t newSize) override;
	private:
		GLuint ubo;
	};

	/**
	 * OpenGLStorageBuffer (SSBO)
	 *
	 * GL_SHADER_STORAGE_BUFFER bindato al binding point 3.
	 * Equivalente del VulkanStorageBuffer ma per OpenGL 4.3+.
	 *
	 * Ottimizzazioni incluse:
	 *  - Chunk-hash (4KB) per evitare upload inutili (stessa logica di VulkanStorageBuffer)
	 *  - Persistent mapping con GL_MAP_PERSISTENT_BIT + GL_MAP_COHERENT_BIT
	 *    se disponibile (GL 4.4+); fallback con glBufferSubData
	 *  - Crescita 1.5x automatica su resize, con copia dati esistenti
	 */
	class OpenGLStorageBuffer : public UniformBuffer {
	public:
		// bindingPoint fisso = 3, ma override possibile
		explicit OpenGLStorageBuffer(uint32_t bindingPoint, size_t initialSize);
		~OpenGLStorageBuffer();

		void bind() override;
		void unbind() override;
		void bindToBindingPoint(uint32_t bindingPoint = 0) override;

		void setData(const void* data, size_t size, BufferUsage usage) override;
		void updateData(const void* data, size_t size, size_t offset) override;
		void resize(size_t newSize) override;
		void shutdown() override;

		GLuint getSSBO() const { return ssbo_; }

	private:
		GLuint   ssbo_ = 0;
		uint32_t bindingPoint_ = 3;

		// Persistent mapping (GL 4.4): puntatore diretto alla memoria GPU
		void* persistentPtr_ = nullptr;
		bool     usePersistent_ = false;

		// Chunk-based dirty tracking (identico a VulkanStorageBuffer)
		static constexpr size_t CHUNK_SIZE = 4096;
		std::vector<size_t> chunkHashes_;

		size_t quickHash(const void* data, size_t len) const;

		// Crea (o ricrea) il buffer; se oldData != nullptr copia i vecchi dati
		void createBuffer(size_t newSize, const void* oldData = nullptr,
			size_t oldSize = 0);
		void unmapPersistent();
	};

} // namespace OnYuu