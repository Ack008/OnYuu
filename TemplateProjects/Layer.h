#pragma once
#include <string>
class Layer {
	public:
		Layer(std::string name) : m_name(name) {}
		virtual ~Layer() = default;
		virtual void onUpdate(float deltaTime) = 0;
		virtual void onEvent(/*Event& event*/) = 0;
		virtual void onImGuiRender() = 0;
		virtual const char* getName() const = 0;
		virtual void onAttach() = 0;
		virtual void onDetach() = 0;
private:
	std::string m_name;
};