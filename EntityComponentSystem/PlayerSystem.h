#pragma once

#include "ECS.h"
using namespace ECS;

class PlayerSystem : public System<PlayerSystem> {
private:
	EntityManager* entityManager;
	int numberOfEntites;

	ComponentType movementComponent;
	ComponentType playerComponent;
public:
	void OnCreate() override;
	void OnUpdate() override;
private:
	int getch_noblock();
};