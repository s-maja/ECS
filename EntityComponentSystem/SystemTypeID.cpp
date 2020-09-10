#include "SystemTypeID.h"
#include <cstdint>

namespace ECS
{
	class ISystem;
	uint64_t SystemTypeID<ISystem>::s_count = 0u;
	template class SystemTypeID<ISystem>;

}
