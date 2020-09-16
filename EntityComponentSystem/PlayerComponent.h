#pragma once
#include "ECS.h"
using namespace ECS;

class PlayerComponent : public IComponent {
public:
	int speed;
};