#include "Collider.h"
#include "Core/Model/GameObject.h"
#include "Core/Model/Components/Transform.h"
#include "Core/Model/Components/MeshComponent.h"
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
        // Trasformazione assoluta di questo collider
        Trasform thisTransform = obj->getAbsoluteTransform();
        glm::mat4 thisMat = thisTransform.getModelMatrix();

        // Trasformazione assoluta dell'altro collider
        Trasform otherTransform = other->obj->getAbsoluteTransform();
        glm::mat4 otherMat = otherTransform.getModelMatrix();

        // Calcola i 8 vertici del box locale di 'this'
        glm::vec3 thisCorners[8] = {
            {minPoint.x, minPoint.y, minPoint.z}, {minPoint.x, minPoint.y, maxPoint.z},
            {minPoint.x, maxPoint.y, minPoint.z}, {minPoint.x, maxPoint.y, maxPoint.z},
            {maxPoint.x, minPoint.y, minPoint.z}, {maxPoint.x, minPoint.y, maxPoint.z},
            {maxPoint.x, maxPoint.y, minPoint.z}, {maxPoint.x, maxPoint.y, maxPoint.z}
        };

        // Calcola i 8 vertici del box locale di 'other'
        glm::vec3 otherCorners[8] = {
            {other->minPoint.x, other->minPoint.y, other->minPoint.z}, {other->minPoint.x, other->minPoint.y, other->maxPoint.z},
            {other->minPoint.x, other->maxPoint.y, other->minPoint.z}, {other->minPoint.x, other->maxPoint.y, other->maxPoint.z},
            {other->maxPoint.x, other->minPoint.y, other->minPoint.z}, {other->maxPoint.x, other->minPoint.y, other->maxPoint.z},
            {other->maxPoint.x, other->maxPoint.y, other->minPoint.z}, {other->maxPoint.x, other->maxPoint.y, other->maxPoint.z}
        };

        // Trova min/max in world space di 'this'
        glm::vec3 thisWorldMin(+INFINITY);
        glm::vec3 thisWorldMax(-INFINITY);
        for (int i = 0; i < 8; i++) {
            glm::vec3 w = thisMat * glm::vec4(thisCorners[i], 1.0f);
            thisWorldMin = glm::min(thisWorldMin, w);
            thisWorldMax = glm::max(thisWorldMax, w);
        }

        // Trova min/max in world space di 'other'
        glm::vec3 otherWorldMin(+INFINITY);
        glm::vec3 otherWorldMax(-INFINITY);
        for (int i = 0; i < 8; i++) {
            glm::vec3 w = otherMat * glm::vec4(otherCorners[i], 1.0f);
            otherWorldMin = glm::min(otherWorldMin, w);
            otherWorldMax = glm::max(otherWorldMax, w);
        }

        // Test AABB classico sugli assi X, Y e Z
        bool overlapX = thisWorldMin.x <= otherWorldMax.x && thisWorldMax.x >= otherWorldMin.x;
        bool overlapY = thisWorldMin.y <= otherWorldMax.y && thisWorldMax.y >= otherWorldMin.y;
        bool overlapZ = thisWorldMin.z <= otherWorldMax.z && thisWorldMax.z >= otherWorldMin.z;

        return overlapX && overlapY && overlapZ;
    }

    // BoxCollider::collideWith(CircleCollider*)
    // Placeholder: qui si dovrebbe implementare il test box-circle. Per ora ritorna false.
    bool BoxCollider::collideWith(CircleCollider* other) {
        // TODO: implementare test box-circle (closest point on AABB -> distance vs radius)
        return false; // Placeholder return value
    }

    bool BoxCollider::colliteWith(Ray ray)
    {
        return false;
    }

    // CircleCollider::hasCollided
    // Double-dispatch: invoca collideWith su `other` passando `this`.
    bool CircleCollider::hasCollided(Collider* other) {
        return other->collideWith(this);
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

    }

    // CircleCollider::collideWith(BoxCollider*)
    // Placeholder: implementare collisione cerchio-box (invocare algoritmo con punto pi? vicino).
    bool CircleCollider::collideWith(BoxCollider* other) {
        // TODO: implementare test circle-box
        return false; // Placeholder return value
    }

    // CircleCollider::collideWith(CircleCollider*)
    // Placeholder: implementare collisione cerchio-cerchio (distance centers <= sum radii).
    bool CircleCollider::collideWith(CircleCollider* other) {
        // TODO: implementare test circle-circle
        return false; // Placeholder return value
    }
} // namespace OnYuu