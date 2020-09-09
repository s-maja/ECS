#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <cstdint>

namespace ECS {

	// Entity struct contains an id used to access entity data 
	// Note that you generally do not use the id, but instead pass the Entity struct to the relevant API method

	class Entity {
	private:
		uint32_t id;
	public:

		uint32_t GetID() {
			return id;
		}

		void SetID(uint32_t i) {
			id = i;
		}
	};

}


#endif // !_ENTITY_H_
