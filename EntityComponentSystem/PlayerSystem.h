#pragma once

#include "ECS.h"
using namespace ECS;

class PlayerSystem : public System<PlayerSystem> {
private:

public:
	void OnCreate() override;
	void OnUpdate() override;
};