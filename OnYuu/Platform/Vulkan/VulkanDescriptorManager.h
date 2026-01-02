#pragma once

#include "VulkanDevice.h"
#include <vector>
#include <unordered_map>

namespace OnYuu {

    /**
     * VulkanDescriptorManager - Gestisce descriptor pools, layouts e sets
     * Responsabilità: Creazione/allocazione descriptor sets, update helpers
     */
    class VulkanDescriptorManager {
    public:
        explicit VulkanDescriptorManager(VulkanDevice* device);
        ~VulkanDescriptorManager();

        // Non copiabile
        VulkanDescriptorManager(const VulkanDescriptorManager&) = delete;
        VulkanDescriptorManager& operator=(const VulkanDescriptorManager&) = delete;

        /**
         * Inizializza descriptor pool
         * @param maxSets Numero massimo di descriptor sets
         * @return true se successo
         */
        bool initialize(uint32_t maxSets = 10000);

        /**
         * Libera tutte le risorse
         */
        void shutdown();

        /**
         * Crea un descriptor set layout
         * @param bindings Vector di binding descriptions
         * @return Handle del layout (VK_NULL_HANDLE se errore)
         */
        VkDescriptorSetLayout createLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

        /**
         * Distrugge un descriptor set layout
         * @param layout Layout da distruggere
         */
        void destroyLayout(VkDescriptorSetLayout layout);

        /**
         * Alloca un descriptor set
         * @param layout Layout da usare
         * @return Handle del descriptor set (VK_NULL_HANDLE se errore)
         */
        VkDescriptorSet allocateSet(VkDescriptorSetLayout layout);

        /**
         * Alloca multipli descriptor sets con lo stesso layout
         * @param layout Layout da usare
         * @param count Numero di sets da allocare
         * @return Vector di descriptor sets
         */
        std::vector<VkDescriptorSet> allocateSets(VkDescriptorSetLayout layout, uint32_t count);

        /**
         * Libera un descriptor set
         * @param set Set da liberare
         */
        void freeSet(VkDescriptorSet set);

        /**
         * Libera multipli descriptor sets
         * @param sets Sets da liberare
         */
        void freeSets(const std::vector<VkDescriptorSet>& sets);

        /**
         * Aggiorna un descriptor set con un buffer
         * @param set Descriptor set da aggiornare
         * @param binding Binding index
         * @param buffer Buffer handle
         * @param size Size del buffer (VK_WHOLE_SIZE per intero)
         * @param offset Offset nel buffer
         * @param descriptorType Tipo di descriptor
         */
        void updateBuffer(VkDescriptorSet set, uint32_t binding,
            VkBuffer buffer, VkDeviceSize size = VK_WHOLE_SIZE,
            VkDeviceSize offset = 0,
            VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        /**
         * Aggiorna un descriptor set con un'immagine
         * @param set Descriptor set da aggiornare
         * @param binding Binding index
         * @param view Image view
         * @param sampler Sampler
         * @param layout Image layout
         */
        void updateImage(VkDescriptorSet set, uint32_t binding,
            VkImageView view, VkSampler sampler,
            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        /**
         * Aggiorna un descriptor set con multipli buffer
         * @param set Descriptor set da aggiornare
         * @param binding Binding index
         * @param buffers Vector di buffer infos
         * @param descriptorType Tipo di descriptor
         */
        void updateBuffers(VkDescriptorSet set, uint32_t binding,
            const std::vector<VkDescriptorBufferInfo>& buffers,
            VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        /**
         * Aggiorna un descriptor set con multiple immagini
         * @param set Descriptor set da aggiornare
         * @param binding Binding index
         * @param images Vector di image infos
         */
        void updateImages(VkDescriptorSet set, uint32_t binding,
            const std::vector<VkDescriptorImageInfo>& images);

        bool isValid() const { return pool_ != VK_NULL_HANDLE; }
        VkDescriptorPool getPool() const { return pool_; }

    private:
        VulkanDevice* device_;
        VkDescriptorPool pool_ = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> layouts_; // Traccia layouts per cleanup
    };

} // namespace OnYuu