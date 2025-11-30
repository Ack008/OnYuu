#pragma once
#include "Input.h"
#include "Platform/OpenGL/OpenGLView.h"
#include "Application/Application.h"
#include "Application/Input/KeyCode.h"
#include <unordered_map>

// `OpenGLInput`:
// Implementazione platform-specific dell'input usando GLFW.
// Implementa i metodi privati virtuali definiti in `Input`.
class OpenGLInput : public Input
{
private:
	// Controlla se un tasto è attualmente premuto (stato GLFW)
	bool _isKeyPressed(KeyCode key) override {
		GLFWwindow* window = (GLFWwindow*)Application::getInstance()->getWindow()->getNativeWindow();
		if (!window) return false;
		int action = glfwGetKey(window, glfwKeyMap.at(key));
		return action == GLFW_PRESS || action == GLFW_REPEAT;
	}

	// Controlla se un pulsante del mouse è premuto
	bool _isMouseButtonPressed(int button) override {
		GLFWwindow* window = (GLFWwindow*)Application::getInstance()->getWindow()->getNativeWindow();
		if (!window) return false;
		int action = glfwGetMouseButton(window, button);
		return action == GLFW_PRESS || action == GLFW_REPEAT;
	}

	// Ottiene la posizione del cursore
	void _getMousePosition(double& xpos, double& ypos) override {
		GLFWwindow* window = (GLFWwindow*)Application::getInstance()->getWindow()->getNativeWindow();
		if (!window) {
			xpos = 0;
			ypos = 0;
			return;
		}
		glfwGetCursorPos(window, &xpos, &ypos);
		uint32_t width = Application::getInstance()->getWindow()->getWidth();
		uint32_t height = Application::getInstance()->getWindow()->getHeight();
		xpos = (2.0 * xpos) / width - 1.0;
		ypos = 1.0 - (2.0 * ypos) / height;

	}

	// Rileva un evento di "rilascio" del tasto (transizione da premuto->rilasciato)
	// Implementazione basata sul confronto con lo stato precedente memorizzato in `keyPreviouslyPressed`.
	bool _isKeyPressedOnce(KeyCode key) override {
		GLFWwindow* window = (GLFWwindow*)Application::getInstance()->getWindow()->getNativeWindow();
		if (!window) return false;

		int glfwKey = glfwKeyMap.at(key);
		int action = glfwGetKey(window, glfwKey);
		bool currentlyPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);

		// stato precedente di default a false se non presente
		bool& wasPressed = keyPreviouslyPressed[key];

		if (currentlyPressed && !wasPressed) {
			// transizione da rilasciato -> premuto: segno che il tasto è stato premuto ora
			wasPressed = true;
			return true; // Nota: questa logica segnala la pressione, non il rilascio
		}

		if (!currentlyPressed) {
			// tasto rilasciato: resetto lo stato per permettere un nuovo evento alla prossima pressione
			wasPressed = false;
		}

