#ifndef _ENTITY_IN_CHUNK_H_
#define _ENTITY_IN_CHUNK_H_

#include "Chunk.h"


namespace ECS {

	class EntityInChunk {
	private:
		Chunk* chunk;
		int indexInChunk;
	public:
	
		ECS::Chunk* GetChunk() {
			return this->chunk;
		}

		void SetChunk(Chunk* chunk) {
			this->chunk = chunk;
		}

		int GetIndexInChunk() {
			return this->indexInChunk;
		}

		void SetIndexInChunk(int index) {
			indexInChunk = index;
		}
	};
}


#endif // !_ENTITY_IN_CHUNK_H_

