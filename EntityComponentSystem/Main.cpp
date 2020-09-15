#include "ECS.h"
#include "MovementComponent.h"
#include "LifetimeComponent.h"
#include "PlayerComponent.h"
#include "EnamyComponent.h"
#include "PlayerSystem.h"
#include "EnamySystem.h"
#include "LifetimeSystem.h"

void Game1() {

    EntityManager* entityManager = ECS::ECS_Engine->GetEntityManager();
    SystemManager* systemManager = ECS::ECS_Engine->GetSystemManager();

    ComponentType movementComponent = entityManager->CreateComponentType<Movement>();
    ComponentType lifeTimeComponent = entityManager->CreateComponentType<Lifetime>();
    ComponentType playerComponent = entityManager->CreateComponentType<PlayerComponent>();
    ComponentType enamyComponent = entityManager->CreateComponentType<EnamyComponent>();

    Entity enamy1 = entityManager->CreateEntity({ movementComponent, lifeTimeComponent, enamyComponent });
    Entity enamy2 = entityManager->CreateEntity({ movementComponent, lifeTimeComponent, enamyComponent });
    Entity enamy3 = entityManager->CreateEntity({ movementComponent, lifeTimeComponent, enamyComponent });

    Entity player1 = entityManager->CreateEntity({ movementComponent });

    bool has;
    has = entityManager->HasComponent(player1, playerComponent);
    cout << "Player has playerComponent: " << has << endl;


    entityManager->AddComponent(player1, playerComponent);

    has = entityManager->HasComponent<PlayerComponent>(player1);
    cout << "Player has playerComponent: " << has << endl;

    int numOfEntites = entityManager->CountEntities();
    std::vector<Entity> allEntites = entityManager->GetAllEntities();
    for (int i = 0; i < numOfEntites; i++) {
        cout << allEntites[i].GetID() << endl;
    }

    Entity testEntity = entityManager->CreateEntity({ lifeTimeComponent, playerComponent, enamyComponent, movementComponent });

    has = entityManager->HasComponent<Lifetime>(testEntity);
    cout << "Test entity has liftime component:" << has << endl;

    entityManager->RemoveComponent(testEntity, lifeTimeComponent);

    has = entityManager->HasComponent<Lifetime>(testEntity);
    cout << "Test entity has liftime component:" << has << endl;

    // entityManager->DestroyEntity(testEntity);
    entityManager->DestroyEntities({ playerComponent, enamyComponent, movementComponent });

    numOfEntites = entityManager->CountEntities();
    allEntites = entityManager->GetAllEntities();
    for (int i = 0; i < numOfEntites; i++) {
        cout << allEntites[i].GetID() << endl;
    }

    cout << endl << "kraj komponenti" << endl;

    PlayerSystem* playerSystem = (PlayerSystem*)systemManager->AddSystem<PlayerSystem>();
    EnamySystem* enamySystem = (EnamySystem*)systemManager->AddSystem<EnamySystem>();
    LifetimeSystem* lifetimeSystem = (LifetimeSystem*)systemManager->AddSystem<LifetimeSystem>();

    systemManager->AddSystemDependency(lifetimeSystem, enamySystem);

}

int main() {
    // initialize global 'Engine' object
    ECS::Initialize();

    const float DELTA_TIME_STEP = 1.0f / 60.0f; // 60hz
    ECS::timeStep = DELTA_TIME_STEP;

    bool bQuit = false;

    //call Test methodes
    //initialization of game world at the begining 
    //all future changes are done using systems
    Game1();

    // run main loop until quit
    while (bQuit == false)
    {
        // Update all Systems
        ECS::ECS_Engine->Update(DELTA_TIME_STEP);

        std::this_thread::sleep_for(std::chrono::milliseconds(60));

    }

    // destroy global 'Engine' object
    ECS::Terminate();
    return 0;
}

