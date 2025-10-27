#pragma once
#include "Layer.h"
#include "Prova.h"
class LayerProva : public Layer {

public:
	LayerProva() : Layer("LayerProva") {}
	virtual ~LayerProva() = default;
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(/*Event& event*/) override;
	virtual void onImGuiRender() override;
	virtual const char* getName() const override { return "LayerProva"; }
	virtual void onAttach() override;
	virtual void onDetach() override;
private:
	Scene* currentScene;
};