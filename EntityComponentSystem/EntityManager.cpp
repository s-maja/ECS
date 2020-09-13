#include "EntityManager.h"
#include <list>
using namespace ECS;

Chunk* ECS::EntityManager::GetChunkWithEmptySlots(Archetype* archetype)
{
	Chunk* chunk = archetype->GetChunksArray()->GetCurrentChunk();
	if (chunk!= NULL && (chunk->GetCount() < chunk->GetCapacity()))
		return chunk;
	else {
		chunk = archetype->GetExistingChunkWithEmptySlots();
		if (chunk != NULL) {
			archetype->GetChunksArray()->Add(chunk);
			return chunk;
		}
		else
			return NULL;
	}
}

ECS::Archetype* ECS::EntityManager::GetOrCreateArchetypeWithTypes(std::initializer_list<ComponentType> types)
{
	//check if archetype already exists
	bool sameTypes = true;
	for (int i = 0; i < countArchetypes; i++) {
		if (archetypes[i].GetTypesCount() != types.size()) continue;

		sameTypes = archetypes[i].Compare(types);

		if (sameTypes)
			return &archetypes[i];
	}

	//add to list of archetpes
	if (countArchetypes == 0) {
		this->archetypes = (Archetype*)malloc(sizeof(Archetype) * capacityInArchetypeArray);
	}
	else if ((countArchetypes % capacityInArchetypeArray) == 0) {
		Archetype* temp = (Archetype*)realloc(archetypes, sizeof(Archetype) * ((unsigned long long)capacityInArchetypeArray + countArchetypes));
		if (temp != NULL)
			this->archetypes = temp;
	}

	Archetype arch = Archetype(types);
	this->archetypes[countArchetypes++] = arch;

	return &archetypes[countArchetypes-1];
}

ECS::Archetype* ECS::EntityManager::GetOrCreateArchetypeWithTypes(std::list<ComponentType> types)
{
	//check if archetype already exists
	bool sameTypes = true;
	for (int i = 0; i < countArchetypes; i++) {
		if (archetypes[i].GetTypesCount() != types.size()) continue;

		sameTypes = archetypes[i].Compare(types);

		if (sameTypes)
			return &archetypes[i];
	}

	//add to list of archetpes
	if (countArchetypes == 0) {
		this->archetypes = (Archetype*)malloc(sizeof(Archetype) * capacityInArchetypeArray);
	}
	else if ((countArchetypes % capacityInArchetypeArray) == 0) {
		Archetype* temp = (Archetype*)realloc(archetypes, sizeof(Archetype) * ((unsigned long long)capacityInArchetypeArray + countArchetypes));
		if (temp != NULL)
			this->archetypes = temp;
	}

	Archetype arch = Archetype(types);
	this->archetypes[countArchetypes++] = arch;;

	return &this->archetypes[countArchetypes-1];
}

void ECS::EntityManager::IncreaseCapacity(int value)
{
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

	//InitializeAdditionalCapacity(startNdx);

	for (int i = startNdx; i != entitiesCapacity; i++)
	{
		entityInChunkByEntity[i].SetIndexInChunk(i);
		entityInChunkByEntity[i].SetChunk(NULL);
	}

	// Last entity indexInChunk identifies that we ran out of space...
	entityInChunkByEntity[entitiesCapacity - 1].SetIndexInChunk(-1);
}

