#ifndef _ARCHETYPE_CHUNK_ARRAY_H_
#define _ARCHETYPE_CHUNK_ARRAY_H_

#include <vector>
#include "Chunk.h"


namespace ECS {

	/* Contains one or more ArchetypeChunk and one Archetype which is description 
	   of components which is saved in this archetype. */

	class ArchetypeChunkArray {
	private:
		Chunk** chunks;
		int count;
		const int ChunkPtrCapacity = 10;
	public:
		ArchetypeChunkArray();

		~ArchetypeChunkArray();

		int CalculateEntityCount();

		void Add(Chunk* chunk);

		int GetNumberOfChunks() {
			return count;
		}

		Chunk* GetChunkByIndex(int index) {
			return chunks[index];
		}

		Chunk* GetCurrentChunk() {
			return chunks[count-1];
		}

	};

}
#endif // !_ARCHETYPE_CHUNK_ARRAY_H_


