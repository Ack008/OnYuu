#include "IndirectDrawSystem.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <iostream>

namespace OnYuu {

    // ============================================================================
    // IndirectDrawBuffer Implementation
    // ============================================================================

    IndirectDrawBuffer::IndirectDrawBuffer(VmaAllocator allocator, uint32_t framesInFlight, uint32_t maxDraws)
        : allocator(allocator)
        , framesInFlight(framesInFlight)
        , maxDrawCommands(maxDraws)
    {
        bufferSize = sizeof(VkDrawIndexedIndirectCommand) * maxDrawCommands;

        indirectBuffers.resize(framesInFlight, VK_NULL_HANDLE);
        indirectAllocations.resize(framesInFlight, VK_NULL_HANDLE);
        indirectAllocInfos.resize(framesInFlight, {});

        for (uint32_t i = 0; i < framesInFlight; i++) {
            // Crea buffer GPU (HOST_VISIBLE per update veloci)
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = bufferSize;
            bufferInfo.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; // STORAGE per GPU culling futuro

            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT;

            vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
                &indirectBuffers[i], &indirectAllocations[i], &indirectAllocInfos[i]);
        }

        cpuCommands.reserve(maxDrawCommands);

        std::cout << "IndirectDrawBuffer created: max " << maxDraws
            << " draws (" << bufferSize << " bytes) x " << framesInFlight << " frames\n";
    }

    IndirectDrawBuffer::~IndirectDrawBuffer() {
        shutdown();
    }

    void IndirectDrawBuffer::addDrawCommand(const VkDrawIndexedIndirectCommand& cmd) {
        if (currentDrawCount >= maxDrawCommands) {
            std::cerr << "IndirectDrawBuffer: exceeded max draws, ignoring\n";
            return;
        }

        cpuCommands.push_back(cmd);
        currentDrawCount++;
        needsUpdate = true;
    }

    void IndirectDrawBuffer::finalize(uint32_t currentFrame) {
        if (!needsUpdate || cpuCommands.empty()) return;

        // Copia CPU commands -> GPU buffer
        size_t copySize = sizeof(VkDrawIndexedIndirectCommand) * currentDrawCount;
        memcpy(indirectAllocInfos[currentFrame].pMappedData, cpuCommands.data(), copySize);

        needsUpdate = false;

#ifdef _DEBUG
        std::cout << "Finalized indirect buffer: " << currentDrawCount << " draws\n";
#endif
    }

    void IndirectDrawBuffer::reset() {
        currentDrawCount = 0;
        cpuCommands.clear();
        needsUpdate = false;
    }

    void IndirectDrawBuffer::executeMultiDrawIndirect(VkCommandBuffer cmd, uint32_t currentFrame) {
        if (currentDrawCount == 0) return;

        // ⚡ UNA SOLA DRAW CALL per tutte le mesh!
        vkCmdDrawIndexedIndirect(
            cmd,
            indirectBuffers[currentFrame],
            0, // offset nel buffer
            currentDrawCount,
            sizeof(VkDrawIndexedIndirectCommand)
        );

#ifdef _DEBUG
        std::cout << "Executed multi draw indirect: " << currentDrawCount
            << " draws in ONE call\n";
#endif
    }

    void IndirectDrawBuffer::shutdown() {
        for (uint32_t i = 0; i < framesInFlight; i++) {
            if (indirectBuffers[i] != VK_NULL_HANDLE) {
                vmaDestroyBuffer(allocator, indirectBuffers[i], indirectAllocations[i]);
                indirectBuffers[i] = VK_NULL_HANDLE;
            }
        }
    }

    // ============================================================================
    // IndirectDrawManager Implementation
    // ============================================================================

    IndirectDrawManager::IndirectDrawManager(VmaAllocator allocator, uint32_t framesInFlight)
        : allocator(allocator), framesInFlight(framesInFlight) {
    }

    IndirectDrawManager::~IndirectDrawManager() {
        shutdown();
    }

    std::shared_ptr<IndirectDrawBuffer> IndirectDrawManager::getOrCreateBuffer(
        int sceneIndex,
        std::shared_ptr<Material> material,
        VkPrimitiveTopology topology
    ) {
        SceneMaterialKey key{ sceneIndex, material, topology };
        auto it = buffers.find(key);
        if (it != buffers.end()) {
            return it->second;
        }

        auto buffer = std::make_shared<IndirectDrawBuffer>(allocator, framesInFlight, 10000);
        buffers.emplace(key, buffer);

        return buffer;
    }

    void IndirectDrawManager::resetAll() {
        for (auto& [mat, buffer] : buffers) {
            buffer->reset();
        }
    }

    void IndirectDrawManager::finalizeAll(uint32_t currentFrame) {
        for (auto& [mat, buffer] : buffers) {
            buffer->finalize(currentFrame);
        }
    }

    void IndirectDrawManager::shutdown() {
        for (auto& [mat, buffer] : buffers) {
            buffer->shutdown();
        }
        buffers.clear();
    }

} // namespace OnYuu