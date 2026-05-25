#pragma once
#include "../StateMachine/State.h"
#include "../StateMachine/StateMachine.h"
#include <string>
#include <filesystem>
#include <iostream>
class MaterialEditingState : public State
{
public:
	MaterialEditingState(StateMachine* stateMachine, const std::filesystem::path& materialPath, std::string materialId) : State(stateMachine), m_materialPath(materialPath), m_materialId(materialId) {
	}
	virtual ~MaterialEditingState() = default;
	virtual void onEnter() override ;
	virtual void onExit() override ;
	virtual void onImGuiRender() override;
private:
	std::filesystem::path m_materialPath;
	std::string m_materialId;
	char m_materialNameBuffer[64 * 1024] = {};

	struct MaterialParamEditor {
		std::string name;
		std::string type;
		bool isTexture = false;
		std::string texturePath;
		char texturePathBuffer[512] = {};
		int intValue = 0;
		float floatValue = 0.0f;
		bool boolValue = false;
		float vec2[2] = { 0.0f, 0.0f };
		float vec3[3] = { 0.0f, 0.0f, 0.0f };
		float vec4[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float mat3[9] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		float mat4[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	};
	std::vector<MaterialParamEditor> m_paramEditors;
	
};