#pragma once
#include "State.h"
class StateMachine
{
public:
	StateMachine() = default;
	~StateMachine() = default;
	void update(float deltaTime) {
		if (currentState_) {
			currentState_->onImGuiRender();
		}
	}
	State* getCurrentState() const { return currentState_; }
	void changeState(State* newState) {
		if (currentState_) {
			currentState_->onExit();
			delete currentState_;
		}
		currentState_ = newState;
		if (currentState_) {
			currentState_->onEnter();
		}
	}
private:
	State* currentState_ = nullptr;
};