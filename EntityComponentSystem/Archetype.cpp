#include "Archetype.h"
using namespace ECS;

int ECS::Archetype::CalculateChunkCapacity(int bufferSize, int* componentSizes, int count)
{
	int totalSize = 0;
	for (int i = 0; i < count; ++i)
		totalSize += componentSizes[i];

	int capacity = bufferSize / totalSize; //The amount of available space in a chunk : size of components for 1 entity
	while (CalculateSpaceRequirement(componentSizes, count, capacity) > bufferSize)
		--capacity;
	return capacity;
}

int ECS::Archetype::CalculateSpaceRequirement(int* componentSizes, int componentCount, int entityCount)
{
	int size = 0;
	for (int i = 0; i < componentCount; ++i)
		size += Align(componentSizes[i] * entityCount, 64); //CacheLineSize = 64;
	return size;
}

int ECS::Archetype::Align(int size, int alignmentPowerOfTwo)
{
	return (size + alignmentPowerOfTwo - 1) & ~(alignmentPowerOfTwo - 1);
}

ECS::Archetype::Archetype(std::initializer_list<ComponentType> componentTypes)
{
	int count = (int)componentTypes.size();

	this->numberOfChunksWithEmptySlots = 0;
	this->entityCount = 0;
	this->types = (ComponentType*)malloc(count*sizeof(ComponentType));
	this->typesCount = count;

	//sort ComponentType by index and put inarray types
	int i = 0;
	while (i != count) {
		int min = INT_MAX;
		ComponentType minComponentType;
		for (auto it = std::begin(componentTypes); it != std::end(componentTypes); ++it) {
			int index = ((ComponentType)*it).GetIndex();
			if (index < min) {
				if (i==0 || (i != 0 && types[i - 1].GetIndex() < index)) {
					min = index;
					minComponentType = *it;
				}
			}
		}
		types[i++] = minComponentType;
	}

	
	this->sizeOfs = (int*) malloc(sizeof(int)*((unsigned long long)count+1));
	this->offsets = (int*)malloc(sizeof(int) * ((unsigned long long)count + 1));
	this->chunks = new ArchetypeChunkArray();
	this->chunksWithEmptySlots = NULL;

	this->offsets[0] = 0;
	this->sizeOfs[0] = sizeof(Entity);
	i = 0;
	for (auto it = std::begin(componentTypes); it != std::end(componentTypes); ++it) {
		sizeOfs[i + 1] = sizeof(*it);
		i++;
	}
	
	this->chunkCapacity = CalculateChunkCapacity(Chunk::GetChunkBufferSize(), sizeOfs, count);

	for (int i = 1; i < count; i++)
		this->offsets[i + 1] = Align(this->chunkCapacity * sizeOfs[i], 64);
}

ECS::Archetype::Archetype(std::list<ComponentType> componentTypes)
{
	int count = componentTypes.size();

	this->numberOfChunksWithEmptySlots = 0;
	this->entityCount = 0;
	this->types = (ComponentType*)malloc(count * sizeof(ComponentType));
	this->typesCount = count;

	//sort ComponentType by index and put inarray types
	int i = 0;
	while (i != count) {
		int min = INT_MAX;
		ComponentType minComponentType;
		for (auto it = std::begin(componentTypes); it != std::end(componentTypes); ++it) {
			int index = ((ComponentType)*it).GetIndex();
			if (index < min) {
				if (i == 0 || (i != 0 && types[i - 1].GetIndex() < index)) {
					min = index;
					minComponentType = *it;
				}
			}
		}
		types[i++] = minComponentType;
	}


	this->sizeOfs = (int*)malloc(sizeof(int) * ((unsigned long long)count + 1));
	this->offsets = (int*)malloc(sizeof(int) * ((unsigned long long)count + 1));
	this->chunks = new ArchetypeChunkArray();
	this->chunksWithEmptySlots = NULL;

	this->offsets[0] = 0;
	this->sizeOfs[0] = sizeof(Entity);
	i = 0;
	for (auto it = std::begin(componentTypes); it != std::end(componentTypes); ++it) {
		sizeOfs[i + 1] = sizeof(*it);
		i++;
	}

	this->chunkCapacity = CalculateChunkCapacity(Chunk::GetChunkBufferSize(), sizeOfs, count);

	for (int i = 1; i < count; i++)
		this->offsets[i + 1] = Align(this->chunkCapacity * sizeOfs[i], 64);
}

ECS::Archetype::~Archetype()
{
	delete(chunks);
	free(offsets);
	free(sizeOfs);
	free(types);

	for (int i = 0; i < numberOfChunksWithEmptySlots; i++) {
		free(chunksWithEmptySlots[i]);
	}
	free(chunksWithEmptySlots);

	chunksWithEmptySlots = NULL;

}

