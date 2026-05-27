#pragma once
#include "../StateMachine/State.h"
#include <filesystem>
class ChoosingShaderState : public State
{
public:
	ChoosingShaderState(StateMachine* stateMachine) : State(stateMachine) {}
	virtual ~ChoosingShaderState() = default;
	virtual void onEnter() override;
	virtual void onExit() override ;
	virtual void onImGuiRender() override;
private:
	std::filesystem::path m_shaderPath;
	std::string m_shaderId;
};