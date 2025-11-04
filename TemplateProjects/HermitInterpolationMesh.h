#pragma once
#include "Engine.h"
class HermitInterpolationMesh {
	public:
        static Mesh generateMesh(const std::vector<glm::vec3>& controlPoints) {
            static HermitInterpolationMesh instance;
            return instance.generateHermitMesh(controlPoints, {},10);
		}
private:
    Mesh generateHermitMesh(const std::vector<glm::vec3>& controlPoints, const std::vector<glm::vec3>& tangents, int segmentsPerCurve, glm::vec4 colorTop,glm::vec4 colorBottom);
	Mesh generateHermitMesh(const std::vector<glm::vec3>& controlPoints, const std::vector<glm::vec3>& tangents, int segmentsPerCurve);
	void t_equispaced(int numCurves);
	std::vector<float> t;
    struct Curva {
        // Dati geometrici della curva
        std::vector<glm::vec3> CP;        // Coordinate dei control points (punti di controllo)
        std::vector<glm::vec4> colCP;     // Colori dei control points
        std::vector<glm::vec3> tg;        // Coordinate dei vettori tangenti ai control points
        std::vector<glm::vec4> col_tg;     // Colori dei vettori tangenti

        // Parametri di interpolazione
        std::vector<glm::vec3> Derivata;  // Vettore derivata: se è uguale al vettore nullo significa che non sono state modificate 
        //T,B e C dellla derivata nel vertice di controllo
        std::vector<float> Tens;       // Tensore di tensione
        std::vector<float> Bias;       // Bias
        std::vector<float> Cont;       // Continuity

        // Informazioni sulla curva

        int ncp;                    // Numero di control points
        int ntg;                    // Numero di vettori tangenti
    };
    float dx(int i, float* t);
    float dy(int i, float* t);
    float DX(int i, float* t);
    float DY(int i, float* t);
	void costruisciCurvaHermit();
    void InterpolazioneHermite(Mesh& mesh, std::vector<float> t, glm::vec4 color_top, glm::vec4 color_bot);
	void triangolarizzazzione_e_rendering(Mesh& mesh);
    Curva curva;
};