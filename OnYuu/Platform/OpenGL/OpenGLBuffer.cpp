#include <glad/glad.h>
#include "OpenGLBuffer.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace OnYuu {

	// =========================================================================
	// OpenGLVertexBuffer (invariato)
	// =========================================================================

	OpenGLVertexBuffer::OpenGLVertexBuffer() { glGenBuffers(1, &vbo); }
	OpenGLVertexBuffer::~OpenGLVertexBuffer() { glDeleteBuffers(1, &vbo); }
	void OpenGLVertexBuffer::bind() { glBindBuffer(GL_ARRAY_BUFFER, vbo); }
	void OpenGLVertexBuffer::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout) { bufferLayout = layout; }
	const BufferLayout& OpenGLVertexBuffer::getLayout() const { return bufferLayout; }

	void OpenGLVertexBuffer::setData(const void* data, size_t size, BufferUsage usage) {
		GLenum glUsage = (usage == BufferUsage::DYNAMIC) ? GL_DYNAMIC_DRAW
			: (usage == BufferUsage::STREAM) ? GL_STREAM_DRAW
			: GL_STATIC_DRAW;
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, glUsage);
		bufferSize = size; usedSize = size;
	}
	void OpenGLVertexBuffer::updateData(const void* data, size_t size, size_t offset) {
		bind();
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		usedSize = std::max(usedSize, offset + size);
	}
	void OpenGLVertexBuffer::resize(size_t newSize) {
		bind();
		glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
		bufferSize = newSize; usedSize = 0;
	}

	// =========================================================================
	// OpenGLIndexBuffer (invariato)
	// =========================================================================

	OpenGLIndexBuffer::OpenGLIndexBuffer() { glGenBuffers(1, &ibo); }
	OpenGLIndexBuffer::~OpenGLIndexBuffer() { glDeleteBuffers(1, &ibo); }
	void OpenGLIndexBuffer::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); }
	void OpenGLIndexBuffer::unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

	void OpenGLIndexBuffer::setData(const void* data, size_t size, BufferUsage usage) {
		GLenum glUsage = (usage == BufferUsage::DYNAMIC) ? GL_DYNAMIC_DRAW
			: (usage == BufferUsage::STREAM) ? GL_STREAM_DRAW
			: GL_STATIC_DRAW;
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, glUsage);
		bufferSize = size; usedSize = size;
	}
	void OpenGLIndexBuffer::updateData(const void* data, size_t size, size_t offset) {
		bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
		usedSize = std::max(usedSize, offset + size);
	}
	void OpenGLIndexBuffer::resize(size_t newSize) {
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
		bufferSize = newSize; usedSize = 0;
	}

	// =========================================================================
	// OpenGLUniformBuffer (invariato)
	// =========================================================================

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t bindingPoint, size_t size) {
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	OpenGLUniformBuffer::~OpenGLUniformBuffer() { glDeleteBuffers(1, &ubo); }
	void OpenGLUniformBuffer::bind() { glBindBuffer(GL_UNIFORM_BUFFER, ubo); }
	void OpenGLUniformBuffer::unbind() { glBindBuffer(GL_UNIFORM_BUFFER, 0); }
	void OpenGLUniformBuffer::bindToBindingPoint(uint32_t bp) {
		bind(); glBindBufferBase(GL_UNIFORM_BUFFER, bp, ubo); unbind();
	}
	void OpenGLUniformBuffer::setData(const void* data, size_t size, BufferUsage usage) {
		GLenum glUsage = (usage == BufferUsage::DYNAMIC) ? GL_DYNAMIC_DRAW
			: (usage == BufferUsage::STREAM) ? GL_STREAM_DRAW
			: GL_STATIC_DRAW;
		bind();
		glBufferData(GL_UNIFORM_BUFFER, size, data, glUsage);
		bufferSize = size; usedSize = size;
	}
	void OpenGLUniformBuffer::updateData(const void* data, size_t size, size_t offset) {
		bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		usedSize = std::max(usedSize, offset + size);
	}
	void OpenGLUniformBuffer::resize(size_t newSize) {
		bind();
		glBufferData(GL_UNIFORM_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
		bufferSize = newSize; usedSize = 0;
	}

	// =========================================================================
	// OpenGLStorageBuffer (SSBO) — NUOVO
	// =========================================================================

	// Usa FNV-1a veloce per chunk di 4KB (stessa logica di VulkanStorageBuffer)
	size_t OpenGLStorageBuffer::quickHash(const void* data, size_t len) const {
		size_t hash = 14695981039346656037ULL;
		const uint8_t* p = static_cast<const uint8_t*>(data);
		for (size_t i = 0; i < len; ++i) {
			hash ^= p[i];
			hash *= 1099511628211ULL;
		}
		return hash;
	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t bindingPoint, size_t initialSize)
		: bindingPoint_(bindingPoint)
	{
		createBuffer(initialSize);
		bufferSize = initialSize;
		std::cout << "[OpenGLStorageBuffer] Created ssbo=" << ssbo_
			<< " bp=" << bindingPoint_ << " size=" << initialSize
			<< (usePersistent_ ? " [persistent]" : " [subdata]") << "\n";
	}

	OpenGLStorageBuffer::~OpenGLStorageBuffer() {
		shutdown();
	}

	void OpenGLStorageBuffer::createBuffer(size_t newSize,
		const void* oldData,
		size_t      oldSize) {
		// Smappa il vecchio buffer se era persistente
		unmapPersistent();

		GLuint newSSBO = 0;
		glGenBuffers(1, &newSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);

		// Prova persistent mapping (GL 4.4 / ARB_buffer_storage)
		// Vantaggi: CPU scrive direttamente in VRAM senza glBufferSubData ogni frame
		usePersistent_ = (GLAD_GL_ARB_buffer_storage != 0 ||
			(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 4)));

		if (usePersistent_) {
			GLbitfield flags = GL_MAP_WRITE_BIT |
				GL_MAP_PERSISTENT_BIT |
				GL_MAP_COHERENT_BIT;
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, newSize, oldData ? nullptr : nullptr, flags);

			// Copia vecchi dati se presenti
			if (oldData && oldSize > 0) {
				void* ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, newSize,
					GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
				if (ptr) {
					std::memcpy(ptr, oldData, std::min(oldSize, newSize));
					glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
				}
			}

			// Mappa persistente
			persistentPtr_ = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, newSize,
				GL_MAP_WRITE_BIT |
				GL_MAP_PERSISTENT_BIT |
				GL_MAP_COHERENT_BIT);
			if (!persistentPtr_) {
				// Fallback: buffer storage senza persistent
				usePersistent_ = false;
				std::cerr << "[OpenGLStorageBuffer] Persistent map failed, using subdata\n";
			}
		}

		if (!usePersistent_) {
			// Fallback GL 4.3: DYNAMIC_DRAW normale
			glBufferData(GL_SHADER_STORAGE_BUFFER, newSize,
				(oldData && oldSize > 0) ? oldData : nullptr,
				GL_DYNAMIC_DRAW);
		}

		// Se c'era un vecchio buffer, distruggilo dopo aver creato il nuovo
		if (ssbo_ != 0)
			glDeleteBuffers(1, &ssbo_);

		ssbo_ = newSSBO;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint_, ssbo_);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Reset chunk hashes — tutti i dati sono nuovi
		chunkHashes_.clear();
	}

	void OpenGLStorageBuffer::unmapPersistent() {
		if (persistentPtr_ && ssbo_ != 0) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			persistentPtr_ = nullptr;
		}
	}

	void OpenGLStorageBuffer::bind() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
	}

	void OpenGLStorageBuffer::unbind() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void OpenGLStorageBuffer::bindToBindingPoint(uint32_t bp) {
		bindingPoint_ = bp;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bp, ssbo_);
	}

	// -------------------------------------------------------------------------
	// setData — upload con chunk-hash dirty tracking
	// -------------------------------------------------------------------------
	void OpenGLStorageBuffer::setData(const void* data, size_t size, BufferUsage /*usage*/) {
		// Ridimensiona se necessario (1.5x, stesso schema di VulkanStorageBuffer)
		if (size > bufferSize) {
			size_t newCap = size + size / 2;
			resize(newCap);
		}

		size_t numChunks = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;
		if (chunkHashes_.size() != numChunks)
			chunkHashes_.assign(numChunks, 0);

		const uint8_t* src = static_cast<const uint8_t*>(data);

		if (usePersistent_ && persistentPtr_) {
			// Scrittura diretta in memoria persistente — nessuna chiamata GL
			uint8_t* dst = static_cast<uint8_t*>(persistentPtr_);
			for (size_t i = 0; i < numChunks; ++i) {
				size_t off = i * CHUNK_SIZE;
				size_t chunkSize = std::min(CHUNK_SIZE, size - off);
				size_t h = quickHash(src + off, chunkSize);
				if (chunkHashes_[i] != h) {
					std::memcpy(dst + off, src + off, chunkSize);
					chunkHashes_[i] = h;
				}
			}
			// Con COHERENT non serve flush — la GPU vede subito i dati
		}
		else {
			// Fallback: glBufferSubData solo per i chunk modificati
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
			for (size_t i = 0; i < numChunks; ++i) {
				size_t off = i * CHUNK_SIZE;
				size_t chunkSize = std::min(CHUNK_SIZE, size - off);
				size_t h = quickHash(src + off, chunkSize);
				if (chunkHashes_[i] != h) {
					glBufferSubData(GL_SHADER_STORAGE_BUFFER,
						static_cast<GLintptr>(off),
						static_cast<GLsizeiptr>(chunkSize),
						src + off);
					chunkHashes_[i] = h;
				}
			}
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		usedSize = size;
	}

	// -------------------------------------------------------------------------
	// updateData — offset write, nessun hash check (write mirato)
	// -------------------------------------------------------------------------
	void OpenGLStorageBuffer::updateData(const void* data, size_t size, size_t offset) {
		if (usePersistent_ && persistentPtr_) {
			std::memcpy(static_cast<uint8_t*>(persistentPtr_) + offset, data, size);
			// Invalida chunk hash interessati così il prossimo setData non li skippa
			size_t firstChunk = offset / CHUNK_SIZE;
			size_t lastChunk = (offset + size - 1) / CHUNK_SIZE;
			for (size_t i = firstChunk; i <= lastChunk && i < chunkHashes_.size(); ++i)
				chunkHashes_[i] = 0;
		}
		else {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER,
				static_cast<GLintptr>(offset),
				static_cast<GLsizeiptr>(size), data);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
		usedSize = std::max(usedSize, offset + size);
	}

	// -------------------------------------------------------------------------
	// resize — ricrea buffer (1.5x), copia dati esistenti
	// -------------------------------------------------------------------------
	void OpenGLStorageBuffer::resize(size_t newSize) {
		if (newSize == bufferSize) return;

		std::cout << "[OpenGLStorageBuffer] Resize " << bufferSize
			<< " -> " << newSize << " bytes\n";

		// Leggi dati correnti (se non siamo su persistent)
		std::vector<uint8_t> backup;
		const void* oldPtr = nullptr;

		if (usePersistent_ && persistentPtr_) {
			// Con persistent mapping i dati sono già accessibili
			oldPtr = persistentPtr_;
		}
		else if (ssbo_ != 0 && usedSize > 0) {
			backup.resize(usedSize);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
				static_cast<GLsizeiptr>(usedSize), backup.data());
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			oldPtr = backup.data();
		}

		createBuffer(newSize, oldPtr, usedSize);
		bufferSize = newSize;
	}

	void OpenGLStorageBuffer::shutdown() {
		unmapPersistent();
		if (ssbo_) {
			glDeleteBuffers(1, &ssbo_);
			ssbo_ = 0;
		}
		chunkHashes_.clear();
	}

} // namespace OnYuu