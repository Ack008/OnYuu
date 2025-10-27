#include "Application.h"
#include "Prova.h"
#include "LayerProva.h"
class ApplicazioneProva : public Application {
public:
	ApplicazioneProva() : Application() {
		pushLayer(new LayerProva());
	}
private:

};

Application* Application::createApplication() {
	return new ApplicazioneProva();
}