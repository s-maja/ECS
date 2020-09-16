#pragma once

#include "ECS.h"
using namespace ECS;

class EnemySystem : public System<EnemySystem> {
private:
	EntityManager* entityManager;
	int numberOfEntites;

	ComponentType movementComponent;
	ComponentType enamyComponent;
	ComponentType lifeTimeComponent;
public:
	void OnCreate() override;
	void OnUpdate() override;


};