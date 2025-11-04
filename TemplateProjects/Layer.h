#pragma once
#include <string>

// Layer: interfaccia astratta che rappresenta un "layer" dell'applicazione.
// Esempi di layer sono: layer di gioco, layer di UI, layer di debug.
//
// Design:
// - Fornisce hook virtuali che il sistema chiamerà durante il ciclo di vita
//   dell'applicazione: `onAttach`, `onDetach`, `onUpdate`, `onEvent`,
//   `onImGuiRender`.
// - L'uso di metodi virtuali permette di avere implementazioni specifiche per
//   ogni layer (ad es. un `DebugLayer` o `GameLayer`).
class Layer {
	public:
		Layer(std::string name) : m_name(name) {}
		virtual ~Layer() = default;
		// Chiamato una volta quando il layer viene attaccato al sistema
		virtual void onAttach() = 0;
		// Chiamato una volta quando il layer viene rimosso dal sistema
		virtual void onDetach() = 0;
		// Chiamato ogni frame con il delta time
		virtual void onUpdate(float deltaTime) = 0;
		// Hook per ricevere eventi (input, resize, ecc.)
		virtual void onEvent(/*Event& event*/) = 0;
		// Renderizzazione della UI (ImGui)
		virtual void onImGuiRender() = 0;
		virtual const char* getName() const = 0;
	private:
	std::string m_name;
};