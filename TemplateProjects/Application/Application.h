#pragma once
#include <vector>
#include "Layer.h"
#include "Application/Layer/ImGuiLayer.h"
#include "Core/View/View.h"
#include "Render/Buffer.h"
// Classe principale dell'applicazione: gestisce il ciclo principale, i layer,
// la finestra e i dati globali da passare agli shader.
class Application {
public:
	// Costruttore / Distruttore
	Application();
	~Application();
	void Shutdown();

	// Avvia il loop principale dell'applicazione
	void Run();

	// Popola / invia i dati globali (es. tempo, risoluzione, frame) agli shader
	void sendGlobalShaderData();

	// Gestisce il resize della finestra: aggiornamenti interni e invio eventi
	void onResize(uint32_t width, uint32_t height);

	// Gestione dei layer (stack): aggiunge / rimuove layer
	void pushLayer(Layer* layer);
	void removeLayer(Layer* layer);

	// Singleton accessor: crea l'istanza al primo accesso (NON thread-safe)
	static Application* getInstance() {
		if (instance == nullptr) {
			instance = new Application();
		}
		return instance;
	}

	// Ritorna la finestra condivisa (shared_ptr)
	std::shared_ptr<Window> getWindow() const { return window; }

	// Factory probabile: metodo statico per creare un'applicazione specifica
	static Application* createApplication();

private:
	// Container dei layer attivi; ownership dei singoli Layer è manuale (raw ptr)
	std::vector<Layer*> layers;

	// Layer dedicato a ImGui (interfaccia): pointer grezzo, probabilmente creato/gestito altrove
	ImGuiLayer* imGuiLayer;

	// Finestra dell'applicazione: gestita con shared_ptr
	std::shared_ptr<Window> window;

	// Istanza singleton statica
	static Application* instance;

	// Struct per i dati globali da passare agli shader (uniform buffer).
	// Nota: i padding sono presenti per rispettare gli allineamenti std140
	// usati spesso nelle Uniform Buffer Object (UBO) in OpenGL/GLSL.
	struct GlobalData {
		float iTime;              // tempo totale o tempo frame
		float padding1[3];        // padding per allineamento a 16 byte
		float iResolution[2];     // risoluzione x, y
		float padding2[2];        // padding per allineamento
		int   iFrame;             // contatore dei frame
		float padding3[3];        // padding: int occupa 4 byte, ma std140 allinea a 16
	};

	// Istanza dei dati globali in memoria e UBO associato
	GlobalData data;
	std::shared_ptr<UniformBuffer> globalDataUBO;
};