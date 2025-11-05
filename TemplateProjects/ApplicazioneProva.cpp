#include "Application.h"
#include "Prova.h"
#include "LayerProva.h"
#include "DebugLayer.h"
class ApplicazioneProva : public Application {
public:
	ApplicazioneProva() : Application() {
		pushLayer(new LayerProva(currentScene));
		pushLayer(new DebugLayer(currentScene));
		currentScene->start();
	}
private:
	Scene* currentScene = new Prova();
};

