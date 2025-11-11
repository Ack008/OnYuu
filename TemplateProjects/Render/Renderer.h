#pragma once
#include <utility>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Core/RenderingTypeEnum.h"
#include "Core/Model/Components/MeshComponent.h"
#include "Core/Shader.h"
#include "Render/MeshGPUusage.h"
class MeshGPUusage;

// Render: interfaccia astratta che rappresenta il sistema di rendering di alto
// livello. Contiene funzioni virtuali che dovranno essere implementate da una
// specifica API (es. OpenGL, DirectX, Vulkan).
//
// Scopo e design:
// - Fornisce un punto centrale (`getInstance`) per ottenere l'implementazione
//   concreta a runtime (pattern singleton/factory). Questo permette di avere
//   più backend grafici senza cambiare codice client.
// - Tiene una lista di `RenderMeshComponent*` che rappresentano le mesh da
//   disegnare ogni frame. Le mesh possono essere aggiunte tramite
//   `addMeshRender`.
// - Mantiene una mappa `meshGPUmap` che associa le mesh di alto livello a
//   strutture che gestiscono l'upload e l'uso delle risorse GPU (`MeshGPUusage`).
class Render
{
public:
	Render() = default;
	~Render() = default;
	// draw: funzione pura; l'implementazione concreta dovrebbe iterare sulle
	// mesh in `meshRenders`, assicurarsi che i dati siano uploadati sulla GPU
	// e invocare i comandi di draw appropriati (es. glDrawElements).
    virtual void draw() = 0;

	// Aggiunge una mesh alla lista di rendering. La funzione di default aggiunge
	// il puntatore alla lista `meshRenders`. Implementazioni concrete possono
	// sovrascriverla per eseguire batching o trasformazioni.
	virtual void addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) { meshRenders.push_back(mesh); };

    // clear: pulisce le liste di oggetti da disegnare o altre risorse temporanee
	virtual void clear() = 0;

	// Gestione della matrice della camera (view-projection). Il renderer userà
	// questa matrice per impostare gli uniform delle shader prima del draw.
	void setCameraMatrix(const glm::mat4& camMatrix) { cameraMatrix = camMatrix; }
	glm::mat4 getCameraMatrix() const { return cameraMatrix; }

	// Accesso globale all'istanza concreta del renderer. Usa il factory method
	// `create()` per costruire l'implementazione corretta in base all'API.
	static std::shared_ptr<Render> getInstance();
protected:

	// Lista di mesh da renderizzare (puntatori non-owning). Il renderer concreto
	// deve gestire l'ordine di draw e il binding delle risorse.
	std::vector<RenderMeshComponent*> meshRenders;
	// Mappa che associa Mesh (dati CPU) alla struttura che gestisce le risorse
	// GPU (VBO, IBO, VAO, ecc.). Questo permette di riutilizzare buffer GPU
	// quando più mesh condividono la stessa geometria.
private:
	glm::mat4 cameraMatrix;
    static std::shared_ptr<Render> s_instance;
	static std::shared_ptr<Render> create();
};
