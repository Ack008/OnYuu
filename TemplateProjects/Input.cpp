#include "Input.h"
#include "API.h"
#include "OpenGLInput.h"
std::shared_ptr<Input> Input::instance = Input::create();
std::shared_ptr<Input> Input::create() {
	switch (apiInUse) {
		case API::OpenGL:
			return std::make_shared<OpenGLInput>();
		break;
	}
}