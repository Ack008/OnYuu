#pragma once
#include "Core/Engine.h"
#include <iostream>
#include "scripts/Controller.h"
class CameraScript : public Component {
private:
	float cameraSpeed = 5.0f; // Velocità di movimento della camera
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    Camera* camera = nullptr;
	std::vector<GameObject> meshObjects;
    Controller& controller;
	bool firstClick = true;
	public:
	CameraScript() = default;
	CameraScript(Controller& controller) : controller(controller){
        
    }
	void start() override {
        camera = &obj->getComponent<Perspective>();
        auto& t = obj->getComponent<Trasform>();
        // Initialize target and direction now that `obj` is available
        target = t.position + glm::vec3(0.0f, 0.0f, -1.0f);
        direction = glm::normalize(target - t.position);
      
	}
 
    void removeObjectTooDistant(float maxDistance) {
        Trasform& transform = obj->getComponent<Trasform>();
        meshObjects.erase(std::remove_if(meshObjects.begin(), meshObjects.end(),
            [&](GameObject& go) {
                Trasform& goTransform = go.getComponent<Trasform>();
                float distance = glm::length(transform.position - goTransform.position);
                return distance > maxDistance;
            }), meshObjects.end());
	}
	//funzione per vedere se un punto è dentro un AABB degli oggetti nella scena
	bool isPointCollidingWithObjects(const glm::vec3& point) {
        // costruisco la AABB del punto
		glm::vec3 pointMin = point - glm::vec3(1.f); // piccola epsilon per evitare problemi di precisione
		glm::vec3 pointMax = point + glm::vec3(1.f);
        for (GameObject& go : meshObjects) {
            if (go.hasComponent<BoxCollider>()) {
                BoxCollider& collider = go.getComponent<BoxCollider>();
				Trasform transform = go.getAbsoluteTransform();
				glm::mat4 modelMatrix = transform.getModelMatrix();
				// Trasformo i punti min e max dell'AABB nello spazio del mondo
				glm::vec3 worldMin = glm::vec3(modelMatrix * glm::vec4(collider.getMinPoint(), 1.0f));
				glm::vec3 worldMax = glm::vec3(modelMatrix * glm::vec4(collider.getMaxPoint(), 1.0f));
                // Controllo la collisione AABB vs AABB
                if (pointMax.x >= worldMin.x && pointMin.x <= worldMax.x &&
                    pointMax.y >= worldMin.y && pointMin.y <= worldMax.y &&
                    pointMax.z >= worldMin.z && pointMin.z <= worldMax.z) {
                    return true; // Il punto collide con questa AABB
				}
            }
        }
        return false; // Il punto non collide con nessun AABB
    }

	void update(float deltaTime) override {
		//ottengo tutti gli oggetti con mesh per le collisioni
		meshObjects = obj->getGameObjectsByComponent<RenderMeshComponent>();
		//prendo solo gli oggetti con distanza minore di 5 unità
		removeObjectTooDistant(15.0f);
		Trasform& transform = obj->getComponent<Trasform>();
		float speed = 5.0f;
		if (Input::isKeyPressed(KeyCode::W)) {
			moveCameraForward(deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::S)) {
			moveCameraBack(deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::A)) {
			moveCameraLeft(deltaTime);
		}
		if (Input::isKeyPressed(KeyCode::D)) {
            moveCameraRight(deltaTime);
        }
        if (Input::isKeyPressed(KeyCode::E)) {
            moveCameraUp(deltaTime);
        }
        if (Input::isKeyPressed(KeyCode::Q)) {
			moveCameraDown(deltaTime);
		}
	    if(Input::isMouseButtonPressed(2)) { // Right mouse button
			mousePicking();
		}    

        if (Input::isMouseButtonPressed(1)) {
            cameraRotating();
        }
        else {
			Input::setMouseState(MouseState::VISIBLE);
        }

        camera->setPosition(obj->getComponent<Trasform>().position);
        camera->setTarget(target);
        camera->setDirection(direction);
	}

