#ifndef _CHUNK_H_
#define _CHUNK_H_

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <typeinfo>
#include "Entity.h"


namespace ECS {

	class Archetype;
	class IComponent;
	class ComponentType;

	//svaki chunk ima archetype i kapacitet mu je kapacitet tog archetype 
	//metoda u ChunkDataUtility.cs -> public static void AddEmptyChunk(Archetype* archetype, Chunk* chunk, SharedComponentValues sharedComponentValues) !!!!

	class Chunk {
	private:
		static unsigned long SequenceNumber; // Incrementing automatically for each chunk

		// Chunk header START
		Archetype* archetype;
		uint32_t count;  //current number of entites in chunk
		uint32_t capacity; //number of entities which fits in chunk
		unsigned long id;
		// Chunk header END

		static const int kBufferOffset = 64; // (must be cache line aligned)
		uint8_t buffer[4] = { 0,0,0,0 };

		static const int kChunkSize = 16 * 1024 - 256; // allocate a bit less to allow for header overhead
		static const int kBufferSize = kChunkSize - kBufferOffset;
	public:
		void Init(Archetype* arch);

		static Chunk* MallocChunk(){
			return (Chunk*)malloc(kChunkSize);
		}

		uint32_t GetCount() {
			return count;
		}

		void IncrementCount() {
			this->count++;
		}

		void DecrementCount() {
			this->count--;
		}

		int GetCapacity() {
			return capacity;
		}

		void SetCapacity(uint32_t capacity) {
			this->capacity = capacity;
		}

		void SetArchetype(Archetype* arch) {
			this->archetype = arch;
		}

		Archetype* GetChunkArchetype() {
			return this->archetype;
		}

		static int GetChunkBufferSize()
		{
			// The amount of available space in a chunk is the max chunk size, kChunkSize,
			// minus the size of the Chunk's metadata stored in the fields preceding Chunk.Buffer
			return kChunkSize - kBufferOffset;
		}

		uint8_t* GetBuffer() {
			return buffer;
		}

		bool Has(ComponentType componentType);
	};

}
#endif // !_CHUNK_H_
