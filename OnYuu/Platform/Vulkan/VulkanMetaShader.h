#pragma once
#include "Core/MetaShader.h"
#include "VulkanVisitor.h"
namespace OnYuu {
	class VulkanMetaShader : public OnYuu::MetaShader {
	public:
		VulkanMetaShader(const std::string& filename);
		void setUniformMat4(const char* name, const float* value) override;
		void setUniformInt(const char* name, int value) override;
		void setUniformFloat(const char* name, float value) override;
		void setUniformVec2(const char* name, const float* value) override;
		void setUniformVec3(const char* name, const float* value) override;
		void setUniformVec4(const char* name, const float* value) override;
		void setUniformMat3(const char* name, const float* value) override;
		void setUniformMat4(const char* name, const float* value, int count) override;
		std::string getUniformName(const std::string& originalName) override;
	private:
		VulkanVisitor vKVisitor;

	};
}