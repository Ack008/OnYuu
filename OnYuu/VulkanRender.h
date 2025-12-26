#pragma once

#include "Renderer.h"
#include <vector>
#include <memory>

// Minimal Vulkan-like Render implementation skeleton (stubbed).
// This implementation avoids direct Vulkan headers so it can compile when
// the Vulkan SDK is not available. It provides an API surface for init(),
// beginFrame(), addMeshRender(), draw(), clear() that a real Vulkan
// implementation can replace later.

class VulkanRender : public Render {
public:
    VulkanRender();
    ~VulkanRender();

    bool init() override;
    void beginFrame() override;
    void draw() override;
    void clear() override;

    virtual void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) override;

private:
    // Opaque placeholders for Vulkan objects so header does not require Vulkan SDK
    void* instance = nullptr;
    void* physicalDevice = nullptr;
    void* device = nullptr;
    void* graphicsQueue = nullptr;
    void* cmdPool = nullptr;
    void* cmdBuffer = nullptr;

    void* renderPass = nullptr;

    bool initialized = false;
};
