#include "EnemySystem.h"
#include "EnemyComponent.h"
#include "MovementComponent.h"
#include "LifetimeComponent.h"

void EnemySystem::OnCreate()
{
    entityManager = ECS::ECS_Engine->GetEntityManager();
    movementComponent = entityManager->CreateComponentType<Movement>();
    lifeTimeComponent = entityManager->CreateComponentType<Lifetime>();
    enamyComponent = entityManager->CreateComponentType<EnemyComponent>();

    Entity** entities = entityManager->GetAllEntitiesWithType<EnemyComponent>(&numberOfEntites);
    for (int i = 0; i < numberOfEntites; i++) {
        Movement m = Movement();
        m.x = 99;
        m.y = 97;
        entityManager->SetComponent(*entities[i], m);
        Lifetime l = Lifetime();
        l.time = 3;
        entityManager->SetComponent(*entities[i], l);
        EnemyComponent e = EnemyComponent();
        e.x1 = 0; e.y1 = 0;
        e.x2 = 5; e.y2 = 5;
        e.tempPatrolTime = 0;
        e.patrolTime = 3;
        entityManager->SetComponent(*entities[i], e);
    }
}

void EnemySystem::OnUpdate()
{
   // cout << "Enamy System " << endl;
    std::initializer_list<ComponentType> types = { movementComponent, lifeTimeComponent, enamyComponent };
    uint8_t*** enemyComponents = entityManager->GetComponentsWithTypes(types, &numberOfEntites);

    for (int i = 0; i < numberOfEntites; i++)
    {
        Movement* m = (Movement*)enemyComponents[0][i];
        Lifetime* l = (Lifetime*)enemyComponents[1][i];
        EnemyComponent* e = (EnemyComponent*)enemyComponents[2][i];

        if (e->patrolTime <= e->tempPatrolTime) {
            e->tempPatrolTime = 0;
            if (m->x == e->x1) {
                m->x = e->x2;
                m->y = e->y2;
            }
            else {
                m->x = e->x1;
                m->y = e->y1;
            }
        }
      

        e->tempPatrolTime += ECS::timeStep;
        l->time -= ECS::timeStep;
    }
}
