#pragma once
#include "../StateMachine/State.h"
#include "../StateMachine/StateMachine.h"
#include <string>
#include <filesystem>
class ChoosingMaterialNameState : public State
{
public:
	ChoosingMaterialNameState(StateMachine* stateMachine,  std::filesystem::path shaderPath, std::string shaderId) 
		: State(stateMachine), m_shaderPath(shaderPath), m_shaderId(shaderId) {}
	virtual ~ChoosingMaterialNameState() = default;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual void onImGuiRender() override;

private:
	std::filesystem::path m_materialPath;
	std::string m_materialId;
	std::filesystem::path m_shaderPath;
	std::string m_shaderId;
	char m_materialNameBuffer[256] = {};
};
