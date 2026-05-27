#pragma once
#include "../StateMachine/State.h"		
#include "../StateMachine/StateMachine.h"
#include <filesystem>
class ShaderFileCreationState : public State
{
public:
	ShaderFileCreationState(StateMachine* stateMachine, std::filesystem::path folderPath) 
		: State(stateMachine), m_folderPath(folderPath) {
	}
	virtual ~ShaderFileCreationState() = default;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual void onImGuiRender() override;
private:
	char shaderNameBuffer[256] = {};
	std::filesystem::path m_folderPath;
	bool m_openPopupNextFrame = false;
	bool shouldClose = false;

};