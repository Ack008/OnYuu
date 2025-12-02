#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include "OpenGLBatchRender.h"
#include "Render/BatchRenderer.h"
#include "Application/Application.h"
#include "Application/AssetManager.h"
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


	if(skybox) {
		drawSkybox();
	}
}
void OpenGLBatchRender::drawSkybox()
{
	glDepthFunc(GL_LEQUAL);

	Camera &camera = *getCurrentCamera();
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
	// The last row and column affect the translation of the skybox (which we don't want to affect)
	view = glm::mat4(glm::mat3(glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getTarget() - camera.getPosition(), camera.getUpVector())));
	uint32_t width = Application::getInstance()->getWindow()->getWidth();
	uint32_t height = Application::getInstance()->getWindow()->getHeight();
	projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
	AssetManager& am = AssetManager::instance();
	Mesh* cubeMesh = am.getMesh("cubeMesh");
	if (meshGPUmap.find(cubeMesh) == meshGPUmap.end()) {
		meshGPUmap[cubeMesh] = MeshGPUusage();
		meshGPUmap[cubeMesh].setMesh(cubeMesh);
	}
	meshGPUmap[cubeMesh].uploadToGPU();
	Material* currentSkyboxMaterial = AssetManager::instance().getMaterial("skyboxMaterial");
	meshGPUmap[cubeMesh].bind();
	currentSkyboxMaterial->bind();
	currentSkyboxMaterial->set("view", view);
	currentSkyboxMaterial->set("projection", projection);
	currentSkyboxMaterial->apply();
	skybox->cubeMap->bind();
	// Determina il tipo di disegno

	GLsizei vertexCount = static_cast<GLsizei>(cubeMesh->position.size());
	if (vertexCount > 0) {

		meshGPUmap[cubeMesh].draw(TRIANGLE);
	}


	// Switch back to the normal depth function
	glDepthFunc(GL_LESS);
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

void OpenGLBatchRender::setSkyBox(SkyBoxComponent* skybox)
{
	this->skybox = skybox;
}
