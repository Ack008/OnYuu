#include <iostream>
#include <vector>
#include "OpenGLShader.h"
#pragma warning(disable:4996)
namespace OnYuu {

	OpenGLShader::OpenGLShader(const char* vertexfilename, const char* fragmentfilename)
	{
		shader = createProgram(vertexfilename, fragmentfilename);
		GLuint blockIndex = glGetUniformBlockIndex(shader, "GlobalData");
		if (blockIndex != GL_INVALID_INDEX) {
			glUniformBlockBinding(shader, blockIndex, 0);
		}
		//shader = createProgram("vertexShaderC.glsl", "fragmentShaderC.glsl");
		int maxLength;
		glGetProgramiv(shader, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

		int uniformCount;
		glGetProgramiv(shader, GL_ACTIVE_UNIFORMS, &uniformCount);

		for (int i = 0; i < uniformCount; i++) {
			char* name = new char[maxLength + 1];
			int size;
			GLenum type;
			glGetActiveUniform(shader, i, maxLength, NULL, &size, &type, name);
			GLint location = glGetUniformLocation(shader, name);
			if (location == -1) {
				continue;
			}
			uniformLocationCache[name] = location;
			std::cout << "Uniform " << name << " at location " << location << " size: " << size << std::endl;
			switch (type) {
			case GL_FLOAT: std::cout << " Type: FLOAT " << std::endl; break;
			case GL_FLOAT_VEC2: std::cout << " Type: VEC2 " << std::endl; break;
			case GL_FLOAT_VEC3: std::cout << " Type: VEC3 " << std::endl; break;
			case GL_FLOAT_VEC4: std::cout << " Type: VEC4 " << std::endl; break;
			case GL_INT: std::cout << " Type: INT " << std::endl; break;
			case GL_BOOL: std::cout << " Type: BOOL " << std::endl; break;
			case GL_FLOAT_MAT3: std::cout << " Type: MAT3 " << std::endl; break;
			case GL_FLOAT_MAT4: std::cout << " Type: MAT4 " << std::endl; break;
			default: std::cout << " Type: OTHER " << std::endl; break;
			}
			delete[] name;
		}
	}

	void OpenGLShader::useShader()
	{
		glUseProgram(shader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(shader);
	}

	void OpenGLShader::setUniformMat4(const char* name, const float* value)
	{
		GLint loc = glGetUniformLocation(shader, name);
		glUniformMatrix4fv(loc, 1, GL_FALSE, value);
	}

	char* OpenGLShader::readShaderSource(const char* shaderFile)
	{
		FILE* fp = fopen(shaderFile, "rb");

		if (fp == NULL) { return NULL; }

		fseek(fp, 0L, SEEK_END);
		long size = ftell(fp);

		fseek(fp, 0L, SEEK_SET);
		char* buf = new char[size + 1];
		fread(buf, 1, size, fp);

		buf[size] = '\0';
		fclose(fp);

		return buf;
	}

	GLuint OpenGLShader::createProgram(const char* vertexfilename, const char* fragmentfilename)
	{

		int success;
		char infoLog[512];


		// Creiamo gli eseguibili degli shader
		//Leggiamo il codice del Vertex Shader
		GLchar* VertexShader = readShaderSource(vertexfilename);
		//Visualizzo sulla console il CODICE VERTEX SHADER
		//std::cout << VertexShader;

		//Generiamo un identificativo per il vertex shader
		GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		//Associamo all'identificativo il codice del vertex shader
		glShaderSource(vertexShaderId, 1, (const char**)&VertexShader, NULL);
		//Compiliamo il Vertex SHader
		glCompileShader(vertexShaderId);

		glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}



		//Leggiamo il codice del Fragment Shader
		const GLchar* FragmentShader = readShaderSource(fragmentfilename);
		//Visualizzo sulla console il CODICE FRAGMENT SHADER
		//std::cout << FragmentShader;

		//Generiamo un identificativo per il FRAGMENT shader
		GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderId, 1, (const char**)&FragmentShader, NULL);
		//Compiliamo il FRAGMENT SHader
		glCompileShader(fragmentShaderId);


		glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}


		//Creiamo un identificativo di un eseguibile e gli colleghiamo i due shader compilati
		GLuint programId = glCreateProgram();

		glAttachShader(programId, vertexShaderId);
		glAttachShader(programId, fragmentShaderId);
		glLinkProgram(programId);
		return programId;
	}

	GLint OpenGLShader::getUniformLocation(const std::string& name)
	{
		if (uniformLocationCache.find(name) != uniformLocationCache.end())
			return uniformLocationCache[name];
		GLint location = glGetUniformLocation(shader, name.c_str());
		if (location == -1)
			std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
		uniformLocationCache[name] = location;
		return location;
	}

	void OpenGLShader::setUniformInt(const char* name, int value)
	{
		GLint loc = getUniformLocation(name);
		if (loc != -1) {
			glUniform1i(loc, value);
		}
		else {
			std::cout << "Uniform " << name << " not found!" << std::endl;
		}
	}
	void OpenGLShader::setUniformFloat(const char* name, float value)
	{
		GLint loc = getUniformLocation(name);
		if (loc != -1) {
			glUniform1f(loc, value);
		}
		else {
			std::cout << "Uniform " << name << " not found!" << std::endl;
		}
	}
	void OpenGLShader::setUniformVec2(const char* name, const float* value)
	{
		GLint loc = getUniformLocation(name);
		if (loc != -1) {
			glUniform2fv(loc, 1, value);
		}
		else {
			std::cout << "Uniform " << name << " not found!" << std::endl;
		}
	}
	void OpenGLShader::setUniformVec3(const char* name, const float* value)
	{
		GLint loc = getUniformLocation(name);
		if (loc != -1) {
			glUniform3fv(loc, 1, value);
		}
		else {
			std::cout << "Uniform " << name << " not found!" << std::endl;
		}
	}
	void OpenGLShader::setUniformVec4(const char* name, const float* value)
	{
		GLint loc = getUniformLocation(name);
		if (loc != -1) {
			glUniform4fv(loc, 1, value);
		}
		else {
			std::cout << "Uniform " << name << " not found!" << std::endl;
		}
	}
	void OpenGLShader::setUniformMat3(const char* name, const float* value)
	{
		GLint loc = getUniformLocation(name);
		if (loc != -1) {
			glUniformMatrix3fv(loc, 1, GL_FALSE, value);
		}
		else {
			std::cout << "Uniform " << name << " not found!" << std::endl;
		}
	}
	void OpenGLShader::setUniformMat4(const char* name, const float* value, int count)
	{
		GLint loc = getUniformLocation(name);
		if (loc != -1) {
			glUniformMatrix4fv(loc, count, GL_FALSE, value);
		}
		else {
			std::cout << "Uniform " << name << " not found!" << std::endl;
		}
	}

}