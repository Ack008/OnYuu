#include "Engine.h"
#include "Application/AssetManager.h"
namespace OnYuu {
Material::Material(std::shared_ptr<Shader> shader)
	: shader_(std::move(shader))
{
}
Material::Material(std::string shaderID)
	: shaderID(std::move(shaderID))
{
}

std::shared_ptr<Shader> Material::getShader() const
{
	if (shader_) {
		return shader_;
	}
	auto metaShader = AssetManager::instance().getShaderPtr(shaderID);
	return metaShader ? metaShader->getShader() : nullptr;
}

void Material::set(const std::string& name, const UniformValue& value)
{
	uniforms_[name] = value;
	alreadySet_[name] = false; // Mark as not set
}
void Material::bind()
{
	auto shader = getShader();
	if (!shader)
	{
		return;
	}
	shader->useShader();
	
	for (auto& [name, _] : alreadySet_) {
		alreadySet_[name] = false; // Reset all uniforms to not set
	}
}
void Material::apply()
{
	auto shader = getShader();
	if (!shader)
	{
		return;
	}
	auto _metaShader = AssetManager::instance().getShaderPtr(shaderID);
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
						shader->setUniformInt(name.c_str(), arg);
				}
				else if constexpr (std::is_same_v<T, float>) {
					if(_metaShader) {
						_metaShader->setUniformFloat(name.c_str(), arg);
					}
					else
						shader->setUniformFloat(name.c_str(), arg);
				}
				else if constexpr (std::is_same_v<T, glm::vec2>) {
					if(_metaShader) {
						_metaShader->setUniformVec2(name.c_str(), &arg[0]);
					}
					else
						shader->setUniformVec2(name.c_str(), &arg[0]);
				}
				else if constexpr (std::is_same_v<T, glm::vec3>) {
					if(_metaShader) {
						_metaShader->setUniformVec3(name.c_str(), &arg[0]);
					}
					else
						shader->setUniformVec3(name.c_str(), &arg[0]);
				}
				else if constexpr (std::is_same_v<T, glm::vec4>) {
					if(_metaShader) {
						_metaShader->setUniformVec4(name.c_str(), &arg[0]);
					}
					else
						shader->setUniformVec4(name.c_str(), &arg[0]);
				}
				else if constexpr (std::is_same_v<T, glm::mat3>) {
					if(_metaShader) {
						_metaShader->setUniformMat3(name.c_str(), &arg[0][0]);
					}
					else
						shader->setUniformMat3(name.c_str(), &arg[0][0]);
				}
				else if constexpr (std::is_same_v<T, glm::mat4>) {
					if(_metaShader) {
						_metaShader->setUniformMat4(name.c_str(), &arg[0][0]);
					}
					else
						shader->setUniformMat4(name.c_str(), &arg[0][0]);
				}
				else if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>) {
					if (arg) {
						arg->bind(slot);
						if(_metaShader) {
							_metaShader->setUniformInt(name.c_str(), slot);
						}
						else
							shader->setUniformInt(name.c_str(), slot);
						textures_.push_back(arg);
						slot++;
					}
				}

			}, value);
		alreadySet_[name] = true;
	}
	shader->flushCostants();
}
} // namespace OnYuu
