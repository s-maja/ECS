#ifndef _SYSTEM_MANAGER_H_
#define _SYSTEM_MANAGER_H_

#include "System.h"
#include "ISystem.h"
#include <vector>
#include <unordered_map>
#include <typeinfo>
using namespace std;

namespace ECS {

	class SystemManager {
	private:
		vector<ISystem*> systemWorkOrder;
		unordered_map<int, ISystem*> systems;
		vector<std::vector<bool>> systemDependencyMatrix;
	public:
		SystemManager();
		~SystemManager();

		template<class T>
		ISystem* AddSystem();

		template<class System_, class Dependency_>
		void AddSystemDependency(System_ target, Dependency_ dependency);

		template<class T>
		T* GetSystem() const;

		template<class T>
		void EnableSystem();

		template<class T>
		void DisableSystem();

		void Update(float_t dt_ms); 

		void UpdateSystemWorkOrder();

	};

}

#endif // !_SYSTEM_MANAGER_H_




