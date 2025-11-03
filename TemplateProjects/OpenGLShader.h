#pragma once
#include "Shader.h"
#include <glad/glad.h>
class OpenGLShader :
    public Shader
{
public:
    OpenGLShader(const char* vertexfilename, const char* fragmentfilename);
    // Ereditato tramite Shader
    void useShader() override;
	~OpenGLShader();
	void setUniformMat4(const char* name, const float* value) override;
    void setUniformInt(const char* name, int value) override;
    void setUniformFloat(const char* name, float value) override;
    void setUniformVec2(const char* name, const float* value) override;
    void setUniformVec3(const char* name, const float* value) override;
    void setUniformVec4(const char* name, const float* value) override;
    void setUniformMat3(const char* name, const float* value) override;
	void setUniformMat4(const char* name, const float* value, int count) override;
	virtual bool isBatchingSupported() const override { return false; }
private:
    char* readShaderSource(const char* shaderFile);
    GLuint createProgram(const char* vertexfilename, const char* fragmentfilename);
private:
    unsigned int shader;
};

