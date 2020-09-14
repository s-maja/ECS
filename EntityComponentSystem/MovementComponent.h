#pragma once

#include "ECS.h"
using namespace ECS;

class Movement : public IComponent {
public:
	float x;
	float y;
};