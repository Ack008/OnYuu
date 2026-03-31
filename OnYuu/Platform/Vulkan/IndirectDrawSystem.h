#pragma once
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include "Core/Material.h"
#include "VulkanBufferPool.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace OnYuu {

    // Struttura per vkCmdDrawIndexedIndirect
    struct VkDrawIndexedIndirectCommand {
        uint32_t indexCount;
        uint32_t instanceCount;
        uint32_t firstIndex;
        int32_t  vertexOffset;
        uint32_t firstInstance;
    };

    // Indirect draw buffer per un materiale
    class IndirectDrawBuffer {
    public:
        IndirectDrawBuffer(VmaAllocator allocator, uint32_t framesInFlight, uint32_t maxDraws = 10000);
        ~IndirectDrawBuffer();

        // Aggiungi un draw command
        void addDrawCommand(const VkDrawIndexedIndirectCommand& cmd);

        // Finalizza e upload su GPU
        void finalize(uint32_t currentFrame);

        // Reset per il prossimo frame
        void reset();

        // Esegui il multi draw indirect
        void executeMultiDrawIndirect(VkCommandBuffer cmd, uint32_t currentFrame);

        // VkBuffer getBuffer(uint32_t currentFrame) const { return indirectBuffers[currentFrame]; }
        uint32_t getDrawCount() const { return currentDrawCount; }
        bool isEmpty() const { return currentDrawCount == 0; }

        void shutdown();

    private:
        VmaAllocator allocator;
        uint32_t framesInFlight;

        // Buffer GPU per i comandi indirect
        std::vector<VkBuffer> indirectBuffers;
        std::vector<VmaAllocation> indirectAllocations;
        std::vector<VmaAllocationInfo> indirectAllocInfos;

        // Capacita e stato corrente
        uint32_t maxDrawCommands = 0;
        uint32_t currentDrawCount = 0;
        VkDeviceSize bufferSize = 0;

        // Staging CPU-side per costruire i comandi
        std::vector<VkDrawIndexedIndirectCommand> cpuCommands;

        bool needsUpdate = false;
    };

    // Manager per tutti gli indirect buffers (uno per materiale)
    class IndirectDrawManager {
    public:
        IndirectDrawManager(VmaAllocator allocator, uint32_t framesInFlight);
        ~IndirectDrawManager();

        // Ottieni o crea buffer per un materiale
        std::shared_ptr<IndirectDrawBuffer> getOrCreateBuffer(
            std::shared_ptr<Material> material
        );

        // Reset tutti i buffer per il nuovo frame
        void resetAll();

        // Finalizza tutti i buffer (upload su GPU)
        void finalizeAll(uint32_t currentFrame);

        void shutdown();
        std::unordered_map<std::shared_ptr<Material>,
			std::shared_ptr<IndirectDrawBuffer>> getBuffers() const { return buffers; }
    private:
        VmaAllocator allocator;
        uint32_t framesInFlight;
        std::unordered_map<std::shared_ptr<Material>,
            std::shared_ptr<IndirectDrawBuffer>> buffers;
    };

    // ============================================================================
    // Struttura estesa per mesh con info per indirect drawing
    // ============================================================================

    struct MeshDrawInfo {
        BufferRegion vertexRegion;
        BufferRegion indexRegion;
        uint32_t indexCount;
        uint32_t vertexCount;

        // Offset in ELEMENTI (non bytes) per indirect draw
        uint32_t firstIndex;      // indexRegion.offset / sizeof(uint32_t)
        int32_t vertexOffset;     // vertexRegion.offset / vertexStride
    };

} // namespace OnYuu