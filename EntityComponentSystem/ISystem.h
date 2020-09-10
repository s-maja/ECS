#ifndef _ISYSTEM_H_
#define _ISYSTEM_H_

namespace ECS {

	class ISystem {
	private:
		bool enabled;
	public:
		ISystem();
		~ISystem();

		virtual inline const size_t GetSystemTypeID() const = 0;


		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate() = 0;

		bool IsEnabled() {
			return enabled;
		}

		void Enable() {
			enabled = true;
		}

		void Disable() {
			enabled = false;
		}
	};

}

#endif // !_ISYSTEM_H_
