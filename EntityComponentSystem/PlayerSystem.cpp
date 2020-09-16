#include "PlayerSystem.h"
#include "MovementComponent.h"
#include "PlayerComponent.h"
#include <Windows.h>
#include <conio.h>
#include <windowsx.h>


void PlayerSystem::OnCreate()
{
    entityManager = ECS::ECS_Engine->GetEntityManager();
    movementComponent = entityManager->CreateComponentType<Movement>();
    playerComponent = entityManager->CreateComponentType<PlayerComponent>();

    Entity** entities = entityManager->GetAllEntitiesWithType<PlayerComponent>(&numberOfEntites);
    for (int i = 0; i < numberOfEntites; i++) {
        Movement m = Movement();
        m.x = 0;
        m.y = 0;
        entityManager->SetComponent(*entities[i], m); //init all components of entities
       
        PlayerComponent p = PlayerComponent();
        p.speed = 10;
        entityManager->SetComponent(*entities[i], p);
    }
}

void PlayerSystem::OnUpdate()
{
    std::initializer_list<ComponentType> types = { movementComponent, playerComponent };
    uint8_t*** playerComponents = entityManager->GetComponentsWithTypes(types, &numberOfEntites);

    for (int i = 0; i < numberOfEntites; i++)
    {
        Movement* m = (Movement*)playerComponents[0][i];
        PlayerComponent* p = (PlayerComponent*)playerComponents[1][i];

        int ch = getch_noblock();
        if (ch!=-1) {
            switch (ch)
            {
            case 'W':
                m->x += 100 * p->speed * ECS::timeStep;
                break;
            case 'S':
                m->x += (-100) * p->speed * ECS::timeStep;
                break;
            case 'A':
                m->y += 100 * p->speed * ECS::timeStep;
                break;
            case 'D':
                m->y += (-100) * p->speed * ECS::timeStep;
                break;
            default:
                break;
            }
           
        }
             

    }
}

int PlayerSystem::getch_noblock()
{
    if (_kbhit())
        return _getch();
    else
        return -1;
}
