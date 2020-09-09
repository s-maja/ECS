#include "EntityManager.h"
#include <list>
using namespace ECS;

Chunk* ECS::EntityManager::GetChunkWithEmptySlots(Archetype archetype)
{
	return archetype.GetExistingChunkWithEmptySlots();
}

ECS::Archetype ECS::EntityManager::GetOrCreateArchetypeWithTypes(std::initializer_list<ComponentType> types)
{
	//check if archetype already exists
	bool sameTypes = true;
	for (int i = 0; i < countArchetypes; i++) {
		if (archetypes[i].GetTypesCount() != types.size()) continue;

		sameTypes = archetypes[i].Compare(types);

		if (sameTypes)
			return archetypes[i];
	}

	Archetype archetype = Archetype(types);

	//add to list of archetpes
	if (countArchetypes == 0) {
		this->archetypes = (Archetype*)malloc(sizeof(Archetype) * capacityInArchetypeArray);
	}else if ((countArchetypes % capacityInArchetypeArray) == 0) {
		Archetype* temp = (Archetype*)realloc(archetypes, sizeof(Archetype) * ((unsigned long long)capacityInArchetypeArray + countArchetypes));
		if (temp != NULL)
			this->archetypes = temp;
	}  
	
	this->archetypes[countArchetypes++] = archetype;

	return archetype;
}

ECS::Archetype ECS::EntityManager::GetOrCreateArchetypeWithTypes(std::list<ComponentType> types)
{
	//check if archetype already exists
	bool sameTypes = true;
	for (int i = 0; i < countArchetypes; i++) {
		if (archetypes[i].GetTypesCount() != types.size()) continue;

		sameTypes = archetypes[i].Compare(types);

		if (sameTypes)
			return archetypes[i];
	}

	Archetype archetype = Archetype(types);

	//add to list of archetpes
	if (countArchetypes == 0) {
		this->archetypes = (Archetype*)malloc(sizeof(Archetype) * capacityInArchetypeArray);
	}
	else if ((countArchetypes % capacityInArchetypeArray) == 0) {
		Archetype* temp = (Archetype*)realloc(archetypes, sizeof(Archetype) * ((unsigned long long)capacityInArchetypeArray + countArchetypes));
		if (temp != NULL)
			this->archetypes = temp;
	}

	this->archetypes[countArchetypes++] = archetype;

	return archetype;
}

EntityManager::EntityManager()
{
	//EntityComponentStore.cs EnsureCapacity
	archetypes = NULL;
	countArchetypes = 0;
	capacityInArchetypeArray = 10;
	entitiesCapacity = 1000;
	nextFreeEntityIndex = 0;

	int value = 1000;

	auto archetypeBytes = (value * sizeof(Archetype*) + 63) & ~63;
	auto chunkDataBytes = (value * sizeof(EntityInChunk) + 63) & ~63;
	auto bytesToAllocate = archetypeBytes + chunkDataBytes;

	uint8_t* bytes = (uint8_t*)malloc(bytesToAllocate);

	Archetype** archetype = (Archetype**)(bytes);
	EntityInChunk* chunkData = (EntityInChunk*)(bytes + archetypeBytes);

	int startNdx = 0;
	if (entitiesCapacity > 0)
	{
		memcpy(archetype, archetypeByEntity, entitiesCapacity * sizeof(Archetype*));
		memcpy(chunkData, entityInChunkByEntity, entitiesCapacity * sizeof(EntityInChunk));

		startNdx = entitiesCapacity - 1;
	}

	archetypeByEntity = archetype;
	entityInChunkByEntity = chunkData;

	entitiesCapacity = value;

	//InitializeAdditionalCapacity(startNdx); impl ove metode ispod

	for (int i = startNdx; i != entitiesCapacity; i++)
	{
		entityInChunkByEntity[i].SetIndexInChunk(i + 1);
		entityInChunkByEntity[i].SetChunk(NULL);
	}

	// Last entity indexInChunk identifies that we ran out of space...
	entityInChunkByEntity[entitiesCapacity - 1].SetIndexInChunk(-1);

}