    void cameraRotating()
    {
        if (firstClick) {
            uint32_t width = Application::getInstance()->getWindow()->getWidth();
            uint32_t height = Application::getInstance()->getWindow()->getHeight();
			Input::setMouseState(MouseState::HIDDEN);
            firstClick = false;
        }
        glm::vec3 orientation = camera->getTarget() - camera->getPosition();
        uint32_t width = Application::getInstance()->getWindow()->getWidth();
        uint32_t height = Application::getInstance()->getWindow()->getHeight();
        double x, y;
		Input::getMouseWindowPos(x, y);
		float sensitivity = 10.f;
        float rotX = sensitivity * (float)(y - (height / 2)) / height;
        float rotY = sensitivity * (float)(x - (width / 2)) / width;

        // Calculates upcoming vertical change in the Orientation
        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, camera->getUpVector())));

        // Decides whether or not the next vertical Orientation is legal or not
        if (abs(glm::angle(newOrientation, camera->getUpVector()) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            orientation = newOrientation;
        }

        // Rotates the Orientation left and right
        orientation = glm::rotate(orientation, glm::radians(-rotY), camera->getUpVector());
		
        // Aggiorno la direzione e il target della camera
        direction = glm::normalize(orientation);
		target = camera->getPosition() + direction;
		// Riporta il mouse al centro della finestra
		Input::setMousePosition(width / 2.0, height / 2.0);


    }
    void mousePicking() {
        double mouseX, mouseY;
        Input::getMousePosition(mouseX, mouseY);
        // converti le coordinate da cube space a world space
        Ray mouseRay = fromScreenToWorldPoint(glm::vec2(mouseX, mouseY));
		// Controlla l'intersezione del raggio con gli oggetti della scena
		bool intersectionFound = false;
        for (GameObject& go : meshObjects) {
			Trasform transform = go.getAbsoluteTransform();
            glm::vec3 sphereCenter = transform.position;
			float sphereRadius = glm::min(transform.scale.x, glm::min(transform.scale.y, transform.scale.z)) * 0.5f; // raggio approssimato
			if (go.hasComponent<BoxCollider>()) {
                BoxCollider& collider = go.getComponent<BoxCollider>();
                // Calcolo il raggio massimo dell'AABB come raggio della sfera
                glm::vec3 aabbSize = collider.getMaxPoint() - collider.getMinPoint();
                sphereRadius = glm::length(aabbSize) * 0.5f * glm::max(transform.scale.x, glm::max(transform.scale.y, transform.scale.z));
            }
            float t;
            if (raySphereIntersect(mouseRay, sphereCenter, sphereRadius, t)) {
                // Intersezione trovata
                controller.setSelectedObject(go);
				intersectionFound = true;
                break; // Se vuoi selezionare solo il primo oggetto intersecato
            }
		}
        if (!intersectionFound) {
			controller.setSelectedObject(GameObject()); // Deseleziona l'oggetto se nessuna intersezione
		}
	}

    bool raySphereIntersect(const Ray& ray, const glm::vec3& sphereCenter, float sphereRadius, float& t) {
        glm::vec3 oc = ray.origin - sphereCenter;
        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            return false; // Nessuna intersezione
        } else {
            t = (-b - sqrt(discriminant)) / (2.0f * a);
            return true; // Intersezione trovata
        }
	}

    Ray fromScreenToWorldPoint(glm::vec2 screenCoord) 
    {
		glm::mat4 viewMatrix = camera->getViewMatrix();
		glm::mat4 projectionMatrix = camera->getProjectionMatrix();
        
        glm::vec4 ray_clip = glm::vec4(screenCoord.x, screenCoord.y, -1.0f, 1.0f);
		glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f); // direzione

		glm::vec4 ray_world = glm::inverse(viewMatrix) * ray_eye;
        glm::vec3 rayDirection = glm::normalize(glm::vec3(ray_world));

        Ray r = { camera->getPosition() , rayDirection};
 
		return r;

    }
    void moveCameraForward(float deltatime)
    {
		Trasform& transform = obj->getComponent<Trasform>();
        // Calcolo la direzione corrente della camera come vettore dal position al target
		direction = target - transform.position;

        // Aggiorno la posizione della camera spostandola in avanti lungo la direzione
        // moltiplicando per velocità e delta time per movimento fluido e indipendente dal frame rate
		if (isPointCollidingWithObjects(transform.position + direction * cameraSpeed * deltatime)) {
            std::cout << "Collision detected, cannot move forward!" << std::endl;
            return; // Esci dalla funzione senza muovere la camera
        }
        transform.position += direction * cameraSpeed * deltatime;

        // Aggiorno il target per mantenere la direzione invariata rispetto alla nuova posizione
        target = transform.position + direction;
    }

    void moveCameraBack(float deltatime)
    {
        Trasform& transform = obj->getComponent<Trasform>();
        // Calcolo la direzione corrente della camera come vettore dal position al target
        direction = target - transform.position;

        // Aggiorno la posizione della camera spostandola in avanti lungo la direzione
        // moltiplicando per velocità e delta time per movimento fluido e indipendente dal frame rate
        if (isPointCollidingWithObjects(transform.position - direction * cameraSpeed * deltatime)) {
            std::cout << "Collision detected, cannot move forward!" << std::endl;
            return; // Esci dalla funzione senza muovere la camera
        }
        transform.position -= direction * cameraSpeed * deltatime;

        // Aggiorno il target per mantenere la direzione invariata rispetto alla nuova posizione
        target = transform.position + direction;
    }

    void moveCameraLeft(float deltatime)
    {
        // Calcolo la direzione della camera
		Trasform& transform = obj->getComponent<Trasform>();
        direction = target - transform.position;

        // Calcolo un vettore perpendicolare alla direzione e al vettore 'up' della camera (slide laterale)
        glm::vec3 slide_vector = cross(direction, glm::vec3(camera->getUpVector())) * cameraSpeed * deltatime;

		if (isPointCollidingWithObjects(transform.position - slide_vector)) {
            std::cout << "Collision detected, cannot move left!" << std::endl;
            return; // Esci dalla funzione senza muovere la camera
        }

        // Traslo la posizione della camera a sinistra lungo il vettore calcolato
        transform.position -= slide_vector;

        // Spostamento equivalente del target per mantenere la direzione invariata
        target -= slide_vector;
    }

    void moveCameraRight(float deltatime)
    {
		Trasform& transform = obj->getComponent<Trasform>();
        direction = target - transform.position;

        // Slide laterale a destra
        glm::vec3 slide_vector = cross(direction, glm::vec3(camera->getUpVector())) * cameraSpeed * deltatime;
        
        if (isPointCollidingWithObjects(transform.position + slide_vector)) {
            std::cout << "Collision detected, cannot move left!" << std::endl;
            return; // Esci dalla funzione senza muovere la camera
        }
		// Traslo la posizione della camera a destra
        transform.position += slide_vector;
        target += slide_vector; // Mantiene la direzione coerente
    }

    void moveCameraUp(float deltatime)
    {
		Trasform& transform = obj->getComponent<Trasform>();
        direction = target - transform.position;

        // Calcolo un vettore perpendicolare alla direzione e al vettore laterale (slide_vector normalizzato)
        glm::vec3 slide_vector = normalize(cross(direction, camera->getUpVector()));

        // Calcolo la direzione verticale come prodotto vettoriale inverso
        glm::vec3 upDirection = cross(direction, slide_vector) * cameraSpeed * deltatime;

		if (isPointCollidingWithObjects(transform.position - upDirection)) {
            std::cout << "Collision detected, cannot move up!" << std::endl;
            return; // Esci dalla funzione senza muovere la camera
        }

        // Sposto la camera verso l'alto lungo la direzione verticale calcolata
        transform.position -= upDirection;

        // Sposto anche il target per mantenere la direzione di vista invariata
        target -= upDirection;
    }

    void moveCameraDown(float deltatime)
    {
		Trasform& transform = obj->getComponent<Trasform>();
        direction = target - transform.position;
        glm::vec3 slide_vector = normalize(cross(direction, glm::vec3(camera->getUpVector())));

        glm::vec3 upDirection = cross(direction, slide_vector) * cameraSpeed * deltatime;

        if (isPointCollidingWithObjects(transform.position + upDirection)) {
            std::cout << "Collision detected, cannot move down!" << std::endl;
            return; // Esci dalla funzione senza muovere la camera
		}
        // Sposto la camera verso il basso
        transform.position += upDirection;

        // Aggiorno il target per mantenere la direzione coerente
        target += upDirection;
    }

};