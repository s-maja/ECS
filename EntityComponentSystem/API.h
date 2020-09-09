#ifndef _API_H_
#define _API_H_

namespace ECS {
	class EntityManager;
	class SystemManager;

	class Engine;

	extern Engine* ECS_Engine;

	void Initialize();
	void Terminate();

}

#endif // !_API_H_



