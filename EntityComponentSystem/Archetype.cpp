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
	this->capacityChunkWithEmptySlots = 10;

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
		sizeOfs[i + 1] = ((ComponentType)*it).GetSize();
		i++;
	}
	
	this->chunkCapacity = CalculateChunkCapacity(Chunk::GetChunkBufferSize(), sizeOfs, count);

	for (int i = 1; i <= count; i++)
		this->offsets[i] = Align(this->chunkCapacity * sizeOfs[i-1], 64);
}

ECS::Archetype::Archetype(std::list<ComponentType> componentTypes)
{
	size_t count = componentTypes.size();

	this->numberOfChunksWithEmptySlots = 0;
	this->entityCount = 0;
	this->types = (ComponentType*)malloc(count * sizeof(ComponentType));
	this->typesCount = count;
	this->capacityChunkWithEmptySlots = 10;

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
		sizeOfs[i + 1] = ((ComponentType)*it).GetSize();
		i++;
	}

	this->chunkCapacity = CalculateChunkCapacity(Chunk::GetChunkBufferSize(), sizeOfs, count);

	for (int i = 1; i <= count; i++)
		this->offsets[i] = Align(this->chunkCapacity * sizeOfs[i-1], 64);
}

ECS::Archetype::Archetype(Archetype&& oldArch)
{
	entityCount = oldArch.entityCount;
	chunkCapacity = oldArch.chunkCapacity;
	typesCount = oldArch.typesCount;

	// Index matches archetype types
	this->sizeOfs = (int*)malloc(sizeof(int) * ((unsigned long long)typesCount + 1));
	this->offsets = (int*)malloc(sizeof(int) * ((unsigned long long)typesCount + 1));
	this->types= (ComponentType*)malloc(typesCount * sizeof(ComponentType));;

	for (int i = 0; i < typesCount; i++) {
		sizeOfs[i] = oldArch.sizeOfs[i];
		offsets[i] = oldArch.offsets[i];
		types[i] = oldArch.types[i];
	}

	chunks = oldArch.chunks;

	chunksWithEmptySlots = NULL;
	numberOfChunksWithEmptySlots = oldArch.numberOfChunksWithEmptySlots;
}

ECS::Archetype::Archetype(const Archetype& oldArch)
{
	entityCount = oldArch.entityCount;
	chunkCapacity = oldArch.chunkCapacity;
	typesCount = oldArch.typesCount;

	// Index matches archetype types
	this->sizeOfs = (int*)malloc(sizeof(int) * ((unsigned long long)typesCount + 1));
	this->offsets = (int*)malloc(sizeof(int) * ((unsigned long long)typesCount + 1));
	this->types = (ComponentType*)malloc(typesCount * sizeof(ComponentType));;

	for (int i = 0; i < typesCount; i++) {
		sizeOfs[i] = oldArch.sizeOfs[i];
		offsets[i] = oldArch.offsets[i];
		types[i] = oldArch.types[i];
	}

	chunks = oldArch.chunks;

	chunksWithEmptySlots = NULL;
	numberOfChunksWithEmptySlots = oldArch.numberOfChunksWithEmptySlots;
}

ECS::Archetype::~Archetype()
{
	delete(chunks);
	if(offsets!=NULL) (offsets);
	if (sizeOfs != NULL) free(sizeOfs);
	if (types != NULL)free(types);

	for (int i = 0; i < numberOfChunksWithEmptySlots; i++) {
		free(chunksWithEmptySlots[i]);
	}
	if(chunksWithEmptySlots!=NULL) free(chunksWithEmptySlots);

	chunksWithEmptySlots = NULL;
}

Archetype& ECS::Archetype::operator=(const Archetype& oldArch)
{
		if (this != &oldArch) {
			/*if(chunks) delete(chunks);
			if (offsets) free(offsets);
			if (sizeOfs) free(sizeOfs);
			if (types) free(types);
			for (int i = 0; i < numberOfChunksWithEmptySlots; i++) {
				free(chunksWithEmptySlots[i]);
			}
			if(chunksWithEmptySlots) free(chunksWithEmptySlots);
			chunksWithEmptySlots = NULL;*/

			//copy
			entityCount = oldArch.entityCount;
			chunkCapacity = oldArch.chunkCapacity;
			typesCount = oldArch.typesCount;
			capacityChunkWithEmptySlots = oldArch.capacityChunkWithEmptySlots;

			// Index matches archetype types
			this->sizeOfs = (int*)malloc(sizeof(int) * ((unsigned long long)typesCount + 1));
			this->offsets = (int*)malloc(sizeof(int) * ((unsigned long long)typesCount + 1));
			this->types = (ComponentType*)malloc(typesCount * sizeof(ComponentType));;

			for (int i = 0; i <= typesCount; i++) {
				sizeOfs[i] = oldArch.sizeOfs[i];
				offsets[i] = oldArch.offsets[i];
				if(i!= typesCount)
					types[i] = oldArch.types[i];
			}

			chunks = new ArchetypeChunkArray();
			*chunks = *oldArch.chunks;

			chunksWithEmptySlots = NULL;
			numberOfChunksWithEmptySlots = oldArch.numberOfChunksWithEmptySlots;
		}
	return *this;
}

Chunk* ECS::Archetype::GetExistingChunkWithEmptySlots()
{
	if (chunksWithEmptySlots == NULL)
		return nullptr;
	Chunk* chunk = chunksWithEmptySlots[0];
	numberOfChunksWithEmptySlots--;

	for (int i = 0; i < numberOfChunksWithEmptySlots; ++i)
		chunksWithEmptySlots[i] = chunksWithEmptySlots[i + 1];

	chunksWithEmptySlots = (Chunk**)realloc(chunksWithEmptySlots, sizeof(Chunk*) * numberOfChunksWithEmptySlots);
	return chunk;
}

void ECS::Archetype::AddToChunkListWithEmptySlots(Chunk* chunk)
{
	if (chunksWithEmptySlots == NULL) {
		chunksWithEmptySlots = (Chunk**)malloc(sizeof(Chunk*) * capacityChunkWithEmptySlots);
		for (int i = 0; i < capacityChunkWithEmptySlots; i++)
			chunksWithEmptySlots[i] = nullptr;
	}
	else if (numberOfChunksWithEmptySlots != 0 && (numberOfChunksWithEmptySlots% capacityChunkWithEmptySlots == 0) ){
		chunksWithEmptySlots = (Chunk**)realloc(chunksWithEmptySlots, sizeof(Chunk*) * (capacityChunkWithEmptySlots+numberOfChunksWithEmptySlots));
		for (int i = numberOfChunksWithEmptySlots; i < capacityChunkWithEmptySlots+numberOfChunksWithEmptySlots; i++)
			chunksWithEmptySlots[i] = nullptr;
	}
	chunksWithEmptySlots[numberOfChunksWithEmptySlots++] = chunk;
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
	for (int t = 1; t <= typesCount; t++) {
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


ComponentType ECS::Archetype::GetTypeInTypeArray(int index)
{
	return types[index];
}

bool ECS::Archetype::Compare(std::initializer_list<ComponentType> componentTypes)
{
	ComponentType* temp = (ComponentType*) malloc(sizeof(ComponentType)*componentTypes.size());

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
	ComponentType* temp = (ComponentType*)malloc(sizeof(ComponentType) * componentTypes.size());

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
