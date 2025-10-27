#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include "OpenGLBatchRender.h"
#include "BatchRenderer.h"
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

std::vector<float> OpenGLBatchRender::getData(const BatchCouple& couple)
{
	std::vector<float> bufferContent;

	// Verifica che esista la entry dei batch per la coppia
	auto batchesPtr = getBatches();
	if (!batchesPtr) return bufferContent;
	auto itBatches = batchesPtr->find(couple);
	if (itBatches == batchesPtr->end()) return bufferContent;

	auto batch = itBatches->second;
	for (int i = 0; i < batch.size(); i++) {
		for (size_t j = 0; j < batch[i].renderMesh->mesh->position.size(); j++)
		{
			const float* ptr = glm::value_ptr(batch[i].renderMesh->mesh->position[j]);
			bufferContent.insert(bufferContent.end(), ptr, ptr + 3);

			ptr = glm::value_ptr(batch[i].renderMesh->mesh->color[j]);
			bufferContent.insert(bufferContent.end(), ptr, ptr + 4);
		}
	}
	// Verifica che esista il VBO per la coppia
	auto itVbo = vbosMap.find(couple);
	if (itVbo == vbosMap.end()) return bufferContent;

	VertexBuffer& vb = itVbo->second;

	// Calcola quanti byte vogliamo copiare
	size_t bufferBytes = bufferContent.size() * sizeof(float);
	size_t copyBytes = std::min(bufferBytes, vb.usedSize);

	if (copyBytes == 0) {
		// nulla da scrivere
		return bufferContent;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);

	void* mapped = glMapBufferRange(GL_ARRAY_BUFFER, 0,
		copyBytes,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	if (!mapped) {
		// la mappatura è fallita: evitare memcpy su NULL
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return bufferContent;
	}

	memcpy(mapped, bufferContent.data(), copyBytes);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return bufferContent;
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
			key.first->set("u_viewProjectionMatrix", Render::getInstance()->getCameraMatrix());
			key.first->apply();
			// Imposta la matrice della camera
		}
		for(RenderData rd : pair.second) {
			//key.first->set("u_modelMatrix", rd.model);
			Mesh* mesh = rd.renderMesh->mesh;
			if (meshGPUmap.find(mesh) == meshGPUmap.end()) {
				meshGPUmap[mesh] = MeshGPUusage();
				meshGPUmap[mesh].setMesh(mesh);
			}
			meshGPUmap[mesh].uploadToGPU();
			
			meshGPUmap[mesh].bind();
			// Determina il tipo di disegno
			GLenum rendering = GL_TRIANGLES;
			switch (key.second) {
			case RenderingTypeEnum::TRIANGLE:
				rendering = GL_TRIANGLES;
				break;
			case RenderingTypeEnum::TRIANGLE_FAN:
				rendering = GL_TRIANGLE_FAN;
				break;
			case RenderingTypeEnum::TRIANGLE_STRIP:
				rendering = GL_TRIANGLE_STRIP;
				break;
			case RenderingTypeEnum::LINE:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			}
			GLsizei vertexCount = static_cast<GLsizei>(mesh->position.size());
			if (vertexCount > 0) {
				meshGPUmap[mesh].bind();
				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL
				);
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
