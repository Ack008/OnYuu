#pragma once
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include "Core/Material.h"
#include "VulkanBufferPool.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>

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

        void addDrawCommand(const VkDrawIndexedIndirectCommand& cmd);
        void finalize(uint32_t currentFrame);
        void reset();
        void executeMultiDrawIndirect(VkCommandBuffer cmd, uint32_t currentFrame);

        uint32_t getDrawCount() const { return currentDrawCount; }
        bool isEmpty() const { return currentDrawCount == 0; }

        void shutdown();

    private:
        VmaAllocator allocator;
        uint32_t framesInFlight;
        std::vector<VkBuffer> indirectBuffers;
        std::vector<VmaAllocation> indirectAllocations;
        std::vector<VmaAllocationInfo> indirectAllocInfos;
        uint32_t maxDrawCommands = 0;
        uint32_t currentDrawCount = 0;
        VkDeviceSize bufferSize = 0;
        std::vector<VkDrawIndexedIndirectCommand> cpuCommands;
        bool needsUpdate = false;
    };

    struct SceneMaterialKey {
        int sceneIndex = -1;
        std::shared_ptr<Material> material;
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        bool operator==(const SceneMaterialKey& other) const {
            return sceneIndex == other.sceneIndex
                && material.get() == other.material.get()
                && topology == other.topology;
        }
    };

    struct SceneMaterialKeyHash {
        size_t operator()(const SceneMaterialKey& key) const {
            size_t h1 = std::hash<int>{}(key.sceneIndex);
            size_t h2 = std::hash<void*>{}(key.material.get());
            size_t h3 = std::hash<uint32_t>{}(static_cast<uint32_t>(key.topology));
            size_t h = h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            return h ^ (h3 + 0x9e3779b9 + (h << 6) + (h >> 2));
        }
    };

    // Manager per tutti gli indirect buffers (uno per coppia scena+materiale+topologia)
    class IndirectDrawManager {
    public:
        IndirectDrawManager(VmaAllocator allocator, uint32_t framesInFlight);
        ~IndirectDrawManager();

        std::shared_ptr<IndirectDrawBuffer> getOrCreateBuffer(
            int sceneIndex,
            std::shared_ptr<Material> material,
            VkPrimitiveTopology topology
        );

        void resetAll();
        void finalizeAll(uint32_t currentFrame);

        void shutdown();
    private:
        VmaAllocator allocator;
        uint32_t framesInFlight;
        std::unordered_map<SceneMaterialKey,
            std::shared_ptr<IndirectDrawBuffer>, SceneMaterialKeyHash> buffers;
    };

    struct MeshDrawInfo {
        BufferRegion vertexRegion;
        BufferRegion indexRegion;
        uint32_t indexCount;
        uint32_t vertexCount;
        uint32_t firstIndex;
        int32_t vertexOffset;
    };

} // namespace OnYuu