Chunk* ECS::Archetype::GetExistingChunkWithEmptySlots()
{
	if (chunksWithEmptySlots == NULL)
		return NULL;
	Chunk* chunk = chunksWithEmptySlots[0];
	numberOfChunksWithEmptySlots--;

	for (int i = 0; i < numberOfChunksWithEmptySlots; ++i)
		chunksWithEmptySlots[i] = chunksWithEmptySlots[i + 1];

	chunksWithEmptySlots = (Chunk**)realloc(chunksWithEmptySlots, sizeof(Chunk*) * numberOfChunksWithEmptySlots);
	return chunk;
}

void ECS::Archetype::AddToChunkListWithEmptySlots(Chunk* chunk)
{
	chunksWithEmptySlots[numberOfChunksWithEmptySlots] = chunk;
}

void ECS::Archetype::AddToChunkList(Chunk* chunk)
{
	chunks->Add(chunk);
}

Entity* ECS::Archetype::AllocateEntity(ComponentType* types)
{
	//ChunkDataUtiliy.cs
	//int AllocateIntoChunk(Chunk * chunk, int count, out int outIndex);
	int allocatedIndex = this->entityCount % chunkCapacity;
	//EntityComponentStoreChunk.cs
	//void AllocateEntities(Archetype * arch, Chunk * chunk, int baseIndex, int count, Entity * outputEntities);
	Entity* entityInChunkStart = (Entity*)chunks->GetCurrentChunk()->GetBuffer() + allocatedIndex;
	this->entityCount++;
	chunks->GetCurrentChunk()->IncrementCount();

	//Initialize Components
	for (int t = 1; t != typesCount; t++) {
		int offset = offsets[t];
		int sizeOf = sizeOfs[t];

		uint8_t* dst = chunks->GetCurrentChunk()->GetBuffer() + (offset + (sizeOf * allocatedIndex));
		memset(dst, 0, sizeOf); 
	}

	return entityInChunkStart;
}

ArchetypeChunkArray* ECS::Archetype::GetChunksArray()
{
	return chunks;
}

int ECS::Archetype::GetIndexInTypeArray(ComponentType type)
{
	for (int i = 0; i < typesCount; i++) {
		if(types[i].GetTypeInfo() == type.GetTypeInfo())
			return i;
	}

	return -1;	
}


template<class T>
int Archetype::GetIndexInTypeArray()
{
	T t;
	for(int i = 0; i < typesCount; i++) {
		if (types[i].GetTypeInfo() == typeid(t))
			return i;
	}

	return -1;
}

ComponentType ECS::Archetype::GetTypeInTypeArray(int index)
{
	return types[index];
}

bool ECS::Archetype::Compare(std::initializer_list<ComponentType> componentTypes)
{
	ComponentType* temp = new ComponentType[componentTypes.size()];

	//sort componentTypes by index and put inarray types
	int i = 0;
	while (i != componentTypes.size()) {
		int min = INT_MAX;
		ComponentType minComponentType;
		for (auto it = std::begin(componentTypes); it != std::end(componentTypes); ++it) {
			int index = ((ComponentType)*it).GetIndex();
			if (index < min) {
				if (i == 0 || (i != 0 && types[i - 1].GetIndex() < index)) {
					min = index;
					minComponentType = *it;
				}
			}
		}
		temp[i++] = minComponentType;
	}

	bool sameTypes = true;
	for (int i = 0; i < componentTypes.size(); i++) {
		if (temp[i].GetIndex()  != (this->types[i]).GetIndex()) {
			sameTypes = false;
			break;
		}
	}

	free(temp);

	return sameTypes;
}

bool ECS::Archetype::Compare(std::list<ComponentType> componentTypes)
{
	ComponentType* temp = new ComponentType[componentTypes.size()];

	//sort componentTypes by index and put inarray types
	int i = 0;
	while (i != componentTypes.size()) {
		int min = INT_MAX;
		ComponentType minComponentType;
		for (auto it = std::begin(componentTypes); it != std::end(componentTypes); ++it) {
			int index = ((ComponentType)*it).GetIndex();
			if (index < min) {
				if (i == 0 || (i != 0 && types[i - 1].GetIndex() < index)) {
					min = index;
					minComponentType = *it;
				}
			}
		}
		temp[i++] = minComponentType;
	}

	bool sameTypes = true;
	for (int i = 0; i < componentTypes.size(); i++) {
		if (temp[i].GetIndex() != (this->types[i]).GetIndex()) {
			sameTypes = false;
			break;
		}
	}

	free(temp);

	return sameTypes;
}
