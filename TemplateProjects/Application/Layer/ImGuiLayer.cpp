#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include "ImGuiLayer.h"
#include "Application/Application.h"
#include "Platform/API.h"
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
	switch (apiInUse) {
		case API::OpenGL:
			io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // Segnala che il renderer supporta l'offset dei vertici
			ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(app->getWindow()->getNativeWindow()), true); // Inizializza l’integrazione con GLFW
			ImGui_ImplOpenGL3_Init("#version 330"); // Inizializza l’integrazione con OpenGL
			break;
		default:
			break;
	}
	// Inizializza l�integrazione con GLFW per OpenGL
}

void ImGuiLayer::onDetach()
{
	switch (apiInUse) {
		case API::OpenGL:
			ImGui_ImplOpenGL3_Shutdown();   // Pulisce integrazione OpenGL
			ImGui_ImplGlfw_Shutdown();       // Pulisce integrazione GLFW
			break;
		default:
			break;
	}
	ImGui::DestroyContext();            // Distrugge il contesto ImGui
}
void ImGuiLayer::begin() {
	switch (apiInUse) {
	case API::OpenGL:
		ImGui_ImplGlfw_NewFrame();          // Prepara frame nuovo per input GLFW
		ImGui_ImplOpenGL3_NewFrame();
		break;
	default:
		break;
	}
	ImGui::NewFrame();                // Inizia un nuovo frame ImGui
}
void ImGuiLayer::end() {
	ImGui::Render();                  // Finalizza il frame ImGui
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Esegue il rendering dei dati di disegno ImGui
}
