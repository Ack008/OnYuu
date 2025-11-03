#pragma once
#include <memory>
#include "MeshComponent.h"
class RenderCommand {
public:
	static void DRAW_ARRAYS(RenderingTypeEnum renderingMode, uint32_t first, size_t count) { s_instance->drawArrays(renderingMode,  first, count); };
	static void DRAW_ELEMENTS(RenderingTypeEnum renderingMode, size_t count) { s_instance->drawElements(renderingMode, count); }
protected:
	static std::shared_ptr<RenderCommand> create();
	virtual void drawArrays(RenderingTypeEnum renderingMode, uint32_t first, size_t count) = 0;
	virtual void drawElements(RenderingTypeEnum renderingMode, size_t count) = 0;
private:
	static std::shared_ptr<RenderCommand> s_instance;

};