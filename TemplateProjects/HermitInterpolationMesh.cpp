#include "HermitInterpolationMesh.h"
#include "earcut.hpp"
#define PHI0(t)  (2.0*t*t*t-3.0*t*t+1)
#define PHI1(t)  (t*t*t-2.0*t*t+t)
#define PSI0(t)  (-2.0*t*t*t+3.0*t*t)
#define PSI1(t)  (t*t*t-t*t)
using namespace glm;
using Coord = float;
using Point2D = std::array<Coord, 2>;

float HermitInterpolationMesh::dx(int i, float* t)
{
	if (i == 0)
		return  0.5 * (1.0 - curva.Tens[i]) * (1.0 - curva.Bias[i]) * (1.0 - curva.Cont[i]) * (curva.CP[i + 1].x - curva.CP[i].x) / (t[i + 1] - t[i]);
	if (i ==curva.CP.size() - 1)
		return  0.5 * (1.0 - curva.Tens[i]) * (1.0 - curva.Bias[i]) * (1.0 - curva.Cont[i]) * (curva.CP[i].x - curva.CP[i - 1].x) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1.0 - curva.Tens[i]) * (1.0 + curva.Bias[i]) * (1.0 + curva.Cont[i]) * (curva.CP.at(i).x - curva.CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - curva.Tens[i]) * (1 - curva.Bias[i]) * (1 - curva.Cont[i]) * (curva.CP.at(i + 1).x - curva.CP.at(i).x) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1.0 - curva.Tens[i]) * (1.0 + curva.Bias[i]) * (1.0 - curva.Cont[i]) * (curva.CP.at(i).x - curva.CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - curva.Tens[i]) * (1 - curva.Bias[i]) * (1 + curva.Cont[i]) * (curva.CP.at(i + 1).x - curva.CP.at(i).x) / (t[i + 1] - t[i]);
}
float HermitInterpolationMesh::dy(int i, float* t)
{
	if (i == 0)
		return 0.5 * (1.0 - curva.Tens[i]) * (1.0 - curva.Bias[i]) * (1.0 - curva.Cont[i]) * (curva.CP.at(i + 1).y - curva.CP.at(i).y) / (t[i + 1] - t[i]);
	if (i == curva.CP.size() - 1)
		return  0.5 * (1.0 - curva.Tens[i]) * (1.0 - curva.Bias[i]) * (1.0 - curva.Cont[i]) * (curva.CP.at(i).y - curva.CP.at(i - 1).y) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1.0 - curva.Tens[i]) * (1.0 + curva.Bias[i]) * (1.0 + curva.Cont[i]) * (curva.CP.at(i).y - curva.CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - curva.Tens[i]) * (1 - curva.Bias[i]) * (1 - curva.Cont[i]) * (curva.CP.at(i + 1).y - curva.CP.at(i).y) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1.0 - curva.Tens[i]) * (1.0 + curva.Bias[i]) * (1.0 - curva.Cont[i]) * (curva.CP.at(i).y - curva.CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - curva.Tens[i]) * (1 - curva.Bias[i]) * (1 + curva.Cont[i]) * (curva.CP.at(i + 1).y - curva.CP.at(i).y) / (t[i + 1] - t[i]);
}

float HermitInterpolationMesh::DX(int i, float* t)
{
	//Nei vertici di controllo per i quali non sono stati modificati i parametri Tens, Bias, Cont il valore della derivata della componente x della curva è quello originale, altrimenti è quello che è stato modificato nella funzione 
	//keyboardfunc  in seguito alla modifica dei valori Tens, Bias e Cont.

	if (curva.Derivata.at(i).x == 0)
		return dx(i, t);

	if (curva.Derivata.at(i).x != 0)
		return curva.Derivata.at(i).x;



}

