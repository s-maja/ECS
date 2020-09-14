#pragma once

#include "ECS.h"
using namespace ECS;

class EnamySystem : public System<EnamySystem> {
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