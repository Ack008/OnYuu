#pragma once
#include <memory>
#include "MeshComponent.h"

// RenderCommand: wrapper di livello basso per i comandi di disegno specifici
// dell'API. La classe espone metodi statici semplici (`DRAW_ARRAYS`,
// `DRAW_ELEMENTS`) che inoltrano le chiamate alla implementazione concreta
// (s_instance). Questo permette al codice client di non dipendere direttamente
// dall'API grafica sottostante.
class RenderCommand {
public:
	// DRAW_ARRAYS e DRAW_ELEMENTS sono utilità statiche che mappano alle
	// implementazioni virtuali di draw nella classe concreta (es. OpenGL).
	static void DRAW_ARRAYS(RenderingTypeEnum renderingMode, uint32_t first, size_t count) { s_instance->drawArrays(renderingMode,  first, count); };
	static void DRAW_ELEMENTS(RenderingTypeEnum renderingMode, size_t count) { s_instance->drawElements(renderingMode, count); }
protected:
	// Factory per creare l'implementazione corretta in base all'API in uso.
	static std::shared_ptr<RenderCommand> create();

	// Metodi che le implementazioni concrete devono fornire per eseguire i
	// draw calls corrispondenti all'API grafica.
	virtual void drawArrays(RenderingTypeEnum renderingMode, uint32_t first, size_t count) = 0;
	virtual void drawElements(RenderingTypeEnum renderingMode, size_t count) = 0;
private:
	static std::shared_ptr<RenderCommand> s_instance;

};