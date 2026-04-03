#include "VulkanBufferPool.h"
#include "VulkanRender.h"
#include <algorithm>
#include <iostream>

namespace OnYuu {

    // ============================================================================
    // GeometryPool Implementation
    // ============================================================================

    GeometryPool::GeometryPool(VmaAllocator allocator,  VulkanRender* renderer, VkDeviceSize initialSize)
        : allocator(allocator)
        , vertexBufferSize(initialSize)
        , indexBufferSize(initialSize / 4) // Indices tipicamente più piccoli
        , stagingBufferSize(16 * 1024 * 1024) // 16MB staging riutilizzabile
		, renderer(renderer)
    {
        // Crea vertex buffer unificato
        VkBufferCreateInfo vertexInfo{};
        vertexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexInfo.size = vertexBufferSize;
        vertexInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateBuffer(allocator, &vertexInfo, &allocInfo,
            &vertexBuffer, &vertexAllocation, nullptr);

        // Crea index buffer unificato
        VkBufferCreateInfo indexInfo{};
        indexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexInfo.size = indexBufferSize;
        indexInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        vmaCreateBuffer(allocator, &indexInfo, &allocInfo,
            &indexBuffer, &indexAllocation, nullptr);

        // Crea staging buffer riutilizzabile (più piccolo dei buffer finali!)
        ensureStagingBuffer(stagingBufferSize);

        std::cout << "GeometryPool created: Vertex=" << vertexBufferSize
            << " Index=" << indexBufferSize
            << " Staging=" << stagingBufferSize << " bytes\n";
    }

    GeometryPool::~GeometryPool() {
        shutdown();
    }

    void GeometryPool::ensureStagingBuffer(VkDeviceSize requiredSize) {
        if (stagingBuffer != VK_NULL_HANDLE && stagingBufferSize >= requiredSize) {
            return; // Già sufficientemente grande
        }

        // Distruggi vecchio staging
        if (stagingBuffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
        }

        // Crea nuovo staging più grande
        stagingBufferSize = std::max(requiredSize, stagingBufferSize * 2);

        VkBufferCreateInfo stagingInfo{};
        stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        stagingInfo.size = stagingBufferSize;
        stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT;

        vmaCreateBuffer(allocator, &stagingInfo, &allocInfo,
            &stagingBuffer, &stagingAllocation, &stagingAllocInfo);

        std::cout << "Staging buffer resized to " << stagingBufferSize << " bytes\n";
    }

    BufferRegion GeometryPool::allocateVertexRegion(VkDeviceSize size) {
        // Allineamento a 16 byte (tipico per vertex data)
        size = (size + 15) & ~15;

        // Cerca nella free-list
        for (auto it = vertexFreeList.begin(); it != vertexFreeList.end(); ++it) {
            if (it->size >= size) {
                BufferRegion region;
                region.buffer = vertexBuffer;
                region.offset = it->offset;
                region.size = size;

                // Aggiorna free block
                it->offset += size;
                it->size -= size;

                if (it->size == 0) {
                    vertexFreeList.erase(it);
                }

                return region;
            }
        }

        // Alloca dalla fine
        if (vertexUsedSize + size > vertexBufferSize) {
            growVertexBuffer(vertexBufferSize * 2);
        }

        BufferRegion region;
        region.buffer = vertexBuffer;
        region.offset = vertexUsedSize;
        region.size = size;

        vertexUsedSize += size;
        return region;
    }

    BufferRegion GeometryPool::allocateIndexRegion(VkDeviceSize size) {
        size = (size + 3) & ~3; // Allineamento a 4 byte

        // Logica simile a allocateVertexRegion
        for (auto it = indexFreeList.begin(); it != indexFreeList.end(); ++it) {
            if (it->size >= size) {
                BufferRegion region;
                region.buffer = indexBuffer;
                region.offset = it->offset;
                region.size = size;

                it->offset += size;
                it->size -= size;

                if (it->size == 0) {
                    indexFreeList.erase(it);
                }

                return region;
            }
        }

        if (indexUsedSize + size > indexBufferSize) {
            growIndexBuffer(indexBufferSize * 2);
        }

        BufferRegion region;
        region.buffer = indexBuffer;
        region.offset = indexUsedSize;
        region.size = size;

        indexUsedSize += size;
        return region;
    }

    void GeometryPool::uploadVertexData(const BufferRegion& region,
        const void* data, VkDeviceSize size) {
        if (size > region.size) {
            std::cerr << "uploadVertexData: size exceeds region\n";
            return;
        }

        ensureStagingBuffer(size);

        // Copia CPU -> Staging
        memcpy(stagingAllocInfo.pMappedData, data, size);

        // Copia Staging -> GPU
        copyBuffer(stagingBuffer, region.buffer, 0, region.offset, size);
    }

