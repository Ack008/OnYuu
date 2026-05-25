#include "VulkanBuffer.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <future>
namespace OnYuu {
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t bindingPoint, size_t size, VmaAllocator allocator_)
	{
		this->bindingPoint = bindingPoint;
		if (allocator_)
			allocator = allocator_;
		else
			allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		// Inizializza il buffer Vulkan qui (crea VkBuffer, VmaAllocation, ecc.)
		VkDeviceSize bufferSize = size;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
			&vulkanBuffer, &vulkanBufferAllocation, &uniformAllocInfo);
		this->bufferSize = size;
		std::cout << "Created VulkanUniformBuffer " << vulkanBuffer << " of size " << size << " bytes\n";
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
	}

	void VulkanUniformBuffer::bind()
	{
	}

	void VulkanUniformBuffer::unbind()
	{
	}

	void VulkanUniformBuffer::bindToBindingPoint(uint32_t bindingPoint)
	{
	}

	void VulkanUniformBuffer::setData(const void* data, size_t size, BufferUsage usage)
	{
		size_t newHash = calculateHash(data, size);

		if (lastDataHash == newHash) {
			return; // Dati identici, skip update
		}

		// Aggiorna buffer
		memcpy(uniformAllocInfo.pMappedData, data, size);
		usedSize = size;
		lastDataHash = newHash;
	}

	void VulkanUniformBuffer::updateData(const void* data, size_t size, size_t offset)
	{
		size_t newHash = calculateHash(data, size);

		memcpy(static_cast<uint8_t*>(uniformAllocInfo.pMappedData) + offset, data, size);
	}

	void VulkanUniformBuffer::resize(size_t newSize)
	{
	}

	void VulkanUniformBuffer::shutdown()
	{
		vmaDestroyBuffer(allocator, vulkanBuffer, vulkanBufferAllocation);
	}



	VulkanStorageBuffer::VulkanStorageBuffer(uint32_t bindingPoint, size_t size, VmaAllocator allocator_)
	{
		this->bindingPoint = bindingPoint;
		// Inizializza il buffer Vulkan qui (crea VkBuffer, VmaAllocation, ecc.)
		if (allocator_)
			allocator = allocator_;
		else
			allocator = ((VulkanRender*)(Render::getInstance().get()))->getAllocator();
		VkDeviceSize bufferSize = size;
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;
		// Questo fa sì che il buffer sia già mappato e scrivibile da CPU

		vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
			&vulkanBuffer, &vulkanBufferAllocation, &storageAllocInfo);
		this->bufferSize = size;
		std::cout << "Created VulkanStorageBuffer " << vulkanBuffer << " of size " << size << " bytes\n";


	}
	VulkanStorageBuffer::~VulkanStorageBuffer()
	{
	}
	void VulkanStorageBuffer::bind()
	{
	}
	void VulkanStorageBuffer::unbind()
	{
	}
	void VulkanStorageBuffer::bindToBindingPoint(uint32_t bindingPoint)
	{
	}
	// Modifica setData() con aggiornamento incrementale:
	void VulkanStorageBuffer::setData(const void* data, size_t size, BufferUsage usage)
	{
		// ✅ Resize se necessario (pre-alloca più spazio per evitare resize frequenti)
		if (size > bufferSize) {
			// Alloca 1.5x invece di 5x per ridurre spreco memoria
			resize(size + size / 2);
		}


		// ✅ Calcola numero di chunk
		size_t numChunks = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;

		// ✅ Ridimensiona array hash se necessario
		if (chunkHashes.size() != numChunks) {
			chunkHashes.resize(numChunks, 0);
		}
		// ✅ Determina numero thread (non più di hardware_concurrency)
		const size_t numThreads = std::min(
			std::thread::hardware_concurrency(),
			(uint32_t)numChunks
		);

		if (numThreads <= 1 || numChunks < 4) {
			normalSetData(data, size, usage, numChunks);
			return;
		}
		else {
			parallelSetData(data, size, usage, numThreads, numChunks);
			return;
		}
		
	}
	void VulkanStorageBuffer::updateData(const void* data, size_t size, size_t offset)
	{
		size_t newHash = calculateHash(data, size);
		memcpy(static_cast<uint8_t*>(storageAllocInfo.pMappedData) + offset, data, size);
	}
	void VulkanStorageBuffer::resize(size_t newSize)
	{
		if (newSize == bufferSize) return;

		std::cout << "Resizing VulkanStorageBuffer from " << bufferSize
			<< " to " << newSize << " bytes\n";

		// Salva vecchie risorse
		VmaAllocation oldAllocation = vulkanBufferAllocation;
		VkBuffer oldBuffer = vulkanBuffer;
		VmaAllocationInfo oldAllocInfo = storageAllocInfo;
		VkDeviceSize oldSize = bufferSize;

		// Crea nuovo buffer
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = newSize;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
			&vulkanBuffer, &vulkanBufferAllocation, &storageAllocInfo) != VK_SUCCESS) {
			// Ripristina stato
			vulkanBuffer = oldBuffer;
			vulkanBufferAllocation = oldAllocation;
			storageAllocInfo = oldAllocInfo;
			throw std::runtime_error("Failed to resize VulkanStorageBuffer");
		}

		this->bufferSize = newSize;

		// Copia dati vecchi
		size_t copySize = std::min(oldSize, newSize);
		if (copySize > 0) {
			memcpy(storageAllocInfo.pMappedData, oldAllocInfo.pMappedData, copySize);
		}

		// ✅ IMPORTANTE: Sincronizza GPU prima di distruggere vecchio buffer
		// Ottieni device da VulkanRender
		VulkanRender* render = static_cast<VulkanRender*>(Render::getInstance().get());
		vkDeviceWaitIdle(render->getInit().device); // ⚠️ Costoso ma necessario

		vmaDestroyBuffer(allocator, oldBuffer, oldAllocation);

		// ✅ Reset hash - forza ricalcolo al prossimo setData
		chunkHashes.clear();
	}
	void VulkanStorageBuffer::shutdown()
	{
		vmaDestroyBuffer(allocator, vulkanBuffer, vulkanBufferAllocation);
	}
	void VulkanStorageBuffer::parallelSetData(const void* data, size_t size, BufferUsage usage, const size_t numThreads, size_t numChunks)
	{
		std::vector<std::future<void>> futures;
		std::atomic<bool> anyChunkChanged{ false };

		const size_t chunksPerThread = (numChunks + numThreads - 1) / numThreads;

		// ✅ Spawna thread worker
		for (size_t t = 0; t < numThreads; ++t) {
			futures.push_back(std::async(std::launch::async, [&, t]() {
				size_t startChunk = t * chunksPerThread;
				size_t endChunk = std::min(startChunk + chunksPerThread, numChunks);

				for (size_t i = startChunk; i < endChunk; ++i) {
					size_t offset = i * CHUNK_SIZE;
					size_t chunkSize = std::min(CHUNK_SIZE, size - offset);

					const uint8_t* chunkData = static_cast<const uint8_t*>(data) + offset;
					size_t newHash = quickHash(chunkData, chunkSize);

					if (chunkHashes[i] != newHash) {
						// ✅ memcpy è thread-safe qui (range non overlappanti)
						memcpy(
							static_cast<uint8_t*>(storageAllocInfo.pMappedData) + offset,
							chunkData,
							chunkSize
						);

						chunkHashes[i] = newHash;
						anyChunkChanged.store(true, std::memory_order_relaxed);
					}
				}
				}));
		}

		// ✅ Aspetta completamento
		for (auto& f : futures) {
			f.wait();
		}

		if (anyChunkChanged.load()) {
			usedSize = size;
		}
	}
	void VulkanStorageBuffer::normalSetData(const void* data, size_t size, BufferUsage usage, size_t numChunks)
	{
		bool anyChunkChanged = false;

		// ✅ Aggiorna solo chunk modificati
		for (size_t i = 0; i < numChunks; ++i) {
			size_t offset = i * CHUNK_SIZE;
			size_t chunkSize = std::min(CHUNK_SIZE, size - offset);

			// Calcola hash del chunk corrente
			const uint8_t* chunkData = static_cast<const uint8_t*>(data) + offset;
			size_t newHash = quickHash(chunkData, chunkSize);

			// Confronta con hash precedente
			if (chunkHashes[i] != newHash) {
				VulkanRender* render = static_cast<VulkanRender*>(Render::getInstance().get());
				// ✅ Copia SOLO questo chunk
				memcpy(
					static_cast<uint8_t*>(storageAllocInfo.pMappedData) + offset,
					chunkData,
					chunkSize
				);

				chunkHashes[i] = newHash;
				anyChunkChanged = true;

			}
		}

		// ✅ Aggiorna size usata
		if (anyChunkChanged) {
			usedSize = size;
		}

	}
} // namespace OnYuu