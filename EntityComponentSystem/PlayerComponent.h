#pragma once
#include "ECS.h"
using namespace ECS;

class PlayerComponent : public IComponent {
public:
	float speed;
};