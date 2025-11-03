#include "RenderCommand.h"
#include "API.h"
#include "OpenGLRenderCommand.h"
std::shared_ptr<RenderCommand> RenderCommand::s_instance = RenderCommand::create();
std::shared_ptr<RenderCommand> RenderCommand::create() {
	switch (apiInUse) {
	case OpenGL:
		return std::make_shared<OpenGLRenderCommand>();
	}
	return NULL;
}