    void GeometryPool::uploadIndexData(const BufferRegion& region,
        const void* data, VkDeviceSize size) {
        if (size > region.size) {
            std::cerr << "uploadIndexData: size exceeds region\n";
            return;
        }

        ensureStagingBuffer(size);
        memcpy(stagingAllocInfo.pMappedData, data, size);
        copyBuffer(stagingBuffer, region.buffer, 0, region.offset, size);
    }

    void GeometryPool::copyBuffer(VkBuffer src, VkBuffer dst,
        VkDeviceSize srcOffset, VkDeviceSize dstOffset,
        VkDeviceSize size) {
        VkCommandBuffer cmd = renderer->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = srcOffset;
        copyRegion.dstOffset = dstOffset;
        copyRegion.size = size;

        vkCmdCopyBuffer(cmd, src, dst, 1, &copyRegion);

        renderer->endSingleTimeCommands(cmd);
    }

    void GeometryPool::freeRegion(const BufferRegion& region) {
        if (region.buffer == vertexBuffer) {
            vertexFreeList.push_back({ region.offset, region.size });
            // Merge contiguous blocks
            std::sort(vertexFreeList.begin(), vertexFreeList.end(),
                [](const FreeBlock& a, const FreeBlock& b) {
                    return a.offset < b.offset;
                });
        }
        else if (region.buffer == indexBuffer) {
            indexFreeList.push_back({ region.offset, region.size });
            std::sort(indexFreeList.begin(), indexFreeList.end(),
                [](const FreeBlock& a, const FreeBlock& b) {
                    return a.offset < b.offset;
                });
        }
    }

    void GeometryPool::defragment()
    {
        // Implementazione opzionale: complessa, richiede tracking delle regioni allocate
        // e copia dei dati per compattare. Non implementata in questa versione.
		LOG( << "Defragmentation not implemented.\n");

    }

    void GeometryPool::growVertexBuffer(VkDeviceSize newSize) {
        LOG( << "Growing vertex buffer from " << vertexBufferSize
            << " to " << newSize << " bytes\n");

        VkBuffer oldBuffer = vertexBuffer;
        VmaAllocation oldAllocation = vertexAllocation;

        // Crea nuovo buffer
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = newSize;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
            &vertexBuffer, &vertexAllocation, nullptr);

        // Copia dati vecchi
        if (vertexUsedSize > 0) {
            copyBuffer(oldBuffer, vertexBuffer, 0, 0, vertexUsedSize);
        }

        // Sync e distruggi vecchio
        vkDeviceWaitIdle(renderer->getInit().device);
        vmaDestroyBuffer(allocator, oldBuffer, oldAllocation);

