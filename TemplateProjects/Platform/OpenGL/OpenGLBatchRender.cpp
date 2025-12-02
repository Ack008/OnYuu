#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include "OpenGLBatchRender.h"
#include "Render/BatchRenderer.h"
#define INITIAL_BUFFER_SIZE_MULTIPLIER 5

/*
PSEUDOCODICE / PIANO (dettagliato):
1. Evitare chiamate OpenGL nel costruttore perché il contesto GL potrebbe non essere ancora creato -> inizializzazione "lazy".
2. Tenere `vao == 0` come segnale che VAO non è ancora stato creato.
3. All'inizio di `draw()` creare e bindare il VAO se necessario (lazy init).
4. In `getData()`:
   - Verificare che la voce `vbosMap[couple]` esista.
   - Costruire `bufferContent` come prima (pos + color).
   - Calcolare il numero di byte da copiare = min(bufferContent_bytes, usedSize).
   - Se `usedSize == 0` o la mappatura ritorna NULL, evitare memcpy e ritornare.
   - Usare `glMapBufferRange` solo se la dimensione da copiare > 0.
5. In `draw()`:
   - Bindare il VBO e il VAO.
   - Chiamare `getData(key)` per aggiornare i dati.
   - Impostare gli attribute pointer con stride in byte (7 * sizeof(float)).
   - Calcolare il numero di vertici = data.size() / 7 (3 pos + 4 color).
   - Usare quel conteggio in `glDrawArrays`.
6. Aggiungere controlli per evitare accessi a puntatori NULL o memcpy fuori dai limiti.
*/

OpenGLBatchRender::~OpenGLBatchRender()
{
}


OpenGLBatchRender::OpenGLBatchRender()
{
		// Non chiamare funzioni OpenGL qui: il contesto potrebbe non essere pronto.
	// Inizializziamo il VAO in modo lazy durante draw().
	vao = 0;
}

void OpenGLBatchRender::draw()
{
	// Creazione lazy del VAO: assicurarsi che il contesto GL sia attivo prima di chiamare draw()
	

	auto batches = getBatches();
	if (!batches) return;
	for (const auto& pair : *batches) {
		const BatchCouple& key = pair.first;
		/*
		// Verifica che esista il VBO associato
		auto itV = vbosMap.find(key);
		if (itV == vbosMap.end()) continue;


		// Aggiorna dati (map + memcpy) e prendi il vettore dei floats
		std::vector<float> data = this->getData(key);
		glBindBuffer(GL_ARRAY_BUFFER, itV->second.vbo);

		

		// Se non ci sono dati, saltare
		if (data.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			continue;
		}

		// Stride e offset in byte (3 floats pos + 4 floats color = 7 float)
		GLsizei stride = static_cast<GLsizei>(7 * sizeof(float));
		const void* posOffset = reinterpret_cast<const void*>(0);
		const void* colOffset = reinterpret_cast<const void*>(3 * sizeof(float));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, posOffset);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, colOffset);
		*/
		// Usa lo shader
		if (key.first) {
			key.first->bind();
			key.first->apply();
			// Imposta la matrice della camera
		}
		for(RenderData rd : pair.second) {
			if (key.first) {
				key.first->set("u_modelMatrix", rd.model);
				key.first->apply();
			}

			Mesh* mesh = rd.renderMesh->mesh;
			if (meshGPUmap.find(mesh) == meshGPUmap.end()) {
				meshGPUmap[mesh] = MeshGPUusage();
				meshGPUmap[mesh].setMesh(mesh);
			}
			meshGPUmap[mesh].uploadToGPU();
			
			meshGPUmap[mesh].bind();
			// Determina il tipo di disegno
			
			GLsizei vertexCount = static_cast<GLsizei>(mesh->position.size());
			if (vertexCount > 0) {
				meshGPUmap[mesh].draw(key.second);
			}
		}

	}
}

void OpenGLBatchRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model)
{
	BatchRender::addMeshRender(mesh, model);
	auto couple = std::make_pair(mesh->material, mesh->renderingType);
	/*
	if (vbosMap.find(couple) == vbosMap.end()) {
		GLuint vbo;
		size_t initialSize = mesh->getSize();
		glGenBuffers(1, &vbo);
		vbosMap[couple] = { vbo, initialSize ,	initialSize * INITIAL_BUFFER_SIZE_MULTIPLIER };
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, initialSize * INITIAL_BUFFER_SIZE_MULTIPLIER, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if (vbosMap[couple].usedSize + mesh->getSize() >= vbosMap[couple].size * 3 / 4 ) {
		vbosMap[couple].usedSize += mesh->getSize();
		vbosMap[couple].size *= INITIAL_BUFFER_SIZE_MULTIPLIER;
		glBindBuffer(GL_ARRAY_BUFFER, vbosMap[couple].vbo);
		glBufferData(GL_ARRAY_BUFFER, vbosMap[couple].size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}*/
}
