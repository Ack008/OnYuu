#pragma once
#include <memory>
#include "KeyCode.h"

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

	// Implementazione singleton/factory: puntatore all'istanza attiva e funzione di creazione
	static std::shared_ptr<Input> instance;
	static std::shared_ptr<Input> create();
public:
	// Wrapper statici per l'accesso all'input dall'applicazione
	static bool isKeyPressed(KeyCode key) {
		return instance->_isKeyPressed(key);
	}
	static bool isKeyPressedOnce(KeyCode key) {
		return instance->_isKeyPressedOnce(key);
	}
	static bool isMouseButtonPressed(int button) {
		return instance->_isMouseButtonPressed(button);
	}
	static void getMousePosition(double& xpos, double& ypos) {
		instance->_getMousePosition(xpos, ypos);
	}
};