EntityManager::~EntityManager()
{
	free(archetypes);
	free(entityInChunkByEntity);
	
	for (int i = 0; i < countArchetypes; i++)
		free(archetypeByEntity[i]);

	free(archetypeByEntity);

	archetypes = NULL;
	countArchetypes = 0;
	archetypeByEntity = NULL;
	entityInChunkByEntity = NULL;
}

template<class T>
bool ECS::EntityManager::HasComponent(Entity entity)
{
	Archetype* archetype = archetypeByEntity[entity.GetID()];
	return archetype->GetIndexInTypeArray(T) != -1;
}

bool ECS::EntityManager::HasComponent(Entity entity, ComponentType componentType)
{
	Archetype* archetype = archetypeByEntity[entity.GetID()];
	return archetype->GetIndexInTypeArray(componentType) != -1;
}

template<class T>
uint8_t* ECS::EntityManager::GetComponentPtr(Entity entity)
{
	Chunk* entityChunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	int entityIndexInChunk = entityInChunkByEntity[entity.GetID()].GetIndexInChunk();

	Archetype* archetype = entityChunk->GetChunkArchetype();

	int indexInTypeArray = archetype->GetIndexInTypeArray(T);

	int offset = archetype->GetOffset(indexInTypeArray);
	int sizeOf = archetype->GetSizeOf(indexInTypeArray);

	return entityChunk->GetBuffer() + (offset + sizeOf * entityIndexInChunk);
}

template<class T>
T EntityManager::GetComponent(Entity entity)
{
	if (!HasComponent<T>(entity))
		return;
	uint8_t* ptr = GetComponentPtr<T>(entity);

	T value(*ptr);
	//CopyPtrToStructure(ptr, out value);
	return value;
}

template<class T>
void ECS::EntityManager::SetComponent(Entity entity, T componentData)
{
	if (!HasComponent<T>(entity))
		return;

	uint8_t* ptr = GetComponentPtr<T>(entity);
	componentData = *ptr;
	//UnsafeUtility.CopyStructureToPtr(ref componentData, ptr);
}


template<class T>
ComponentType ECS::EntityManager::Create()
{
	return ComponentType(typeid(T), sizeof(T));
}

int ECS::EntityManager::GetComponentCount(Entity entity)
{
	Chunk* chunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	return chunk->GetChunkArchetype()->GetTypesCount();
}

bool ECS::EntityManager::AddComponent(Entity entity, ComponentType componentType)
{
	//EnityDataAccess.cs
	if (HasComponent(entity, componentType))
		return false;

	//Move(entity, dstChunk) ECSChanegArch.cs

	Archetype* oldArchetype = archetypeByEntity[entity.GetID()];
	Chunk* oldChunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	int oldIndexInChunk = entityInChunkByEntity[entity.GetID()].GetIndexInChunk();

	std::list<ComponentType> newTypes;
	ComponentType* oldTypes = oldArchetype->GetTypes();
	for (int i = 0; i < oldArchetype->GetTypesCount(); i++) {
		newTypes.push_back(oldTypes[i]);
	}
	newTypes.push_back(componentType);

	Archetype* newArchetype = &GetOrCreateArchetypeWithTypes(newTypes);
    Chunk* newChunk = newArchetype->GetChunksArray()->GetCurrentChunk();
	int newIndexInChunk = newChunk->GetCount();

	archetypeByEntity[entity.GetID()] = newArchetype;
	entityInChunkByEntity[entity.GetID()].SetChunk(newChunk);
	entityInChunkByEntity[entity.GetID()].SetIndexInChunk(newIndexInChunk);
	newChunk->IncrementCount();
	oldChunk->DecrementCount();

	if (oldChunk->GetCount() == 0) {
		oldArchetype->AddToChunkListWithEmptySlots(oldChunk);
	}

	//copy to new chunk and delete from old
	int newTypeIndex = componentType.GetIndex();
	int newt = 1;
	for (int t = 1; t != oldArchetype->GetTypesCount(); t++) {
		int newOffset = newArchetype->GetOffset(newt);
		int newSizeOf = newArchetype->GetSizeOf(newt);

		int oldOffset = oldArchetype->GetOffset(t);
		int oldSizeOf = oldArchetype->GetSizeOf(t);

		int typeIndex = newArchetype->GetTypeInTypeArray(newt).GetIndex();

		uint8_t* newDst = newChunk->GetBuffer() + (newOffset + (newSizeOf * newIndexInChunk));
		uint8_t* oldDst = oldChunk->GetBuffer() + (oldOffset + (oldSizeOf * oldIndexInChunk));
		if (typeIndex == newTypeIndex) {
			memcpy(newDst, oldDst, newSizeOf); 
			memset(oldDst, 0, oldSizeOf);
		}
		else {
			memset(newDst, 0, newSizeOf);
			t--;
		}
		newt++;
	}
	

	return true;
}

