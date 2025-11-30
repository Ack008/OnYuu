#pragma once

#include "Core/Engine.h"
class Controller : public Component {
private:
	GameObject selected;
public:
	void start() override {
	}
	void update(float dt) override {
	}
	GameObject getSelectedObject() const {
		return selected;
	}
	void setSelectedObject(GameObject obj) {
		selected = obj;
	}

private:
	
};