#include "API.h"
#include "Engine.h"
using namespace ECS;

Engine* ECS::ECS_Engine = nullptr; 
float ECS::timeStep = 1.0f / 60.0f;

void ECS::Initialize()
{
	if (ECS_Engine == nullptr)
		ECS_Engine = new Engine();
}

void ECS::Terminate()
{
	if (ECS_Engine != nullptr)
	{
		delete ECS_Engine;
		ECS_Engine = nullptr;
	}
}