#include "TypeID.h"
#include <cstdint>

namespace ECS
{
	class ISystem;
	uint64_t TypeID<ISystem>::s_count = 0u;
	template class TypeID<ISystem>;

}
