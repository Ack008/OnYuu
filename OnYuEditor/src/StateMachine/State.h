#pragma once
class StateMachine;
class State{
public:
	State(StateMachine* stateMachine) 
		:m_stateMachine(stateMachine)
		{}
	virtual ~State() = default;
	virtual void onEnter() {}
	virtual void onExit() {}
	virtual void onImGuiRender() {}
protected:
	StateMachine* m_stateMachine;
};