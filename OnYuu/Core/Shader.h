#pragma once
#include <memory>
namespace OnYuu {
// Shader: interfaccia astratta che rappresenta un programma shader. Le
// implementazioni concrete (es. OpenGLShader) forniranno il binding dei
// programmi e la gestione degli uniform.
	class Shader
	{
	public:
		virtual void useShader() = 0;
		virtual ~Shader() = default;

		// Metodi per impostare uniform di diversi tipi. Le implementazioni concrete
		// tradurranno queste chiamate nelle chiamate API appropriate (es. glUniform*).
		virtual void setUniformMat4(const char* name, const float* value) = 0;
		virtual void setUniformInt(const char* name, int value) = 0;
		virtual void setUniformFloat(const char* name, float value) = 0;
		virtual void setUniformVec2(const char* name, const float* value) = 0;
		virtual void setUniformVec3(const char* name, const float* value) = 0;
		virtual void setUniformVec4(const char* name, const float* value) = 0;
		virtual void setUniformMat3(const char* name, const float* value) = 0;
		virtual void setUniformMat4(const char* name, const float* value, int count) = 0;
		// Forza l'upload delle costanti settati finora. In opengl sarà vuoto, ma sarà utile per DX11
		virtual void flushCostants() = 0;
		// Indica se lo shader supporta il batching (upload di più matrici/oggetti
		// in un'unica chiamata/array di uniform). Utile per ottimizzazioni nel
		// renderer.
		virtual bool isBatchingSupported() const = 0;
		virtual void shutdown() {};

		// Factory per creare uno shader dalla sorgente (file vert/frag).
		static std::shared_ptr<Shader> create(const char* vertexfilename, const char* fragmentfilename);

	};

}