        vertexBufferSize = newSize;
    }

    void GeometryPool::growIndexBuffer(VkDeviceSize newSize) {
        // Implementazione identica a growVertexBuffer, ma per index buffer
        LOG( << "Growing index buffer from " << indexBufferSize
            << " to " << newSize << " bytes\n");

        VkBuffer oldBuffer = indexBuffer;
        VmaAllocation oldAllocation = indexAllocation;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = newSize;
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
            &indexBuffer, &indexAllocation, nullptr);

        if (indexUsedSize > 0) {
            copyBuffer(oldBuffer, indexBuffer, 0, 0, indexUsedSize);
        }

        vkDeviceWaitIdle(renderer->getInit().device);
        vmaDestroyBuffer(allocator, oldBuffer, oldAllocation);

        indexBufferSize = newSize;
    }

    void GeometryPool::shutdown() {
        if (stagingBuffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
            stagingBuffer = VK_NULL_HANDLE;
        }
        if (vertexBuffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, vertexBuffer, vertexAllocation);
            vertexBuffer = VK_NULL_HANDLE;
        }
        if (indexBuffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, indexBuffer, indexAllocation);
            indexBuffer = VK_NULL_HANDLE;
        }
    }
    void GeometryPool::registerMesh(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame) {
        //std::lock_guard<std::mutex> lock(trackerMutex_);

        auto& info = meshUsageTracker_[mesh];
        info.lastUsedFrame = currentFrame;
        info.refCount = 1;
        info.markedForDeletion = false;

        LOG( << "[GeometryPool] Registered new mesh (total cached: "
            << meshUsageTracker_.size() << ")\n");
    }



    void GeometryPool::updateMeshUsage(const std::shared_ptr<Mesh>& mesh, uint64_t currentFrame) {
        std::lock_guard<std::mutex> lock(trackerMutex_);

        auto it = meshUsageTracker_.find(mesh);
        if (it != meshUsageTracker_.end()) {
            it->second.lastUsedFrame = currentFrame; // ✅ Aggiorna timestamp
            it->second.refCount++;
        }
        else {
            // Mesh non ancora registrata (fallback)
            std::cerr << "[GeometryPool] WARNING: Mesh used but not registered!\n";
            registerMesh(mesh, currentFrame);
        }
    }


    void GeometryPool::collectGarbage(uint64_t currentFrame, uint32_t framesToKeep) {
        //std::lock_guard<std::mutex> lock(trackerMutex_);

        std::vector<std::shared_ptr<Mesh>> toDelete;

        LOG( << "[GC] Scanning " << meshUsageTracker_.size() << " cached meshes...\n");

        for (auto it = meshUsageTracker_.begin(); it != meshUsageTracker_.end();) {
            const auto& mesh = it->first;
            auto& info = it->second;

            uint64_t framesSinceLastUse = currentFrame - info.lastUsedFrame;

            // ✅ Elimina mesh NON usate da N frame
            if (framesSinceLastUse > framesToKeep && !info.markedForDeletion) {
                LOG( << "[GC] Marking mesh for deletion (unused for "
                    << framesSinceLastUse << " frames, last used: "
                    << info.lastUsedFrame << ")\n");

                info.markedForDeletion = true;
                toDelete.push_back(mesh);
            }

            ++it;
        }

        // Elimina mesh marchiate
        if (!toDelete.empty()) {
            LOG( "[GC] Deleting " << toDelete.size() << " meshes...\n");

            for (const auto& mesh : toDelete) {
                renderer->removeCachedMesh(mesh); // Libera risorse GPU
                meshUsageTracker_.erase(mesh);
            }

            LOG( "[GC] Remaining cached meshes: " << meshUsageTracker_.size() << "\n");
        }
        else {
            LOG( "[GC] No meshes to delete\n");
        }
    }


    // ============================================================================
    // PooledMeshGPU Implementation
    // ============================================================================

    PooledMeshGPU::PooledMeshGPU(Mesh& mesh, std::shared_ptr<GeometryPool> pool)
        : mesh(mesh), pool(pool) {
    }

    PooledMeshGPU::~PooledMeshGPU() {
        shutdown();
    }

    void PooledMeshGPU::uploadToGPU() {
        if (uploaded || mesh.empty()) return;
        vertexCount = static_cast<uint32_t>(mesh.position.size());
        // Calcola dimensioni
        VkDeviceSize vertexSize =
            sizeof(glm::vec3) * mesh.position.size() +
            sizeof(glm::vec4) * mesh.color.size() +
            sizeof(glm::vec2) * mesh.texCoord.size() +
            sizeof(glm::vec3) * mesh.normal.size();

        VkDeviceSize indexSize = sizeof(uint32_t) * mesh.indices.size();
        indexCount = static_cast<uint32_t>(mesh.indices.size());

        // Alloca regioni
        vertexRegion = pool->allocateVertexRegion(vertexSize);
        if (!mesh.indices.empty()) {
            indexRegion = pool->allocateIndexRegion(indexSize);
        }

        // Prepara vertex data interlacciati
        std::vector<float> vertexData;
        vertexData.reserve(mesh.position.size() * 12); // 12 floats per vertex

        for (size_t i = 0; i < mesh.position.size(); i++) {
            vertexData.push_back(mesh.position[i].x);
            vertexData.push_back(mesh.position[i].y);
            vertexData.push_back(mesh.position[i].z);
            vertexData.push_back(mesh.color[i].r);
            vertexData.push_back(mesh.color[i].g);
            vertexData.push_back(mesh.color[i].b);
            vertexData.push_back(mesh.color[i].a);
            vertexData.push_back(mesh.texCoord[i].x);
            vertexData.push_back(mesh.texCoord[i].y);
            vertexData.push_back(mesh.normal[i].x);
            vertexData.push_back(mesh.normal[i].y);
            vertexData.push_back(mesh.normal[i].z);
        }

        // Upload
        pool->uploadVertexData(vertexRegion, vertexData.data(), vertexSize);

        if (!mesh.indices.empty()) {
            pool->uploadIndexData(indexRegion, mesh.indices.data(), indexSize);
        }

        uploaded = true;

        

        LOG( << "Uploaded mesh to pool: Vertex offset=" << vertexRegion.offset
            << " Index offset=" << indexRegion.offset << "\n");
    }

    void PooledMeshGPU::drawInstanced(VkCommandBuffer cmd, uint32_t instanceCount,
        uint32_t firstInstance) {
        if (!uploaded) return;

        VkBuffer vertexBuffers[] = { vertexRegion.buffer };
        VkDeviceSize offsets[] = { vertexRegion.offset };

        vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

        if (indexCount > 0) {
            vkCmdBindIndexBuffer(cmd, indexRegion.buffer, indexRegion.offset,
                VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(cmd, indexCount, instanceCount, 0, 0, firstInstance);
        }
        else {
            vkCmdDraw(cmd, static_cast<uint32_t>(mesh.position.size()),
                instanceCount, 0, firstInstance);
        }
    }

    void PooledMeshGPU::shutdown() {
        if (uploaded) {
            pool->freeRegion(vertexRegion);
            if (indexCount > 0) {
                pool->freeRegion(indexRegion);
            }
            uploaded = false;
        }
    }

} // namespace OnYuu