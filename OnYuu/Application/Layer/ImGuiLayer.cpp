#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_vulkan.h>
#include "ImGuiLayer.h"
#include "Application/Application.h"
#include "Platform/API.h"
#include "Render/Renderer.h"
#include "Platform/Vulkan/VulkanRender.h"
#include <iostream>
namespace OnYuu {
	void ImGuiLayer::onUpdate(float deltaTime) {


	}
	void ImGuiLayer::onEvent(/*Event& event*/) {
	}
	void ImGuiLayer::onImGuiRender() {
	}
	void ImGuiLayer::onAttach() {
		IMGUI_CHECKVERSION();               // Controlla compatibilit� versione ImGui
		ImGui::CreateContext();             // Crea il contesto ImGui (necessario)
		ImGuiIO& io = ImGui::GetIO();       // Ottiene l�oggetto IO di ImGui (config e input)
		io.FontGlobalScale = 2.0f;          // Scala globale del font (doppio delle dimensioni)
		ImGui::StyleColorsDark();            // Imposta tema scuro predefinito
		Application* app = Application::getInstance();
		switch (Render::getAPI()) {
		case API::OpenGL:
			io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // Segnala che il renderer supporta l'offset dei vertici
			ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(app->getWindow()->getNativeWindow()), true); // Inizializza l’integrazione con GLFW
			ImGui_ImplOpenGL3_Init("#version 330"); // Inizializza l’integrazione con OpenGL
			break;
		case API::Vulkan:
		{

			io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // Segnala che il renderer supporta l'offset dei vertici

			VulkanRender* vulkanRender = static_cast<VulkanRender*>(Render::getInstance().get());
			ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(app->getWindow()->getNativeWindow()), true);
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = vulkanRender->getInit().instance;
			init_info.PhysicalDevice = vulkanRender->getInit().device.physical_device;
			init_info.Device = vulkanRender->getInit().device;
			init_info.QueueFamily = vulkanRender->getInit().device.get_queue_index(vkb::QueueType::graphics).value();
			init_info.Queue = vulkanRender->getRenderData().graphics_queue;
			init_info.PipelineCache = VK_NULL_HANDLE;
			vulkanInit(vulkanRender->getInit().device);
			init_info.DescriptorPool = imguiDescriptorPool;
			init_info.MinImageCount = vulkanRender->getInit().swapchain.image_count;
			init_info.ImageCount = vulkanRender->getInit().swapchain.image_count;
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			init_info.RenderPass = vulkanRender->getRenderData().render_pass;
			init_info.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
			init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
			init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &vulkanRender->getInit().swapchain.image_format;


			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;


			ImGui_ImplVulkan_Init(&init_info);

		}


		break;
		default:
			break;
		}
	}



	void ImGuiLayer::onDetach()
	{
		switch (Render::getAPI()) {
		case API::OpenGL:
			ImGui_ImplOpenGL3_Shutdown();   // Pulisce integrazione OpenGL
			ImGui_ImplGlfw_Shutdown();
			// Pulisce integrazione GLFW
			break;
		case API::Vulkan:
		{
			VulkanRender* vulkanRender = static_cast<VulkanRender*>(Render::getInstance().get());
			// IMPORTANTE: aspetta che la GPU abbia finito prima di distruggere
			vkDeviceWaitIdle(vulkanRender->getInit().device);

			// Distruggi il descriptor pool
			ImGui_ImplVulkan_Shutdown();    // Pulisce integrazione Vulkan
			ImGui_ImplGlfw_Shutdown();       // Pulisce integrazione GLFW
			if (imguiDescriptorPool != VK_NULL_HANDLE) {
				vkDestroyDescriptorPool(vulkanRender->getInit().device,
					imguiDescriptorPool,
					nullptr);
				imguiDescriptorPool = VK_NULL_HANDLE;
			}
		}
		break;
		default:
			break;
		}
		ImGui::DestroyContext();            // Distrugge il contesto ImGui
	}
	void ImGuiLayer::begin() {
		switch (Render::getAPI()) {
		case API::OpenGL:
			ImGui_ImplGlfw_NewFrame();          // Prepara frame nuovo per input GLFW
			ImGui_ImplOpenGL3_NewFrame();
			break;
		case API::Vulkan:
			ImGui_ImplGlfw_NewFrame();          // Prepara frame nuovo per input GLFW
			ImGui_ImplVulkan_NewFrame();
			break;
		default:
			break;
		}
		ImGui::NewFrame();                // Inizia un nuovo frame ImGui
	}

	void ImGuiLayer::end() {
		ImGui::Render();                  // Finalizza il frame ImGui
		switch (Render::getAPI()) {
		case API::OpenGL:
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Esegue il rendering dei dati ImGui con OpenGL
			break;
		case API::Vulkan:
		{
			VulkanRender* vulkanRender = static_cast<VulkanRender*>(Render::getInstance().get());
			uint32_t i = vulkanRender->getRenderData().current_frame;
			VkCommandBuffer command_buffer = vulkanRender->getRenderData().command_buffers[i];
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer); // Esegue il rendering dei dati ImGui con Vulkan

		}
		break;
		default:
			break;
		}
	}

	void ImGuiLayer::vulkanInit(VkDevice device)
	{
		imguiDescriptorPool;

		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(device, &pool_info, nullptr, &imguiDescriptorPool);
	}

} // namespace OnYuu