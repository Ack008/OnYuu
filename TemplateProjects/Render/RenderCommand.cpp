#include "Render/RenderCommand.h"
#include "Platform/API.h"
#include "Platform/OpenGL/OpenGLRenderCommand.h"

// Inizializza lo singleton con l'implementazione corretta basata su `apiInUse`.
std::shared_ptr<RenderCommand> RenderCommand::s_instance = RenderCommand::create();

std::shared_ptr<RenderCommand> RenderCommand::create() {
	switch (apiInUse) {
	case OpenGL:
		return std::make_shared<OpenGLRenderCommand>();
		break;
	}
	// Se non è stato selezionato un API valido, ritorniamo nullptr.
	return nullptr;
}