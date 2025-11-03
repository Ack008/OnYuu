#pragma once
#include <variant>
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Shader.h"
class Material {
public:
	using UniformValue = std::variant<
		int, float, glm::vec2, glm::vec3, glm::vec4,
		glm::mat3, glm::mat4
	>;
	Material(std::shared_ptr<Shader> shader);
	Material(const Material&) = default;
	Material(Material&&) noexcept = default;
	Material& operator=(const Material&) = default;
	Material& operator=(Material&&) noexcept = default;
	virtual ~Material() = default;
	// Add material properties and methods here
	void set(const std::string& name, const UniformValue& value);
	void apply();
	void bind();
	std::shared_ptr<Shader> getShader() const { return _shader; }
private:
	std::shared_ptr<Shader> _shader;
	std::unordered_map<std::string, UniformValue> uniforms_;
	std::unordered_map<std::string, bool> alreadySet_;
};