#include "Input.h"
#include "Platform/API.h"
#include "Render/Renderer.h"
#include "Platform/OpenGL/Input/OpenGLInput.h"
namespace OnYuu {
std::shared_ptr<Input> Input::instance = Input::create();

void Input::reset()
{
	instance = Input::create();
}

std::shared_ptr<Input> Input::create() {
	switch (Render::getAPI()) {
		case API::OpenGL:
		case API::Vulkan:
			return std::make_shared<OpenGLInput>();
		break;
	}
	return std::make_shared<OpenGLInput>();
}
} // namespace OnYuu