		return false;
	}

	// Mappa KeyCode -> tasto GLFW
	std::unordered_map<KeyCode, int> glfwKeyMap = {
	{KeyCode::Space, GLFW_KEY_SPACE},
	{KeyCode::Apostrophe, GLFW_KEY_APOSTROPHE},
	{KeyCode::Comma, GLFW_KEY_COMMA},
	{KeyCode::Minus, GLFW_KEY_MINUS},
	{KeyCode::Period, GLFW_KEY_PERIOD},
	{KeyCode::Slash, GLFW_KEY_SLASH},

	{KeyCode::D0, GLFW_KEY_0},
	{KeyCode::D1, GLFW_KEY_1},
	{KeyCode::D2, GLFW_KEY_2},
	{KeyCode::D3, GLFW_KEY_3},
	{KeyCode::D4, GLFW_KEY_4},
	{KeyCode::D5, GLFW_KEY_5},
	{KeyCode::D6, GLFW_KEY_6},
	{KeyCode::D7, GLFW_KEY_7},
	{KeyCode::D8, GLFW_KEY_8},
	{KeyCode::D9, GLFW_KEY_9},

	{KeyCode::Semicolon, GLFW_KEY_SEMICOLON},
	{KeyCode::Equal, GLFW_KEY_EQUAL},

	{KeyCode::A, GLFW_KEY_A},
	{KeyCode::B, GLFW_KEY_B},
	{KeyCode::C, GLFW_KEY_C},
	{KeyCode::D, GLFW_KEY_D},
	{KeyCode::E, GLFW_KEY_E},
	{KeyCode::F, GLFW_KEY_F},
	{KeyCode::G, GLFW_KEY_G},
	{KeyCode::H, GLFW_KEY_H},
	{KeyCode::I, GLFW_KEY_I},
	{KeyCode::J, GLFW_KEY_J},
	{KeyCode::K, GLFW_KEY_K},
	{KeyCode::L, GLFW_KEY_L},
	{KeyCode::M, GLFW_KEY_M},
	{KeyCode::N, GLFW_KEY_N},
	{KeyCode::O, GLFW_KEY_O},
	{KeyCode::P, GLFW_KEY_P},
	{KeyCode::Q, GLFW_KEY_Q},
	{KeyCode::R, GLFW_KEY_R},
	{KeyCode::S, GLFW_KEY_S},
	{KeyCode::T, GLFW_KEY_T},
	{KeyCode::U, GLFW_KEY_U},
	{KeyCode::V, GLFW_KEY_V},
	{KeyCode::W, GLFW_KEY_W},
	{KeyCode::X, GLFW_KEY_X},
	{KeyCode::Y, GLFW_KEY_Y},
	{KeyCode::Z, GLFW_KEY_Z},

	{KeyCode::LeftBracket, GLFW_KEY_LEFT_BRACKET},
	{KeyCode::Backslash, GLFW_KEY_BACKSLASH},
	{KeyCode::RightBracket, GLFW_KEY_RIGHT_BRACKET},
	{KeyCode::GraveAccent, GLFW_KEY_GRAVE_ACCENT},

	{KeyCode::Escape, GLFW_KEY_ESCAPE},
	{KeyCode::Enter, GLFW_KEY_ENTER},
	{KeyCode::Tab, GLFW_KEY_TAB},
	{KeyCode::Backspace, GLFW_KEY_BACKSPACE},
	{KeyCode::Insert, GLFW_KEY_INSERT},
	{KeyCode::Delete, GLFW_KEY_DELETE},
	{KeyCode::Right, GLFW_KEY_RIGHT},
	{KeyCode::Left, GLFW_KEY_LEFT},
	{KeyCode::Down, GLFW_KEY_DOWN},
	{KeyCode::Up, GLFW_KEY_UP},

	{KeyCode::PageUp, GLFW_KEY_PAGE_UP},
	{KeyCode::PageDown, GLFW_KEY_PAGE_DOWN},
	{KeyCode::Home, GLFW_KEY_HOME},
	{KeyCode::End, GLFW_KEY_END},

	{KeyCode::CapsLock, GLFW_KEY_CAPS_LOCK},
	{KeyCode::ScrollLock, GLFW_KEY_SCROLL_LOCK},
	{KeyCode::NumLock, GLFW_KEY_NUM_LOCK},
	{KeyCode::PrintScreen, GLFW_KEY_PRINT_SCREEN},
	{KeyCode::Pause, GLFW_KEY_PAUSE},

	{KeyCode::F1, GLFW_KEY_F1},
	{KeyCode::F2, GLFW_KEY_F2},
	{KeyCode::F3, GLFW_KEY_F3},
	{KeyCode::F4, GLFW_KEY_F4},
	{KeyCode::F5, GLFW_KEY_F5},
	{KeyCode::F6, GLFW_KEY_F6},
	{KeyCode::F7, GLFW_KEY_F7},
	{KeyCode::F8, GLFW_KEY_F8},
	{KeyCode::F9, GLFW_KEY_F9},
	{KeyCode::F10, GLFW_KEY_F10},
	{KeyCode::F11, GLFW_KEY_F11},
	{KeyCode::F12, GLFW_KEY_F12},

	{KeyCode::KP0, GLFW_KEY_KP_0},
	{KeyCode::KP1, GLFW_KEY_KP_1},
	{KeyCode::KP2, GLFW_KEY_KP_2},
	{KeyCode::KP3, GLFW_KEY_KP_3},
	{KeyCode::KP4, GLFW_KEY_KP_4},
	{KeyCode::KP5, GLFW_KEY_KP_5},
	{KeyCode::KP6, GLFW_KEY_KP_6},
	{KeyCode::KP7, GLFW_KEY_KP_7},
	{KeyCode::KP8, GLFW_KEY_KP_8},
	{KeyCode::KP9, GLFW_KEY_KP_9},
	{KeyCode::KPDecimal, GLFW_KEY_KP_DECIMAL},
	{KeyCode::KPDivide, GLFW_KEY_KP_DIVIDE},
	{KeyCode::KPMultiply, GLFW_KEY_KP_MULTIPLY},
	{KeyCode::KPSubtract, GLFW_KEY_KP_SUBTRACT},
	{KeyCode::KPAdd, GLFW_KEY_KP_ADD},
	{KeyCode::KPEnter, GLFW_KEY_KP_ENTER},
	{KeyCode::KPEqual, GLFW_KEY_KP_EQUAL},

	{KeyCode::LeftShift, GLFW_KEY_LEFT_SHIFT},
	{KeyCode::LeftControl, GLFW_KEY_LEFT_CONTROL},
	{KeyCode::LeftAlt, GLFW_KEY_LEFT_ALT},
	{KeyCode::LeftSuper, GLFW_KEY_LEFT_SUPER},
	{KeyCode::RightShift, GLFW_KEY_RIGHT_SHIFT},
	{KeyCode::RightControl, GLFW_KEY_RIGHT_CONTROL},
	{KeyCode::RightAlt, GLFW_KEY_RIGHT_ALT},
	{KeyCode::RightSuper, GLFW_KEY_RIGHT_SUPER},
	{KeyCode::Menu, GLFW_KEY_MENU}
	};
	std::unordered_map<KeyCode, bool> keyPreviouslyPressed;
};