#ifndef _COMPONENT_TYPE_
#define _COMPONENT_TYPE_

#include <iostream>
#include <typeinfo>
#include "IComponent.h"
using namespace std;

namespace ECS {

	class ComponentType {
	private:
		static int INDEX;
		int index;
		int size;
		const type_info* type;
	public:
		ComponentType() {}

		ComponentType(const type_info& type, int size) {
			this->size = size;
			index = INDEX++;
			this->type = &type;
		}

		ComponentType& operator=(const ComponentType& compType) {
			if (this != &compType) {
				index = compType.index;
				size = compType.size;
				type = compType.type;
			}
			return *this;
		}

		int GetIndex() {
			return index;
		}

		int GetSize() {
			return size;
		}

		const type_info& GetTypeInfo() {
			return *type;
		}
		
	};

	

};
#endif // !_COMPONENT_TYPE_
