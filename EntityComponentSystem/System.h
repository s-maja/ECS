#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "SystemManager.h"
#include "ISystem.h"
#include "TypeID.h"

namespace ECS {

	template<class T>
	class System : public ISystem {
		friend class SystemManager;

	public:
		static const size_t STATIC_SYSTEM_TYPE_ID;

	private:
		SystemManager* systemManagerInstance;

	public:
		System()  {
		}
		virtual ~System(){
			OnDestroy();
		}

		virtual inline const size_t GetSystemTypeID() const
		{
			return STATIC_SYSTEM_TYPE_ID;
		}



		void SetSystemManagerInstance(SystemManager* manager) {
			systemManagerInstance = manager;
		}

		template<class Dependencie>
		void AddDependencies(Dependencie dependencie){
			this->systemManagerInstance->AddSystemDependency(this, dependencie);
		}

		virtual void OnCreate() override {}
		virtual void OnDestroy() override {}
		virtual void OnUpdate() override {}

	}; 


	template<class T>
	const size_t System<T>::STATIC_SYSTEM_TYPE_ID = TypeID<ISystem>::Get<T>();
	
}

#endif // !_SYSTEM_H_
