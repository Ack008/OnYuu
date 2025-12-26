#pragma once
#include "Engine.h"
namespace OnYuu {
// Classe: HermitInterpolationMesh
// Descrizione: fornisce funzioni per generare una mesh a partire da una curva
// interpolata mediante Hermite (con parametri tension, bias, continuity).
// Utilizzo tipico: chiamare `HermitInterpolationMesh::generateMesh(controlPoints)`
// che restituisce un oggetto `Mesh` pronto per il rendering.
// Nota: la classe mantiene uno stato interno (struct `Curva` e vettore `t`)
// utilizzato dalle funzioni di calcolo e dalla triangolarizzazione.
class HermitInterpolationMesh {
public:
    // Interfaccia pubblica: genera una mesh Hermite a partire dai punti di controllo.
    // - controlPoints: lista dei punti di controllo (x,y,z). La componente z viene
    //   tipicamente ignorata nelle funzioni di interpolazione 2D, ma è mantenuta
    //   per compatibilità con la struttura `Mesh`.
    // Restituisce: `Mesh` contenente `position`, `color` e `indices`.
    static Mesh generateMesh(const std::vector<glm::vec3>& controlPoints) {
        static HermitInterpolationMesh instance;
        return instance.generateHermitMesh(controlPoints, {},10);
    }

    static Mesh generateMesh(const std::vector<std::vector<glm::vec3>>& controlPoints, std::vector<glm::vec4> colorTop, std::vector<glm::vec4 >colorBottom) {
        static HermitInterpolationMesh instance;
        return instance.generateHermitMesh(controlPoints, {}, 10,colorTop, colorBottom);
    }

    static Mesh generateMesh(const std::vector<std::vector<glm::vec3>>& controlPoints, glm::vec4 colorTop,glm::vec4 colorBottom) {
        static HermitInterpolationMesh instance;
        std::vector<glm::vec4> colorBottomVector;
        for (int i = 0; i < controlPoints.size(); i++) {
            colorBottomVector.push_back(colorBottom);
        }

        std::vector<glm::vec4> colorTopVector;
        for (int i = 0; i < controlPoints.size(); i++) {
            colorTopVector.push_back(colorBottom);
        }
        return instance.generateHermitMesh(controlPoints, {}, 10, colorTopVector, colorBottomVector);
    }
private:
    // Funzioni interne per la generazione della mesh (sovraccaricate):
    // - version con colori top/bottom
    Mesh generateHermitMesh(const std::vector<glm::vec3>& controlPoints, const std::vector<glm::vec3>& tangents, int segmentsPerCurve, glm::vec4 colorTop,glm::vec4 colorBottom);
    // - versione base che usa colori di default
    Mesh generateHermitMesh(const std::vector<glm::vec3>& controlPoints, const std::vector<glm::vec3>& tangents, int segmentsPerCurve);
    Mesh generateHermitMesh(const std::vector<std::vector<glm::vec3>>& controlPoints, const std::vector<std::vector<glm::vec3>>& tangents, int segmentsPerCurve, std::vector<glm::vec4> colorTop, std::vector<glm::vec4 >colorBottom);

    // Calcola parametri t equispaziati per i punti di controllo (usato nella parametrizzazione)
    void t_equispaced(int numCurves);
    std::vector<float> t; // vettore dei parametri t per i punti di controllo

    // Struttura che rappresenta una curva e i suoi parametri associati
    struct Curva {
        // Dati geometrici della curva
        std::vector<glm::vec3> CP;        // Coordinate dei control points (punti di controllo)
        std::vector<glm::vec4> colCP;     // Colori dei control points (opzionale, usato per debug o visualizzazione)
        std::vector<glm::vec3> tg;        // Coordinate dei vettori tangenti ai control points (se forniti)
        std::vector<glm::vec4> col_tg;    // Colori dei vettori tangenti (opzionale)

        // Parametri di interpolazione per ogni punto di controllo
        std::vector<glm::vec3> Derivata;  // Derivate specifiche sui vertici; se vettore nullo viene calcolata automaticamente
        std::vector<float> Tens;          // Tensione per ogni punto (influenzano la forma della Hermite curve)
        std::vector<float> Bias;          // Bias per ogni punto
        std::vector<float> Cont;          // Continuity per ogni punto

        // Informazioni sulla curva
        int ncp = 0;    // Numero di control points
        int ntg = 0;    // Numero di vettori tangenti
    };

    // Funzioni helper per la componente x/y della derivata originale calcolata
    float dx(int i, float* t);
    float dy(int i, float* t);

    // Funzioni che restituiscono la derivata effettiva da usare nella valutazione
    // (se presente in `curva.Derivata` viene usata quella, altrimenti si calcola)
    float DX(int i, float* t);
    float DY(int i, float* t);

    // Costruisce la curva Hermit (placeholder per eventuale logica di setup)
    void costruisciCurvaHermit();

    // Esegue l'interpolazione Hermite e popola il Mesh con posizioni e colori
    void InterpolazioneHermite(Mesh& mesh, std::vector<float> t, glm::vec4 color_top, glm::vec4 color_bot);

    // Triangolarizza la curva (2D) usando earcut e imposta gli indici nella mesh
    void triangolarizzazzione_e_rendering(Mesh& mesh);

    Curva curva; // stato interno usato dalle funzioni di generazione
};
} // namespace OnYuu