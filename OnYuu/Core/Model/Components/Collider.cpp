#include "Collider.h"
#include "Core/Model/GameObject.h"
#include "Core/Model/Components/Transform.h"
#include "Core/Model/Components/MeshComponent.h"
#include <algorithm>
#include <cmath>
#include <limits>
namespace OnYuu {

    // Collider.cpp
    // Implementazioni dei collider: BoxCollider e CircleCollider.
    // Commenti in italiano descrivono cosa fa ogni funzione e le assunzioni
    // fatte (es. uso del RenderMeshComponent per ricavare dimensioni iniziali).

    // BoxCollider::update
    // Metodo chiamato ogni frame per aggiornare lo stato del collider.
    // Attualmente vuoto perché il collider si basa sulle trasformazioni del GameObject
    // e non mantiene uno stato dinamico proprio (la logica di aggiornamento potrebbe
    // includere calcoli per collider orientati o aggiornamento della AABB).
    void BoxCollider::update(float dt) {

    }

    void BoxCollider::setDimensions(float w, float h, float d) {
        width = w;
        height = h;
        depth = d;
        // Aggiorna i punti min/max basandosi sulle nuove dimensioni
        minPoint = glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0);
        maxPoint = glm::vec3(width / 2.0f, height / 2.0f, depth / 2.0);
    }

    // BoxCollider::start
    // Inizializza dimensioni e punti min/max del box basandosi (se presente)
    // sul Mesh associato al RenderMeshComponent dell'oggetto.
    // - Calcola bounding box in spazio locale usando le posizioni dei vertici
    // - Imposta `width`, `height`, `minPoint`, `maxPoint`
    // Nota: i valori sono in coordinate del modello locale; l'applicazione delle
    // trasformazioni viene fatta durante il test di collisione (vedi collideWith).
    void BoxCollider::start() {
        if (obj->hasComponent<RenderMeshComponent>()) {
            auto& renderMeshComp = obj->getComponent<RenderMeshComponent>();
            if (renderMeshComp.mesh) {
                // Assumiamo che la mesh abbia dati di posizione per determinare width/height
                if (!renderMeshComp.mesh->position.empty()) {
                    // Calcola min/max in spazio locale
                    float minX = renderMeshComp.mesh->position[0].x;
                    float maxX = renderMeshComp.mesh->position[0].x;
                    float minY = renderMeshComp.mesh->position[0].y;
                    float maxY = renderMeshComp.mesh->position[0].y;
                    float minZ = renderMeshComp.mesh->position[0].z;
                    float maxZ = renderMeshComp.mesh->position[0].z;
                    for (const auto& pos : renderMeshComp.mesh->position) {
                        if (pos.x < minX) minX = pos.x;
                        if (pos.x > maxX) maxX = pos.x;
                        if (pos.y < minY) minY = pos.y;
                        if (pos.y > maxY) maxY = pos.y;
                        if (pos.z < minZ) minZ = pos.z;
                        if (pos.z > maxZ) maxZ = pos.z;
                    }
                    width = maxX - minX;
                    height = maxY - minY;
                    maxPoint = glm::vec3(maxX, maxY, maxZ);
                    minPoint = glm::vec3(minX, minY, minZ);
                }
            }
        }
    }

    // BoxCollider::hasCollided
    // Interfaccia generale: chiede all'altro collider di risolvere la collisione
    // specifica tramite il double-dispatch (other->collideWith(this)).
    bool BoxCollider::hasCollided(Collider* other) {
        return other->collideWith(this);
    }

    // BoxCollider::collideWith(BoxCollider*)
    // Test semplice AABB vs AABB usando i punti min/max trasformati dal Transform
    // dell'oggetto (si assume getModelMatrix() restituisca una matrice 4x4 per
    // trasformazioni locali -> mondo). Il test controlla la sovrapposizione sulle
    // assi X e Y. Questo è adatto a box axis-aligned dopo aver applicato la
    // trasformazione; se si vogliono box orientati servono test separati.
    bool BoxCollider::collideWith(BoxCollider* other) {
        // =========================
        // Vecchia implementazione AABB (commentata, come richiesto)
        // =========================
        // Trasform thisTransform = obj->getAbsoluteTransform();
        // glm::mat4 thisMat = thisTransform.getModelMatrix();
        // Trasform otherTransform = other->obj->getAbsoluteTransform();
        // glm::mat4 otherMat = otherTransform.getModelMatrix();
        //
        // glm::vec3 thisCorners[8] = {
        //     {minPoint.x, minPoint.y, minPoint.z}, {minPoint.x, minPoint.y, maxPoint.z},
        //     {minPoint.x, maxPoint.y, minPoint.z}, {minPoint.x, maxPoint.y, maxPoint.z},
        //     {maxPoint.x, minPoint.y, minPoint.z}, {maxPoint.x, minPoint.y, maxPoint.z},
        //     {maxPoint.x, maxPoint.y, minPoint.z}, {maxPoint.x, maxPoint.y, maxPoint.z}
        // };
        // glm::vec3 otherCorners[8] = {
        //     {other->minPoint.x, other->minPoint.y, other->minPoint.z}, {other->minPoint.x, other->minPoint.y, other->maxPoint.z},
        //     {other->minPoint.x, other->maxPoint.y, other->minPoint.z}, {other->minPoint.x, other->maxPoint.y, other->maxPoint.z},
        //     {other->maxPoint.x, other->minPoint.y, other->minPoint.z}, {other->maxPoint.x, other->minPoint.y, other->maxPoint.z},
        //     {other->maxPoint.x, other->maxPoint.y, other->minPoint.z}, {other->maxPoint.x, other->maxPoint.y, other->maxPoint.z}
        // };
        // glm::vec3 thisWorldMin(+INFINITY);
        // glm::vec3 thisWorldMax(-INFINITY);
        // for (int i = 0; i < 8; i++) {
        //     glm::vec3 w = thisMat * glm::vec4(thisCorners[i], 1.0f);
        //     thisWorldMin = glm::min(thisWorldMin, w);
        //     thisWorldMax = glm::max(thisWorldMax, w);
        // }
        // glm::vec3 otherWorldMin(+INFINITY);
        // glm::vec3 otherWorldMax(-INFINITY);
        // for (int i = 0; i < 8; i++) {
        //     glm::vec3 w = otherMat * glm::vec4(otherCorners[i], 1.0f);
        //     otherWorldMin = glm::min(otherWorldMin, w);
        //     otherWorldMax = glm::max(otherWorldMax, w);
        // }
        // bool overlapX = thisWorldMin.x <= otherWorldMax.x && thisWorldMax.x >= otherWorldMin.x;
        // bool overlapY = thisWorldMin.y <= otherWorldMax.y && thisWorldMax.y >= otherWorldMin.y;
        // bool overlapZ = thisWorldMin.z <= otherWorldMax.z && thisWorldMax.z >= otherWorldMin.z;
        // return overlapX && overlapY && overlapZ;

        // =========================
        // Nuova implementazione SAT (OBB vs OBB)
        // =========================
        Trasform thisTransform = obj->getAbsoluteTransform();
        glm::mat4 thisMat = thisTransform.getModelMatrix();

        Trasform otherTransform = other->obj->getAbsoluteTransform();
        glm::mat4 otherMat = otherTransform.getModelMatrix();

        glm::vec3 thisCenterLocal = (minPoint + maxPoint) * 0.5f;
        glm::vec3 otherCenterLocal = (other->minPoint + other->maxPoint) * 0.5f;

        glm::vec3 thisCenterWorld = glm::vec3(thisMat * glm::vec4(thisCenterLocal, 1.0f));
        glm::vec3 otherCenterWorld = glm::vec3(otherMat * glm::vec4(otherCenterLocal, 1.0f));

        glm::vec3 thisHalfExtentsLocal = (maxPoint - minPoint) * 0.5f;
        glm::vec3 otherHalfExtentsLocal = (other->maxPoint - other->minPoint) * 0.5f;

        glm::vec3 A[3];
        glm::vec3 B[3];
        float a[3];
        float b[3];

        for (int i = 0; i < 3; i++) {
            glm::vec3 axisA = glm::vec3(thisMat[i]);
            float lenA = glm::length(axisA);
            if (lenA > 0.0f) {
                A[i] = axisA / lenA;
                a[i] = thisHalfExtentsLocal[i] * lenA;
            }
            else {
                A[i] = glm::vec3(0.0f);
                a[i] = 0.0f;
            }

            glm::vec3 axisB = glm::vec3(otherMat[i]);
            float lenB = glm::length(axisB);
            if (lenB > 0.0f) {
                B[i] = axisB / lenB;
                b[i] = otherHalfExtentsLocal[i] * lenB;
            }
            else {
                B[i] = glm::vec3(0.0f);
                b[i] = 0.0f;
            }
        }

        float R[3][3];
        float AbsR[3][3];
        const float epsilon = 1e-6f;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                R[i][j] = glm::dot(A[i], B[j]);
                AbsR[i][j] = std::abs(R[i][j]) + epsilon;
            }
        }

        glm::vec3 tWorld = otherCenterWorld - thisCenterWorld;
        float t[3] = {
            glm::dot(tWorld, A[0]),
            glm::dot(tWorld, A[1]),
            glm::dot(tWorld, A[2])
        };

        float ra, rb;

        // Test assi A0, A1, A2
        for (int i = 0; i < 3; i++) {
            ra = a[i];
            rb = b[0] * AbsR[i][0] + b[1] * AbsR[i][1] + b[2] * AbsR[i][2];
            if (std::abs(t[i]) > ra + rb) return false;
        }

        // Test assi B0, B1, B2
        for (int j = 0; j < 3; j++) {
            ra = a[0] * AbsR[0][j] + a[1] * AbsR[1][j] + a[2] * AbsR[2][j];
            rb = b[j];
            float tProj = std::abs(t[0] * R[0][j] + t[1] * R[1][j] + t[2] * R[2][j]);
            if (tProj > ra + rb) return false;
        }

        // Test prodotti vettoriali A[i] x B[j]
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                ra = a[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + a[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
                rb = b[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + b[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
                float tProj = std::abs(t[(i + 2) % 3] * R[(i + 1) % 3][j] - t[(i + 1) % 3] * R[(i + 2) % 3][j]);
                if (tProj > ra + rb) return false;
            }
        }

        return true;
    }

    // BoxCollider::collideWith(CircleCollider*)
    // Placeholder: qui si dovrebbe implementare il test box-circle. Per ora ritorna false.
    bool BoxCollider::collideWith(CircleCollider* other) {
        return other->collideWith(this);
    }

    bool BoxCollider::colliteWith(Ray ray)
    {
        // =========================
        // Vecchia implementazione (AABB world-space) - commentata come richiesto
        // =========================
        // Trasform transform = obj->getAbsoluteTransform();
        // glm::mat4 model = transform.getModelMatrix();
        //
        // glm::vec3 corners[8] = {
        //     {minPoint.x, minPoint.y, minPoint.z}, {minPoint.x, minPoint.y, maxPoint.z},
        //     {minPoint.x, maxPoint.y, minPoint.z}, {minPoint.x, maxPoint.y, maxPoint.z},
        //     {maxPoint.x, minPoint.y, minPoint.z}, {maxPoint.x, minPoint.y, maxPoint.z},
        //     {maxPoint.x, maxPoint.y, minPoint.z}, {maxPoint.x, maxPoint.y, maxPoint.z}
        // };
        //
        // glm::vec3 worldMin(+std::numeric_limits<float>::infinity());
        // glm::vec3 worldMax(-std::numeric_limits<float>::infinity());
        // for (int i = 0; i < 8; i++) {
        //     glm::vec3 w = model * glm::vec4(corners[i], 1.0f);
        //     worldMin = glm::min(worldMin, w);
        //     worldMax = glm::max(worldMax, w);
        // }
        //
        // const float eps = 1e-6f;
        // float tMin = 0.0f;
        // float tMax = std::numeric_limits<float>::infinity();
        //
        // for (int axis = 0; axis < 3; axis++) {
        //     const float origin = ray.origin[axis];
        //     const float dir = ray.direction[axis];
        //     const float minA = worldMin[axis];
        //     const float maxA = worldMax[axis];
        //
        //     if (std::abs(dir) < eps) {
        //         if (origin < minA || origin > maxA)
        //             return false;
        //         continue;
        //     }
        //
        //     float t1 = (minA - origin) / dir;
        //     float t2 = (maxA - origin) / dir;
        //     if (t1 > t2) std::swap(t1, t2);
        //
        //     tMin = std::max(tMin, t1);
        //     tMax = std::min(tMax, t2);
        //
        //     if (tMin > tMax)
        //         return false;
        // }
        //
        // return tMax >= 0.0f;

        // =========================
        // Nuova implementazione: Ray vs OBB
        // Trasformo il raggio in local-space del box e faccio slab test su min/max locali.
        // =========================
        Trasform transform = obj->getAbsoluteTransform();
        glm::mat4 model = transform.getModelMatrix();
        glm::mat4 invModel = glm::inverse(model);

        glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(ray.origin, 1.0f));
        glm::vec3 localDirection = glm::normalize(glm::vec3(invModel * glm::vec4(ray.direction, 0.0f)));

        const float eps = 1e-6f;
        float tMin = 0.0f;
        float tMax = std::numeric_limits<float>::infinity();

        for (int axis = 0; axis < 3; axis++) {
            const float origin = localOrigin[axis];
            const float dir = localDirection[axis];
            const float minA = minPoint[axis];
            const float maxA = maxPoint[axis];

            if (std::abs(dir) < eps) {
                if (origin < minA || origin > maxA)
                    return false;
                continue;
            }

            float t1 = (minA - origin) / dir;
            float t2 = (maxA - origin) / dir;
            if (t1 > t2) std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax)
                return false;
        }

        return tMax >= 0.0f;
    }

    // CircleCollider::hasCollided
    // Double-dispatch: invoca collideWith su `other` passando `this`.
    bool CircleCollider::hasCollided(Collider* other) {
        return other->collideWith(this);
    }

    bool CircleCollider::colliteWith(Ray ray)
    {
        Trasform transform = obj->getAbsoluteTransform();
        glm::vec3 center = transform.position;

        float scaleFactor = std::max({ std::abs(transform.scale.x), std::abs(transform.scale.y), std::abs(transform.scale.z) });
        float radiusWorld = radius * scaleFactor;

        glm::vec3 m = ray.origin - center;
        float b = glm::dot(m, ray.direction);
        float c = glm::dot(m, m) - radiusWorld * radiusWorld;

        if (c > 0.0f && b > 0.0f)
            return false;

        float discriminant = b * b - c;
        if (discriminant < 0.0f)
            return false;

        return true;
	}


    // CircleCollider::update
    // Metodo chiamato ogni frame per aggiornare lo stato del collider.
    // Attualmente vuoto perché il collider basato su raggio non mantiene uno stato dinamico proprio.
    void CircleCollider::update(float dt) {

    }

    // CircleCollider::start
    // Inizializza il raggio del collider (attualmente non fa nulla in quanto
    // il raggio viene passato al costruttore).
    void CircleCollider::start() {
		if (obj->hasComponent<RenderMeshComponent>()) {
            auto& renderMeshComp = obj->getComponent<RenderMeshComponent>();
            if (renderMeshComp.mesh) {
                if (!renderMeshComp.mesh->position.empty()) {
                    // Calcola raggio come distanza massima dal centro ai vertici
                    glm::vec3 center(0.0f);
                    for (const auto& pos : renderMeshComp.mesh->position) {
                        center += pos;
                    }
                    center /= static_cast<float>(renderMeshComp.mesh->position.size());
                    float maxDistSq = 0.0f;
                    for (const auto& pos : renderMeshComp.mesh->position) {
                        float distSq = glm::dot(pos - center, pos - center);
                        if (distSq > maxDistSq) {
                            maxDistSq = distSq;
                        }
                    }
                    radius = std::sqrt(maxDistSq);
                }
            }
        }

    }

    // CircleCollider::collideWith(BoxCollider*)
    // Placeholder: implementare collisione cerchio-box (invocare algoritmo con punto pi? vicino).
    bool CircleCollider::collideWith(BoxCollider* other) {
        Trasform circleTransform = obj->getAbsoluteTransform();
        glm::vec3 circleCenterWorld = circleTransform.position;
        float circleScale = std::max({ std::abs(circleTransform.scale.x), std::abs(circleTransform.scale.y), std::abs(circleTransform.scale.z) });
        float radiusWorld = radius * circleScale;

        Trasform boxTransform = other->obj->getAbsoluteTransform();
        glm::mat4 boxModel = boxTransform.getModelMatrix();
        glm::mat4 invBoxModel = glm::inverse(boxModel);

        glm::vec3 circleCenterLocal = glm::vec3(invBoxModel * glm::vec4(circleCenterWorld, 1.0f));

        glm::vec3 closestPointLocal;
        closestPointLocal.x = std::max(other->getMinPoint().x, std::min(circleCenterLocal.x, other->getMaxPoint().x));
        closestPointLocal.y = std::max(other->getMinPoint().y, std::min(circleCenterLocal.y, other->getMaxPoint().y));
        closestPointLocal.z = std::max(other->getMinPoint().z, std::min(circleCenterLocal.z, other->getMaxPoint().z));

        glm::vec3 delta = circleCenterLocal - closestPointLocal;
        float distSq = glm::dot(delta, delta);

        return distSq <= (radiusWorld * radiusWorld);
    }

    // CircleCollider::collideWith(CircleCollider*)
    // Placeholder: implementare collisione cerchio-cerchio (distance centers <= sum radii).
    bool CircleCollider::collideWith(CircleCollider* other) {
        Trasform thisTransform = obj->getAbsoluteTransform();
        Trasform otherTransform = other->obj->getAbsoluteTransform();

        glm::vec3 thisCenter = thisTransform.position;
        glm::vec3 otherCenter = otherTransform.position;

        float thisScale = std::max({ std::abs(thisTransform.scale.x), std::abs(thisTransform.scale.y), std::abs(thisTransform.scale.z) });
        float otherScale = std::max({ std::abs(otherTransform.scale.x), std::abs(otherTransform.scale.y), std::abs(otherTransform.scale.z) });

        float r1 = radius * thisScale;
        float r2 = other->radius * otherScale;

        glm::vec3 delta = thisCenter - otherCenter;
        float distSq = glm::dot(delta, delta);
        float radii = r1 + r2;

        return distSq <= (radii * radii);
    }
} // namespace OnYuu