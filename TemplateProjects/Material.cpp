#include "Material.h"

Material::Material(std::shared_ptr<Shader> shader)
	: _shader(shader)
{
}
void Material::set(const std::string& name, const UniformValue& value)
{
	uniforms_[name] = value;
}
void Material::apply() const
{
	_shader->useShader();
	for (const auto& [name, value] : uniforms_) {
		std::visit([&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, int>) {
				_shader->setUniformInt(name.c_str(), arg);
			}
			else if constexpr (std::is_same_v<T, float>) {
				_shader->setUniformFloat(name.c_str(), arg);
			}
			else if constexpr (std::is_same_v<T, glm::vec2>) {
				_shader->setUniformVec2(name.c_str(), &arg[0]);
			}
			else if constexpr (std::is_same_v<T, glm::vec3>) {
				_shader->setUniformVec3(name.c_str(), &arg[0]);
			}
			else if constexpr (std::is_same_v<T, glm::vec4>) {
				_shader->setUniformVec4(name.c_str(), &arg[0]);
			}
			else if constexpr (std::is_same_v<T, glm::mat3>) {
				_shader->setUniformMat3(name.c_str(), &arg[0][0]);
			}
			else if constexpr (std::is_same_v<T, glm::mat4>) {
				_shader->setUniformMat4(name.c_str(), &arg[0][0]);
			}
			}, value);
	}
}
