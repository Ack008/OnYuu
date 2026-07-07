#include "OpenGLBatchRender.h"
#include "Render/BatchRenderer.h"
#include "Application/Application.h"
#include "Application/AssetManager.h"
#include "Platform/OpenGL/OpenGLRenderTarget.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>

namespace OnYuu {

    // =========================================================================
    // Helpers
    // =========================================================================

    GLenum OpenGLBatchRender::toGLTopology(RenderingTypeEnum t) {
        switch (t) {
        case RenderingTypeEnum::TRIANGLE_FAN:   return GL_TRIANGLE_FAN;
        case RenderingTypeEnum::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
        case RenderingTypeEnum::LINE:           return GL_LINES;
        default:                                return GL_TRIANGLES;
        }
    }

    // =========================================================================
    // Costruttore / Distruttore
    // =========================================================================

    OpenGLBatchRender::OpenGLBatchRender() {}
    OpenGLBatchRender::~OpenGLBatchRender() {}

    // =========================================================================
    // lazyInit
    // =========================================================================

    void OpenGLBatchRender::lazyInit() {
        if (initialized_) return;

        geometryPool_ = std::make_shared<OpenGLGeometryPool>(64 * 1024 * 1024,
            16 * 1024 * 1024);
        indirectManager_ = std::make_shared<OpenGLIndirectDrawManager>();
        cameraUBO_ = UniformBuffer::create(2, sizeof(CameraInfo));
        lightsUBO_ = UniformBuffer::create(1, sizeof(LightInfo));

        // SSBO binding point 3 — model matrices indicizzate con gl_DrawID
        modelMatrixSSBO_ = std::make_shared<OpenGLStorageBuffer>(
            MODEL_MATRIX_SSBO_BINDING,
            SSBO_INITIAL_COUNT * sizeof(glm::mat4)
        );

        initialized_ = true;
        std::cout << "[OpenGLBatchRender] Lazy init complete (SSBO bp="
            << MODEL_MATRIX_SSBO_BINDING << ")\n";
    }

    // =========================================================================
    // registeringCallbacks
    // =========================================================================

    void OpenGLBatchRender::registeringCallbacks() {
        materialCreateObserverID_ =
            AssetManager::instance().registerOnMaterialCreationObserver(
                [this](const std::string& id, bool) {
                    auto& c = materialCache_[id];
                    c.dirty = true; c.initialized = false;
                });

        materialModifyObserverID_ =
            AssetManager::instance().registerOnMaterialModificationObserver(
                [this](const std::string& id, bool) {
                    materialCache_[id].dirty = true;
                });

        materialDeleteObserverID_ =
            AssetManager::instance().registerOnMaterialRemovalObserver(
                [this](const std::string& id, bool) {
                    pendingInvalidations_.push_back(id);
                });
    }

