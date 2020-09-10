#include "Engine.h"
using namespace ECS;


ECS::Engine::Engine()
{
	engineTime = new Timer();
	systemManager = new SystemManager();
	entityManager = new EntityManager();
}

ECS::Engine::~Engine()
{
	delete entityManager;
	entityManager = nullptr;

	delete systemManager;
	systemManager = nullptr;
}

void ECS::Engine::Update(float tick_ms)
{
	// Advance engine time
	engineTime->Tick(tick_ms);

	// Update all running systems
	systemManager->Update(tick_ms);

	// Finalize pending destroyed entities
	//entityManager->RemoveDestroyedEntities(); :TODO
}

