#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "EntityManager.h"
#include "Timer.h"
#include "SystemManager.h"

namespace ECS {

	class Engine {
	private:
		Timer* engineTime;
		EntityManager* entityManager;
		SystemManager* systemManager;
	public:

		Engine();
		~Engine();

		EntityManager* GetEntityManager() { return entityManager; }
		
		SystemManager* GetSystemManager() { return systemManager; }

		void Update(float tick_ms);
	};

}

#endif // !_ENGINE_H_
