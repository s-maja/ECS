#include "ECS.h"

int main() {
    // initialize global 'Engine' object
    ECS::Initialize();

    const float DELTA_TIME_STEP = 1.0f / 60.0f; // 60hz

    bool bQuit = false;

    // run main loop until quit
    while (bQuit == false)
    {
        // Update all Systems, remove destroyed components and entities ...
        ECS::ECS_Engine->Update(DELTA_TIME_STEP);
        /*
            ECS::ECS_Engine->GetEntityManager()->...;
            ECS::ECS_Engine->GetComponentManager()->...;
            ECS::ECS_Engine->GetSystemManager()->...;

        */
        // more logic ...
    }

    // destroy global 'Engine' object
    ECS::Terminate();
    return 0;
}

