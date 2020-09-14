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

	template<class T>
	inline ISystem* SystemManager::AddSystem()
	{
		const uint64_t STID = T::STATIC_SYSTEM_TYPE_ID;

		// avoid multiple registrations of the same system
		auto it = this->systems.find(STID);
		if ((it != this->systems.end()) && (it->second != nullptr))
			return (T*)it->second;


		// create new system
		T* system = new T();
		if (system != nullptr)
		{
			system->SetSystemManagerInstance(this);
			this->systems[STID] = system;
			system->OnCreate();
		}

		// resize dependency matrix
		if (STID + 1 > this->systemDependencyMatrix.size())
		{
			this->systemDependencyMatrix.resize(STID + 1);
			for (int i = 0; i < this->systemDependencyMatrix.size(); ++i)
				this->systemDependencyMatrix[i].resize(STID + 1);
		}

		// add to work list
		this->systemWorkOrder.push_back(system);

	}

	template<class System_, class Dependency_>
	inline void ECS::SystemManager::AddSystemDependency(System_ target, Dependency_ dependency)
	{
		const int TARGET_ID = target->GetSystemTypeID();
		const int DEPEND_ID = dependency->GetSystemTypeID();

		if (this->systemDependencyMatrix[TARGET_ID][DEPEND_ID] != true)
		{
			this->systemDependencyMatrix[TARGET_ID][DEPEND_ID] = true;
		}

		this->UpdateSystemWorkOrder();
	}

	template<class T>
	inline T* ECS::SystemManager::GetSystem() const
	{
		auto it = this->systems.find(T::STATIC_SYSTEM_TYPE_ID);

		return it != this->systems.end() ? (T*)it->second : nullptr;
	}

	template<class T>
	inline void ECS::SystemManager::EnableSystem()
	{

		const size_t STID = T::STATIC_SYSTEM_TYPE_ID;

		// get system
		auto it = this->systems.find(STID);
		if (it != this->systems.end())
		{
			if (it->second->IsEnabled() == true)
				return;

			// enable system
			it->second->Enable();
		}
	}

	template<class T>
	inline void ECS::SystemManager::DisableSystem()
	{
		const size_t STID = T::STATIC_SYSTEM_TYPE_ID;

		// get system
		auto it = this->systems.find(STID);
		if (it != this->systems.end())
		{
			if (it->second->IsEnabled() == false)
				return;

			// enable system
			it->second->Disable();
		}
	}
}

#endif // !_SYSTEM_MANAGER_H_