EntityManager::EntityManager()
{
	//EntityComponentStore.cs EnsureCapacity
	archetypes = NULL;
	countArchetypes = 0;
	capacityInArchetypeArray = 10;
	entitiesCapacity = 0;
	nextFreeEntityIndex = 0;

	IncreaseCapacity(1000);
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

bool ECS::EntityManager::HasComponent(Entity entity, ComponentType componentType)
{
	Archetype* archetype = archetypeByEntity[entity.GetID()];
	return archetype->GetIndexInTypeArray(componentType) != -1;
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

	Archetype* oldArchetype = archetypeByEntity[entity.GetID()];
	Chunk* oldChunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	int oldIndexInChunk = entityInChunkByEntity[entity.GetID()].GetIndexInChunk(); 
	int lastIndexInOldChunk = oldChunk->GetCount() - 1;

	std::list<ComponentType> newTypes;
	ComponentType* oldTypes = oldArchetype->GetTypes();
	for (int i = 0; i < oldArchetype->GetTypesCount(); i++) {
		newTypes.push_back(oldTypes[i]);
	}
	newTypes.push_back(componentType);

	Archetype* newArchetype = GetOrCreateArchetypeWithTypes(newTypes);
	newTypes.clear();

	Chunk* newChunk = GetChunkWithEmptySlots(newArchetype);
	if (newChunk == NULL) {
		newChunk = Chunk::MallocChunk();
		newChunk->Init(newArchetype);
		newArchetype->AddToChunkList(newChunk);
	}
	int newIndexInChunk = newChunk->GetCount();

	archetypeByEntity[entity.GetID()] = newArchetype;
	entityInChunkByEntity[entity.GetID()].SetChunk(newChunk);
	entityInChunkByEntity[entity.GetID()].SetIndexInChunk(newIndexInChunk);
	newChunk->IncrementCount();
	oldChunk->DecrementCount();

	if (oldChunk->GetCount() == 0) {
		oldArchetype->AddToChunkListWithEmptySlots(oldChunk); 
		oldArchetype->GetChunksArray()->SetNull(oldChunk->GetId());
	}

	//copy to new chunk and delete from old
	int newTypeIndex = componentType.GetIndex();
	int newt = 0;
	bool cleanedNewComponentType = false;
	for (int t = 0; t <= oldArchetype->GetTypesCount(); t++) {
		int newOffset = newArchetype->GetOffset(newt);
		int newSizeOf = newArchetype->GetSizeOf(newt);

		int oldOffset = oldArchetype->GetOffset(t); 
		int oldSizeOf = oldArchetype->GetSizeOf(t);

		uint8_t* newDst = newChunk->GetBuffer() + (newOffset + (newSizeOf * newIndexInChunk));
		uint8_t* oldDst = oldChunk->GetBuffer() + (oldOffset + (oldSizeOf * oldIndexInChunk));
		uint8_t* lastEntityDst = oldChunk->GetBuffer() + (oldOffset + (oldSizeOf * lastIndexInOldChunk));

		int typeIndex = -1;
		if (t != 0) 
			typeIndex = newArchetype->GetTypeInTypeArray(newt - 1).GetIndex();

		if ((typeIndex != newTypeIndex) || t==0) {
			memcpy(newDst, oldDst, newSizeOf); 
			//copy last Enitity in chunk to positon of moved Entity
			if (lastIndexInOldChunk != oldIndexInChunk) {
				memcpy(oldDst, lastEntityDst, oldSizeOf);
			}
			memset(lastEntityDst, 0, oldSizeOf);
		}
		else {
			memset(newDst, 0, newSizeOf);
			cleanedNewComponentType = true;
			t--;
		}
		newt++;
	}
	if (!cleanedNewComponentType) {
		int i = oldArchetype->GetTypesCount();
		int newOffset = newArchetype->GetOffset(i);
		int newSizeOf = newArchetype->GetSizeOf(i);
		uint8_t* newDst = newChunk->GetBuffer() + (newOffset + (newSizeOf * newIndexInChunk));
		memset(newDst, 0, newSizeOf);
	}

	//ako nije poslednji u izbacenom chunk moram prebaciti poslednji na njegovo mjesto haos
	
	oldArchetype->DecrementEntityCount();
	newArchetype->IncerementEntityCount();
	return true;
}

bool ECS::EntityManager::RemoveComponent(Entity entity, ComponentType componentType)
{
	if (!HasComponent(entity, componentType))
		return false;
	
	Archetype* oldArchetype = archetypeByEntity[entity.GetID()];
	Chunk* oldChunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	int oldIndexInChunk = entityInChunkByEntity[entity.GetID()].GetIndexInChunk();
	int lastIndexInOldChunk = oldChunk->GetCount() - 1;

	std::list<ComponentType> newTypes;
	ComponentType* oldTypes = oldArchetype->GetTypes();
	for (int i = 0; i < oldArchetype->GetTypesCount(); i++) {
		if (oldTypes[i].GetIndex() == componentType.GetIndex()) continue;
		newTypes.push_back(oldTypes[i]);
	}
	
	Archetype* newArchetype = GetOrCreateArchetypeWithTypes(newTypes);
	newTypes.clear();

	Chunk* newChunk = GetChunkWithEmptySlots(newArchetype);
	if (newChunk == NULL) {
		newChunk = Chunk::MallocChunk();
		newChunk->Init(newArchetype);
		newArchetype->AddToChunkList(newChunk);
	}
	int newIndexInChunk = newChunk->GetCount();

	archetypeByEntity[entity.GetID()] = newArchetype;
	entityInChunkByEntity[entity.GetID()].SetChunk(newChunk);
	entityInChunkByEntity[entity.GetID()].SetIndexInChunk(newIndexInChunk);
	newChunk->IncrementCount();
	oldChunk->DecrementCount();

	if (oldChunk->GetCount() == 0) {
		oldArchetype->AddToChunkListWithEmptySlots(oldChunk);
		oldArchetype->GetChunksArray()->SetNull(oldChunk->GetId());
	}

	//copy to new chunk and delete from old; old have more types than new archetype
	int newt = 0;
	for (int t = 0; t != oldArchetype->GetTypesCount(); t++) {
		int newOffset = newArchetype->GetOffset(newt);
		int newSizeOf = newArchetype->GetSizeOf(newt);

		int oldOffset = oldArchetype->GetOffset(t);
		int oldSizeOf = oldArchetype->GetSizeOf(t);

		int newTypeIndex = newArchetype->GetTypeInTypeArray(newt - 1).GetIndex();
		int oldTypeIndex = newArchetype->GetTypeInTypeArray(newt - 1).GetIndex();

		uint8_t* newDst = newChunk->GetBuffer() + (newOffset + (newSizeOf * newIndexInChunk));
		uint8_t* oldDst = oldChunk->GetBuffer() + (oldOffset + (oldSizeOf * oldIndexInChunk));
		uint8_t* lastEntityDst = oldChunk->GetBuffer() + (oldOffset + (oldSizeOf * lastIndexInOldChunk));

		if (oldTypeIndex == newTypeIndex) {
			memcpy(newDst, oldDst, newSizeOf);
			if (lastIndexInOldChunk != oldIndexInChunk) {
				memcpy(oldDst, lastEntityDst, oldSizeOf);
			}
			memset(lastEntityDst, 0, oldSizeOf);
			newt++;
		}
		else {
			if (lastIndexInOldChunk != oldIndexInChunk) {
				memcpy(oldDst, lastEntityDst, oldSizeOf);
			}
			memset(lastEntityDst, 0, oldSizeOf);
		}
	}

	oldArchetype->DecrementEntityCount();
	newArchetype->IncerementEntityCount();
	return true;
}

ECS::Archetype* ECS::EntityManager::CreateArchetype(std::initializer_list<ComponentType> types)
{
	return GetOrCreateArchetypeWithTypes(types);
}

Entity ECS::EntityManager::CreateEntity(std::initializer_list<ComponentType> types)
{
	int numberOfEntites = CountEntities();
	
	if (entitiesCapacity == numberOfEntites) {
		IncreaseCapacity(entitiesCapacity + 1000);
	}

	Archetype* archetype = GetOrCreateArchetypeWithTypes(types);
	Chunk* chunk = GetChunkWithEmptySlots(archetype);
	if (chunk == NULL) {
		chunk = Chunk::MallocChunk();
		chunk->Init(archetype);
		archetype->AddToChunkList(chunk);
	}

	Entity* entity = archetype->AllocateEntity(archetype->GetTypes());
	entity->SetID(nextFreeEntityIndex);

	archetypeByEntity[nextFreeEntityIndex] = archetype;
	entityInChunkByEntity[nextFreeEntityIndex] = EntityInChunk();
	entityInChunkByEntity[nextFreeEntityIndex].SetChunk(chunk);
	entityInChunkByEntity[nextFreeEntityIndex].SetIndexInChunk(chunk->GetCount()-1);

	nextFreeEntityIndex = numberOfEntites+1;

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
	int positionOfLastElement = currentChunk->GetCount()-1;
	int newPosition = index;

	currentChunk->DecrementCount();

	for (int i = 0; i <= archetype->GetTypesCount(); i++) {
		int offset = archetype->GetOffset(i);
		int sizeOf = archetype->GetSizeOf(i);

		uint8_t* dst = chunk->GetBuffer() + (offset + sizeOf * index);
		uint8_t* ptr = currentChunk->GetBuffer() + (offset + sizeOf*positionOfLastElement);
		memcpy(dst, ptr, sizeOf);
		memset(ptr, 0, sizeOf);
	}
	

	if (currentChunk->GetCount() == 0) {
		archetype->AddToChunkListWithEmptySlots(currentChunk);
		archetype->GetChunksArray()->SetNull(currentChunk->GetId());
	}

	nextFreeEntityIndex--; //free index in entity array, da li je to dobar free index valjda jeste

}

void ECS::EntityManager::DestroyEntities(std::initializer_list<ComponentType> types)
{
	Archetype* archetype = GetOrCreateArchetypeWithTypes(types);
	ArchetypeChunkArray* chunks  = archetype->GetChunksArray();
	for (int chunkNum = 0; chunkNum < chunks->GetNumberOfChunks(); chunkNum++) {
		Chunk* chunk = chunks->GetChunkByIndex(chunkNum);

		chunk->SetCount(0);
		uint8_t* dst = chunk->GetBuffer();
		memset(dst, 0, chunk->GetChunkBufferSize());


		archetype->AddToChunkListWithEmptySlots(chunk);
		archetype->GetChunksArray()->SetNull(chunk->GetId());
	}
	archetype->SetCount(0);
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

void ECS::EntityManager::SetArchetype(Entity entity, Archetype* newArchetype)
{
	Archetype* oldArchetype = archetypeByEntity[entity.GetID()];
	Chunk* oldChunk = entityInChunkByEntity[entity.GetID()].GetChunk();
	int oldIndexInChunk = entityInChunkByEntity[entity.GetID()].GetIndexInChunk();

	Chunk* newChunk = GetChunkWithEmptySlots(newArchetype);
	if (newChunk == NULL) {
		newChunk = Chunk::MallocChunk();
		newChunk->Init(newArchetype);
		newArchetype->AddToChunkList(newChunk);
	}
	int newIndexInChunk = newChunk->GetCount();

	archetypeByEntity[entity.GetID()] = newArchetype;
	entityInChunkByEntity[entity.GetID()].SetChunk(newChunk);
	entityInChunkByEntity[entity.GetID()].SetIndexInChunk(newIndexInChunk);
	newChunk->IncrementCount();
	oldChunk->DecrementCount();

	if (oldChunk->GetCount() == 0) {
		oldArchetype->AddToChunkListWithEmptySlots(oldChunk);
		oldArchetype->GetChunksArray()->SetNull(oldChunk->GetId());
	}

	//copy to new chunk and delete from old
	int newt = 1;
	for (int t = 1; t != oldArchetype->GetTypesCount(); t++) {
		int newOffset = newArchetype->GetOffset(newt);
		int newSizeOf = newArchetype->GetSizeOf(newt);

		int oldOffset = oldArchetype->GetOffset(t);
		int oldSizeOf = oldArchetype->GetSizeOf(t);

		int typeIndexNew = newArchetype->GetTypeInTypeArray(newt - 1).GetIndex();
		int typeIndexOld = oldArchetype->GetTypeInTypeArray(t - 1).GetIndex();

		uint8_t* newDst = newChunk->GetBuffer() + (newOffset + (newSizeOf * newIndexInChunk));
		uint8_t* oldDst = oldChunk->GetBuffer() + (oldOffset + (oldSizeOf * oldIndexInChunk));
		if (typeIndexNew == typeIndexOld) {
			memcpy(newDst, oldDst, newSizeOf);
			memset(oldDst, 0, oldSizeOf);
		}
		else {
			memset(newDst, 0, newSizeOf); //ne moze ovako jer ne znam je li old ili new archetype ima vise tipova
			t--;
		}
		newt++;
	}
}
