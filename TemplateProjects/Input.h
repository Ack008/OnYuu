#pragma once
#include <memory>
#include "KeyCode.h"
class Input {
private:
	virtual bool _isKeyPressed(KeyCode key) = 0;
	virtual bool _isMouseButtonPressed(int button) = 0;
	virtual void _getMousePosition(double& xpos, double& ypos) = 0;
	virtual bool _isKeyReleased(KeyCode key) = 0;

	static std::shared_ptr<Input> instance;
	static std::shared_ptr<Input> create();
public:
	static bool isKeyPressed(KeyCode key) {
		return instance->_isKeyPressed(key);
	}
	static bool isKeyReleased(KeyCode key) {
		return instance->_isKeyReleased(key);
	}
	static bool isMouseButtonPressed(int button) {
		return instance->_isMouseButtonPressed(button);
	}
	static void getMousePosition(double& xpos, double& ypos) {
		instance->_getMousePosition(xpos, ypos);
	}
};