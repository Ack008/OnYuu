#pragma once

#include "../StateMachine/State.h"
class IdleState : public State
{
public :
	IdleState(StateMachine* stateMachine) : State(stateMachine) {}
	virtual ~IdleState() = default;
	virtual void onEnter() override {};
	virtual void onExit() override {};
	virtual void onImGuiRender() override {};
};
		