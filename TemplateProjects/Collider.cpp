#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshComponent.h"

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
                for (const auto& pos : renderMeshComp.mesh->position) {
                    if (pos.x < minX) minX = pos.x;
                    if (pos.x > maxX) maxX = pos.x;
                    if (pos.y < minY) minY = pos.y;
                    if (pos.y > maxY) maxY = pos.y;
                }
                width = maxX - minX;
                height = maxY - minY;
                maxPoint = glm::vec3(maxX, maxY, 0.0f);
                minPoint = glm::vec3(minX, minY, 0.0f);
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
    Trasform thisTransform = obj->getComponent<Trasform>();
    glm::vec3 thisMin = thisTransform.getModelMatrix() * glm::vec4(this->getMinPoint(),1);
    glm::vec3 thisMax = thisTransform.getModelMatrix() * glm::vec4(this->getMaxPoint(),1);
    Trasform otherTransform = other->obj->getComponent<Trasform>();
    glm::vec3 otherMin = otherTransform.getModelMatrix() * glm::vec4(other->getMinPoint(),1);
    glm::vec3 otherMax = otherTransform.getModelMatrix() * glm::vec4(other->getMaxPoint(),1);
    // Verifica sovrapposizione sugli assi X e Y
    bool overlapX = thisMin.x <= otherMax.x && thisMax.x >= otherMin.x;
    bool overlapY = thisMin.y <= otherMax.y && thisMax.y >= otherMin.y;
    return overlapX && overlapY;
}

// BoxCollider::collideWith(CircleCollider*)
// Placeholder: qui si dovrebbe implementare il test box-circle. Per ora ritorna false.
bool BoxCollider::collideWith(CircleCollider* other) {
    // TODO: implementare test box-circle (closest point on AABB -> distance vs radius)
    return false; // Placeholder return value
}

// CircleCollider::hasCollided
// Double-dispatch: invoca collideWith su `other` passando `this`.
bool CircleCollider::hasCollided(Collider* other) {
    return other->collideWith(this);
}

// CircleCollider::collideWith(BoxCollider*)
// Placeholder: implementare collisione cerchio-box (invocare algoritmo con punto più vicino).
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
