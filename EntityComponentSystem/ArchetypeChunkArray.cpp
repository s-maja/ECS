#include "ArchetypeChunkArray.h"

ECS::ArchetypeChunkArray::ArchetypeChunkArray()
{
	count = 0;
	unsigned long chunkPtrSize = (unsigned long)(sizeof(Chunk*) * ChunkPtrCapacity);
	chunks = (Chunk**)malloc(chunkPtrSize);
}

ECS::ArchetypeChunkArray::~ArchetypeChunkArray()
{
	for (int i = 0; i < count; i++)
		free(chunks[i]);
	free(chunks);
	chunks = NULL;
}

int ECS::ArchetypeChunkArray::CalculateEntityCount()
{
	int numberOfEntities = 0;
	for (int i = 0; i < count; i++)
		numberOfEntities = chunks[i]->GetCount();

	return numberOfEntities;
}

void ECS::ArchetypeChunkArray::Add(Chunk* chunk)
{
	if (count > 0 && (count % ChunkPtrCapacity == 0)) {
		unsigned long chunkPtrSize = (unsigned long)(sizeof(Chunk*) * ((unsigned long long)count + ChunkPtrCapacity));
		Chunk** newChunks = (Chunk**)realloc(chunks, chunkPtrSize);
		if (newChunks != NULL)
			chunks = newChunks;
	}
	chunks[count++] = chunk;
}