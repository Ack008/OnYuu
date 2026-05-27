#pragma once
#include "../StateMachine/State.h"
#include "../StateMachine/StateMachine.h"
#include <string>
#include "Core/Engine.h"
class ErrorWindowState : public State
{
public:
	ErrorWindowState(StateMachine* stateMachine, const std::string& errorMessage) : State(stateMachine), m_errorMessage(errorMessage) {
	}
	virtual ~ErrorWindowState() = default;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual void onImGuiRender() override;
private:
	std::string m_errorMessage;
	bool m_openPopupNextFrame = false;
	bool shouldClose = false;

};
