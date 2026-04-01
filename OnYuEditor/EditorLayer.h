#pragma once
#include "Core/Engine.h"

class EditorLayer :
	public Layer
{
public:
	EditorLayer() : Layer("EditorLayer") {}
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(/*Event& event*/) override;
	virtual void onImGuiRender() override;
	virtual void onAttach() override;
	virtual void onDetach() override;

	

};