bool ECS::EntityManager::RemoveComponent(Entity entity, ComponentType componentType)
{
	if (!HasComponent(entity, componentType))
		return false;
	
	Archetype* oldArchetype = archetypeByEntity[entity.GetID()];
	Chunk* oldChunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	int oldIndexInChunk = entityInChunkByEntity[entity.GetID()].GetIndexInChunk();

	std::list<ComponentType> newTypes;
	ComponentType* oldTypes = oldArchetype->GetTypes();
	for (int i = 0; i < oldArchetype->GetTypesCount(); i++) {
		if (oldTypes[i].GetIndex() == componentType.GetIndex()) continue;
		newTypes.push_back(oldTypes[i]);
	}
	
	Archetype* newArchetype = &GetOrCreateArchetypeWithTypes(newTypes);
	Chunk* newChunk = newArchetype->GetChunksArray()->GetCurrentChunk();
	int newIndexInChunk = newChunk->GetCount();

	archetypeByEntity[entity.GetID()] = newArchetype;
	entityInChunkByEntity[entity.GetID()].SetChunk(newChunk);
	entityInChunkByEntity[entity.GetID()].SetIndexInChunk(newIndexInChunk);
	newChunk->IncrementCount();
	oldChunk->DecrementCount();

	if (oldChunk->GetCount() == 0) {
		oldArchetype->AddToChunkListWithEmptySlots(oldChunk);
	}

	//copy to new chunk and delete from old
	int newTypeIndex = componentType.GetIndex();
	int newt = 1;
	for (int t = 1; t != oldArchetype->GetTypesCount(); t++) {
		int newOffset = newArchetype->GetOffset(newt);
		int newSizeOf = newArchetype->GetSizeOf(newt);

		int oldOffset = oldArchetype->GetOffset(t);
		int oldSizeOf = oldArchetype->GetSizeOf(t);

		int typeIndex = newArchetype->GetTypeInTypeArray(newt).GetIndex();

		uint8_t* newDst = newChunk->GetBuffer() + (newOffset + (newSizeOf * newIndexInChunk));
		uint8_t* oldDst = oldChunk->GetBuffer() + (oldOffset + (oldSizeOf * oldIndexInChunk));
		if (typeIndex == newTypeIndex) {
			memcpy(newDst, oldDst, newSizeOf);
			memset(oldDst, 0, oldSizeOf);
		}
		else {
			memset(newDst, 0, newSizeOf);
			t--;
		}
		newt++;
	}


	return true;
}

ECS::Archetype ECS::EntityManager::CreateArchetype(std::initializer_list<ComponentType> types)
{
	return GetOrCreateArchetypeWithTypes(types);
}

