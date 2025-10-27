#pragma once
#include "Layer.h"
class ImGuiLayer : public Layer {
public:
		ImGuiLayer() : Layer("ImGuiLayer") {}
		virtual ~ImGuiLayer() = default;
		virtual void onUpdate(float deltaTime) override;
		virtual void onEvent(/*Event& event*/) override;
		virtual void onImGuiRender() override;
		virtual const char* getName() const override { return "ImGuiLayer"; }
		virtual void onAttach() override;
		virtual void onDetach() override;
		void begin();
		void end();
};