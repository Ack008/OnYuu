#pragma once
#include "Scene.h"
#include "Layer.h"
#include "Material.h"
#include <memory>
#include "MeshComponent.h"

// DebugLayer: livello di rendering/utility usato per disegnare geometria di supporto
// (wireframes, punti di debug, mesh semplici) per facilitare il debug della scena.
//
// Questa classe eredita da `Layer` e fornisce hook standard come `onAttach`,
// `onUpdate` e `onImGuiRender`.
//
// Note di progettazione:
// - È pensata come livello opzionale che può essere attaccato/detached dalla
//   `Scene` o dall'Application per mostrare informazioni di debug.
// - Tiene riferimenti a `Material` e `Shader` (con smart pointer) in modo che
//   la gestione delle risorse sia condivisa e sicura.
// - `debugMeshes` contiene componenti di rendering locali, mentre
//   `debugMeshData` mantiene i dati delle mesh condivisibili tra più mesh.
class DebugLayer : public Layer {
	public:
	// Costruttore: riceve la scena corrente a cui associare il layer di debug
	DebugLayer(Scene* scene) : Layer("DebugLayer"), currentScene(scene) {}
	virtual ~DebugLayer() = default;

	// onUpdate: chiamato ogni frame con il deltaTime. Qui si dovrebbe aggiornare
	// la logica del debug (es. animazioni di overlay, ricalcolo di bounding box,
	// ecc.) e inviare comandi di rendering alla pipeline.
	virtual void onUpdate(float deltaTime) override;

	// onEvent: hook per gestire eventi (input, resize, ecc.). In questo progetto
	// è lasciato come stub commentato, ma l'interfaccia è presente per future
	// estensioni (es. abilitare/disabilitare il debug tramite tasti).
	virtual void onEvent(/*Event& event*/) override;

	// onImGuiRender: usato per mostrare UI di debug con ImGui (ad es. toggle per
	// layer, statistiche, proprietà dei materiali/mesh di debug).
	virtual void onImGuiRender() override;

	virtual const char* getName() const override { return "DebugLayer"; }
	virtual void onAttach() override;
	virtual void onDetach() override;

private:
	// Puntatore non-owning alla scena corrente. La scena è proprietaria e gestita
	// altrove, il DebugLayer la usa per leggere informazioni e inviare mesh di
	// debug.
	Scene* currentScene = nullptr;

	// Resource sharing: il materiale e lo shader usati per il rendering di
	// elementi di debug. Conservati come shared_ptr per poterli riutilizzare e
	// evitare deallocazioni premature.
	std::shared_ptr<Material> debugMaterial;
	std::shared_ptr<Shader> debugShader;

	// Contenitori per le mesh di debug:
	// - debugMeshes: componenti che possono essere immediatamente inviati al
	//   renderer per il draw.
	// - debugMeshData: i dati sottostanti (geometria) condivisibili tra più
	//   componenti; mantenuti come shared_ptr per semplicità di gestione.
	std::vector<std::unique_ptr<RenderMeshComponent>> debugMeshes;
	std::vector<std::shared_ptr<Mesh>> debugMeshData;
};