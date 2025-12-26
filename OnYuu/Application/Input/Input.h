#pragma once
#include <memory>
#include "KeyCode.h"
namespace OnYuu {
enum MouseState {
	HIDDEN,
	VISIBLE,
};
// Classe astratta `Input`:
// - Definisce l'interfaccia di input indipendente dalla piattaforma.
// - Le classi concrete (es. `OpenGLInput`) implementano i metodi privati virtuali
//   e vengono istanziate tramite la factory `create()`.
// Note d'uso:
// - `Input::isKeyPressed`, `isMouseButtonPressed`, `getMousePosition`, `isKeyReleased`
//   sono metodi statici di comodo che inoltrano la chiamata all'istanza piattaforma-specifica.
// - È responsabilità dell'applicazione creare/inizializzare l'istanza (`instance`) prima
//   dell'uso. Se `instance` è `nullptr` l'applicazione potrebbe crashare; le implementazioni
//   concrete possono scegliere di gestire il caso di assenza di window.
class Input {
private:
	// Metodi da implementare nella classe concreta (platform-specific)
	virtual bool _isKeyPressed(KeyCode key) = 0;
	virtual bool _isMouseButtonPressed(int button) = 0;
	virtual void _getMousePosition(double& xpos, double& ypos) = 0;
	virtual bool _isKeyPressedOnce(KeyCode key) = 0;
	virtual void _getMouseWindowPos(double& xpos, double& ypos) = 0;
	virtual void _setMousePosition(double xpos, double ypos) = 0;
	virtual void _setMouseState(MouseState state) = 0;

	// Implementazione singleton/factory: puntatore all'istanza attiva e funzione di creazione
	static std::shared_ptr<Input> instance;
	static std::shared_ptr<Input> create();
public:
	// Wrapper statici per l'accesso all'input dall'applicazione
	static bool isKeyPressed(KeyCode key) {
		return instance->_isKeyPressed(key);
	}
	// Ritorna true solo la prima volta che il tasto viene premuto
	static bool isKeyPressedOnce(KeyCode key) {
		return instance->_isKeyPressedOnce(key);
	}
	// Ritorna true se il tasto del mouse è premuto
	static bool isMouseButtonPressed(int button) {
		return instance->_isMouseButtonPressed(button);
	}
	// Ritorna la posizione corrente del mouse
	static void getMousePosition(double& xpos, double& ypos) {
		instance->_getMousePosition(xpos, ypos);
	}
	// Ritorna la variazione della posizione del mouse dall'ultima chiamata
	static void getMouseWindowPos(double& xpos, double& ypos) {
		instance->_getMouseWindowPos(xpos, ypos);
	}
	// Imposta la posizione del mouse
	static void setMousePosition(double xpos, double ypos) {
		instance->_setMousePosition(xpos, ypos);
	}
	static void setMouseState(MouseState state) {
		instance->_setMouseState(state);
	}
};
} // namespace OnYuu