#include "OpenGLIndirectDraw.h"
#include <iostream>
#include <cstring>

namespace OnYuu {

    // ============================================================================
    // OpenGLIndirectBuffer
    // ============================================================================

    OpenGLIndirectBuffer::OpenGLIndirectBuffer(uint32_t maxDraws)
        : maxDrawCommands_(maxDraws)
    {
        cpuCommands_.reserve(maxDraws);

        // Crea il Draw Indirect Buffer Object con spazio pre-allocato
        glGenBuffers(1, &dibo_);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, dibo_);
        glBufferData(GL_DRAW_INDIRECT_BUFFER,
            static_cast<GLsizeiptr>(sizeof(GLDrawElementsIndirectCommand) * maxDraws),
            nullptr,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

        std::cout << "[OpenGLIndirectBuffer] Created: max " << maxDraws
            << " draws (" << sizeof(GLDrawElementsIndirectCommand) * maxDraws
            << " bytes)\n";
    }

    OpenGLIndirectBuffer::~OpenGLIndirectBuffer() {
        shutdown();
    }

    void OpenGLIndirectBuffer::addDrawCommand(const GLDrawElementsIndirectCommand& cmd) {
        if (currentDrawCount_ >= maxDrawCommands_) {
            std::cerr << "[OpenGLIndirectBuffer] Max draws exceeded, ignoring\n";
            return;
        }
        cpuCommands_.push_back(cmd);
        currentDrawCount_++;
        needsUpdate_ = true;
    }

    void OpenGLIndirectBuffer::finalize() {
        if (!needsUpdate_ || cpuCommands_.empty()) return;

        GLsizeiptr copySize = static_cast<GLsizeiptr>(
            sizeof(GLDrawElementsIndirectCommand) * currentDrawCount_);

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, dibo_);
        // glBufferSubData è sufficiente: il buffer è già grande abbastanza
        glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, copySize, cpuCommands_.data());
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

        needsUpdate_ = false;
    }

    void OpenGLIndirectBuffer::reset() {
        currentDrawCount_ = 0;
        cpuCommands_.clear();
        needsUpdate_ = false;
    }

    void OpenGLIndirectBuffer::executeMultiDraw(GLenum primitiveMode) {
        if (currentDrawCount_ == 0) return;

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, dibo_);

        // Una sola drawcall per tutte le mesh del bucket
        glMultiDrawElementsIndirect(
            primitiveMode,
            GL_UNSIGNED_INT,
            nullptr,                                              // offset 0 nel DIBO
            static_cast<GLsizei>(currentDrawCount_),
            sizeof(GLDrawElementsIndirectCommand)
        );

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    }

    void OpenGLIndirectBuffer::shutdown() {
        if (dibo_) {
            glDeleteBuffers(1, &dibo_);
            dibo_ = 0;
        }
    }

    // ============================================================================
    // OpenGLIndirectDrawManager
    // ============================================================================

    OpenGLIndirectDrawManager::~OpenGLIndirectDrawManager() {
        shutdown();
    }

    std::shared_ptr<OpenGLIndirectBuffer> OpenGLIndirectDrawManager::getOrCreateBuffer(
        int sceneIndex,
        std::shared_ptr<Material> material,
        GLenum topology)
    {
        GLSceneMaterialKey key{ sceneIndex, material, topology };
        auto it = buffers_.find(key);
        if (it != buffers_.end()) return it->second;

        auto buf = std::make_shared<OpenGLIndirectBuffer>(10000);
        buffers_.emplace(key, buf);
        return buf;
    }

    void OpenGLIndirectDrawManager::resetAll() {
        for (auto& [key, buf] : buffers_) buf->reset();
    }

    void OpenGLIndirectDrawManager::finalizeAll() {
        for (auto& [key, buf] : buffers_) buf->finalize();
    }

    void OpenGLIndirectDrawManager::shutdown() {
        for (auto& [key, buf] : buffers_) buf->shutdown();
        buffers_.clear();
    }

} // namespace OnYuu