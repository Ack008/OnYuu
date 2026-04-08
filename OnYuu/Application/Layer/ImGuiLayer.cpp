#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>
#include "ImGuiLayer.h"
#include "Application/Application.h"
#include "Platform/API.h"
#include "Render/Renderer.h"
#include "Platform/Vulkan/VulkanRender.h"
#include "ImGuizmo/ImGuizmo.h"
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
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // Segnala che il backend supporta i cursori del mouse
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Abilita la navigazione da tastiera
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// abilita il supporto per i viewports (multi-finestra)
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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
			init_info.Instance = vulkanRender->getVkInstance();
			init_info.PhysicalDevice = vulkanRender->getDevice()->getPhysicalDevice();
			init_info.Device = vulkanRender->getDevice()->getDevice();
			init_info.QueueFamily = vulkanRender->getQueueFamily();
			init_info.Queue = vulkanRender->getGraphicQueue();
			init_info.PipelineCache = VK_NULL_HANDLE;
			vulkanInit(vulkanRender->getInit().device);
			init_info.DescriptorPool = imguiDescriptorPool;
			init_info.MinImageCount = vulkanRender->getInit().swapchain.image_count;
			init_info.ImageCount = vulkanRender->getInit().swapchain.image_count;
			init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

			init_info.UseDynamicRendering = true;
			init_info.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
			init_info.PipelineInfoMain.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
			init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
			static VkFormat format; // ImGui needs this pointer to be stable if we recreate pipeline
			format = vulkanRender->getInit().swapchain.image_format;
			init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &format;
			init_info.PipelineInfoMain.PipelineRenderingCreateInfo.depthAttachmentFormat = vulkanRender->getDepthFormat();
			if (vulkanRender->getDepthFormat() == VK_FORMAT_D32_SFLOAT_S8_UINT || vulkanRender->getDepthFormat() == VK_FORMAT_D24_UNORM_S8_UINT)
				init_info.PipelineInfoMain.PipelineRenderingCreateInfo.stencilAttachmentFormat = vulkanRender->getDepthFormat();
			init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

			init_info.PipelineInfoForViewports.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
			init_info.PipelineInfoForViewports.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
			init_info.PipelineInfoForViewports.PipelineRenderingCreateInfo.pColorAttachmentFormats = &format;
			init_info.PipelineInfoForViewports.MSAASamples = VK_SAMPLE_COUNT_1_BIT;


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
		ImGui::NewFrame();     
		ImGuizmo::BeginFrame();// Inizia un nuovo frame ImGui
	}

	void ImGuiLayer::end() {
		ImGui::Render();                  // Finalizza il frame ImGui
		ImGuiIO& io = ImGui::GetIO();
		switch (Render::getAPI()) {
		case API::OpenGL:
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Esegue il rendering dei dati ImGui con OpenGL
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
			break;
		case API::Vulkan:
		{
			VulkanRender* vulkanRender = static_cast<VulkanRender*>(Render::getInstance().get());
			uint32_t i = vulkanRender->getCurrentFrame();
			VkCommandBuffer command_buffer = vulkanRender->getCommandManager()->getCommandBuffer(i);
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer); // Esegue il rendering dei dati ImGui con Vulkan
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);

			}
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