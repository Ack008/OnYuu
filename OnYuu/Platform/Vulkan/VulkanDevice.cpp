#include "VulkanDevice.h"

namespace OnYuu {

    VulkanDevice::~VulkanDevice() {
        shutdown();
    }

    VulkanDevice::VulkanDevice(VulkanDevice&& other) noexcept
        : device_(std::move(other.device_))
        , dispatch_(std::move(other.dispatch_))
        , graphicsQueue_(other.graphicsQueue_)
        , presentQueue_(other.presentQueue_)
        , graphicsQueueFamily_(other.graphicsQueueFamily_)
        , presentQueueFamily_(other.presentQueueFamily_) {
        other.reset();
    }

    VulkanDevice& VulkanDevice::operator=(VulkanDevice&& other) noexcept {
        if (this != &other) {
            shutdown();

            device_ = std::move(other.device_);
            dispatch_ = std::move(other.dispatch_);
            graphicsQueue_ = other.graphicsQueue_;
            presentQueue_ = other.presentQueue_;
            graphicsQueueFamily_ = other.graphicsQueueFamily_;
            presentQueueFamily_ = other.presentQueueFamily_;

            other.reset();
        }
        return *this;
    }

    bool VulkanDevice::initialize(vkb::Instance& instance, VkSurfaceKHR surface, const Config& config) {
        if (isValid()) {
            std::cerr << "VulkanDevice: Already initialized\n";
            return false;
        }

        // Seleziona physical device
        vkb::PhysicalDeviceSelector selector(instance);

        auto physDeviceRet = selector
            .set_surface(surface)
            .set_minimum_version(1, 2)
            .add_required_extensions(config.requiredExtensions)
            .select();

        if (!physDeviceRet) {
            std::cerr << "VulkanDevice: Failed to select physical device: "
                << physDeviceRet.error().message() << "\n";

            if (physDeviceRet.error() == vkb::PhysicalDeviceError::no_suitable_device) {
                const auto& reasons = physDeviceRet.detailed_failure_reasons();
                if (!reasons.empty()) {
                    std::cerr << "GPU Selection failure reasons:\n";
                    for (const std::string& reason : reasons) {
                        std::cerr << "  - " << reason << "\n";
                    }
                }
            }
            return false;
        }

        vkb::PhysicalDevice physicalDevice = physDeviceRet.value();

        // Setup descriptor indexing features
        VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
        indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        indexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        indexingFeatures.runtimeDescriptorArray = VK_TRUE;
        indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
        indexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;

        // Crea logical device
        vkb::DeviceBuilder deviceBuilder{ physicalDevice };

        auto deviceRet = deviceBuilder
            .add_pNext(&indexingFeatures)
            .build();

        if (!deviceRet) {
            std::cerr << "VulkanDevice: Failed to create device: "
                << deviceRet.error().message() << "\n";
            return false;
        }

        device_ = deviceRet.value();
        dispatch_ = device_.make_table();

        // Ottieni le queue
        auto graphicsQueue = device_.get_queue(vkb::QueueType::graphics);
        auto presentQueue = device_.get_queue(vkb::QueueType::present);

        if (!graphicsQueue || !presentQueue) {
            std::cerr << "VulkanDevice: Failed to get queues\n";
            shutdown();
            return false;
        }

        graphicsQueue_ = graphicsQueue.value();
        presentQueue_ = presentQueue.value();

        auto graphicsFamily = device_.get_queue_index(vkb::QueueType::graphics);
        auto presentFamily = device_.get_queue_index(vkb::QueueType::present);

        if (!graphicsFamily || !presentFamily) {
            std::cerr << "VulkanDevice: Failed to get queue families\n";
            shutdown();
            return false;
        }

        graphicsQueueFamily_ = graphicsFamily.value();
        presentQueueFamily_ = presentFamily.value();

        std::cout << "VulkanDevice: Initialized successfully\n";
        std::cout << "  - Graphics Queue Family: " << graphicsQueueFamily_ << "\n";
        std::cout << "  - Present Queue Family: " << presentQueueFamily_ << "\n";

        return true;
    }

    void VulkanDevice::shutdown() {
        if (device_.device != VK_NULL_HANDLE) {
            dispatch_.deviceWaitIdle();
            vkb::destroy_device(device_);
            reset();
            std::cout << "VulkanDevice: Shutdown complete\n";
        }
    }

    void VulkanDevice::waitIdle() const {
        if (device_.device != VK_NULL_HANDLE) {
            dispatch_.deviceWaitIdle();
        }
    }

    void VulkanDevice::reset() {
        device_ = {};
        dispatch_ = {};
        graphicsQueue_ = VK_NULL_HANDLE;
        presentQueue_ = VK_NULL_HANDLE;
        graphicsQueueFamily_ = 0;
        presentQueueFamily_ = 0;
    }

} // namespace OnYuu