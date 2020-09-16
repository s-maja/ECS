#ifndef _ARCHETYPE_H_
#define _ARCHETYPE_H_

#include <string>
#include <iostream>
#include <typeinfo>
#include <initializer_list>
#include <list>
#include "ArchetypeChunkArray.h"
#include "Chunk.h"
#include "IComponent.h"
#include "ComponentType.h"
using namespace std;

namespace ECS {

	class Archetype {
	private:
		int entityCount;
		int chunkCapacity; //Number of entites which fits in chunk of this archetype
		
		ComponentType* types;
		int typesCount;

		// Index matches archetype types
		int* offsets;
		int* sizeOfs;

		ArchetypeChunkArray* chunks;

		Chunk** chunksWithEmptySlots;
		int numberOfChunksWithEmptySlots;
		int capacityChunkWithEmptySlots;

		int CalculateChunkCapacity(int bufferSize, int* componentSizes, int count);
		int CalculateSpaceRequirement(int* componentSizes, int componentCount, int entityCount);
		int Align(int size, int alignmentPowerOfTwo);
	public:
		Archetype(std::initializer_list<ComponentType> componentTypes);
		Archetype(std::list<ComponentType> componentTypes);
		Archetype(const Archetype&);
		Archetype(Archetype&&);
		~Archetype();
		Archetype& operator=(const Archetype& t);

		int GetTypesCount() {
			return this->typesCount;
		}

		ComponentType* GetTypes() {
			return types;
		}

		void SetTypesCount(int count) {
			typesCount = count;
		}

		int GetChunkCapacity() {
			return chunkCapacity;
		}

		void IncerementEntityCount() {
			entityCount++;
		}

		void DecrementEntityCount() {
			entityCount--;
		}

		void SetCount(int i) {
			this->entityCount = i;
		}

		int GetEntityCount() {
			return entityCount;
		}

		void IncEntityCount() {
			this->entityCount++;
		}

		int GetOffset(int index) {
			return offsets[index];
		}

		int GetSizeOf(int index) {
			return sizeOfs[index];
		}

		Chunk* GetExistingChunkWithEmptySlots();

		void AddToChunkListWithEmptySlots(Chunk* chunk);
		
		void AddToChunkList(Chunk* chunk);

		Entity* AllocateEntity(ComponentType* types);

		ArchetypeChunkArray* GetChunksArray();

		int GetIndexInTypeArray(ComponentType type);

		template<class T> int GetIndexInTypeArray();

		ComponentType GetTypeInTypeArray(int index);

		bool Compare(std::initializer_list<ComponentType> componentTypes);

		bool Compare(std::list<ComponentType> componentTypes);
	
	};

	template<class T>
	inline int Archetype::GetIndexInTypeArray()
	{
		T t;
		for (int i = 0; i < typesCount; i++) {
			if (types[i].GetTypeInfo() == typeid(t))
				return i;
		}

		return -1;
	}
}

#endif // !_ARCHETYPE_H_
