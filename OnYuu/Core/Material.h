#pragma once
#include <variant>
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"
#include "MetaShader.h"
namespace OnYuu {
// Material: incapsula le proprietà necessarie per il rendering (uniforms,
// shader utilizzato, ecc.). È un livello di astrazione sopra lo `Shader` e
// permette di cambiare proprietà senza toccare direttamente il codice dello
// shader.
class Material {
public:
	using UniformValue = std::variant<
		int, float, glm::vec2, glm::vec3, glm::vec4,
		glm::mat3, glm::mat4, std::shared_ptr<Texture>>;

	Material(std::shared_ptr<Shader> shader);
	Material(std::shared_ptr<MetaShader> metaShader);
	Material(const Material&) = default;
	Material(Material&&) noexcept = default;
	Material& operator=(const Material&) = default;
	Material& operator=(Material&&) noexcept = default;
	virtual ~Material() = default;
	// Aggiunge/imposta un uniform nel materiale. Il materiale memorizza i
	// valori e quando `apply()` viene chiamato invierà i dati allo shader.
	void set(const std::string& name, const UniformValue& value);
	// Applica tutti gli uniform al relativo shader. Normalmente chiamato prima
	// di invocare i comandi di draw.
	void apply();
	// Bind del materiale se necessario (es. bind di texture). Qui può essere
	// effettuato anche il set del programma shader.
	void bind();
	std::shared_ptr<Shader> getShader() const { return _shader; }

	/*
	Pseudocodice (piano dettagliato):
	- Evitare la copia della mappa degli uniform quando si legge: restituire una reference const.
	- Se si desidera permettere modifiche alla mappa dall'esterno, fornire anche un overload non-const che restituisca una reference non-const.
	- Implementare:
	  - const std::unordered_map<std::string, UniformValue>& getUniforms() const { return uniforms_; }
	  - std::unordered_map<std::string, UniformValue>& getUniforms() { return uniforms_; }
	- Questo mantiene efficienza evitando copie non necessarie e mantiene il contratto const correctness.
	*/

	// Restituisce una reference const per evitare copie
	const std::unordered_map<std::string, UniformValue>& getUniforms() const { return uniforms_; }
	// Overload non-const per modificare la mappa se necessario
	std::unordered_map<std::string, UniformValue>& getUniforms() { return uniforms_; }
	// get texture
	const std::vector<std::shared_ptr<Texture>>& getTextures() const { return textures_; }

private:
	std::shared_ptr<Shader> _shader;
	std::shared_ptr<MetaShader> _metaShader;
	// Mappa dei uniform da applicare allo shader
	std::unordered_map<std::string, UniformValue> uniforms_;
	// Traccia quali uniform sono già stati impostati (per evitare set ridondanti)
	std::unordered_map<std::string, bool> alreadySet_;
	// texture
	std::vector<std::shared_ptr<Texture>> textures_;

	friend class AssetManager; // Se AssetManager deve accedere a membri privati per gestire materiali
	friend class SceneHierarchyPanel; // Se SceneHierarchyPanel deve accedere a membri privati per modificare materiali
};
}