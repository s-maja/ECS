#include "Chunk.h"
#include "Archetype.h"
#include "IComponent.h"
#include "ComponentType.h"
using namespace ECS;

unsigned long Chunk::SequenceNumber = 0;

void ECS::Chunk::Init(Archetype* arch)
{
	this->count = 0;
	this->capacity = arch->GetChunkCapacity();
	this->id = SequenceNumber++;
	this->archetype = arch;
}

bool ECS::Chunk::Has(ComponentType componentType)

{
	ComponentType* types = archetype->GetTypes();
	int count = archetype->GetTypesCount();
	bool has = false;
	for (int i = 0; i < count; i++) {
		if (types[i].GetTypeInfo() == componentType.GetTypeInfo()) {
			has = true;
			break;
		}
	}
	return has;
}
