#pragma once
#include "Core/Engine.h"
#include "PlayerScripts.h"
#include <random>
#include "BallScript.h"
class ControllerScript : public Component 
{
	public:
	ControllerScript(PlayerScripts& player) : player(player) {}
	// Ereditato tramite Component
	virtual void start() override
	{
		//istanzio una riga e colonne di nemici all'inizio del gioco
		AssetManager& am = AssetManager::instance();
		am.getMaterial("colorUniformSupporterMaterial")->set("uColor", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		setGame();
	}
	void setGame()
	{
		for (int i = 0; i < numRows; ++i)
		{
			for (int j = 0; j < numCols; ++j)
			{
				Prefab* enemyPrefab = &enemy2;
				if (rand() % 2 == 0) {
					enemyPrefab = &enemy1;
				}
				GameObject enemy = obj->instantiatePrefabNow(enemyPrefab);
				Trasform& enemyTransform = enemy.getComponent<Trasform>();
				enemyTransform.position = glm::vec3(
					(j - numCols / 2) * spacingCol,
					(i + 2) * spacingRow,
					0.0f
				);
			}
		}
	}
	;
	virtual void update(float dt) override
	{
		srand((unsigned int)time);
		time += dt;
		
		count = obj->findGameObjectsByTag("Enemy").size();
		victory = (count == 0 );
		if (victory && Input::isKeyPressed(KeyCode::Enter))
		{
			//reset del gioco
			GameObject BALL = obj->findGameObjectByTag("Ball");
			BALL.getComponent<Trasform>().position = glm::vec3(0, 0, 0);
			RigidBody& rb = BALL.getComponent<RigidBody>();
			rb.setVelocity(glm::vec3(0, 0, 0));
			BALL.getComponent<BallScript>().reset();
			//istanzio una riga e colonne di nemici all'inizio del gioco
			setGame();
			victory = false;
		}
	};
	uint32_t getEnemyCount() const {
		return count;
	}
	bool& getVictory() {
		return victory;
	}
private:
	bool victory = false;
	uint32_t count = 0;
	float spacingRow = 6;
	float spacingCol = 6.5;
	int numRows = 6;
	int numCols = 15;
	float time = 0;
	PlayerScripts& player;
	Enemy1 enemy1;
	Enemy2 enemy2;

};