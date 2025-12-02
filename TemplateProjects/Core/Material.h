#pragma once
#include <variant>
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"
// Material: incapsula le proprietà necessarie per il rendering (uniforms,
// shader utilizzato, ecc.). È un livello di astrazione sopra lo `Shader` e
// permette di cambiare proprietà senza toccare direttamente il codice dello
// shader.
class Material {
public:
	using UniformValue = std::variant<
		int, float, glm::vec2, glm::vec3, glm::vec4,
		glm::mat3, glm::mat4, Texture*>;
	

	Material(std::shared_ptr<Shader> shader);
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
private:
	std::shared_ptr<Shader> _shader;
	// Mappa dei uniform da applicare allo shader
	std::unordered_map<std::string, UniformValue> uniforms_;
	// Traccia quali uniform sono già stati impostati (per evitare set ridondanti)
	std::unordered_map<std::string, bool> alreadySet_;
};