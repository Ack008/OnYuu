#pragma once
#include <vulkan/vulkan.h>
#include "Core/Model/Components/MeshComponent.h"
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>



namespace OnYuu {
    class VulkanRender;
    // Rappresenta un range all'interno di un buffer
    struct BufferRegion {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceSize offset = 0;
        VkDeviceSize size = 0;
        uint32_t poolIndex = 0;
    };

    // Tracking per mesh usage
    struct MeshUsageInfo {
        uint64_t lastUsedFrame = 0;
        std::atomic<uint32_t> refCount{ 0 };
        bool markedForDeletion = false;
    };


    // Pool di geometry (vertex + index buffer unificati)
    class GeometryPool {
    public:
        GeometryPool(VmaAllocator allocator, VulkanRender* renderer, VkDeviceSize initialSize = 64 * 1024 * 1024);
        // 64MB default
        ~GeometryPool();

        // Alloca spazio per una mesh e ritorna la regione
        BufferRegion allocateVertexRegion(VkDeviceSize size);
        BufferRegion allocateIndexRegion(VkDeviceSize size);

        // Carica dati usando staging temporaneo
        void uploadVertexData(const BufferRegion& region, const void* data, VkDeviceSize size);
        void uploadIndexData(const BufferRegion& region, const void* data, VkDeviceSize size);

        // Libera una regione (opzionale - usa un free-list interno)
        void freeRegion(const BufferRegion& region);

        // Compatta il buffer rimuovendo frammentazione (chiamare quando idle)
        void defragment();

        VkBuffer getVertexBuffer() const { return vertexBuffer; }
        VkBuffer getIndexBuffer() const { return indexBuffer; }
        // ✅ RINOMINA: markMeshUsed -> updateMeshUsage (più chiaro)
        void updateMeshUsage(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame);


        // ✅ NUOVO: Inizializza tracking quando la mesh viene uploadata
        void registerMesh(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame);

        // ✅ Garbage collection basato su LRU (Least Recently Used)
        void collectGarbage(uint64_t currentFrame, uint32_t framesToKeep = 180);
        void shutdown();

    private:
        struct FreeBlock {
            VkDeviceSize offset;
            VkDeviceSize size;
        };

        VmaAllocator allocator;

        // Buffer unificati
        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VmaAllocation vertexAllocation = VK_NULL_HANDLE;
        VkDeviceSize vertexBufferSize = 0;
        VkDeviceSize vertexUsedSize = 0;
        std::vector<FreeBlock> vertexFreeList;

        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation indexAllocation = VK_NULL_HANDLE;
        VkDeviceSize indexBufferSize = 0;
        VkDeviceSize indexUsedSize = 0;
        std::vector<FreeBlock> indexFreeList;

        // Staging temporaneo (riutilizzato)
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VmaAllocation stagingAllocation = VK_NULL_HANDLE;
        VmaAllocationInfo stagingAllocInfo = {};
        VkDeviceSize stagingBufferSize = 0;

        void ensureStagingBuffer(VkDeviceSize requiredSize);
        void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize srcOffset,
            VkDeviceSize dstOffset, VkDeviceSize size);
        void growVertexBuffer(VkDeviceSize newSize);
        void growIndexBuffer(VkDeviceSize newSize);
		VulkanRender* renderer = nullptr;



        
        // Tracking delle mesh
        std::unordered_map<std::shared_ptr<Mesh>, MeshUsageInfo> meshUsageTracker_;
        std::mutex trackerMutex_;
    };

    // Mesh modificata per usare il pool
    class PooledMeshGPU {
    public:
        PooledMeshGPU(Mesh& mesh, std::shared_ptr<GeometryPool> pool);
        ~PooledMeshGPU();

        void uploadToGPU();
        void shutdown();

        // Draw usando offset nel buffer pooled
        void drawInstanced(VkCommandBuffer cmd, uint32_t instanceCount, uint32_t firstInstance);

        bool isUploaded() const { return uploaded; }
        const Mesh& getMesh() const { return mesh; }
        const BufferRegion& getVertexRegion() const { return vertexRegion; }
        const BufferRegion& getIndexRegion() const { return indexRegion; }
        uint32_t getIndexCount() const { return indexCount; }
        uint32_t getVertexCount() const { return vertexCount; }
    private:
        Mesh& mesh;
        std::shared_ptr<GeometryPool> pool;

        BufferRegion vertexRegion;
        BufferRegion indexRegion;
        uint32_t indexCount = 0;
        bool uploaded = false;
        uint32_t vertexCount = 0;
    };

} // namespace OnYuu