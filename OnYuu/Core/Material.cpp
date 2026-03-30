#include "Engine.h"
namespace OnYuu {
Material::Material(std::shared_ptr<Shader> shader)
	: _shader(shader)
{
}
Material::Material(std::shared_ptr<MetaShader> metaShader)
	: _metaShader(metaShader), _shader(metaShader->getShader())
{
}


void Material::set(const std::string& name, const UniformValue& value)
{
	uniforms_[name] = value;
	alreadySet_[name] = false; // Mark as not set
}
void Material::bind()
{
	if (!_shader)
	{
		return;
	}
	_shader->useShader();
	
	for (auto& [name, _] : alreadySet_) {
		alreadySet_[name] = false; // Reset all uniforms to not set
	}
}
void Material::apply()
{
	if (!_shader)
	{
		return;
	}
	int slot = 0;
	textures_.clear();
	for (const auto& [name, value] : uniforms_) {
		if (alreadySet_.find(name) != alreadySet_.end() && alreadySet_.at(name)) {
			continue; // Skip already set uniforms
		}
		std::visit([&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, int>) {
					if(_metaShader) {
						_metaShader->setUniformInt(name.c_str(), arg);
					}
					else
						_shader->setUniformInt(name.c_str(), arg);
				}
				else if constexpr (std::is_same_v<T, float>) {
					if(_metaShader) {
						_metaShader->setUniformFloat(name.c_str(), arg);
					}
					else
						_shader->setUniformFloat(name.c_str(), arg);
				}
				else if constexpr (std::is_same_v<T, glm::vec2>) {
					if(_metaShader) {
						_metaShader->setUniformVec2(name.c_str(), &arg[0]);
					}
					else
						_shader->setUniformVec2(name.c_str(), &arg[0]);
				}
				else if constexpr (std::is_same_v<T, glm::vec3>) {
					if(_metaShader) {
						_metaShader->setUniformVec3(name.c_str(), &arg[0]);
					}
					else
					_shader->setUniformVec3(name.c_str(), &arg[0]);
				}
				else if constexpr (std::is_same_v<T, glm::vec4>) {
					if(_metaShader) {
						_metaShader->setUniformVec4(name.c_str(), &arg[0]);
					}
					else
					_shader->setUniformVec4(name.c_str(), &arg[0]);
				}
				else if constexpr (std::is_same_v<T, glm::mat3>) {
					if(_metaShader) {
						_metaShader->setUniformMat3(name.c_str(), &arg[0][0]);
					}
					else
					_shader->setUniformMat3(name.c_str(), &arg[0][0]);
				}
				else if constexpr (std::is_same_v<T, glm::mat4>) {
					if(_metaShader) {
						_metaShader->setUniformMat4(name.c_str(), &arg[0][0]);
					}
					else
					_shader->setUniformMat4(name.c_str(), &arg[0][0]);
				}
				else if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>) {
					if (arg) {
						arg->bind(slot);
						if(_metaShader) {
							_metaShader->setUniformInt(name.c_str(), slot);
						}
						else
						_shader->setUniformInt(name.c_str(), slot);
						textures_.push_back(arg);
						slot++;
					}
				}

			}, value);
		alreadySet_[name] = true;
	}
	_shader->flushCostants();
}
} // namespace OnYuu
