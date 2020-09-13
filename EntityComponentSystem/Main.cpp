#include "ECS.h"
#include "MovementComponent.h"
#include "Lifetime.h"
#include "RotationComponent.h"

void Game1() {
    EntityManager* entityManager = ECS::ECS_Engine->GetEntityManager();

    ComponentType movementComponent = entityManager->CreateComponentType<MovementComponent>();
    ComponentType lifeTimeComponent = entityManager->CreateComponentType<Lifetime>();
    ComponentType rotationComponent = entityManager->CreateComponentType<RotationComponent>();


    Entity enamy1 = entityManager->CreateEntity({ movementComponent, lifeTimeComponent });
    Entity enamy2 = entityManager->CreateEntity({ movementComponent, lifeTimeComponent });
    Entity enamy3 = entityManager->CreateEntity({ movementComponent, lifeTimeComponent });

    Entity player1 = entityManager->CreateEntity({ movementComponent, rotationComponent });

 
    bool has;
    has = entityManager->HasComponent(player1, lifeTimeComponent);
    cout << "Player has lifetime:" << has << endl;
    has = entityManager->HasComponent<Lifetime>(player1);
    cout << "Player has lifetime:" << has << endl;

    //set component
    MovementComponent m = MovementComponent();
    m.speed = 9;
    m.x = 5;
    m.y = 7;
    entityManager->SetComponent<MovementComponent>(enamy2, m);
    //get component
    entityManager->GetComponent<MovementComponent>(enamy2, &m);
    cout << "Movement component:" << m.speed << " " << m.x << " " << m.y << endl;


    //add component - probaj oba posebno pa oba zajedno AddComponent
    entityManager->AddComponent(enamy2, rotationComponent);
   // entityManager->AddComponent(player1, lifeTimeComponent); 

    has = entityManager->HasComponent<RotationComponent>(enamy2);
    cout << "Player has rotation component:" << has << endl;

    //get component
    entityManager->GetComponent<MovementComponent>(enamy2, &m);
    cout << "Movement component:" << m.speed << " " << m.x << " " << m.y << endl;

    int numOfEntites = entityManager->CountEntities();
    std::vector<Entity> allEntites = entityManager->GetAllEntities();
    for (int i = 0; i < numOfEntites; i++) {
        cout << allEntites[i].GetID() << endl;
    }

    //remove component
    entityManager->RemoveComponent(player1, rotationComponent);
    has = entityManager->HasComponent<RotationComponent>(player1);
    cout << "Player has rotation component:" << has << endl;
       

    //destroy entity by enitty
     entityManager->DestroyEntity(enamy2);
    //destory entity by component
    entityManager->DestroyEntities({ movementComponent, lifeTimeComponent });
    numOfEntites = entityManager->CountEntities();
    allEntites = entityManager->GetAllEntities();
    for (int i = 0; i < numOfEntites; i++) {
        cout << allEntites[i].GetID() << endl;
    }


    cout << "kraj" << endl;

   
    //set archetype
}



int main() {
    // initialize global 'Engine' object
    ECS::Initialize();

    const float DELTA_TIME_STEP = 1.0f / 60.0f; // 60hz

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
    }

    // destroy global 'Engine' object
    ECS::Terminate();
    return 0;
}

