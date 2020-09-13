#pragma once

#include "ECS.h"
using namespace ECS;

class EnamySystem : public System<EnamySystem> {
private:

public:
	virtual void OnCreate() override {}
	virtual void OnDestroy() override {}
	virtual void OnUpdate() override {}


};