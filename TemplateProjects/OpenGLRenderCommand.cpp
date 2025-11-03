#include <glad/glad.h>
#include "OpenGLRenderCommand.h"

void OpenGLRenderCommand::drawArrays(RenderingTypeEnum renderingMode, uint32_t first, size_t count)
{
	GLenum rendering = GL_TRIANGLES;
	switch (renderingMode) {
	case RenderingTypeEnum::TRIANGLE:
		rendering = GL_TRIANGLES;
		break;
	case RenderingTypeEnum::TRIANGLE_FAN:
		rendering = GL_TRIANGLE_FAN;
		break;
	case RenderingTypeEnum::TRIANGLE_STRIP:
		rendering = GL_TRIANGLE_STRIP;
		break;
	case RenderingTypeEnum::LINE:
		rendering = GL_LINE_LOOP;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	}
	if (count > 0) {
		glDrawArrays(rendering, first, count);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL
		);
	}

}

void OpenGLRenderCommand::drawElements(RenderingTypeEnum renderingMode, size_t count)
{
	glLineWidth(10);
	GLenum rendering = GL_TRIANGLES;
	switch (renderingMode) {
	case RenderingTypeEnum::TRIANGLE:
		rendering = GL_TRIANGLES;
		break;
	case RenderingTypeEnum::TRIANGLE_FAN:
		rendering = GL_TRIANGLE_FAN;
		break;
	case RenderingTypeEnum::TRIANGLE_STRIP:
		rendering = GL_TRIANGLE_STRIP;
		break;
	case RenderingTypeEnum::LINE:
		rendering = GL_LINE_LOOP;
		break;
	}
	glDrawElements(rendering, count, GL_UNSIGNED_INT, 0);
}
