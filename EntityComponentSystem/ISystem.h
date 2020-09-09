#ifndef _ISYSTEM_H_
#define _ISYSTEM_H_

namespace ECS {

	// Implement SystemBase to create a systems in ECS.
	class ISystem {
	private:

	public:
		ISystem();
		~ISystem();

		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate() {}
	};

}

#endif // !_ISYSTEM_H_
