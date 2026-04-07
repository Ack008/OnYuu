#pragma once
#include "Core/Engine.h"

class ViewportPanel
{
public:
	ViewportPanel();
	~ViewportPanel();
	void onImGuiRender();
	void setViewportTexture(std::shared_ptr < RenderTarget > rt) { m_renderTarget = rt; }
	void setSceneContext(std::shared_ptr<Scene> context) { m_Context = context; }
private:
	std::shared_ptr<RenderTarget> m_renderTarget;
	std::shared_ptr<Scene> m_Context;
};
