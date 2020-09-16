#include <chrono>
using namespace std;
using namespace std::chrono;

#include "ECS.h"
#include "MovementComponent.h"
#include "LifetimeComponent.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"
#include "PlayerSystem.h"
#include "EnemySystem.h"
#include "LifetimeSystem.h"

void Game1() {

    EntityManager* entityManager = ECS::ECS_Engine->GetEntityManager();
    SystemManager* systemManager = ECS::ECS_Engine->GetSystemManager();

    //creating component types
    ComponentType movementComponent = entityManager->CreateComponentType<Movement>();
    ComponentType lifeTimeComponent = entityManager->CreateComponentType<Lifetime>();
    ComponentType playerComponent = entityManager->CreateComponentType<PlayerComponent>();
    ComponentType enamyComponent = entityManager->CreateComponentType<EnemyComponent>();

    //creating entities
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

    //add all systems
    PlayerSystem* playerSystem = (PlayerSystem*)systemManager->AddSystem<PlayerSystem>();
    EnemySystem* enemySystem = (EnemySystem*)systemManager->AddSystem<EnemySystem>();
    LifetimeSystem* lifetimeSystem = (LifetimeSystem*)systemManager->AddSystem<LifetimeSystem>();

    systemManager->AddSystemDependency(lifetimeSystem, enemySystem);

}


void Test1() {
    int number = 100000;
    PlayerComponent** playerComponentsOOP = new PlayerComponent*[number];
    for (int i = 0; i < number; i++) {
        playerComponentsOOP[i] = new PlayerComponent();
        playerComponentsOOP[i]->speed = 0;
    }

    EntityManager* entityManager = ECS::ECS_Engine->GetEntityManager();
    ComponentType playerComponent = entityManager->CreateComponentType<PlayerComponent>();
    for (int i = 0; i < number; i++) {
        Entity enamy1 = entityManager->CreateEntity({ playerComponent });
    }
    int count;
    PlayerComponent** components = entityManager->GetComponentsWithType<PlayerComponent>(&count);

    cout << "Size of component: " << sizeof(PlayerComponent) << endl;

    //test speed of object-oriented desgin
    auto start = high_resolution_clock::now();
    for (int i = 0; i < number; i++) {
        playerComponentsOOP[i]->speed += 1;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start); 
    cout << "Time taken in OO design is : " << duration.count() << " microsec " << endl;

    //test speed in ecs
    start = high_resolution_clock::now();
    for (int i = 0; i < count; i++) {
        components[i]->speed += 1;
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken in ECS is : " << duration.count() << " microsec " << endl;
    cout << endl;
}

int main() {
    // initialize global 'Engine' object
    ECS::Initialize();

    const float DELTA_TIME_STEP = 1.0f / 60.0f; // 60hz
    ECS::timeStep = DELTA_TIME_STEP;

    bool bQuit = false;

    //Test1();

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

