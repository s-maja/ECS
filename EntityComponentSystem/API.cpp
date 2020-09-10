#include "API.h"
#include "Engine.h"
using namespace ECS;

Engine* ECS::ECS_Engine = nullptr; 

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

	// check for memory leaks
}