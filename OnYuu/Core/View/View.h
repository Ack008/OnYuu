#pragma once
#include <stdint.h>
#include <memory>
namespace OnYuu {
// Window (interfaccia astratta)
// ------------------------------
// Scopo:
//   - Fornire un'interfaccia astratta per la finestra/contesto di rendering.
//   - Implementazioni concrete (es. GLFW, Win32, SDL) devono fornire i metodi
//   - per il disegno, la gestione del tempo e dello stato della finestra.
//
// Note generali:
//   - Questa classe e un'interfaccia: molti metodi sono puri virtuali e devono
//     essere implementati dalla piattaforma specifica.
//   - `create` e' un factory method che dovrebbe restituire l'implementazione
//     corretta per la piattaforma corrente (es. una Window basata su GLFW).
//   - `getNativeWindow` restituisce un puntatore non tipizzato alla finestra
//     nativa (es. GLFWwindow*). Usare con cautela e solo quando necessario.
//

class Window {
public:
	// Costruisce la finestra con larghezza e altezza specificate.
	Window(uint32_t width, uint32_t height);

	// Disegna il contenuto della finestra. Implementazione piattaforma-specifica.
	virtual void draw() = 0;

	// Restituisce il tempo trascorso per l'ultimo frame (delta time).
	virtual double getFrameTime() = 0;

	// Restituisce il tempo assoluto dall'avvio (in secondi o altra unita,
	// definita dall'implementazione).
	virtual double getTime() = 0;

	// Indica se la finestra deve chiudersi (es. utente ha premuto close).
	virtual bool shouldClose() = 0;

	// Preparazioni per il frame corrente (poll degli eventi, clearing, ecc.).
	virtual void beginFrame() = 0;

	// Accessori per dimensione
	uint32_t getWidth() const;
	uint32_t getHeight() const;

	// Ridimensiona la finestra (aggiorna dimensioni interne e potenzialmente il viewport).
	void resize(uint32_t width, uint32_t height);

	// Restituisce un puntatore al sistema nativo della finestra (es. GLFWwindow*).
	// Tipo void* per non introdurre dipendenze dall'header specifico del backend.
	virtual void* getNativeWindow() const = 0;

	// Factory: crea un'istanza concreta di Window per la piattaforma corrente.
	static std::shared_ptr<Window> create(uint32_t width, uint32_t height);

private:
	uint32_t  _witdh, _height;
};
} // namespace OnYuu