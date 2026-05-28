#include "Engine.h"
#include "Application/AssetManager.h"
namespace OnYuu {
Material::Material(std::shared_ptr<Shader> shader)
	: shader_(std::move(shader))
{
	// notify created via AssetManager when added to manager (AssetManager handles notification)
}
Material::Material(std::string shaderID)
	: shaderID(std::move(shaderID))
{
}

void Material::setShader(std::shared_ptr<Shader> shader)
{
	shader_ = std::move(shader);
	// mark all uniforms as not-set so apply() will re-apply them
	for (auto& kv : alreadySet_) kv.second = false;
	// notify modification
	AssetManager::instance().notifyMaterialModified(this);
}

void Material::setShaderByID(const std::string& shaderID)
{
	if (this->shaderID != shaderID) {
		this->shaderID = shaderID;
		shader_.reset(); // Clear cached shader pointer to force re-resolve
		for (auto& kv : alreadySet_) kv.second = false; // Mark all uniforms as not-set
		AssetManager::instance().notifyMaterialModified(this);
	}
}

std::shared_ptr<Shader> Material::getShader() const
{
	// Prefer resolving the shader via AssetManager using shaderID so that
	// runtime recompiles/updates propagate automatically to materials.
	if (!shaderID.empty()) {
		std::string normID = shaderID;
		std::replace(normID.begin(), normID.end(), '\\', '/');
		auto metaShader = AssetManager::instance().getShaderPtr(normID);
		if (metaShader && metaShader->getShader()) return metaShader->getShader();
	}
	// Fallback to any cached shader pointer the material might hold
	if (shader_) {
		return shader_;
	}
	return nullptr;
}

void Material::set(const std::string& name, const UniformValue& value)
{

	uniforms_[name] = value;
	alreadySet_[name] = false; // Mark as not set
	AssetManager::instance().notifyMaterialModified(this);
	// Notify AssetManager that this material was modified so renderers can react
}
void Material::bind()
{
	auto shader = getShader();
	if (!shader)
	{
		return;
	}
	shader->useShader();
	for (auto& [name, value] : uniforms_) {
		alreadySet_[name] = false; // Reset all to not set for the new bind
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
	getShader()->flushCostants();
}
} // namespace OnYuu
