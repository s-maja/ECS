#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "EntityManager.h"

namespace ECS {

	class Engine {
	private:
		EntityManager* ECS_EntityManager;
	public:

		EntityManager* GetEntityManager() { return ECS_EntityManager; }

		void Update(float tick_ms);
	};

}

#endif // !_ENGINE_H_