float HermitInterpolationMesh::DY(int i, float* t)
{
	// Nei vertici di controllo per i quali non sono stati modificati i parametri Tens, Bias, Cont il valore della derivata della componente y della curva è quello originale, altrimenti è quello che è stato modificato nella funzione
		//keyboardfunc  in seguito alla modifica dei valori Tens, Bias e Cont.

	if (curva.Derivata.at(i).y == 0)
		return dy(i, t);

	if (curva.Derivata.at(i).y != 0)
		return curva.Derivata.at(i).y;

}
void HermitInterpolationMesh::costruisciCurvaHermit()
{

	
}
void HermitInterpolationMesh::InterpolazioneHermite(Mesh& mesh, std::vector<float> t, vec4 color_top, vec4 color_bot)
{	
	int pval = 200; //numero di punti della curva da calcolare
	float p_t = 0, p_b = 0, p_c = 0, x, y;
	float passotg = 1.0 / (float)(pval - 1);
	int j;


	float tg = 0, tgmapp, ampiezza;
	int i = 0;
	int is = 0; //indice dell'estremo sinistro dell'intervallo [t(i),t(i+1)] a cui il punto tg
	//appartiene

	for (tg = 0; tg <= 1; tg += passotg)
	{
		//Localizzo l'intervallo a cui tg appartiente
		if (tg > t[is + 1]) is++;

		ampiezza = (t[is + 1] - t[is]);

		//mappo tg nell'intervallo [0,1]

		tgmapp = (tg - t[is]) / ampiezza;

		x = curva.CP[is].x * PHI0(tgmapp) + DX(is, t.data()) * PHI1(tgmapp) * ampiezza + curva.CP[is + 1].x * PSI0(tgmapp) + DX(is + 1, t.data()) * PSI1(tgmapp) * ampiezza;
		y = curva.CP[is].y * PHI0(tgmapp) + DY(is, t.data()) * PHI1(tgmapp) * ampiezza + curva.CP[is + 1].y * PSI0(tgmapp) + DY(is + 1, t.data()) * PSI1(tgmapp) * ampiezza;

		mesh.position.push_back(vec3(x, y, 0.0));

		mesh.color.push_back(color_bot * t[is] + color_top * (1 - t[is]));
	}

	


}

void HermitInterpolationMesh::triangolarizzazzione_e_rendering(Mesh& mesh)
{
	std::vector<Point2D> polygon2D;
	polygon2D.reserve(mesh.position.size());

	for (const auto& v : mesh.position)
		polygon2D.push_back(Point2D{ v.x, v.y });


	std::vector<std::vector<Point2D>> polygon = { polygon2D };
	// Triangolazione facebndo uso della funzione eartcut
	std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
	// Output degli indici dei triangoli
	mesh.indices = indices;
	
}

Mesh HermitInterpolationMesh::generateHermitMesh(const std::vector<glm::vec3>& controlPoints, const std::vector<glm::vec3>& tangents, int segmentsPerCurve, glm::vec4 colorTop, glm::vec4 colorBottom)
{
	Mesh mesh;
	curva.Bias.clear();
	curva.Cont.clear();
	curva.Tens.clear();
	curva.colCP.clear();
	curva.tg.clear();
	curva.col_tg.clear();
	curva.CP = controlPoints;
	curva.ncp = controlPoints.size();
	for (int i = 0; i < controlPoints.size(); i++)
	{
		curva.colCP.push_back(vec4(0.0, 1.0, 1.0, 1.0));

		// Inizializza parametri aggiuntivi (tensione, bias, continuità, derivata) per il nuovo punto
		curva.Tens.push_back(0.0);
		curva.Bias.push_back(0.0);
		curva.Cont.push_back(0.0);
		curva.Derivata.push_back(vec3(0.0, 0.0, 0.0));
	}
	vec4 color_top = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 color_bot = vec4(1.0, 0.8, 1.0, 1.0);
	t_equispaced(controlPoints.size());
	InterpolazioneHermite(mesh, t, colorTop,colorBottom);

	//Calcolo di una misura normalizzata della distanza di ciascun vertice
	//  dal  centroide (il punto medio geometrico) della curva. 
	vec3 centroid(0.0f);
	for (auto& v : mesh.position)
		centroid += v;
	centroid /= mesh.position.size();


	// Calcolo della distanza massima dal centro
	float maxDist = 0.0f;
	for (auto& v : mesh.position) {
		float dist = distance(v, centroid);
		maxDist = std::max(maxDist, dist);
	}

	// Calcolo valori normalizzati per ogni vertice

	for (auto& v : mesh.position) {
		float dist = glm::distance(v, centroid);
		//curva->distNormalized.push_back(dist / maxDist);
	}
	triangolarizzazzione_e_rendering(mesh);
	return mesh;
}

Mesh HermitInterpolationMesh::generateHermitMesh(const std::vector<glm::vec3>& controlPoints, const std::vector<glm::vec3>& tangents, int segmentsPerCurve)
{
	return generateHermitMesh(controlPoints, tangents, segmentsPerCurve, glm::vec4{ 1 }, glm::vec4{ 1,1,1,1 });
}

void HermitInterpolationMesh::t_equispaced(int numCurves)
{
    t.clear();
    float step = 1.0 / (numCurves - 1);
    for (int i = 0; i < numCurves; ++i) {
        t.push_back((float)i * step);
	}
    t.push_back(1.0f);
}
