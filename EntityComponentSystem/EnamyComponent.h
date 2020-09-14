#pragma once
#include "ECS.h"
using namespace ECS;

class EnamyComponent : public IComponent {
public:
	float x1;
	float y1;
	float x2;
	float y2;
	float patrolTime;
	float tempPatrolTime;
};