    // =========================================================================
    // BeginFrame
    // =========================================================================
	void OpenGLBatchRender::resetStats() {
        lastFrameTime_ = Application::getInstance()->getWindow()->getTime();
		indirectDrawCalls_ = 0;
		totalBatches_ = 0;
	}
    void OpenGLBatchRender::BeginFrame() {
		resetStats();   
        lazyInit();
        processPendingInvalidations();
        frameNumber_++;

        if (frameNumber_ % GC_INTERVAL_FRAMES == 0)
            collectGarbage();

        indirectManager_->resetAll();

        int sceneIndex = 0;
        for (RenderScene& scene : renderScenes) {
            if (scene.target) {
                auto glTarget = std::dynamic_pointer_cast<OpenGLRenderTarget>(scene.target);
                if (glTarget) {
                    glTarget->bind();
                    glViewport(0, 0,
                        static_cast<GLsizei>(glTarget->getWidth()),
                        static_cast<GLsizei>(glTarget->getHeight()));
                }
                else {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
            }
            else {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                auto w = Application::getInstance()->getWindow()->getWidth();
                auto h = Application::getInstance()->getWindow()->getHeight();
                glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
            }

            glClearColor(0.1f, 0.2f, 0.7f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderScene(scene, sceneIndex++);
        }
		indirectDrawCalls_ = indirectManager_->getTotalIndirectDrawCalls();
        for (auto rd : renderScenes) {
            totalBatches_ += rd.batches.size();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        auto w = Application::getInstance()->getWindow()->getWidth();
        auto h = Application::getInstance()->getWindow()->getHeight();
        glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
		auto& stats = getStatsRef();
		stats.indirectDrawCalls = indirectDrawCalls_;
		stats.totalBatches = totalBatches_;
		stats.frameTime = Application::getInstance()->getWindow()->getTime() - lastFrameTime_;

    }

    // =========================================================================
    // renderScene
    // =========================================================================

    void OpenGLBatchRender::renderScene(RenderScene& scene, int sceneIndex) {
        updateGlobalUBOs(scene);

        // 1. Costruisce draw commands + raccoglie model matrices nell'ordine corretto
        std::vector<glm::mat4> modelMatrices;
        modelMatrices.reserve(256);
        buildDrawData(scene, sceneIndex, modelMatrices);

        // 2. Carica le model matrices nell'SSBO (binding point 3)
        //    setData usa chunk-hashing: aggiorna solo le zone cambiate
        if (!modelMatrices.empty()) {
            modelMatrixSSBO_->setData(
                modelMatrices.data(),
                modelMatrices.size() * sizeof(glm::mat4),
                BufferUsage::DYNAMIC
            );
        }

        // 3. Carica i draw commands CPU->GPU (glBufferSubData nel DIBO)
        indirectManager_->finalizeAll();

        // 4. Bind VAO globale (VBO+IBO del geometry pool)
        geometryPool_->bind();

        // 5. Per ogni bucket: shader → uniforms → glMultiDrawElementsIndirect
        for (const auto& [key, batch] : scene.batches) {
            if (batch.empty()) continue;

            const std::string& materialId = key.first;
            auto materialPtr = AssetManager::instance().getMaterialPtr(materialId);
            if (!materialPtr) continue;

            GLenum topology = toGLTopology(key.second);
            GLuint programID = getOrCreateProgram(materialPtr->getShader().get(), key.second);
            if (programID == 0) continue;

            glUseProgram(programID);

            // bind()+apply() solo se dirty (uniform cambiate dall'ultimo frame)
            applyMaterialIfDirty(materialId, materialPtr.get());
			materialPtr->bind();
			materialPtr->apply();
            // Una sola GL draw call per tutto il bucket
            executeDrawBucket(sceneIndex, materialPtr, topology);
        }

        geometryPool_->unbind();

        if (scene.skybox)
            renderSkybox(scene);
    }

    // =========================================================================
    // updateGlobalUBOs
    // =========================================================================

    void OpenGLBatchRender::updateGlobalUBOs(RenderScene& scene) {
        if (!scene.activeCamera) return;

        CameraInfo cam;
        cam.view = scene.activeCamera->getViewMatrix();
        cam.projection = scene.activeCamera->getProjectionMatrix();
        cam.position = glm::vec4(scene.activeCamera->getPosition(), 1.0f);
        cameraUBO_->bind();
        cameraUBO_->updateData(&cam, sizeof(CameraInfo), 0);

        LightInfo lights{};
        lights.count = static_cast<int>(std::min(scene.sceneLight.size(), size_t(125)));
        for (int i = 0; i < lights.count; ++i) {
            const auto& l = scene.sceneLight[i];
            lights.lights[i].position = glm::vec4(l.position, 1.0f);
            lights.lights[i].color = l.light.color;
            lights.lights[i].intensity = l.light.intensity;
        }
        lightsUBO_->bind();
        lightsUBO_->updateData(&lights, sizeof(LightInfo), 0);
    }

    // =========================================================================
    // getOrUploadMesh
    // =========================================================================

    OpenGLBatchRender::MeshDrawCacheEntry&
        OpenGLBatchRender::getOrUploadMesh(const std::shared_ptr<Mesh>& mesh) {
        auto it = meshCache_.find(mesh);
        if (it != meshCache_.end()) {
            geometryPool_->updateMeshUsage(mesh, frameNumber_);
            return it->second;
        }

        auto pooled = std::make_shared<PooledMeshGL>(*mesh, geometryPool_);
        pooled->uploadToGPU();
        geometryPool_->registerMesh(mesh, frameNumber_);

        GLDrawElementsIndirectCommand cmd{};
        cmd.count = pooled->getIndexCount();
        cmd.instanceCount = 1;   // sovrascritta sotto
        cmd.firstIndex = pooled->getFirstIndex();
        cmd.baseVertex = pooled->getBaseVertex();
        cmd.baseInstance = 0;   // sovrascritta sotto

        MeshDrawCacheEntry entry;
        entry.pooledMesh = std::move(pooled);
        entry.drawCmd = cmd;

        auto [ins, _] = meshCache_.emplace(mesh, std::move(entry));
        return ins->second;
    }

    // =========================================================================
    // buildDrawData
    //
    //   Percorre i batch della scena e produce due strutture parallele:
    //
    //   A) IndirectBuffer (per bucket):
    //      Un DrawCommand per ogni mesh distinta nel bucket.
    //      baseInstance punta al primo indice nell'SSBO per quel DrawCommand.
    //      instanceCount = numero di istanze di quella mesh nel batch.
    //
    //   B) modelMatrices (array flat):
    //      Per ogni DrawCommand, le model matrices delle istanze, nello
    //      stesso ordine. L'indice SSBO di una istanza è:
    //        baseInstance + gl_InstanceID
    //
    //   Questo permette allo shader di fare:
    //      mat4 model = u_models[gl_BaseInstance + gl_InstanceID];
    //   oppure con ARB_shader_draw_parameters:
    //      mat4 model = u_models[gl_DrawID];   // per istanze singole
    // =========================================================================

    void OpenGLBatchRender::buildDrawData(RenderScene& scene, int sceneIndex,
        std::vector<glm::mat4>& modelMatrices) {
        for (const auto& [key, batch] : scene.batches) {
            if (batch.empty()) continue;

            auto materialPtr = AssetManager::instance().getMaterialPtr(key.first);
            if (!materialPtr) continue;

            GLenum topology = toGLTopology(key.second);
            auto indirectBuffer = indirectManager_->getOrCreateBuffer(
                sceneIndex, materialPtr, topology);

            // Raggruppa per mesh: mesh → lista model matrices delle sue istanze
            std::unordered_map<std::shared_ptr<Mesh>, std::vector<glm::mat4>> meshInstances;
            for (const auto& rd : batch) {
                if (rd.renderMesh && rd.renderMesh->getMesh())
                    meshInstances[rd.renderMesh->getMesh()].push_back(rd.model);
            }

            for (auto& [meshPtr, matrices] : meshInstances) {
                auto& entry = getOrUploadMesh(meshPtr);

                // baseInstance = posizione corrente nell'SSBO
                uint32_t baseInstance = static_cast<uint32_t>(modelMatrices.size());

                // Appende le matrices all'array flat (nell'ordine delle istanze)
                for (const auto& m : matrices)
                    modelMatrices.push_back(m);

                // Costruisce il DrawCommand con baseInstance per indicizzare l'SSBO
                GLDrawElementsIndirectCommand cmd = entry.drawCmd;
                cmd.instanceCount = static_cast<uint32_t>(matrices.size());
                cmd.baseInstance = baseInstance;

                indirectBuffer->addDrawCommand(cmd);
            }
        }
    }

    // =========================================================================
    // executeDrawBucket
    // =========================================================================

    void OpenGLBatchRender::executeDrawBucket(int sceneIndex,
        const std::shared_ptr<Material>& material,
        GLenum topology) {
        auto buf = indirectManager_->getOrCreateBuffer(sceneIndex, material, topology);
        if (!buf->isEmpty())
            buf->executeMultiDraw(topology);
    }

    // =========================================================================
    // applyMaterialIfDirty
    // =========================================================================

    bool OpenGLBatchRender::applyMaterialIfDirty(const std::string& materialId,
        Material* material) {
        auto& cache = materialCache_[materialId];
        if (!cache.dirty && cache.initialized) return false;

        material->bind();
        material->apply();
        cache.dirty = false;
        cache.initialized = true;
        return true;
    }

    // =========================================================================
    // getOrCreateProgram
    // =========================================================================

    GLuint OpenGLBatchRender::getOrCreateProgram(Shader* shader,
        RenderingTypeEnum topology) {
        if (!shader) return 0;
        ShaderCacheKey key{ shader, topology };
        auto it = shaderCache_.find(key);
        if (it != shaderCache_.end()) return it->second;

        shader->useShader();
        GLint prog = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
        shaderCache_[key] = static_cast<GLuint>(prog);
        return static_cast<GLuint>(prog);
    }

    // =========================================================================
    // renderSkybox
    // =========================================================================

    void OpenGLBatchRender::renderSkybox(RenderScene& scene) {
        glDepthFunc(GL_LEQUAL);

        Camera& camera = *scene.activeCamera;
        auto w = Application::getInstance()->getWindow()->getWidth();
        auto h = Application::getInstance()->getWindow()->getHeight();

        glm::mat4 view = glm::mat4(glm::mat3(
            glm::lookAt(camera.getPosition(),
                camera.getPosition() + camera.getTarget() - camera.getPosition(),
                camera.getUpVector())));
        glm::mat4 proj = glm::perspective(
            glm::radians(45.0f), static_cast<float>(w) / h, 0.1f, 100.0f);

        auto cubeMeshPtr = AssetManager::instance().getMeshPtr("cubeMesh");
        if (!cubeMeshPtr) { glDepthFunc(GL_LESS); return; }

        auto& entry = getOrUploadMesh(cubeMeshPtr);

        Material* skyMat = AssetManager::instance().getMaterial("skyboxMaterial");
        if (!skyMat) { glDepthFunc(GL_LESS); return; }

        geometryPool_->bind();
        skyMat->bind();
        skyMat->set("view", view);
        skyMat->set("projection", proj);
        skyMat->apply();

        if (scene.skybox && scene.skybox->cubeMap)
            scene.skybox->cubeMap->bind();

        entry.pooledMesh->drawInstanced(1);

        geometryPool_->unbind();
        glDepthFunc(GL_LESS);
    }

    // =========================================================================
    // addMeshRender / setSkyBox / submit
    // =========================================================================

    void OpenGLBatchRender::addMeshRender(RenderMeshComponent* mesh, glm::mat4 model) {
        BatchRender::addMeshRender(mesh, model);
    }

    void OpenGLBatchRender::setSkyBox(SkyBoxComponent* skybox) {
        BatchRender::setSkyBox(skybox);
        skybox_ = skybox;
        if (skybox && skybox->cubeMap) skybox->cubeMap->bind();
    }

    void OpenGLBatchRender::submit() {
        BatchRender::submit();
        renderScenes.clear();
    }

    // =========================================================================
    // processPendingInvalidations / invalidateMaterial
    // =========================================================================

    void OpenGLBatchRender::processPendingInvalidations() {
        for (const auto& id : pendingInvalidations_)
            materialCache_.erase(id);
        pendingInvalidations_.clear();
    }

    void OpenGLBatchRender::invalidateMaterial(const std::shared_ptr<Material>& material) {
        if (!material) return;
        for (const auto& [id, mat] : AssetManager::instance().getMaterials()) {
            if (mat == material) {
                materialCache_[id].dirty = true;
                return;
            }
        }
    }

    // =========================================================================
    // collectGarbage / removeCachedMesh
    // =========================================================================

    void OpenGLBatchRender::collectGarbage() {
        geometryPool_->collectGarbage(frameNumber_, GC_FRAMES_TO_KEEP, this);
    }

    void OpenGLBatchRender::removeCachedMesh(const std::shared_ptr<Mesh>& mesh) {
        auto it = meshCache_.find(mesh);
        if (it != meshCache_.end()) {
            if (it->second.pooledMesh) it->second.pooledMesh->shutdown();
            meshCache_.erase(it);
        }
    }

    // =========================================================================
    // Shutdown
    // =========================================================================

    void OpenGLBatchRender::Shutdown() {
        if (!initialized_) return;

        if (materialCreateObserverID_ != static_cast<size_t>(-1))
            AssetManager::instance().unregisterOnMaterialCreationObserver(
                materialCreateObserverID_);
        if (materialModifyObserverID_ != static_cast<size_t>(-1))
            AssetManager::instance().unregisterOnMaterialModificationObserver(
                materialModifyObserverID_);
        if (materialDeleteObserverID_ != static_cast<size_t>(-1))
            AssetManager::instance().unregisterOnMaterialRemovalObserver(
                materialDeleteObserverID_);

        for (auto& [mesh, entry] : meshCache_)
            if (entry.pooledMesh) entry.pooledMesh->shutdown();
        meshCache_.clear();

        if (modelMatrixSSBO_) { modelMatrixSSBO_->shutdown(); modelMatrixSSBO_.reset(); }
        if (indirectManager_) { indirectManager_->shutdown(); indirectManager_.reset(); }
        if (geometryPool_) { geometryPool_->shutdown();    geometryPool_.reset(); }

        materialCache_.clear();
        shaderCache_.clear();
        pendingInvalidations_.clear();
        initialized_ = false;
    }

} // namespace OnYuu