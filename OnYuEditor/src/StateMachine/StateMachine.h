#pragma once
#include "State.h"

class StateMachine
{
public:
	StateMachine() = default;
	~StateMachine() {
		if (pendingState_) {
			delete pendingState_;
		}
		if (currentState_) {
			currentState_->onExit();
			delete currentState_;
		}
	}

	void update(float deltaTime) {
		isUpdating_ = true;
		if (currentState_) {
			currentState_->onImGuiRender();
		}
		isUpdating_ = false;

		if (hasPendingTransition_) {
			applyStateChange(pendingState_);
			pendingState_ = nullptr;
			hasPendingTransition_ = false;
		}
	}

	State* getCurrentState() const { return currentState_; }

	void changeState(State* newState) {
		if (isUpdating_) {
			if (pendingState_) {
				delete pendingState_;
			}
			pendingState_ = newState;
			hasPendingTransition_ = true;
			return;
		}

		applyStateChange(newState);
	}

private:
	void applyStateChange(State* newState) {
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
	State* pendingState_ = nullptr;
	bool isUpdating_ = false;
	bool hasPendingTransition_ = false;
};