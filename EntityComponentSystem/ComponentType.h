#ifndef _COMPONENT_TYPE_
#define _COMPONENT_TYPE_

#include <iostream>
#include <typeinfo>
#include "IComponent.h"
using namespace std;

namespace ECS {

	class ComponentType  {
	public:
		int size;
		const type_info* type;
		static int INDEX;
		int index;
	public:
		ComponentType() {}

		ComponentType(const type_info& type, int size) {
			this->size = size;
			this->type = &type;
			this->index = INDEX++;
		}

		ComponentType& operator=(const ComponentType& compType) {
			if (this != &compType) {
				size = compType.size;
				type = compType.type;
				index = compType.index;
			}
			return *this;
		}
		int GetSize() {
			return size;
		}

		const type_info& GetTypeInfo() {
			return *type;
		}
		const int GetIndex() {
			return index;
		}
		
	};

}
#endif // !_COMPONENT_TYPE_
