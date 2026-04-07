#pragma once
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>

#include "MyGLSLVisitor.h"
#include "SemanticVisitor.h"
#include "Shader.h"

namespace OnYuu {
	class MetaShader {
	public:
		MetaShader(const std::string& src, bool isShaderSource = false);
		void setShader(std::shared_ptr<Shader> shader) { shader_ = shader; }
		std::shared_ptr<Shader> getShader() const { return shader_; }
		void useShader() { if (shader_) shader_->useShader(); }
		virtual void setUniformMat4(const char* name, const float* value) = 0;
		virtual void setUniformInt(const char* name, int value) = 0;
		virtual void setUniformFloat(const char* name, float value) = 0;
		virtual void setUniformVec2(const char* name, const float* value) = 0;
		virtual void setUniformVec3(const char* name, const float* value) = 0;
		virtual void setUniformVec4(const char* name, const float* value) = 0;
		virtual void setUniformMat3(const char* name, const float* value) = 0;
		virtual std::string getUniformName(const std::string& originalName) = 0;
		virtual void setUniformMat4(const char* name, const float* value, int count) = 0;
		bool hasErrors() const { return sem.hasErrors(); }
		static std::shared_ptr<MetaShader> create(const std::string& filename, bool isShaderSource = false);
		void flushCostants() { if (shader_) shader_->flushCostants(); }
		void shutdown() { if (shader_) shader_->shutdown(); }
	protected:
		SemanticVisitor& getSemanticVisitor() { return sem; }
		MyGLSLVisitor& getGLSLVisitor() { return glslVisitor; }
	private:
		std::string readFile(const std::string& filename);
		SemanticVisitor sem;
		MyGLSLVisitor glslVisitor;
	private:
		std::string code;
		std::shared_ptr<Shader> shader_;
	};
} // namespace OnYuu