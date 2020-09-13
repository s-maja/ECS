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
		ArchetypeChunkArray(const ArchetypeChunkArray& oldArch);
		ArchetypeChunkArray(ArchetypeChunkArray&& oldArch);
		ArchetypeChunkArray& operator=(const ArchetypeChunkArray& oldArch);

		int CalculateEntityCount();

		void Add(Chunk* chunk);

		int GetNumberOfChunks() {
			return count;
		}

		Chunk* GetChunkByIndex(int index) {
			return chunks[index];
		}

		Chunk* GetCurrentChunk() {
			if (count == 0)
				return NULL;
			return chunks[count-1];
		}

		void SetNull(int id) {
			for (int i = 0; i < count; i++)
				if (chunks[i]->GetId() == id) {
					chunks[i] = nullptr;
					count--;
				}

		}
	};

}
#endif // !_ARCHETYPE_CHUNK_ARRAY_H_


