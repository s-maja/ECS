#include "LifetimeSystem.h"
#include "LifetimeComponent.h"

void LifetimeSystem::OnUpdate()
{
    entityManager = ECS::ECS_Engine->GetEntityManager();

    Entity** entities = entityManager->GetAllEntitiesWithType<Lifetime>(&numberOfEntites);
    for (int i = 0; i < numberOfEntites; i++) {
        Lifetime lifetime;
        entityManager->GetComponent(*entities[i], &lifetime);

        if (lifetime.time <= 0)
            entityManager->DestroyEntity(*entities[i]);
    }
}
