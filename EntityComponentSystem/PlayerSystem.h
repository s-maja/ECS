#pragma once

#include "ECS.h"
using namespace ECS;

class PlayerSystem : public System<PlayerSystem> {
private:

public:
	virtual void OnCreate() override {}
	virtual void OnDestroy() override {}
	virtual void OnUpdate() override {}


};