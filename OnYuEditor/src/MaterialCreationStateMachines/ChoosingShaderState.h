#pragma once
#include "../StateMachine/State.h"
class ChoosingShaderState : public State
{
public:
	ChoosingShaderState(StateMachine* stateMachine) : State(stateMachine) {}
	virtual ~ChoosingShaderState() = default;
	virtual void onEnter() override;
	virtual void onExit() override ;
	virtual void onImGuiRender() override;
};