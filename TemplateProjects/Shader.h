#pragma once
class Shader
{
public:
	virtual void useShader() = 0;
	virtual ~Shader() = default;
	virtual void setUniformMat4(const char* name, const float* value) = 0;
	virtual void setUniformInt(const char* name, int value) = 0;
	virtual void setUniformFloat(const char* name, float value) = 0;
	virtual void setUniformVec2(const char* name, const float* value) = 0;
	virtual void setUniformVec3(const char* name, const float* value) = 0;
	virtual void setUniformVec4(const char* name, const float* value) = 0;
	virtual void setUniformMat3(const char* name, const float* value) = 0;
	virtual void setUniformMat4(const char* name, const float* value, int count) = 0;

};

