#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "Core/Model/Components/MeshComponent.h"
#include "Core/Material.h"
#include "Core/Texture.h"

// AssetManager
// -----------------
// Scopo:
//   - Registro singleton per le risorse dell'applicazione (mesh, materiali).
//   - Conserva e permette il lookup delle risorse per nome usando std::shared_ptr.
//   - I client possono ottenere uno std::shared_ptr (consapevole della ownership)
//     oppure un puntatore grezzo (comodita; NON trasferisce ownership).
//
// Note / raccomandazioni:
//   - Ownership: le risorse sono memorizzate come std::shared_ptr. Restituire un
//     puntatore grezzo tramite getMesh()/getMaterial() e comodo ma puo diventare
//     dangling se lo shared_ptr corrispondente viene rimosso o se la vita del
//     manager termina. Preferire getMeshPtr()/getMaterialPtr() quando si vuole
//     mantenere ownership.
//   - Thread-safety: l'accesso alle mappe non e sincronizzato. Se l'applicazione
//     carica o legge asset da piu thread, proteggere add/get con un mutex o usare
//     un contenitore concorrente.
//   - Dipendenze nell'header: l'header include attualmente le definizioni concrete
//     di Mesh e Material. Per ridurre il coupling di compilazione e possibile
//     usare dichiarazioni forward (ad es. "class Mesh;" e "class Material;")
//     qui e includere i file concreti nel file di implementazione.
//   - Sicurezza del singleton: usare una static locale in instance() garantisce
//     l'inizializzazione lazy thread-safe in C++11 e successivi.
//   - Copy / move: per un singleton conviene cancellare copy e assign per evitare
//     copie accidentali. Esempio (da aggiungere nell'implementazione o qui):
//       AssetManager(const AssetManager&) = delete;
//       AssetManager& operator=(const AssetManager&) = delete;
//   - Comportamento quando non trovato: i metodi get* dovrebbero definire chiaramente
//     cosa succede se il nome non e presente (restituire nullptr o uno shared_ptr vuoto).
//     I chiamanti devono controllare il valore restituito.

class AssetManager {
public:
    // Restituisce l'istanza singleton. L'implementazione dovrebbe usare una
    // variabile statica locale per garantire l'inizializzazione thread-safe.
    static AssetManager& instance();

    // Mesh
    // ----
    // Aggiunge una mesh al manager. Il manager conserva uno shared_ptr alla mesh.
    std::shared_ptr<Mesh> addMesh(const std::string& name, std::shared_ptr<Mesh> mesh);
    // Restituisce lo shared_ptr per la mesh (vuoto se non trovato).
    std::shared_ptr<Mesh> getMeshPtr(const std::string& name) const;
    // Comodita: restituisce un puntatore grezzo alla mesh (puo essere nullptr).
    // NON trasferisce ownership.
    Mesh* getMesh(const std::string& name) const; // raw pointer convenience

    // Material
    // --------
    // Aggiunge un materiale al manager. Il manager conserva uno shared_ptr al materiale.
    std::shared_ptr<Material> addMaterial(const std::string& name, std::shared_ptr<Material> mat);
    // Restituisce lo shared_ptr per il materiale (vuoto se non trovato).
    std::shared_ptr<Material> getMaterialPtr(const std::string& name) const;
    // Puntatore grezzo comodita (puo essere nullptr).
    Material* getMaterial(const std::string& name) const;

    std::shared_ptr<Texture> addTexture(const std::string& name, std::shared_ptr<Texture> tex);
	std::shared_ptr<Texture> getTexturePtr(const std::string& name) const;
	Texture* getTexture(const std::string& name) const;
	// ottieni la mappa delle mesh
    const std::unordered_map<std::string, std::shared_ptr<Mesh>>& getMeshes() const {
        return meshes_;
	}
    // ottieni la mappa dei materiali
    const std::unordered_map<std::string, std::shared_ptr<Material>>& getMaterials() const {
        return materials_;
	}
private:
    AssetManager() = default;

    // Considerare di cancellare copy/move per far rispettare il comportamento singleton.
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;
};