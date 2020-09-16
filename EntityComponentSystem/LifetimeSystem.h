#pragma once

#include "ECS.h"
using namespace ECS;

class LifetimeSystem : public System<LifetimeSystem> {
private:
	EntityManager* entityManager;
	int numberOfEntites;

public:
	void OnUpdate() override;

};