Entity ECS::EntityManager::CreateEntity(std::initializer_list<ComponentType> types)
{
	int numberOfEntites = CountEntities();

	if (archetypeByEntity == NULL) {
		archetypeByEntity = (Archetype**)malloc(entitiesCapacity * sizeof(Archetype*));
	}
	else if (numberOfEntites != 0 && numberOfEntites % capacityInArchetypeArray == 0) {
		archetypeByEntity = (Archetype**)realloc(archetypeByEntity, (numberOfEntites + entitiesCapacity) * sizeof(Archetype*));
	}

	if (entityInChunkByEntity == NULL) {
		entityInChunkByEntity = (EntityInChunk*)malloc(entitiesCapacity * sizeof(EntityInChunk));
	}
	else if (numberOfEntites != 0 && numberOfEntites % entitiesCapacity == 0) {
		entityInChunkByEntity = (EntityInChunk*)realloc(entityInChunkByEntity, (numberOfEntites + entitiesCapacity) * sizeof(EntityInChunk));
	}

	Archetype archetype = GetOrCreateArchetypeWithTypes(types);
	Chunk* chunk = GetChunkWithEmptySlots(archetype);
	if (chunk == NULL) {
		chunk = Chunk::MallocChunk();
		chunk->Init(&archetype);
		archetype.AddToChunkList(chunk);
	}

	Entity* entity = archetype.AllocateEntity(archetype.GetTypes());
	entity->SetID(nextFreeEntityIndex);

	archetypeByEntity[nextFreeEntityIndex] = &archetype;
	entityInChunkByEntity[nextFreeEntityIndex] = EntityInChunk();
	entityInChunkByEntity[nextFreeEntityIndex].SetChunk(chunk);
	entityInChunkByEntity[nextFreeEntityIndex].SetIndexInChunk(chunk->GetCount());
	chunk->IncrementCount();
	archetype.IncerementEntityCount();

	nextFreeEntityIndex = numberOfEntites++;

	return *entity;
}

void ECS::EntityManager::DestroyEntity(Entity entity)
{
	Chunk* chunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	int index = entityInChunkByEntity[entity.GetID()].GetIndexInChunk();
	Archetype* archetype = chunk->GetChunkArchetype();
	
	archetype->DecrementEntityCount();

	//entity from last position switch to this free place
	Chunk* currentChunk = archetype->GetChunksArray()->GetCurrentChunk();
	int positionOfLastElement = currentChunk->GetCount();
	int newPosition = entity.GetID();

	currentChunk->DecrementCount();

	for (int i = 1; i < archetype->GetTypesCount(); i++) {
		int offset = archetype->GetOffset(i);
		int sizeOf = archetype->GetSizeOf(i);

		uint8_t* dst = chunk->GetBuffer() + (offset + sizeOf * index);
		uint8_t* ptr = currentChunk->GetBuffer() + (offset + sizeOf*positionOfLastElement);
		memcpy(dst, ptr, sizeOf);
		memset(ptr, 0, sizeOf);
	}
	

	if (currentChunk->GetCount() == 0) {
		archetype->AddToChunkListWithEmptySlots(currentChunk);
	}


	Entity* lastEntity = (Entity*)currentChunk->GetBuffer() + positionOfLastElement;

	lastEntity->SetID(newPosition); // da li ovo treba
	nextFreeEntityIndex--; //free index in entity array, da li je to dobar free index valjda jeste

}

void ECS::EntityManager::DestroyEntities(IComponent types[])
{
}

int ECS::EntityManager::CountEntities()
{
		int entityCount = 0;
		for (int i = 0; i < countArchetypes; ++i)
		{
			entityCount += archetypes[i].GetEntityCount();
		}

		return entityCount;
}

std::vector<Entity> ECS::EntityManager::GetAllEntities()
{
	std::vector<Entity> allEntites;
	for (int archIndex = 0; archIndex < countArchetypes; archIndex++) {
		ArchetypeChunkArray* chunks = archetypes[archIndex].GetChunksArray();

		for (int chunkNum = 0; chunkNum < chunks->GetNumberOfChunks(); chunkNum++) {
			Entity* entities = (Entity*)chunks->GetChunkByIndex(chunkNum)->GetBuffer();
			int count = chunks->GetChunkByIndex(chunkNum)->GetCount();

			for (int i = 0; i < count ; i++) {
				allEntites.push_back(entities[i]);
			}
		}
	}
	return allEntites;
}
