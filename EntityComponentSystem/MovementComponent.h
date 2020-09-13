#pragma once

#include "ECS.h"
using namespace ECS;

class MovementComponent : public IComponent {
public:
	int speed;
	int x;
	int y;
};