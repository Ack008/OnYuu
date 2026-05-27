#pragma once
#include "../StateMachine/State.h"
#include "../StateMachine/StateMachine.h"
#include "Core/Engine.h"
#include <string>
#include <filesystem>
#include <iostream>

class ShaderEditingState : public State
{
public:
	ShaderEditingState(StateMachine* stateMachine, const std::filesystem::path& shaderPath, std::string shaderId) : State(stateMachine), m_shaderPath(shaderPath), m_shaderId(shaderId) {
	}
	virtual ~ShaderEditingState() = default;
	virtual void onEnter() override ;
	virtual void onExit() override ;
	virtual void onImGuiRender() override;
	void modifyMaterials(std::shared_ptr<OnYuu::MetaShader>& meta);
private:
	std::filesystem::path m_shaderPath;
	std::string m_shaderId;
	char m_shaderNameBuffer[64 *1024] = {};
	StateMachine errorWindowStateMachine;

	
	
};