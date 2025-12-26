#pragma once
#include "Render/RenderCommand.h"
namespace OnYuu {

class OpenGLRenderCommand : public RenderCommand {
	// Ereditato tramite RenderCommand
	virtual void drawArrays(RenderingTypeEnum renderingMode, uint32_t first, size_t count) override;
	virtual void drawElements(RenderingTypeEnum renderingMode, size_t count) override;

};
} // namespace OnYuu