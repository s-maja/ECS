#ifndef _ENTITY_MANAGER_H_
#define _ENTITY_MANAGER_H_

#include <typeinfo>
#include "IComponent.h"
#include "Entity.h"
#include "Archetype.h"
#include "EntityInChunk.h"
#include "ComponentType.h"

namespace ECS {

	// The EntityManager manages entities and components in a World.
	// The EntityManager provides an API to create, read, update, and destroy entities.

	class EntityManager {
	private:
		//Index matches entites
		Archetype** archetypeByEntity;
		EntityInChunk* entityInChunkByEntity;

		Archetype* archetypes; //array of all archetypes which exists
		int countArchetypes;

		int capacityInArchetypeArray;
		int entitiesCapacity;
		int nextFreeEntityIndex;

		Chunk* GetChunkWithEmptySlots(Archetype* archetype);
		Archetype* GetOrCreateArchetypeWithTypes(std::initializer_list<ComponentType> types);
		Archetype* GetOrCreateArchetypeWithTypes(std::list<ComponentType> types);
		template<class T> uint8_t* GetComponentPtr(Entity entity);
		void IncreaseCapacity(int value);
	public:
		EntityManager();
		~EntityManager();

		// Gets the value of a component for an entity.
		template<class T> void GetComponent(Entity entity, T* value);

		// Sets the value of a component of an entity.
		template<class T> void SetComponent(Entity entity, T componentData);

		// Check if entity has this component.
		template<class T> bool HasComponent(Entity entity);

		bool HasComponent(Entity entity, ComponentType componentType);

		// Gets the number of component types associated with an entity.
		int GetComponentCount(Entity entity);

		// Adding a component changes the entity's archetype and results in the entity being moved to a different chunk.
		bool AddComponent(Entity entity, ComponentType componentType);

		// Removing a component changes an entity's archetype and results in the entity being moved to a different chunk.
		bool RemoveComponent(Entity entity, ComponentType componentType);

		// Creates an archetype from a set of component types.
		Archetype* CreateArchetype(std::initializer_list<ComponentType> types);

		// The EntityManager creates the entity in the first available chunk with the matching archetype that has enough space.
		Entity CreateEntity(std::initializer_list<ComponentType> types);

		////get last entity from last chunk in that archetype and put on that place 
		// Destroys an entity.
		void DestroyEntity(Entity entity);

		// Destroy all entities having a common set of component types.
		void DestroyEntities(std::initializer_list<ComponentType> types);

		int CountEntities();

		// Gets all the entities managed by this EntityManager.
		std::vector<Entity> GetAllEntities();

		template<class T>
		ComponentType CreateComponentType();

		void SetArchetype(Entity entity, Archetype* archetype);
		
	};

	template<class T>
	inline ComponentType ECS::EntityManager::CreateComponentType() {
		T t;
		return ComponentType(typeid(t), sizeof(T));
	}


	template<class T>
	inline bool ECS::EntityManager::HasComponent(Entity entity)
	{
		Archetype* archetype = archetypeByEntity[entity.GetID()];
		return archetype->GetIndexInTypeArray<T>() != -1;
	}

	template<class T>
	inline void EntityManager::GetComponent(Entity entity, T* value)
	{

		if (!HasComponent<T>(entity))
			return ;

		uint8_t* ptr = GetComponentPtr<T>(entity);
	
		memcpy(value, ptr, sizeof(T));
	}

	template<class T>
	inline void ECS::EntityManager::SetComponent(Entity entity, T componentData)
	{
		if (!HasComponent<T>(entity))
			return;

		uint8_t* ptr = GetComponentPtr<T>(entity);
		memcpy(ptr, &componentData, sizeof(T));
		//UnsafeUtility.CopyStructureToPtr(ref componentData, ptr);
	}

	template<class T>
	inline uint8_t* ECS::EntityManager::GetComponentPtr(Entity entity)
	{
		Chunk* entityChunk = entityInChunkByEntity[entity.GetID()].GetChunk();
		int entityIndexInChunk = entityInChunkByEntity[entity.GetID()].GetIndexInChunk();

		Archetype* archetype = entityChunk->GetChunkArchetype();

		int indexInTypeArray = archetype->GetIndexInTypeArray<T>();

		int offset = archetype->GetOffset(indexInTypeArray+1); //index in type array starts at 0, but first compoenents starts at 1
		int sizeOf = archetype->GetSizeOf(indexInTypeArray+1);

		return entityChunk->GetBuffer() + (offset + sizeOf * entityIndexInChunk);
	}
}

#endif // !_ENTITY_MANAGER_H_
